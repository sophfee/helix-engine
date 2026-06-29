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

	~Buffer() override;

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

	void allocate(std::size_t size, void const *data, std::optional<gl::BufferStorageMask> flags) const;

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

	void download(i64 const p_offset, i64 const p_size, u8 *out_data) const;
	void download(i64 const p_offset, u8 *out_data) const;
	void download(u8 *out_data) const;
	void download(u8 *out_data, i64 &out_size) const;

	void recreate(std::size_t size, void const *data, std::optional<gl::BufferStorageMask> flags, bool copy_old_data_into_new = false);
	
	void dispose() override;
	[[nodiscard]] bool disposed() const override;

	[[nodiscard]] bool operator==(Buffer const& other) const { return buffer_object_ == other.buffer_object_; }
	[[nodiscard]] bool operator!=(Buffer const& other) const { return buffer_object_ != other.buffer_object_; }

	friend class VertexArray;
};

template <typename T>
class TypedBuffer : public Buffer {
public:
	TypedBuffer() = default;
	
	template <size_t N>
	TypedBuffer(T (&data)[N], std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt)
		: storage_mask_(storage_mask) {
		allocate(sizeof(T) * N, data, storage_mask);
	}

	TypedBuffer(TypedBuffer const &) = delete;
	TypedBuffer(TypedBuffer &&) = delete;
	TypedBuffer& operator=(TypedBuffer const &) = delete;
	TypedBuffer& operator=(TypedBuffer &&) = delete;

	template <std::size_t N>
	void allocateElements(T (&data)[N], std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = N;
		allocate(sizeof(T) * N, data, storage_mask);
	}

	template <size_t N>
	void allocateElements(std::array<T, N> const &arr, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = N;
		allocate(sizeof(T) * N, arr.data(), storage_mask);
	}
	
	void allocateElements(std::size_t const count, T const *data, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = count;
		allocate(sizeof(T) * count, data, storage_mask);
	}

	void allocateElements(std::size_t const count, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = count;
		allocate(sizeof(T) * count, nullptr, storage_mask);
	}

	void uploadElements(std::size_t const count, T const *data, gl::BufferUsageARB const usage) {
		count_ = count;
		upload(sizeof(T) * count, data, usage);
	}

	template <std::size_t N>
	void uploadElements(T (&data)[N], gl::BufferUsageARB const usage) {
		count_ = N;
		upload(sizeof(T) * N, data, usage);
	}

	void updateElements(std::size_t const count, i64 const offset, T const *data) {
		update(sizeof(T) * count, offset * sizeof(T), data);
	}

	template <size_t N>
	void updateElements(T (&data)[N], i64 const offset) {
		update(sizeof(T) * N, offset * sizeof(T), data);
	}

	T *mapElements(gl::BufferAccessARB const access) {
		mapped_address = static_cast<T *>(map(access));
		return mapped_address;
	}

	T *mapElementsRange(i64 const offset, i64 const length, gl::MapBufferAccessMask const access) {
		mapped_address = static_cast<T *>(mapRange(sizeof(T) * offset, sizeof(T) * length, access));
		return mapped_address;
	}

	void flushMappedElementsRange(i64 const offset, i64 const count) const {
		flushMappedRange(offset * sizeof(T), count * sizeof(T));
	}

	void downloadElements(Vec<T> &outVec) {
		if (outVec.size() < count_)
			outVec.resize(count_);
		download(0, sizeof(T) * count_, reinterpret_cast<u8 *>(outVec.data()));
	}

	[[nodiscard]] T &operator[](std::size_t index) const {
		if (!mapped_address) throw std::runtime_error("Buffer must be mapped before accessing data.");
		if (index >= count_) throw std::out_of_range("Index out of range.");
		return mapped_address[index];
	}

private:
	std::size_t count_ = 0;
	T *mapped_address = nullptr;
	std::optional<gl::BufferStorageMask> storage_mask_ = std::nullopt;
};