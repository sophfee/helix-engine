#pragma once

#include "graphics.hpp"
#include "types.hpp"
#include "math.hpp"

class ScreenSpaceShadows {
public:

	struct DispatchData
	{
		ivec3 WaveCount;			// Compute Shader Dispatch(X,Y,Z) wave counts X/Y/Z
		ivec2 WaveOffset;			// This value is passed in to shader. It will be different for each dispatch
	};

	struct DispatchList
	{
		vec4 LightCoordinate;		// This value is passed in to shader, this will be the same value for all dispatches for this light
		DispatchData Dispatch[8];	// List of dispatches (max count is 8)
		int DispatchCount;			// Number of compute dispatches written to the list
	};
	
	struct DispatchParameters {
		 vec4 LightCoordinate				= 		vec4(0.0f);
		ivec2 WaveOffset					= 		ivec2(0);
		float SurfaceThickness				= 		0.005f;
		float BilinearThreshold				= 		0.02f;
		float ShadowContrast				= 		4.0f;
		 bool IgnoreEdgePixels				= 		false;
		 bool UsePrecisionOffset			= 		false;
		 bool BilinearSamplingOffsetMode	= 		false;
		 bool DebugOutputEdgeMask			= 		false;
		 bool DebugOutputThreadIndex		= 		false;
		 bool DebugOutputWaveIndex			= 		false;
		 vec2 DepthBounds 					= 		vec2(0.0f, 1.0f);
		 bool UseEarlyOut 					= 		false;
		float FarDepthValue					= 		0.0f;
		float NearDepthValue				= 		0.0f;
		 vec2 InvDepthTextureSize			= 		vec2(0.0f, 1.0f);
	};
	
	ScreenSpaceShadows();
	ScreenSpaceShadows(DispatchParameters params);
	~ScreenSpaceShadows();

	void bind(u32 index) const;
	void upload() const;
	void dispatch(DispatchList const &list);

	static DispatchList buildDispatchList(vec4 inLightProjection, ivec2 inViewportSize, ivec2 inMinRenderBounds, ivec2 inMaxRenderBounds, bool inExpandedZRange = false, int inWaveSize = 64);
	
	DispatchParameters parameters;
private:
	CTexture output_texture_;
	CBuffer buffer_;
	static CSharedPtr<CProgram> program_;

	static struct uniformList_t {
		int outputTexture;
		int depthTexture;
	} uniformList;
	
	static void createProgram();
};