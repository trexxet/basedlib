#pragma once

#include <stdexcept>
#include <vector>

namespace Basedlib {

template <typename T>
class RingBuffer {
	std::vector<T> data;
	size_t head = 0, cap;

	size_t wrap (size_t pos) const noexcept { return pos >= cap ? pos % cap : pos; }
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

	const T& operator[] (size_t idx) const {
		if (idx >= size()) [[unlikely]] throw std::out_of_range ("idx >= size()");
		if (full()) [[likely]] idx = wrap (head + idx);
		return data[idx];
	}

	const T& back () const {
		if (empty()) [[unlikely]] throw std::out_of_range ("size() == 0");
		if (full()) [[likely]] return data[(head > 0 ? head : cap) - 1];
		return data[head - 1];
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
