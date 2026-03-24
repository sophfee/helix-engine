// ReSharper disable CppDefaultCaseNotHandledInSwitchStatement
// ReSharper disable CppCStyleCast
#include "screen_space_shadows.hpp"

#include "graphics.hpp"

ScreenSpaceShadows::uniformList_t ScreenSpaceShadows::uniformList = {
	.outputTexture = 0,
	.depthTexture  = 0
};

SharedPtr<Program> ScreenSpaceShadows::program_ = nullptr;

ScreenSpaceShadows::ScreenSpaceShadows() : parameters(), output_texture_(gl::TextureTarget::Texture2D) {
	buffer_.allocStorage(
		sizeof(DispatchParameters), &parameters,
		gl::BufferStorageMask::DynamicStorageBit
	);
}

ScreenSpaceShadows::ScreenSpaceShadows(DispatchParameters params)
	: parameters(std::move(params)), output_texture_(gl::TextureTarget::Texture2D)
{
	buffer_.allocStorage(
		sizeof(DispatchParameters), &parameters,
		gl::BufferStorageMask::DynamicStorageBit
	);
}

ScreenSpaceShadows::~ScreenSpaceShadows() = default;

void ScreenSpaceShadows::bind(u32 const index) const {
	buffer_.bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, index);
}

void ScreenSpaceShadows::upload() const {
	buffer_.upload(
		sizeof(DispatchParameters), &parameters,
		gl::BufferUsageARB::StaticDraw
	);
}

void ScreenSpaceShadows::dispatch(DispatchList const &list) {

	buffer_.bindBufferBase(gl::BufferTargetARB::ShaderStorageBuffer, 0);
	parameters.LightCoordinate = vec4(list.LightCoordinate[0], list.LightCoordinate[1], list.LightCoordinate[2], list.LightCoordinate[3]);
	
	for (int i = 0; i < list.DispatchCount; i++) {
		auto const &[WaveCount, WaveOffset] = list.Dispatch[i];
		parameters.WaveOffset = WaveOffset;
		
		buffer_.update(sizeof(ivec2),
			offsetof(DispatchParameters, WaveOffset),
			&WaveOffset);
	}
}

