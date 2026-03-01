#pragma once

#include <array>
#include <concepts>
#include <optional>

#include "Function.hpp"

namespace Basedlib {

// TODO: ensure proper constexpr usage

template <typename T>
concept FSMEnum = std::is_scoped_enum_v<T> && requires { T::COUNT; };

template <FSMEnum StateEnum, FSMEnum EventEnum, typename Context = std::nullptr_t>
class FSM {
	StateEnum _state;
	Context* ctx;

	template <FSMEnum FE>
	static constexpr size_t idx (FE i) { return static_cast<size_t> (i); }

public:
	const StateEnum& state = _state;

	using StateCallback = std::conditional_t<
		std::is_same_v<Context, std::nullptr_t>,
		Function <void ()>,
		Function <void (Context*)>
	>;

	// return new state or nullopt if event is not permitted in current state
	using EventCallbackResult = std::optional<StateEnum>;
	using EventCallback = std::conditional_t<
		std::is_same_v<Context, std::nullptr_t>,
		Function <EventCallbackResult (FSM*)>,
		Function <EventCallbackResult (FSM*, Context*)>
	>;

	struct StateCallbacks {
		StateCallback on_enter;
		StateCallback on_exit;
	};

	struct StateCallbacksInitializer {
		StateEnum st;
		StateCallbacks cb;
	};

	struct EventCallbacks {
		EventCallback on_event;
	};

	struct EventCallbacksInitializer {
		EventEnum ev;
		EventCallbacks cb;
	};

	static consteval StateCallbacksInitializer state_cb (StateEnum st, StateCallbacks cb) { return {st, cb}; }
	static consteval EventCallbacksInitializer event_cb (EventEnum ev, EventCallbacks cb) { return {ev, cb}; }
	
	using StatesCallbacks = std::array<StateCallbacks, idx(StateEnum::COUNT)>;
	using EventsCallbacks = std::array<EventCallbacks, idx(EventEnum::COUNT)>;

private:
	StatesCallbacks statesCallbacks;
	EventsCallbacks eventsCallbacks;

	inline void call_state_cb (StateCallback& cb) {
		if (!cb) return;
		if constexpr (std::same_as<Context, std::nullptr_t>) { cb(); }
		else { cb (ctx); }
	}

	inline EventCallbackResult call_event_cb (EventCallback& cb) {
		if (!cb) return std::nullopt;
		if constexpr (std::same_as<Context, std::nullptr_t>) { return cb (this); }
		else { return cb (this, ctx); }
	}

	inline void enter_state (StateEnum st) {
		_state = st;
		call_state_cb (statesCallbacks[idx(_state)].on_enter);
	}

	inline void exit_state () {
		call_state_cb (statesCallbacks[idx(_state)].on_exit);
	}

	constexpr FSM (StateEnum initState, Context* ctx, StatesCallbacks&& statesCallbacks, EventsCallbacks&& eventsCallbacks)
		:_state (initState), ctx (ctx), statesCallbacks (std::move (statesCallbacks)), eventsCallbacks (std::move (eventsCallbacks)) {
		enter_state (initState);
	}

public:
	void switch_state (StateEnum next) {
		if (_state == next) return;
		exit_state();
		enter_state (next);
	}

	inline EventCallbackResult event (EventEnum ev) {
		return call_event_cb (eventsCallbacks[idx(ev)].on_event);
	}

	template <typename... Ts>
	static constexpr FSM make (StateEnum initState, Context* ctx, Ts... xs) {
		StatesCallbacks s {};
		EventsCallbacks e {};

		auto add = [&]<typename T> (T x) {
			if constexpr (std::same_as<T, StateCallbacksInitializer>) { s[idx(x.st)] = x.cb; }
			else if constexpr (std::same_as<T, EventCallbacksInitializer>) { e[idx(x.ev)] = x.cb; }
			else { static_assert (false, "Wrong FSM callback initializer!"); }
		};
		(add(xs), ...);

		return FSM (initState, ctx, std::move(s), std::move(e));
	}

	FSM () = delete;
};

}
