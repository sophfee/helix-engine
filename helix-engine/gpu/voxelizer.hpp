#pragma once
#include "math.hpp"
#include "types.hpp"

struct RenderPassInfo;
class Buffer;
class Program;
class Texture;

class Voxelizer {
public:
	Box<Texture> world_grid_;
	Box<Program> program_;


	struct VoxelizerData {
		alignas(16) vec3 gridMin;
		alignas(16) vec3 gridMax;
	};

	mutable VoxelizerData settings_;
	Box<Buffer> data_buffer_;

protected:

	void updateBuffer();
	
public:
	Voxelizer();
	~Voxelizer();

	void setGridMinimum(vec3 const &min);
	void setGridMaximum(vec3 const &max);

	[[nodiscard]] vec3 getGridMinimum() const;
	[[nodiscard]] vec3 getGridMaximum() const;

	void integrityCheck();

	void useProgram() const;

	RenderPassInfo const &renderPassInfo() const;
	
	i32 programUniform(std::string_view name) const;
};
