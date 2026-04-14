#include "Basedlib/RingBuffer.hpp"
#include "Basedtest/Suite.hpp"

constexpr std::size_t capacity = 3;

BT_SCENARIO_TEST (test_rb_create) {
	Basedlib::RingBuffer<int> rb (capacity);

	BT_ASSERT_EQ (rb.capacity(), capacity);
	BT_ASSERT_EQ (rb.size(), 0);
	BT_ASSERT (!rb.full());
	BT_ASSERT (rb.empty());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_rb_append_no_wrap) {
	Basedlib::RingBuffer<int> rb (capacity);
	rb.push (1);
	rb.push (2);

	BT_ASSERT_EQ (rb.size(), 2);
	BT_ASSERT (!rb.full());
	BT_ASSERT (!rb.empty());
	BT_ASSERT_EQ (rb[0], 1);
	BT_ASSERT_EQ (rb[1], 2);
	BT_ASSERT_EQ (rb.back(), rb[1]);
	BT_ASSERT_EQ (rb.front(), rb[0]);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_rb_append_cap) {
	Basedlib::RingBuffer<int> rb (capacity);
	rb.push (1);
	rb.push (2);
	rb.push (3);

	BT_ASSERT_EQ (rb.size(), capacity);
	BT_ASSERT (rb.full());
	BT_ASSERT (!rb.empty());
	BT_ASSERT_EQ (rb[0], 1);
	BT_ASSERT_EQ (rb[1], 2);
	BT_ASSERT_EQ (rb[2], 3);
	BT_ASSERT_EQ (rb.back(), rb[2]);
	BT_ASSERT_EQ (rb.front(), rb[0]);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_rb_append_wrap) {
	Basedlib::RingBuffer<int> rb (capacity);
	rb.push (1);
	rb.push (2);
	rb.push (3);
	rb.push (4);

	BT_ASSERT_EQ (rb.size(), capacity);
	BT_ASSERT (rb.full());
	BT_ASSERT (!rb.empty());
	BT_ASSERT_EQ (rb[0], 2);
	BT_ASSERT_EQ (rb[1], 3);
	BT_ASSERT_EQ (rb[2], 4);
	BT_ASSERT_EQ (rb.back(), rb[2]);
	BT_ASSERT_EQ (rb.front(), rb[0]);
	BT_SUCCESS;
}

int main () {
	return Basedtest::Suite ("RingBuffer", tests (
		BT_SUITE_SCENARIO (test_rb_create),
		BT_SUITE_SCENARIO (test_rb_append_no_wrap),
		BT_SUITE_SCENARIO (test_rb_append_cap),
		BT_SUITE_SCENARIO (test_rb_append_wrap)
	)).run_rc();
}
