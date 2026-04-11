#include "Basedlib/FSM.hpp"
#include "Basedtest/Suite.hpp"

#include <format>
#include <string>
#include <string_view>

#include "Basedlib/Class.hpp"
#include "Basedlib/PrettyEnum.hpp"

enum class States { ST_A, ST_B, ST_C, ST_D };
std::string_view to_string (States st) { return Basedlib::FSM::Enum<States>::to_string (st); }

enum class Events { EV_A, EV_B };

struct Counters {
	size_t evA_count = 0;
	size_t evB_count = 0;

	size_t stA_enter = 0;
	size_t stA_exit = 0;
	size_t stB_enter = 0;
	size_t stB_exit = 0;
	size_t stC_enter = 0;

	BASED_CLASS_DEFAULT_EQUALITY (Counters);

	std::string to_string() const {
		return std::format (
			"{{evA_count: {}, evB_count: {}, stA_enter: {}, stA_exit: {}, stB_enter: {}, stB_exit: {}, stC_enter: {}}}",
			evA_count, evB_count, stA_enter, stA_exit, stB_enter, stB_exit, stC_enter
		);
	}
};

using FSM = Basedlib::FSM::FSM <Basedlib::FSM::Enum<States>, Basedlib::FSM::Enum<Events>, Counters>;

FSM::EventCallbackResult fsm_ev_a (FSM* fsm, Counters* ctx) {
	ctx->evA_count++;

	States state = fsm->state();
	switch (fsm->state()) {
		case States::ST_A: state = States::ST_B; break;
		case States::ST_B: state = States::ST_C; break;
		case States::ST_C: state = States::ST_D; break;
		case States::ST_D: default: return FSM::EventNotPermitted;
	}

	return fsm->switch_state (state);
}

void fsm_st_b_enter (Counters *ctx) { ctx->stB_enter++; }
void fsm_st_b_exit  (Counters *ctx) { ctx->stB_exit++; }

Counters ctx {};

FSM fsm (States::ST_A, &ctx, FSM::make_callbacks (
	FSM::state_cb <States::ST_A> ({
		.on_enter = [] (Counters* ctx) { ctx->stA_enter++; },
		.on_exit  = [] (Counters* ctx) { ctx->stA_exit++; }
	}),
	FSM::state_cb <States::ST_B> ({.on_enter = fsm_st_b_enter, .on_exit = fsm_st_b_exit}),
	FSM::state_cb <States::ST_C> ({.on_enter = [] (Counters* ctx) { ctx->stC_enter++; }}),
	FSM::event_cb <Events::EV_A> (fsm_ev_a),
	FSM::event_cb <Events::EV_B> ([] (FSM* fsm, Counters* ctx) -> FSM::EventCallbackResult {
		ctx->evB_count++;

		States state = fsm->state();
		switch (fsm->state()) {
			case States::ST_B: state = States::ST_A; break;
			case States::ST_C: state = States::ST_B; break;
			case States::ST_D: state = States::ST_C; break;
			default: break;
		}

		return fsm->switch_state (state);
	})
));

void reset_fsm (States state) {
	fsm.switch_state (state);
	ctx = {0};
}

BT_SCENARIO_TEST (test_fsm_created) {
	BT_ASSERT_EQ (fsm.state(), States::ST_A);
	BT_ASSERT (fsm.ctx() == &ctx);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ev_a) {
	reset_fsm (States::ST_A);
	BT_ASSERT_EQ (fsm.state(), States::ST_A);

	BT_ASSERT_EQ (fsm.event (Events::EV_A), States::ST_B);
	BT_ASSERT_EQ (fsm.event (Events::EV_A), States::ST_C);
	BT_ASSERT_EQ (fsm.event (Events::EV_A), States::ST_D);

	BT_ASSERT (fsm.event (Events::EV_A) == FSM::EventNotPermitted);
	BT_ASSERT_EQ (fsm.state(), States::ST_D);

	Counters expected = {
		.evA_count = 4, .evB_count = 0,
		.stA_enter = 0, .stA_exit = 1,
		.stB_enter = 1, .stB_exit = 1,
		.stC_enter = 1
	};
	BT_ASSERT_EQ (ctx, expected);

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_ev_b) {
	reset_fsm (States::ST_D);
	BT_ASSERT_EQ (fsm.state(), States::ST_D);

	BT_ASSERT_EQ (fsm.event (Events::EV_B), States::ST_C);
	BT_ASSERT_EQ (fsm.event (Events::EV_B), States::ST_B);
	BT_ASSERT_EQ (fsm.event (Events::EV_B), States::ST_A);
	BT_ASSERT_EQ (fsm.event (Events::EV_B), States::ST_A);

	Counters expected = {
		.evA_count = 0, .evB_count = 4,
		.stA_enter = 1, .stA_exit = 0,
		.stB_enter = 1, .stB_exit = 1,
		.stC_enter = 1
	};
	BT_ASSERT_EQ (ctx, expected);

	BT_SUCCESS;
}

int main () {
	return Basedtest::Suite ("testFSMSuite", tests (
		BT_SUITE_SCENARIO (test_fsm_created),
		BT_SUITE_SCENARIO (test_ev_a),
		BT_SUITE_SCENARIO (test_ev_b)
	)).run_rc();
}
