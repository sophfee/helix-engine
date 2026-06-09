#include "buffer.h"

#include "render_server.h"
#include "engine/engine.h"

#ifdef _DEBUG
struct BufferAllocationTracker {
	std::atomic<size_t> allocated_bytes_;
	std::atomic<size_t> num_buffers_;
};
static BufferAllocationTracker buffer_tracker_state{};
#endif

#ifdef _DEBUG
Buffer::Buffer(): buffer_object_(0), is_deleted_(false), allocated_bytes_(0) {
	buffer_tracker_state.num_buffers_ += 1;
#else
Buffer::Buffer(): is_deleted_(false) {
#endif
	glCreateBuffers(1, &buffer_object_);
	gpuDebugf("Buffer #%u has been born.", buffer_object_);
	RenderServer::singleton().track(this);
}

Buffer::Buffer(u32 const uiBufferObject): buffer_object_(uiBufferObject), is_deleted_(false) {
	RenderServer::singleton().track(this);
}
Buffer::~Buffer() {
	if (!is_deleted_) {
#ifdef _DEBUG
		buffer_tracker_state.allocated_bytes_ -= allocated_bytes_;
		buffer_tracker_state.num_buffers_ -= 1;
#endif
		gpuDebugf("Buffer #%u is being deleted.", buffer_object_);
		glDeleteBuffers(1, &buffer_object_);
	}
}

void Buffer::bind(gl::BufferTargetARB p_target) const {
	glBindBuffer(static_cast<GLenum>(p_target), buffer_object_);
	gpu_check;
}
void Buffer::unbind() const {}

void Buffer::setLabel(_STD string const &p_label) const {
	glObjectLabel(static_cast<GLenum>(gl::ObjectIdentifier::Buffer), buffer_object_, static_cast<GLsizei>(p_label.length()), p_label.c_str());
	gpu_check;
}
void Buffer::allocStorage(std::size_t const size, void const *data, std::optional<gl::BufferStorageMask> flags) const {
	assert(Engine::singleton()->isOnMainThread());
#ifdef _DEBUG
	allocated_bytes_ = size;
	buffer_tracker_state.allocated_bytes_ += size;
#endif
	glNamedBufferStorage(buffer_object_, static_cast<GLsizeiptr>(size), data, flags.has_value() ? static_cast<GLbitfield>(flags.value()) : 0);
}

_STD size_t Buffer::size() const {
	i32 i_size = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_SIZE, &i_size);
	return i_size;
}

bool Buffer::immutable() const {
	assert(Engine::singleton()->isOnMainThread());
	i32 i_immutable = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_IMMUTABLE_STORAGE, &i_immutable);
	return i_immutable == GL_TRUE;
}

void Buffer::upload(std::size_t const size, void const *data, gl::BufferUsageARB usage) const {
	assert(Engine::singleton()->isOnMainThread());
#ifdef _DEBUG
	allocated_bytes_ = size;
	buffer_tracker_state.allocated_bytes_ += size;
#endif
	glNamedBufferData(buffer_object_, static_cast<GLsizeiptr>(size), data, static_cast<GLenum>(usage));
	gpu_check;
}

void Buffer::update(std::size_t const size, i64 const offset, void const *data) const {
	assert(Engine::singleton()->isOnMainThread());
	glNamedBufferSubData(buffer_object_, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
	gpu_check;
}
void * Buffer::map(gl::BufferAccessARB access) const {
	assert(Engine::singleton()->isOnMainThread());
	void *p = glMapNamedBuffer(buffer_object_, static_cast<GLenum>(access));
	gpu_check;
	return p;
}

void * Buffer::mapRange(i64 const offset, i64 const length, gl::MapBufferAccessMask access) const {
	assert(Engine::singleton()->isOnMainThread());
	void *p = glMapNamedBufferRange(buffer_object_, offset, length, static_cast<GLenum>(access));
	gpu_check;
	return p;
}
bool Buffer::unmap() const {
	bool const b = glUnmapNamedBuffer(buffer_object_);
	gpu_check;
	return b;
}

void Buffer::flushMappedRange(i64 const offset, i64 const length) const {
	glFlushMappedNamedBufferRange(buffer_object_, offset, length);
	gpu_check;
}

void Buffer::invalidateData() const {
	glInvalidateBufferData(buffer_object_);
	gpu_check;
}

void Buffer::invalidateSubData(i64 const p_off, i64 const p_len) const {
	glInvalidateBufferSubData(buffer_object_, p_len, p_off);
}

void Buffer::bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index) const {
	glBindBufferBase(static_cast<GLenum>(p_target), p_index, buffer_object_);
}

void Buffer::bindBufferBase(gl::BufferTargetARB const p_target, u32 const p_index, i64 const p_offset, i64 const p_size) const {
	glBindBufferRange(static_cast<GLenum>(p_target), p_index, buffer_object_, p_offset, p_size);
}

void Buffer::dispose() {
	if (is_deleted_)_UNLIKELY { return; }
	glDeleteBuffers(1, &buffer_object_);
	is_deleted_ = true;
}
bool Buffer::disposed() const {
	return is_deleted_;
}


