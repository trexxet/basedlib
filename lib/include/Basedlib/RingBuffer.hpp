#pragma once

#include <cassert>
#include <vector>

namespace Basedlib {

// TODO: make constexpr friendly?

/// @brief Append-only ring buffer. [0] (front) is the oldest item, back is newest.
template <typename T>
class RingBuffer {
	std::vector<T> data;
	/// @brief head is the next write position
	std::size_t head = 0;
	std::size_t cap;

	std::size_t wrap (std::size_t pos) const noexcept { return pos == cap ? 0 : pos; }

public:
	std::size_t capacity () const noexcept { return cap; }
	std::size_t size () const noexcept { return data.size(); }
	bool full () const noexcept { return size() == cap; }
	bool empty () const noexcept { return size() == 0; }

	void push (T val) {
		if (full()) [[likely]] data[head] = std::move (val);
		else data.emplace_back (std::move (val));
		head = wrap (head + 1);
	}

	const T& operator[] (std::size_t idx) const noexcept {
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

	RingBuffer (std::size_t capacity) : cap (capacity) {
		assert (cap > 0 && "RingBuffer must have capacity > 0");
		data.reserve (cap);
	}
	RingBuffer () = delete;

	class Iterator {
		const RingBuffer& rb;
		std::size_t idx;
	public:
		constexpr Iterator (const RingBuffer& rb, std::size_t idx) : rb (rb), idx (idx) { }
		constexpr bool operator!= (const Iterator& other) const noexcept {
			return (rb != other.rb) || (idx != other.idx);
		}
		constexpr const T& operator*() const { return rb[idx]; }
		constexpr Iterator& operator++() noexcept { idx++; return *this; }
	};

	constexpr Iterator begin() const noexcept { return Iterator (*this, 0); }
	constexpr Iterator end() const noexcept { return Iterator (*this, size()); }
};

}
