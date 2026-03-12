#include "Basedlib/FSM.hpp"

#include <cstdio>
#include <print>

enum class States {
	ST_A,
	ST_B,
	ST_C,
	ST_D
};

enum class Events {
	EV_A, // States: A->B, B->C, C->D, D not permitted
	EV_B  // States: A->A (ignored), B->A, C->B, D->C
};

struct Context {
	size_t evA_count = 0;
	size_t evB_count = 0;
};

/* Note that callbacks won't have a Context argument if Context is nullptr_t (as by default).
 * If enums has > 32 items, the range should be increased in the Enum template arg (as in PrettyEnum).
 */
using FSM = Basedlib::FSM::FSM <Basedlib::FSM::Enum<States>, Basedlib::FSM::Enum<Events>, Context,
	[] (std::string_view s) { std::print ("FSM Logger: {}\n", s); }
>;
/* This also creates the following type aliases:
 * FSM::State = States
 * FSM::Event = Events
 * FSM::EventCallbackResult = std::optional<States>
 * FSM::StateCallback = void (Context*) or void ()
 * FSM::EventCallback = EventCallbackResult (FSM*, Context*) or EventCallbackResult (FSM*)
 * Also, the following const is declared:
 * FSM::EventNotPermitted = std::nullopt
 */

void print_callback (std::string_view str) { std::print ("Callback: {}\n", str); }

// Event callback as a separate function
FSM::EventCallbackResult fsm_ev_a (FSM* fsm, Context* ctx) {
	print_callback ("event EV_A");
	ctx->evA_count++;

	States newState = fsm->state();
	switch (fsm->state()) {
		case States::ST_A: newState = States::ST_B; break;
		case States::ST_B: newState = States::ST_C; break;
		case States::ST_C: newState = States::ST_D; break;
		case States::ST_D: default: return FSM::EventNotPermitted;
	}

	fsm->switch_state (newState);
	return newState;
}

// State callbacks as separate functions
void fsm_st_b_enter (Context *ctx) { print_callback ("enter ST_B"); }
void fsm_st_b_exit  (Context *ctx) { print_callback ("exit ST_B"); }

int main () {
	Context ctx {};

	FSM fsm (States::ST_A, &ctx, FSM::make_callbacks (
		// State callbacks as non-capturing lambdas
		FSM::state_cb <States::ST_A> ({
			.on_enter = [] (Context* ctx) { print_callback ("enter ST_A"); },
			.on_exit  = [] (Context* ctx) { print_callback ("exit ST_A"); }
		}),
		FSM::state_cb <States::ST_B> ({.on_enter = fsm_st_b_enter, .on_exit = fsm_st_b_exit}),
		// Callbacks can be safely omitted
		FSM::state_cb <States::ST_C> ({.on_enter = [] (Context* ctx) { print_callback ("enter ST_C"); }}),
		FSM::event_cb <Events::EV_A> ({.on_event = fsm_ev_a}),
		// Event callback as a non-capturing lambda
		FSM::event_cb <Events::EV_B> ({.on_event = [] (FSM* fsm, Context* ctx) -> FSM::EventCallbackResult {
			print_callback ("event EV_B");
			ctx->evB_count++;

			States newState = fsm->state();
			switch (fsm->state()) {
				case States::ST_B: newState = States::ST_A; break;
				case States::ST_C: newState = States::ST_B; break;
				case States::ST_D: newState = States::ST_C; break;
				default: break;
			}

			fsm->switch_state (newState);
			return newState;
		}})
	));

	auto trigger_event = [&fsm] (Events ev) {
		if (!fsm.event (ev))
			std::print ("event({}) returned false (= std::nullopt = FSM::EventNotPermitted)\n", Basedlib::PrettyEnum<Events>::to_string (ev));
	};

	std::print ("Enter 1 to trigger EV_A, 2 to trigger EV_B, any other input to stop\n");
	bool running = true;
	while (running) {
		int input = std::getchar();
		if (input == '\n') continue;
		switch (input - '0') {
			case 1: trigger_event (Events::EV_A); break;
			case 2: trigger_event (Events::EV_B); break;
			default: running = false; break;
		}
	}

	std::print ("Event count EV_A: {} EV_B: {}\n", ctx.evA_count, ctx.evB_count);

	return 0;
}