ScreenSpaceShadows::DispatchList ScreenSpaceShadows::buildDispatchList(vec4 inLightProjection, ivec2 inViewportSize, ivec2 inMinRenderBounds, ivec2 inMaxRenderBounds, bool const inExpandedZRange, int const inWaveSize) {
	DispatchList result = {};
	
	// Floating point division in the shader has a practical limit for precision when the light is *very* far off screen (~1m pixels+)
	// So when computing the light XY coordinate, use an adjusted w value to handle these extreme values
	float xy_light_w = inLightProjection[3];

	if (float const fp_limit = 0.000002f * static_cast<float>(inWaveSize); xy_light_w >= 0 && xy_light_w < fp_limit) xy_light_w = fp_limit;
	else if (xy_light_w < 0 && xy_light_w > -fp_limit) xy_light_w = -fp_limit;

	// Need precise XY pixel coordinates of the light
	result.LightCoordinate[0] = (inLightProjection[0] / xy_light_w * +0.5f + 0.5f) * static_cast<float>(inViewportSize[0]);
	result.LightCoordinate[1] = (inLightProjection[1] / xy_light_w * -0.5f + 0.5f) * static_cast<float>(inViewportSize[1]);
	result.LightCoordinate[2] = inLightProjection[3] <= 0.01f ? 0.0f : inLightProjection[2] / inLightProjection[3];
	result.LightCoordinate[3] = inLightProjection[3] > 0 ? 1 : -1;

	if (inExpandedZRange)
		result.LightCoordinate[2] = result.LightCoordinate[2] * 0.5f + 0.5f;

	int const light_xy[2] = { (std::lround)(result.LightCoordinate[0]), (std::lround)(result.LightCoordinate[1]) };

	// Make the bounds inclusive, relative to the light
	int const biased_bounds[4] =
	{
		+(inMinRenderBounds[0] - light_xy[0]),
		-(inMaxRenderBounds[1] - light_xy[1]),
		+(inMaxRenderBounds[0] - light_xy[0]),
		-(inMinRenderBounds[1] - light_xy[1]),
	};

	// Process 4 quadrants around the light center,
	// They each form a rectangle with one corner on the light XY coordinate
	// If the rectangle isn't square, it will need breaking in two on the larger axis
	// 0 = bottom left, 1 = bottom right, 2 = top left, 2 = top right
	for (int q = 0; q < 4; q++)
	{
		// Quads 0 and 3 needs to be +1 vertically, 1 and 2 need to be +1 horizontally
		bool const vertical = q == 0 || q == 3;

		// Bounds relative to the quadrant
		int const bounds[4] =
		{
			(std::max)(0,  q & 1 ? biased_bounds[0] : -biased_bounds[2]) / inWaveSize,
			(std::max)(0,  q & 2 ? biased_bounds[1] : -biased_bounds[3]) / inWaveSize,
			(std::max)(0, (q & 1 ? biased_bounds[2] : -biased_bounds[0]) + inWaveSize * (vertical ? 1 : 2) - 1) / inWaveSize,
			(std::max)(0, (q & 2 ? biased_bounds[3] : -biased_bounds[1]) + inWaveSize * (vertical ? 2 : 1) - 1) / inWaveSize,
		};

		if (bounds[2] - bounds[0] > 0 && bounds[3] - bounds[1] > 0)
		{
			int const bias_x = q == 2 || q == 3 ? 1 : 0;
			int const bias_y = q == 1 || q == 3 ? 1 : 0;
			DispatchData &disp = result.Dispatch[result.DispatchCount++];
			
			disp.WaveCount[0]  =  inWaveSize;
			disp.WaveCount[1]  =  bounds[2] - bounds[0];
			disp.WaveCount[2]  =  bounds[3] - bounds[1];
			disp.WaveOffset[0] = (q & 1 ? +bounds[0] : -bounds[2]) + bias_x;
			disp.WaveOffset[1] = (q & 2 ? -bounds[3] : +bounds[1]) + bias_y;
			
			// We want the far corner of this quadrant relative to the light,
			// as we need to know where the diagonal light ray intersects with the edge of the bounds
			int axis_delta;
			switch (q) {
				case 0:
					axis_delta = +biased_bounds[0] - biased_bounds[1];
					break;
				case 1:
					axis_delta = +biased_bounds[2] + biased_bounds[1];
					break;
				case 2:
					axis_delta = -biased_bounds[0] - biased_bounds[3];
					break;
				case 3:
					axis_delta = -biased_bounds[2] + biased_bounds[3];
					break;
				default:
					break;
			}

			axis_delta = (axis_delta + inWaveSize - 1) / inWaveSize;

			if (axis_delta > 0)
			{
				DispatchData& disp2 = result.Dispatch[result.DispatchCount++];

				// Take copy of current volume
				disp2 = disp;

				switch (q) {
					case 0:
						// Split on Y, split becomes -1 larger on x
						disp2.WaveCount[2]  = (std::min)(disp.WaveCount[2], axis_delta);
						disp.WaveCount[2] -= disp2.WaveCount[2];
						disp2.WaveOffset[1] = disp.WaveOffset[1] + disp.WaveCount[2];
						disp2.WaveOffset[0]--;
						disp2.WaveCount[1] ++;
						break;
					case 1:
						// Split on X, split becomes +1 larger on y
						disp2.WaveCount[1] = (std::min)(disp.WaveCount[1], axis_delta);
						disp.WaveCount[1] -= disp2.WaveCount[1];
						disp2.WaveOffset[0] = disp.WaveOffset[0] + disp.WaveCount[1];
						disp2.WaveCount[2] ++;
						break;
					case 2:
						// Split on X, split becomes -1 larger on y
						disp2.WaveCount[1] = (std::min)(disp.WaveCount[1], axis_delta);
						disp.WaveCount[1] -= disp2.WaveCount[1];
						disp.WaveOffset[0] += disp2.WaveCount[1];
						disp2.WaveCount[2] ++;
						disp2.WaveOffset[1]--;
						break;
					case 3:
						// Split on Y, split becomes +1 larger on x
						disp2.WaveCount[2] = (std::min)(disp.WaveCount[2], axis_delta);
						disp.WaveCount[2] -= disp2.WaveCount[2];
						disp.WaveOffset[1] += disp2.WaveCount[2];
						disp2.WaveCount[1] ++;
						break;
					default:
						break;
				}

				// Remove if too small
				if (disp2.WaveCount[1] <= 0 || disp2.WaveCount[2] <= 0)
					disp2 = result.Dispatch[--result.DispatchCount];
				
				if (disp.WaveCount[1] <= 0 || disp.WaveCount[2] <= 0)
					disp = result.Dispatch[--result.DispatchCount];
			}
		}
	}

	// Scale the shader values by the wave count, the shader expects this
	for (int i = 0; i < result.DispatchCount; i++)
	{
		result.Dispatch[i].WaveOffset[0] *= inWaveSize;
		result.Dispatch[i].WaveOffset[1] *= inWaveSize;
	}

	return result;
}

void ScreenSpaceShadows::createProgram() {
	program_.reset(new Program());

	Shader compute(gl::ShaderType::ComputeShader, "shaders\\bend_ss_shadows.comp");
	program_->setLabel("Bend Screenspace Shadows");
	program_->attach(compute);
	program_->link();

	uniformList.depthTexture  = program_->uniformLocation("DepthTexture");
	uniformList.outputTexture = program_->uniformLocation("OutputTexture");
}