#pragma once

#include <stdexcept>
#include <vector>

namespace Basedlib {

template <typename T>
class RingBuffer {
	std::vector<T> data;
	size_t head = 0, cap;

	inline size_t wrap (size_t pos) const { return pos >= cap ? pos % cap : pos; }
public:
	inline size_t capacity () const { return cap; }
	inline size_t size () const { return data.size(); }
	inline bool full () const { return size() == cap; }

	void push (T val) {
		if (full()) [[likely]] data[head] = std::move (val);
		else data.emplace_back (std::move (val));
		head = wrap (head + 1);
	}

	const T& operator[] (size_t idx) const {
		if (idx >= size()) [[unlikely]] throw std::out_of_range ("idx >= size()");
		if (full()) [[likely]] idx = wrap (head + idx);
		return data[idx];
	}

	RingBuffer (size_t capacity) : cap (capacity) { data.reserve (capacity); }
	RingBuffer () = delete;

	class Iterator {
		const RingBuffer& rb;
		size_t idx;
	public:
		Iterator (const RingBuffer& rb, size_t idx) : rb (rb), idx (idx) { }
		bool operator!= (const Iterator& other) const { return idx != other.idx; }
		const T& operator*() const { return rb[idx]; }
		Iterator& operator++() { idx++; return *this; }
	};

	Iterator begin() const { return Iterator (*this, 0); }
	Iterator end() const { return Iterator (*this, size()); }
};

}
