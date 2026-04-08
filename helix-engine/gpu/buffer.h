#pragma once

#include "graphics.hpp"

class Buffer {
	inline static u32 bound_object_ = 0xFFFFFFFFu;
	u32 buffer_object_;
	bool is_deleted_;

public:
	Buffer();

	Buffer(u32 const uiBufferObject);

	~Buffer() {
		if (!is_deleted_) {
			gpuDebugf("Buffer #%u is being deleted.", buffer_object_);
			glDeleteBuffers(1, &buffer_object_);
		}
	}

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

	void   bind() const;
	void unbind() const;

	void setLabel(_STD string const& p_label) const;

	void allocStorage(_STD size_t const size, void const *data, gl::BufferStorageMask flags) const {
		glNamedBufferStorage(buffer_object_, static_cast<GLsizeiptr>(size), data, static_cast<GLbitfield>(flags));
	}

	_NODISCARD _STD size_t size() const;
	_NODISCARD bool immutable() const;

	// upload full data 
	void upload(_STD size_t const size, void const *data, gl::BufferUsageARB usage) const {
		glNamedBufferData(buffer_object_, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
	}

	// Upload sub data
	void update(_STD size_t const size, i64 const offset, void const *data) const {
		glNamedBufferSubData(buffer_object_, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
	}

	void invalidateData() const {
		glInvalidateBufferData(buffer_object_);
	}

	void invalidateSubData(i64 const p_off, i64 const p_len) const {
		glInvalidateBufferSubData(buffer_object_, p_len, p_off);
	}

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index) const {
		glBindBufferBase(static_cast<GLenum>(p_target), p_index, buffer_object_);
	}

	void bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index, i64 const p_offset, i64 const p_size) const {
		glBindBufferRange(static_cast<GLenum>(p_target), p_index, buffer_object_, p_offset, p_size);
	}

	friend class VertexArray;
};