#pragma once

#include "graphics.hpp"

class Buffer : public IDisposable {
	
public:
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 buffer_object_;
	bool is_deleted_;
#ifdef _DEBUG
	mutable size_t allocated_bytes_;
#endif
	Buffer();

	Buffer(u32 const uiBufferObject);

	~Buffer();

	template <_STD size_t N>
	static void deleteBuffers(Buffer (&buffers)[N]) {
		_STD array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!buffers[i].is_deleted_) {
				objects[i] = buffers[i].buffer_object_;
				buffers[i].is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <_STD size_t N>
	static void deleteBuffers(_STD shared_ptr<Buffer> (&buffers)[N]) {
		_STD array<GLuint, N> objects;
		for (size_t i = 0; i < N; i++) {
			if (!(buffers[i]->is_deleted_)) {
				objects[i] = buffers[i]->buffer_object_;
				buffers[i]->is_deleted_ = true;
			}
		}
		glDeleteBuffers(N, objects.data());
	}

	template <_STD size_t N>
	static _STD array<_STD shared_ptr<Buffer>, N> createBuffers() {
		_STD array<_STD reference_wrapper<Buffer>, N> buffers{};
		_STD array<u32, N> objects{};
		glCreateBuffers(N, objects.data());
		for (size_t i = 0; i < N; i++)
			buffers[i] = _STD make_shared<Buffer>({objects[i]});
		return buffers;
	}

	Buffer(Buffer const &) = delete;
	Buffer(Buffer &&) = delete;
	Buffer& operator=(Buffer const& p) = delete;
	Buffer& operator=(Buffer&& p) = delete;

	void   bind(gl::BufferTargetARB p_target) const;
	void unbind() const;

	void setLabel(_STD string const& p_label) const;

	void allocStorage(std::size_t size, void const *data, std::optional<gl::BufferStorageMask> flags) const;

	_NODISCARD _STD size_t size() const;
	_NODISCARD bool immutable() const;

	// upload full data 
	void upload(_STD size_t const size, void const *data, gl::BufferUsageARB usage) const;

	// Upload sub data
	void update(_STD size_t const size, i64 const offset, void const *data) const;

	void *map(gl::BufferAccessARB access) const;
	void *mapRange(i64 offset, i64 length, gl::MapBufferAccessMask access) const;
	bool unmap() const;
	void flushMappedRange(i64 offset, i64 length) const;

	void invalidateData() const;

	void invalidateSubData(i64 const p_off, i64 const p_len) const;

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index) const;

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index, i64 const p_offset, i64 const p_size) const;
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	[[nodiscard]] bool operator==(Buffer const& other) const { return buffer_object_ == other.buffer_object_; }
	[[nodiscard]] bool operator!=(Buffer const& other) const { return !(*this == other); }

	friend class VertexArray;
};