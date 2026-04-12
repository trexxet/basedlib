#pragma once

#include <cassert>
#include <vector>

namespace Basedlib {

/// @brief Append-only ring buffer. [0] (front) is the oldest item, back is newest.
template <typename T>
class RingBuffer {
	std::vector<T> data;
	/// @brief head is the next write position
	size_t head = 0;
	size_t cap;

	size_t wrap (size_t pos) const noexcept { return pos == cap ? 0 : pos; }

public:
	size_t capacity () const noexcept { return cap; }
	size_t size () const noexcept { return data.size(); }
	bool full () const noexcept { return size() == cap; }
	bool empty () const noexcept { return size() == 0; }

	void push (T val) {
		if (full()) [[likely]] data[head] = std::move (val);
		else data.emplace_back (std::move (val));
		head = wrap (head + 1);
	}

	const T& operator[] (size_t idx) const noexcept {
		assert (idx < size() && "RingBuffer: [idx] >= size()");
		if (full()) [[likely]] idx = wrap (head + idx);
		return data[idx];
	}

	const T& back () const noexcept {
		assert (!empty() && "RingBuffer: back() on empty");
		return data[(head > 0 ? head : cap) - 1];
	}

	const T& front () const noexcept {
		assert (!empty() && "RingBuffer: front() on empty");
		return full() ? data[head] : data[0];
	}

	RingBuffer (size_t capacity) : cap (capacity) {
		assert (cap > 0 && "RingBuffer must have capacity > 0");
		data.reserve (cap);
	}
	RingBuffer () = delete;

	class Iterator {
		const RingBuffer& rb;
		size_t idx;
	public:
		Iterator (const RingBuffer& rb, size_t idx) : rb (rb), idx (idx) { }
		bool operator!= (const Iterator& other) const noexcept { return idx != other.idx; }
		const T& operator*() const { return rb[idx]; }
		Iterator& operator++() noexcept { idx++; return *this; }
	};

	Iterator begin() const noexcept { return Iterator (*this, 0); }
	Iterator end() const noexcept { return Iterator (*this, size()); }
};

}
