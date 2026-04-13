#pragma once

#include <cassert>
#include <cstddef>
#include <concepts>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include "Class.hpp"
#include "Traits.hpp"

namespace Basedlib {

// TODO: use std::inplace_vector when C++26

// Constexpr-friendly vector of fixed capacity
template <typename T, std::size_t N>
requires std::is_nothrow_destructible_v<T>
class StaticVector {
	constexpr static std::size_t sizen (std::size_t n) noexcept { return n * sizeof(T); }

	alignas (T) std::byte data_[sizen(N)];
	std::size_t size_ = 0;

	constexpr std::byte* raw_addr (std::size_t idx) noexcept { return data_ + sizen (idx); };
	constexpr const std::byte* raw_addr (std::size_t idx) const noexcept { return data_ + sizen (idx); };
	constexpr T* addr (std::size_t idx) noexcept {
		return std::launder (reinterpret_cast <T*> (raw_addr (idx)));
	}
	constexpr const T* addr (std::size_t idx) const noexcept {
		return std::launder (reinterpret_cast <const T*> (raw_addr (idx)));
	}

	constexpr void do_copy (const StaticVector& other) {
		clear();
		for (std::size_t i = 0; i < other.size_; i++)
			new (addr (i)) T (other[i]);
		size_ = other.size_;
	}

	constexpr void do_move (StaticVector&& other) {
		clear();
		for (std::size_t i = 0; i < other.size_; i++)
			new (addr (i)) T (std::move (other[i]));
		size_ = other.size_;
		other.clear();
	}

public:
	constexpr std::size_t capacity () const noexcept { return N; }
	constexpr std::size_t size () const noexcept { return size_; }
	constexpr bool full () const noexcept { return size_ == N; }
	constexpr bool empty () const noexcept { return size_ == 0; }

	constexpr const T* data () const noexcept {
		return size_ ? addr(0) : nullptr;
	}

	constexpr T& back () noexcept {
		assert (!empty() && "StaticVector: back() on empty");
		return *addr (size_ - 1);
	}

	constexpr const T& back () const noexcept {
		assert (!empty() && "StaticVector: back() on empty");
		return *addr (size_ - 1);
	}

	constexpr T& operator[] (std::size_t idx) noexcept {
		assert (idx < size() && "StaticVector: [idx] >= size()");
		return *addr (idx);
	}

	constexpr const T& operator[] (std::size_t idx) const noexcept {
		assert (idx < size() && "StaticVector: [idx] >= size()");
		return *addr (idx);
	}

	constexpr void pop_back () {
		assert (!empty() && "StaticVector: pop_back() on empty");
		if constexpr (!std::is_trivially_destructible_v<T>)
			addr (size_ - 1) -> ~T();
		size_--;
	}

	template <typename... Args>
	constexpr T& emplace_back (Args&&... args) {
		assert (!full() && "StaticVector: emplace_back() on full");
		T* p = new (addr (size_)) T (std::forward<Args> (args)...);
		size_++;
		return *std::launder (p);
	}

	constexpr void clear () noexcept {
		if constexpr (std::is_trivially_destructible_v<T>)
			size_ = 0;
		else
			while (!empty()) pop_back();
	}

	// TODO: sentinel?
	template <typename It> requires std::input_iterator<It>
	constexpr explicit StaticVector (It first, It last) {
		for (; first != last && !full(); first++)
			emplace_back (*first);
	}

	constexpr explicit StaticVector (const std::vector<T>& v) :
		StaticVector (v.begin(), v.end()) { }

	constexpr explicit StaticVector (std::vector<T>&& v) :
		StaticVector (std::make_move_iterator (v.begin()), std::make_move_iterator (v.end())) { }

	StaticVector () = default;

	constexpr ~StaticVector () noexcept { clear(); }

	constexpr BASED_CLASS_COPY_CONSTRUCTOR (StaticVector) requires std::copy_constructible<T> {
		do_copy (other);
	}

	BASED_CLASS_COPY_CONSTRUCTOR (StaticVector) requires (!std::copy_constructible<T>) = delete;

	constexpr BASED_CLASS_MOVE_CONSTRUCTOR (StaticVector) requires std::move_constructible<T> {
		do_move (other);
	}
	
	BASED_CLASS_MOVE_CONSTRUCTOR (StaticVector) requires (!std::move_constructible<T>) = delete;

	constexpr BASED_CLASS_COPY_ASSIGNMENT (StaticVector) requires std::copy_constructible<T> {
		if (this != &other) do_copy (other);
		return *this;
	}

	BASED_CLASS_COPY_ASSIGNMENT (StaticVector) requires (!std::copy_constructible<T>) = delete;

	constexpr BASED_CLASS_MOVE_ASSIGNMENT (StaticVector) requires std::move_constructible<T> {
		if (this != &other) do_move (other);
		return *this;
	}

	BASED_CLASS_MOVE_ASSIGNMENT (StaticVector) requires (!std::move_constructible<T>) = delete;

	class Iterator {
		const StaticVector& sv;
		std::size_t idx;
	public:
		constexpr Iterator (const StaticVector& sv, std::size_t idx) : sv (sv), idx (idx) { }
		constexpr bool operator!= (const Iterator& other) const noexcept {
			return (sv != other.sv) || (idx != other.idx);
		}
		constexpr const T& operator*() const { return sv[idx]; }
		constexpr Iterator& operator++() noexcept { idx++; return *this; }
	};

	constexpr Iterator begin() const noexcept { return Iterator (*this, 0); }
	constexpr Iterator end() const noexcept { return Iterator (*this, size()); }
};

template <typename... T> requires all_same<T...>
StaticVector (T...) -> StaticVector <first_variadic_t<T...>, sizeof...(T)>;

}
