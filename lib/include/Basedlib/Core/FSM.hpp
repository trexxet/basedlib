#pragma once

#include <array>
#include <concepts>
#include <format>
#include <optional>
#include <string_view>

#include "Basedlib/Class.hpp"
#include "Basedlib/Core/Function.hpp"
#include "Basedlib/Meta/PrettyEnum.hpp"

namespace Basedlib::FSM {

template <typename T, std::size_t maxIndex = 32>
using Enum = PrettyEnum <T, maxIndex>;

using LogCallbackT = Function <void (std::string_view)>;

template <PrettyEnumT States, PrettyEnumT Events, typename Context = std::nullptr_t, LogCallbackT logCallback = LogCallbackT {}>
class FSM {
public:
	using State = States::Enum;
	using Event = Events::Enum;

private:
	State _state;
	Context* _ctx;
	static constexpr bool hasContext = !std::is_same_v <Context, std::nullptr_t>;

public:
	State state() const noexcept { return _state; }
	Context* ctx() const noexcept { return _ctx; }

	using StateCallback = std::conditional_t <hasContext, void (Context*), void ()>;

	// return new state or nullopt (EventNotPermitted) if event is not permitted in current state
	using EventCallbackResult = std::optional<State>;
	using EventCallback = std::conditional_t <hasContext,
		EventCallbackResult (FSM*, Context*),
		EventCallbackResult (FSM*)
	>;
	static constexpr auto EventNotPermitted = std::nullopt;

	struct StateCallbacks {
		Function<StateCallback> on_enter;
		Function<StateCallback> on_exit;
	};

	using EventCallbacks = Function<EventCallback>;

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

	void call_state_cb (const Function<StateCallback>& cb) {
		if (!cb) return;
		if constexpr (hasContext) cb (_ctx); else cb ();
	}

	EventCallbackResult call_event_cb (const Function<EventCallback>& cb) {
		if (!cb) return std::nullopt;
		if constexpr (hasContext) return cb (this, _ctx); else return cb (this);
	}

	void enter_state (State st) {
		_state = st;
		call_state_cb (callbacks.states[States::idx(_state)].on_enter);
	}

	void exit_state () {
		call_state_cb (callbacks.states[States::idx(_state)].on_exit);
	}

public:
	State switch_state (State next) {
		if (_state == next) return next;

		if constexpr (logCallback)
			logCallback (std::format ("Switch state {} -> {}", States::to_string (_state), States::to_string (next)));

		exit_state();
		enter_state (next);
		return next;
	}

	EventCallbackResult event (Event ev) {
		const Function<EventCallback>& cb = callbacks.events[Events::idx(ev)];

		if constexpr (logCallback) {
			std::string_view evStr = Events::to_string (ev);
			logCallback (std::format ("Event {}", evStr));

			EventCallbackResult result = call_event_cb (cb);

			if (!result)
				logCallback (std::format ("Event {} is not permitted in state {}", evStr, States::to_string (_state)));
			return result;
		}

		return call_event_cb (cb);
	}

	template <typename... Initializers> requires (is_cb_initializer<Initializers>() && ...)
	static consteval Callbacks make_callbacks (Initializers... cbInits) {
		Callbacks cb {};
		(cbInits (cb), ...);
		return cb;
	}

	FSM () = delete;
	FSM (State initState, Context* ctx, Callbacks callbacks)
		: _state (initState), _ctx (ctx), callbacks (std::move (callbacks)) {
		enter_state (initState);
	}
	BASED_CLASS_NO_COPY_DEFAULT_MOVE (FSM);
};

}
