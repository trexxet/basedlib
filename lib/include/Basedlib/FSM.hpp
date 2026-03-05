#pragma once

#include <array>
#include <concepts>
#include <optional>

#include "Class.hpp"
#include "Function.hpp"
#include "PrettyEnum.hpp"

namespace Basedlib::FSM {

template <typename T, size_t maxIndex = 32>
using Enum = PrettyEnum<T, maxIndex>;

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
	State state() const { return _state; }

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

	struct EventCallbacks {
		EventCallback on_event;
	};

	using StatesCallbacks = std::array<StateCallbacks, States::size>;
	using EventsCallbacks = std::array<EventCallbacks, Events::size>;
	struct Callbacks {
		StatesCallbacks states;
		EventsCallbacks events;
	};

	template <State st>
	static consteval auto state_cb (StateCallbacks cb) {
		return [cb] (Callbacks& cbs) consteval {
			cbs.states[States::idx(st)] = cb;
		};
	}
	template <Event ev>
	static consteval auto event_cb (EventCallbacks cb) {
		return [cb] (Callbacks& cbs) consteval {
			cbs.events[Events::idx(ev)] = cb;
		};
	}

private:
	Callbacks callbacks;

	template <class Initializer>
	static consteval bool is_cb_initializer() {
		return requires (Initializer init, Callbacks& cb) { init (cb); };
	}

	void call_state_cb (const StateCallback& cb) {
		if (!cb) return;
		if constexpr (hasContext) cb (ctx); else cb ();
	}

	EventCallbackResult call_event_cb (const EventCallback& cb) {
		if (!cb) return std::nullopt;
		if constexpr (hasContext) return cb (this, ctx); else return cb (this);
	}

	void enter_state (State st) {
		_state = st;
		call_state_cb (callbacks.states[States::idx(_state)].on_enter);
	}

	void exit_state () {
		call_state_cb (callbacks.states[States::idx(_state)].on_exit);
	}

public:
	void switch_state (State next) {
		if (_state == next) return;
		exit_state();
		enter_state (next);
	}

	EventCallbackResult event (Event ev) {
		return call_event_cb (callbacks.events[Events::idx(ev)].on_event);
	}

	template <typename... Initializers> requires (is_cb_initializer<Initializers>() && ...)
	static consteval Callbacks make_callbacks (Initializers... cbInits) {
		Callbacks cb {};
		(cbInits (cb), ...);
		return cb;
	}

	FSM () = delete;
	FSM (State initState, Context* ctx, Callbacks callbacks)
		:_state (initState), ctx (ctx), callbacks (std::move (callbacks)) {
		enter_state (initState);
	}
	BASED_CLASS_NO_COPY_DEFAULT_MOVE (FSM);
};

}
