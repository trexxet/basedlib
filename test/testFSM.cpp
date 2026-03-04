#include "Basedlib/FSM.hpp"

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
using FSM = Basedlib::FSM::FSM <Basedlib::FSM::Enum<States>, Basedlib::FSM::Enum<Events>, Context>;

// Event callback as a separate function
FSM::EventCallbackResult fsm_ev_a (FSM* fsm, Context* ctx) {
	std::print ("EV_A\n");
	ctx->evA_count++;

	States newState = fsm->state;
	switch (fsm->state) {
		case States::ST_A: newState = States::ST_B; break;
		case States::ST_B: newState = States::ST_C; break;
		case States::ST_C: newState = States::ST_D; break;
		case States::ST_D: default: return std::nullopt;
	}

	fsm->switch_state (newState);
	return newState;
}

// State callbacks as separate functions
void fsm_st_b_enter (Context *ctx) { std::print ("Enter ST_B\n"); }
void fsm_st_b_exit (Context *ctx) { std::print ("Exit ST_B\n"); }

int main () {
	Context ctx {};

	FSM fsm = FSM::make (States::ST_A, &ctx,
		// State callbacks as non-capturing lambdas
		FSM::state_cb <States::ST_A> ({
			.on_enter = [](Context* ctx) { std::print ("Enter ST_A\n"); },
			.on_exit = [](Context* ctx) { std::print ("Exit ST_A\n"); }
		}),
		FSM::state_cb <States::ST_B> ({.on_enter = fsm_st_b_enter, .on_exit = fsm_st_b_exit}),
		// Callbacks can be safely omitted
		FSM::state_cb <States::ST_C> ({.on_enter = [](Context* ctx) { std::print ("Enter ST_C\n"); }}),
		FSM::event_cb <Events::EV_A> ({.on_event = fsm_ev_a}),
		// Event callback as a non-capturing lambda
		FSM::event_cb <Events::EV_B> ({.on_event = [](FSM* fsm, Context* ctx) -> FSM::EventCallbackResult {
			std::print ("EV_B\n");
			ctx->evB_count++;

			States newState = fsm->state;
			switch (fsm->state) {
				case States::ST_B: newState = States::ST_A; break;
				case States::ST_C: newState = States::ST_B; break;
				case States::ST_D: newState = States::ST_C; break;
				default: break;
			}

			fsm->switch_state (newState);
			return newState;
		}})
	);

	fsm.event (Events::EV_A);
	fsm.event (Events::EV_A);
	fsm.event (Events::EV_A);
	if (!fsm.event (Events::EV_A))
		std::print ("EV_A not permitted in ST_D!\n");
	fsm.event (Events::EV_B);
	fsm.event (Events::EV_B);
	fsm.event (Events::EV_B);
	fsm.event (Events::EV_B);

	std::print ("Event count EV_A: {} EV_B: {}\n", ctx.evA_count, ctx.evB_count);

	return 0;
}
