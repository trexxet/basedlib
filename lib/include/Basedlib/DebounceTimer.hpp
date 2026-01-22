#pragma once

#include <chrono>

namespace Basedlib {

class DebounceTimer {
	using Clock = std::chrono::steady_clock;
	using Tp = Clock::time_point;

	Clock::duration duration;
	Tp last;
	inline static Tp now () noexcept { return Clock::now(); }

public:
	void trigger () noexcept { last = now(); }
	bool ready () const noexcept { return now() - last >= duration; }

	template <typename Rep, typename Period>
	DebounceTimer (std::chrono::duration<Rep, Period> duration) noexcept
		: duration (std::chrono::duration_cast<Clock::duration> (duration)), last (Tp::min()) { }
};

}
