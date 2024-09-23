/**
 * @file RingBuffer.h
 *
 * @brief A simple ring buffer.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <stddef.h>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <istream>

// NOTE: The ring buffer cannot be resized after construction.
//       This is also not a very safe structure

/**
 * @brief A simple ring buffer with FIFO semantics for primitive types.
 * 
 * @tparam T The type of the elements in the buffer. T must be a primitive type
 * for which memcpy and sizeof() are well-defined.
 * 
 * NOTE: RingBuffers cannot be resized after construction, and can only ever
 * hold the number of elements they were constructed with.
 */
template<typename T>
class RingBuffer {

public:

	/**
	 * @brief Constructs a new RingBuffer with the given size.
	 * 
	 * If the size is 0, the buffer will be permanently empty.
	 * 
	 * @param size The number of elements the buffer can hold.
	 */
	RingBuffer(size_t size = 1000000); // default size 1,000,000 elements

	RingBuffer(RingBuffer &other);
	~RingBuffer();

	RingBuffer &operator=(RingBuffer &other);

	/**
	 * @brief Returns the first element in the buffer as a reference.
	 * 
	 * @return The first element in the buffer.
	 * 
	 * @throws std::logic_error if the buffer is empty.
	 */
	T& front() const;

	/**
	 * @brief Adds an element to the end of the buffer.
	 * 
	 * @param byte The element to add to the buffer.
	 * 
	 * @returns True if the element was successfully added, false
	 * otherwise. If the buffer is full, the element will not be added.
	 */
	bool push(const T& byte);

	/**
	 * @brief Removes the first element from the buffer.
	 * 
	 * The RingBuffer exhibits FIFO semantics, so the first element added
	 * is the first element removed.
	 * 
	 * @returns True if an element was successfully removed, false otherwise.
	 * If the buffer is empty, no element will be removed.
	 */
	bool pop();

	/**
	 * @brief Returns the next 'size' elements from the 
	 * front of the buffer, up to the size of the buffer.
	 * If 'size' is greater than the number of elements in the buffer,
	 * only as many elements as are available will be returned.
	 * 
	 * @param size The number of elements to get from the buffer.
	 * 
	 * @return A vector containing copies of the next 'size' elements 
	 * from the buffer, or as many as are available if size() < size.
	 * 
	 * @throws std::logic_error if size > size().
	 */
	std::vector<T> get(size_t size) const;

	/**
	 * @brief Returns the next 'size' elements from the
	 * front of the buffer, up to the size of the buffer.
	 * If 'size' is greater than the number of elements in the buffer,
	 * the function will only copy as many elements as are available
	 * into data. Returns the number of elements actually copied.
	 * 
	 * REQUIRES: data is at least 'size' elements long.
	 * 
	 * @param[out] data A pointer to an array where the data will be copied.
	 * The array must be at least 'size' elements long.
	 * @param size The number of elements to get from the buffer.
	 */
	size_t get(T *data, size_t size) const;

	/**
	 * @brief Inserts the given data at the back of the buffer. If the buffer
	 * fills up, overflow data is discarded. Returns the number of elements
	 * actually inserted (less than size if the buffer overflowed).
	 * 
	 * @param data The data to insert into the buffer.
	 * 
	 * @return The number of elements actually inserted.
	 */
	size_t insert(const std::vector<T> &data);

	/**
	 * @brief Inserts the given data at the back of the buffer. If the buffer
	 * fills up, overflow data is discarded. Returns the number of elements
	 * actually inserted (less than size if the buffer overflowed).
	 * 
	 * REQUIRES: data is at least 'size' elements long.
	 * 
	 * @param data The data to insert into the buffer. The array must be at
	 * least 'size' elements long.
	 * @param size The number of elements in the data array.
	 * 
	 * @return The number of elements actually inserted.
	 */
	size_t insert(T const *data, size_t size);

	/**
	 * @brief Removes the next 'size' elements from the front of the buffer,
	 * up to the size of the buffer. If 'size' is greater than the number of
	 * elements in the buffer, this will clear the buffer.
	 * 
	 * @param size The number of elements to remove from the buffer.
	 * 
	 * @return The number of elements actually removed.
	 */
	size_t erase(size_t size);

	/**
	 * @brief Returns the number of elements in the buffer.
	 * 
	 * @return The number of elements in the buffer.
	 */
	size_t size() const;

	/**
	 * @brief Returns the number of elements the buffer can hold.
	 * 
	 * @return The number of elements the buffer can hold.
	 */
	size_t capacity() const;

	/**
	 * @brief Clears the buffer of all elements. Does not change the capacity
	 * or reserved memory.
	 */
	void clear();

	/**
	 * @brief Returns true if the buffer is empty.
	 * 
	 * @return True if the buffer is empty, false otherwise.
	 */
	bool empty() const;

	/**
	 * @brief Returns true if the buffer is full.
	 * 
	 * @return True if the buffer is full, false otherwise.
	 */
	bool full() const;

	/**
	 * @brief Reads the contents of in into the buffer, up to the capacity of
	 * the buffer. Overwrites the buffer.
	 * 
	 * @param in The input stream to read from.
	 * @param rb The RingBuffer to read into.
	 */
	template<typename U>
	friend std::istream &operator>>(std::istream &in, RingBuffer<U> &rb);

private:

	// The number of elements the buffer can hold
	size_t bufferCapacity;

