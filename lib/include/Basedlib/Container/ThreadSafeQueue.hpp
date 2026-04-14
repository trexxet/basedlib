#pragma once

#include <mutex>
#include <optional>
#include <queue>

namespace Basedlib {

template <typename T>
class ThreadSafeQueue {
	mutable std::mutex mtx;
	std::queue<T> q;

public:
	void push (T val) {
		std::scoped_lock lock (mtx);
		q.push (std::move (val));
	}

	std::optional<T> pop () {
		std::scoped_lock lock (mtx);
		if (q.empty())
			return std::nullopt;
		T top = std::move (q.front());
		q.pop();
		return top;
	}

	bool empty () const {
		std::scoped_lock lock (mtx);
		return q.empty();
	}
};

}
