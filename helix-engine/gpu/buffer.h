#pragma once

#include "graphics.hpp"

/**
 * Buffer Objects are OpenGL Objects that store an array of unformatted memory allocated by the OpenGL context (AKA the GPU). These can be used to store vertex data, pixel data retrieved from images or the framebuffer, and a variety of other things. 
 */
class Buffer : public IDisposable {
public:
	inline static u32 bound_object_ = 0xFFFFFFFFu;

	u32 buffer_object_;

	bool is_deleted_;

	mutable void *mapped_address_;
	
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

	/**
	 * @brief Map all of a buffer object's data store into the client's address space.
	 *
	 * Maps the entire data store of a specified buffer object into the client's address space. The data can then be directly read and/or written relative to the returned pointer, depending on the specified access policy.
	 * 
	 * If an error is generated, a <code>NULL</code> pointer is returned.  
	 * If no error occurs, the returned pointer will reflect an allocation aligned to the value of <code>GL_MIN_MAP_BUFFER_ALIGNMENT</code> basic machine units.  
	 * The returned pointer values may not be passed as parameter values to GL commands. For example, they may not be used to specify array pointers, or to specify or query pixel or texture image data; such actions produce undefined results, although implementations may not check for such behavior for performance reasons.
	 * No GL error is generated if the returned pointer is accessed in a way inconsistent with access (e.g. used to read from a mapping made with access <code>GL_WRITE_ONLY</code> or write to a mapping made with access <code>GL_READ_ONLY</code>), but the result is undefined and system errors (possibly including program termination) may occur.
	 * Mappings to the data stores of buffer objects may have nonstandard performance characteristics. For example, such mappings may be marked as uncacheable regions of memory, and in such cases reading from them may be very slow. To ensure optimal performance, the client should use the mapping in a fashion consistent with the values of <code>GL_BUFFER_USAGE</code> for the buffer object and of access. Using a mapping in a fashion inconsistent with these values is liable to be multiple orders of magnitude slower than using normal memory. 
	 *
	 * @param access Used to modify and/or query the corresponding range of the data store according to the value of access.<br><code>gl::BufferAccessARB::ReadOnly</code> indicates that the returned pointer may be used to read buffer object data.<br> <code>gl::BufferAccessARB::WriteOnly</code> indicates that the returned pointer may be used to modify buffer object data.<br> <code>gl::BufferAccessARB::ReadWrite</code> indicates that the returned pointer may be used to read and to modify buffer object data.
	 * @since 4.5
	 * @return A pointer to the beginning of the mapped range is returned once all pending operations on that buffer object have completed.
	 */
	void *map(gl::BufferAccessARB access) const;
	void *mapRange(i64 offset, i64 length, gl::MapBufferAccessMask access) const;
	bool unmap() const;
	void flushMappedRange(i64 offset, i64 length) const;

	void invalidateData() const;

	void invalidateSubData(i64 const p_off, i64 const p_len) const;

	void bindToBackedBufferBlock(gl::BufferTargetARB const p_target, u32 const p_index) const;

	void bindToBackedBufferBlockRange(gl::BufferTargetARB const p_target, u32 const p_index, i64 const p_offset, i64 const p_size) const;

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

template <typename T = u8>
class TypedBuffer : public Buffer {

	using vec_type = std::vector<T>;
	using vec_const_iter = typename vec_type::const_iterator;
	using vec_iter = typename vec_type::iterator;
	
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

	void allocateElement(T data, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask.has_value() ? storage_mask : std::nullopt;
		count_ = 1;
		allocate(sizeof(T), &data, storage_mask);
	}

	void allocateElements(std::size_t const count, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = count;
		allocate(sizeof(T) * count, nullptr, storage_mask);
	}

	void allocateElements(std::vector<T> const &elements, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = elements.size();
		allocate(sizeof(T) * count_, elements.data(), storage_mask);
	}

	void allocateElements(std::vector<T> const &elements, std::size_t const element_count, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		storage_mask_ = storage_mask;
		count_ = element_count;
		allocate(sizeof(T) * element_count, elements.data(), storage_mask);
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

	T *mappedAddress() const {
		return static_cast<T *>(mapped_address_);
	}

	T *mapElements(gl::BufferAccessARB const access) {
		mappedAddress() = static_cast<T *>(map(access));
		return mappedAddress();
	}

	T *mapElementsRange(i64 const offset, i64 const length, gl::MapBufferAccessMask const access) {
		return static_cast<T *>(mapRange(sizeof(T) * offset, sizeof(T) * length, access));
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
		if (!mappedAddress()) throw std::runtime_error("Buffer must be mapped before accessing data.");
		if (index >= count_) throw std::out_of_range("Index out of range.");
		return mappedAddress()[index];
	}
	
	void allocateElements(vec_const_iter const &start, vec_const_iter const &end, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt) {
		if (count_ <= 0)
			return;
		storage_mask_ = storage_mask;
		count_ = std::distance(start, end);
		allocate(sizeof(T) * count_, &*start, storage_mask);
	}

	void recreateElements(std::size_t const count, T const *data, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt, bool copy_old_data_into_new = false) {
		if (count_ <= 0)
			return;
		storage_mask_ = storage_mask;
		count_ = count;
		recreate(sizeof(T) * count, data, storage_mask, copy_old_data_into_new);
	}

	void recreateElements(std::vector<T> const &elements, std::optional<gl::BufferStorageMask> const storage_mask = std::nullopt, bool copy_old_data_into_new = false) {
		if (count_ <= 0)
			return;
		storage_mask_ = storage_mask;
		count_ = elements.size();
		recreate(sizeof(T) * count_, elements.data(), storage_mask, copy_old_data_into_new);
	}

	TypedBuffer &operator=(Vec<T> const &rhs) {
		allocateElements(rhs);
		return *this;
	}

	TypedBuffer &operator=(T const *rhs) {
		allocateElement(*rhs);
		return *this;
	}

	TypedBuffer &operator=(T const &rhs) {
		allocateElement(rhs);
		return *this;
	}
	
	[[nodiscard]] std::size_t elementCount() const { return count_; }
	[[nodiscard]] std::optional<gl::BufferStorageMask> storageMask() const { return storage_mask_; }

private:
	std::size_t count_ = 0;
	std::optional<gl::BufferStorageMask> storage_mask_ = std::nullopt;
};