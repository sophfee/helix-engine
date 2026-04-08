#include "buffer.h"


Buffer::Buffer(): is_deleted_(false) {
	glCreateBuffers(1, &buffer_object_);
	gpuDebugf("Buffer #%u has been born.", buffer_object_);
}

Buffer::Buffer(u32 const uiBufferObject): buffer_object_(uiBufferObject), is_deleted_(false) {
}

void Buffer::setLabel(_STD string const &p_label) const {
	glObjectLabel(static_cast<GLenum>(gl::ObjectIdentifier::Buffer), buffer_object_, static_cast<GLsizei>(p_label.length()), p_label.c_str());
	gpu_check;
}

_STD size_t Buffer::size() const {
	i32 i_size = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_SIZE, &i_size);
	return i_size;
}

bool Buffer::immutable() const {
	i32 i_immutable = 0;
	glGetNamedBufferParameteriv(buffer_object_, GL_BUFFER_IMMUTABLE_STORAGE, &i_immutable);
	return i_immutable == GL_TRUE;
}