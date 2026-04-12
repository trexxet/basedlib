#include "Basedlib/DebounceTimer.hpp"
#include "Basedtest/Suite.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace Basedtest;
using Basedlib::DebounceTimer;

BT_SCENARIO_TEST (test_debounce_init) {
	DebounceTimer t (100ms);
	BT_ASSERT (t.ready());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_debounce_trigger_immediate) {
	DebounceTimer t (100ms);
	t.trigger();
	BT_ASSERT (!t.ready());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_debounce_trigger_wait) {
	DebounceTimer t (10ms);
	t.trigger();
	std::this_thread::sleep_for (30ms);
	BT_ASSERT (t.ready());
	BT_SUCCESS;
}

int main () {
	return Suite ("DebounceTimer", tests (
		BT_SUITE_SCENARIO (test_debounce_init),
		BT_SUITE_SCENARIO (test_debounce_trigger_immediate),
		BT_SUITE_SCENARIO (test_debounce_trigger_wait)
	)).run_rc();
}
