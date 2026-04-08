#include "Basedlib/RingBuffer.hpp"
#include "Basedtest/Suite.hpp"

const size_t capacity = 3;

BT_SCENARIO_TEST (test_rb_create) {
	Basedlib::RingBuffer<int> rb (capacity);

	BT_ASSERT (rb.capacity() == capacity);
	BT_ASSERT (rb.size() == 0);
	BT_ASSERT (!rb.full());
	BT_ASSERT (rb.empty());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_rb_append_no_wrap) {
	Basedlib::RingBuffer<int> rb (capacity);
	rb.push (1);
	rb.push (2);

	BT_ASSERT (rb.size() == 2);
	BT_ASSERT (!rb.full());
	BT_ASSERT (!rb.empty());
	BT_ASSERT (rb[0] == 1);
	BT_ASSERT (rb[1] == 2 && rb.back() == rb[1]);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_rb_append_wrap) {
	Basedlib::RingBuffer<int> rb (capacity);
	rb.push (1);
	rb.push (2);
	rb.push (3);
	rb.push (4);

	BT_ASSERT (rb.size() == capacity);
	BT_ASSERT (rb.full());
	BT_ASSERT (!rb.empty());
	BT_ASSERT (rb[0] == 2);
	BT_ASSERT (rb[1] == 3);
	BT_ASSERT (rb[2] == 4 && rb.back() == rb[2]);
	BT_SUCCESS;
}

int main () {
	return Basedtest::Suite ("testRingbufferSuite", tests (
		BT_SUITE_SCENARIO (test_rb_create),
		BT_SUITE_SCENARIO (test_rb_append_no_wrap),
		BT_SUITE_SCENARIO (test_rb_append_wrap)
	)).run().rc();
}