	// The index of the back of the buffer, where elements are added
	size_t head;

	// The index of the front of the buffer, where elements are removed
	size_t tail;
	
	// Flag used to determine if the buffer is empty. We will find that
	// head == tail in both the full and empty cases, so we use this flag
	// to differentiate between the two.
	bool isEmpty;

	// An array acting as the raw data buffer.
	T* buffer;

};

///////////////////////////////////////////////////////////////////////////////
// RingBuffer Implementation
///////////////////////////////////////////////////////////////////////////////

template<typename T>
size_t RingBuffer<T>::erase(size_t size) {

	if(size >= this->size()) {

		size = this->size();

		clear();
		return size;

	}

	// We shouldn't get this far in the zero capacity case, so we need not
	// worry about modding by zero

	tail = (tail + size) % bufferCapacity;

	if(tail == head) isEmpty = true;

	return size;

}

template<typename T>
size_t RingBuffer<T>::get(T *data, size_t size) const {

	size = std::min(size, this->size());

	int partition = std::min(bufferCapacity - tail, size);

	std::memcpy(data, buffer + tail, partition * sizeof(T));
	std::memcpy(data + partition, buffer, (size - partition) * sizeof(T));

	return size;

}

template<typename T>
std::vector<T> RingBuffer<T>::get(size_t size) const {

	std::vector<T> data(size);

	// This both copies the data and resizes the output vector to fit the data
	data.resize(get(data.data(), size));

	return data;

}

template<typename T>
size_t RingBuffer<T>::insert(T const *data, size_t size) {

	size = std::min(size, capacity() - this->size());

	int partition = std::min(bufferCapacity - head, size);

	std::memcpy(buffer + head, data, partition * sizeof(T));
	std::memcpy(buffer, data + partition, (size - partition) * sizeof(T));

	// This takes care of both the empty data case and the bufferCapacity = 0
	// case
	if(size == 0) return 0;

	head = (head + size) % bufferCapacity;

	isEmpty = false;

	return size;

}

template<typename T>
size_t RingBuffer<T>::insert(const std::vector<T> &data) {

	return insert(data.data(), data.size());

}

template<typename T>
bool RingBuffer<T>::empty() const {

	return isEmpty;

}


template<typename T>
bool RingBuffer<T>::full() const {

	if(capacity() == 0) return true;

	if(empty()) return false;

	return head == tail;

}

template<typename T>
size_t RingBuffer<T>::capacity() const {

	return bufferCapacity;

}

template<typename T>
T &RingBuffer<T>::front() const {

	if(empty()) throw std::logic_error("LockableStream -- Tried to access an empty RingBuffer!");

	return buffer[tail];

}

template<typename T>
bool RingBuffer<T>::push(const T &byte) {

	if(full()) return false;

	// In the zero capacity case, we should not get here, so we don't need to
	// worry about modding by zero

	buffer[head] = byte;

	head = (head + 1) % bufferCapacity;

	isEmpty = false;

	return true;

}

template<typename T>
bool RingBuffer<T>::pop() {

	if(empty()) return false;

	// In the zero capacity case, we should not get here, so we don't need to
	// worry about modding by zero

	tail = (tail + 1) % bufferCapacity;

	if(tail == head) isEmpty = true;

	return true;

}

template<typename T>
size_t RingBuffer<T>::size() const {

	if(full()) return bufferCapacity;

	// In the zero capacity case, we should not get here, so we don't need to
	// worry about modding by zero

	return (head - tail + bufferCapacity) % bufferCapacity;

}

template<typename T>
void RingBuffer<T>::clear() {

	head = 0;
	tail = 0;

	isEmpty = true;

}

template<typename T>
RingBuffer<T>::RingBuffer(size_t size) 
	: bufferCapacity(size), 
	  head(0), 
	  tail(0), 
	  isEmpty(true) {

	buffer = new T[bufferCapacity];

}

template<typename T>
RingBuffer<T>::RingBuffer(RingBuffer &other) 
	: bufferCapacity(other.bufferCapacity), 
	  head(other.head), 
	  tail(other.tail), 
	  isEmpty(other.isEmpty) {

	buffer = new T[bufferCapacity];

	std::memcpy(buffer, other.buffer, bufferCapacity);

}

template<typename T>
RingBuffer<T>::~RingBuffer() {

	delete[] buffer;
	buffer = nullptr;

}

template<typename T>
RingBuffer<T> &RingBuffer<T>::operator=(RingBuffer<T> &other) {

	if(this == &other) return *this;

	bufferCapacity = other.bufferCapacity;
	head = other.head;
	tail = other.tail;
	isEmpty = other.isEmpty;

	delete[] buffer;

	buffer = new T[bufferCapacity];

	std::memcpy(buffer, other.buffer, bufferCapacity);

	return *this;

}

template<typename T>
std::istream &operator>>(std::istream &in, RingBuffer<T> &rb) {

	// Do nothing for the zero capacity case
	if(rb.capacity() == 0) return in;

	rb.clear();

	in.read((char*)(rb.buffer), rb.bufferCapacity * sizeof(T));
	rb.head = (in.gcount() / sizeof(T)) % rb.bufferCapacity;

	if(in.gcount() / sizeof(T) > 0) rb.isEmpty = false;

	in.clear();

	// Back the read pointer up to the last complete element we read
	in.seekg(-(in.gcount() % sizeof(T)), in.cur);

	return in;

}