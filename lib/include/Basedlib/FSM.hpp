#pragma once

#include <array>
#include <concepts>
#include <optional>

#include "Function.hpp"
#include "PrettyEnum.hpp"

namespace Basedlib::FSM {

template <typename T, size_t maxIndex = 32>
using Enum = PrettyEnum<T, maxIndex>;

// TODO: ensure proper constexpr usage

template <PrettyEnumT States, PrettyEnumT Events, typename Context = std::nullptr_t>
class FSM {
public:
	using State = States::Enum;
	using Event = Events::Enum;

private:
	State _state;
	Context* ctx;
	static constexpr bool hasContext = !std::is_same_v <Context, std::nullptr_t>;

public:
	const State& state = _state;

	using StateCallback = std::conditional_t <hasContext,
		Function <void (Context*)>,
		Function <void ()>
	>;

	// return new state or nullopt if event is not permitted in current state
	using EventCallbackResult = std::optional<State>;
	using EventCallback = std::conditional_t <hasContext,
		Function <EventCallbackResult (FSM*, Context*)>,
		Function <EventCallbackResult (FSM*)>
	>;

	struct StateCallbacks {
		StateCallback on_enter;
		StateCallback on_exit;
	};

	struct StateCallbacksInitializer {
		State st;
		StateCallbacks cb;
	};

	struct EventCallbacks {
		EventCallback on_event;
	};

	struct EventCallbacksInitializer {
		Event ev;
		EventCallbacks cb;
	};

	template <State st>
	static consteval StateCallbacksInitializer state_cb (StateCallbacks cb) { return {st, cb}; }
	template <Event ev>
	static consteval EventCallbacksInitializer event_cb (EventCallbacks cb) { return {ev, cb}; }
	
	using StatesCallbacks = std::array<StateCallbacks, States::size>;
	using EventsCallbacks = std::array<EventCallbacks, Events::size>;

private:
	StatesCallbacks statesCallbacks;
	EventsCallbacks eventsCallbacks;

	inline void call_state_cb (StateCallback& cb) {
		if (!cb) return;
		if constexpr (hasContext) cb (ctx); else cb ();
	}

	inline EventCallbackResult call_event_cb (EventCallback& cb) {
		if (!cb) return std::nullopt;
		if constexpr (hasContext) return cb (this, ctx); else return cb (this);
	}

	inline void enter_state (State st) {
		_state = st;
		call_state_cb (statesCallbacks[States::idx(_state)].on_enter);
	}

	inline void exit_state () {
		call_state_cb (statesCallbacks[States::idx(_state)].on_exit);
	}

	constexpr FSM (State initState, Context* ctx, StatesCallbacks&& statesCallbacks, EventsCallbacks&& eventsCallbacks)
		:_state (initState), ctx (ctx), statesCallbacks (std::move (statesCallbacks)), eventsCallbacks (std::move (eventsCallbacks)) {
		enter_state (initState);
	}

public:
	void switch_state (State next) {
		if (_state == next) return;
		exit_state();
		enter_state (next);
	}

	inline EventCallbackResult event (Event ev) {
		return call_event_cb (eventsCallbacks[Events::idx(ev)].on_event);
	}

	template <typename... Ts>
	static constexpr FSM make (State initState, Context* ctx, Ts... xs) {
		StatesCallbacks s {};
		EventsCallbacks e {};

		auto add = [&]<typename T> (T x) {
			if constexpr (std::same_as<T, StateCallbacksInitializer>) { s[States::idx(x.st)] = x.cb; }
			else if constexpr (std::same_as<T, EventCallbacksInitializer>) { e[Events::idx(x.ev)] = x.cb; }
			else { static_assert (false, "Wrong FSM callback initializer!"); }
		};
		(add(xs), ...);

		return FSM (initState, ctx, std::move(s), std::move(e));
	}

	FSM () = delete;
};

}
