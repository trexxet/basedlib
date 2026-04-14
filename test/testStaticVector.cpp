#include "Basedlib/StaticVector.hpp"
#include "Basedtest/Suite.hpp"

#include <concepts>
#include <format>
#include <vector>

#include "Basedlib/Class.hpp"

using namespace Basedtest;
using Basedlib::StaticVector;
using Basedlib::StaticVectorView;

constinit struct Counters {
	std::size_t ctor, dtor, copyCtor, moveCtor, copyAss, moveAss;
	void reset () {
		ctor = 0; dtor = 0; copyCtor = 0; moveCtor = 0; copyAss = 0; moveAss = 0;
	}
	std::size_t diff () const { return (ctor + copyCtor + moveCtor) - dtor; }
	std::string to_string () const {
		return std::format (
			"{{ctor = {} dtor = {} copyCtor = {} moveCtor = {} copyAss = {} moveAss = {}}}",
			ctor, dtor, copyCtor, moveCtor, copyAss, moveAss
		);
	}
	BASED_CLASS_DEFAULT_EQUALITY (Counters);
} counters {0};

struct Foo {
	int x;
	Foo () = delete;
	Foo (int v) : x (v) { counters.ctor++; }
	~Foo () noexcept { counters.dtor++; }
	BASED_CLASS_COPY_CONSTRUCTOR (Foo) { x = other.x; counters.copyCtor++; }
	BASED_CLASS_COPY_ASSIGNMENT (Foo) { x = other.x; counters.copyAss++; return *this; }
	BASED_CLASS_MOVE_CONSTRUCTOR (Foo) { x = other.x; counters.moveCtor++; }
	BASED_CLASS_MOVE_ASSIGNMENT (Foo) { x = other.x; counters.moveAss++; return *this; }
};

BT_SCENARIO_TEST (test_sv_create_empty) {
	StaticVector<int, 8> sv;
	BT_ASSERT_EQ (sv.capacity(), 8);
	BT_ASSERT_EQ (sv.size(), 0);
	BT_ASSERT (sv.empty());
	BT_ASSERT (!sv.full());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_create_ctad) {
	StaticVector sv {1, 2, 3};
	static_assert (std::same_as <decltype(sv), StaticVector<int, 3>>);
	BT_ASSERT_EQ (sv.capacity(), 3);
	BT_ASSERT_EQ (sv.size(), 3);
	BT_ASSERT (!sv.empty());
	BT_ASSERT (sv.full());
	for (int i : {1, 2, 3})
		BT_ASSERT_EQ (sv[i - 1], i);
	BT_ASSERT_EQ (sv.back(), 3);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_clear) {
	StaticVector sv {1, 2, 3};
	sv.clear();
	BT_ASSERT_EQ (sv.capacity(), 3);
	BT_ASSERT_EQ (sv.size(), 0);
	BT_ASSERT (sv.empty());
	BT_ASSERT (!sv.full());
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_modify_step) {
	counters.reset();
	StaticVector<Foo, 8> sv;
	BT_ASSERT_EQ (counters, Counters {0});

	Foo foo1 {1};
	counters.reset();
	sv.emplace_back (foo1);
	BT_ASSERT_EQ (counters, (Counters {0, 0, 1, 0, 0, 0}));
	BT_ASSERT_EQ (sv.size(), 1);
	counters.reset();

	sv.emplace_back (Foo {2});
	BT_ASSERT_EQ (counters, (Counters {1, 1, 0, 1, 0, 0}));
	BT_ASSERT_EQ (sv.size(), 2);
	counters.reset();

	sv.pop_back();
	BT_ASSERT_EQ (counters, (Counters {0, 1, 0, 0, 0, 0}));
	BT_ASSERT_EQ (sv.size(), 1);
	counters.reset();

	sv.pop_back();
	BT_ASSERT_EQ (counters, (Counters {0, 1, 0, 0, 0, 0}));
	BT_ASSERT_EQ (sv.size(), 0);
	counters.reset();

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_modify_sum) {
	counters.reset();
	{
		Foo foo1 {1};
		StaticVector<Foo, 8> sv;
		sv.emplace_back (foo1);
		sv.emplace_back (Foo {2});
	}
	BT_ASSERT_EQ (counters.diff(), 0);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_copy_ctor) {
	StaticVector<Foo, 8> src;
	src.emplace_back (1);
	src.emplace_back (2);
	counters.reset();

	StaticVector<Foo, 8> dst (src);
	BT_ASSERT_EQ (dst.size(), 2);
	BT_ASSERT_EQ (dst[0].x, 1);
	BT_ASSERT_EQ (dst[1].x, 2);

	BT_ASSERT_EQ (src.size(), 2);
	BT_ASSERT_EQ (src[0].x, 1);
	BT_ASSERT_EQ (src[1].x, 2);

	BT_ASSERT_EQ (counters, (Counters {0, 0, 2, 0, 0, 0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_move_ctor) {
	StaticVector<Foo, 8> src;
	src.emplace_back (1);
	src.emplace_back (2);
	counters.reset();

	StaticVector<Foo, 8> dst (std::move(src));
	BT_ASSERT_EQ (dst.size(), 2);
	BT_ASSERT_EQ (dst[0].x, 1);
	BT_ASSERT_EQ (dst[1].x, 2);

	BT_ASSERT (src.empty());

	BT_ASSERT_EQ (counters, (Counters {0, 2, 0, 2, 0, 0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_copy_ass) {
	StaticVector<Foo, 8> src, dst;
	src.emplace_back (1);
	src.emplace_back (2);
	dst.emplace_back (8);
	dst.emplace_back (9);
	counters.reset();
	dst = src;

	BT_ASSERT_EQ (dst.size(), 2);
	BT_ASSERT_EQ (dst[0].x, 1);
	BT_ASSERT_EQ (dst[1].x, 2);

	BT_ASSERT_EQ (src.size(), 2);
	BT_ASSERT_EQ (src[0].x, 1);
	BT_ASSERT_EQ (src[1].x, 2);

	BT_ASSERT_EQ (counters, (Counters {0, 2, 2, 0, 0, 0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_move_ass) {
	StaticVector<Foo, 8> src, dst;
	src.emplace_back (1);
	src.emplace_back (2);
	dst.emplace_back (8);
	dst.emplace_back (9);
	counters.reset();
	dst = std::move (src);

	BT_ASSERT_EQ (dst.size(), 2);
	BT_ASSERT_EQ (dst[0].x, 1);
	BT_ASSERT_EQ (dst[1].x, 2);

	BT_ASSERT (src.empty());

	BT_ASSERT_EQ (counters, (Counters {0, 4, 0, 2, 0, 0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_copy_self) {
	StaticVector<Foo, 8> src;
	src.emplace_back (1);
	src.emplace_back (2);
	counters.reset();
	src = src;

	BT_ASSERT_EQ (src.size(), 2);
	BT_ASSERT_EQ (src[0].x, 1);
	BT_ASSERT_EQ (src[1].x, 2);

	BT_ASSERT_EQ (counters, (Counters {0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_move_self) {
	StaticVector<Foo, 8> src;
	src.emplace_back (1);
	src.emplace_back (2);
	counters.reset();
	src = std::move (src);

	BT_ASSERT_EQ (src.size(), 2);
	BT_ASSERT_EQ (src[0].x, 1);
	BT_ASSERT_EQ (src[1].x, 2);

	BT_ASSERT_EQ (counters, (Counters {0}));

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_from_vector) {
	std::vector<int> vec {1, 2, 3};

	StaticVector<int, 2> sv2 (vec);
	BT_ASSERT_EQ (sv2.size(), 2);
	BT_ASSERT_EQ (sv2[0], 1);
	BT_ASSERT_EQ (sv2[1], 2);

	StaticVector<int, 4> sv4 (vec);
	BT_ASSERT_EQ (sv4.size(), 3);
	BT_ASSERT_EQ (sv4[0], 1);
	BT_ASSERT_EQ (sv4[1], 2);
	BT_ASSERT_EQ (sv4[2], 3);

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_equal) {
	StaticVector<int, 2> sv1 {1, 2};
	StaticVector<int, 2> sv2 {1, 3};
	StaticVector<int, 4> sv3 {1, 2, 3};
	StaticVector<int, 4> sv4 {1, 2};

	BT_ASSERT_NE (sv1, sv2);
	BT_ASSERT_NE (sv1, sv3);
	BT_ASSERT_EQ (sv1, sv4);
	BT_ASSERT_NE (sv3, sv4);

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_sv_view) {
	StaticVector<int, 4> sv4 {1, 2, 3};
	StaticVectorView<int> svView = sv4.view();

	BT_ASSERT_EQ (sv4.size(), svView.size());
	BT_ASSERT_EQ (sv4[0], svView[0]);
	BT_ASSERT_EQ (sv4[1], svView[1]);
	BT_ASSERT_EQ (sv4[2], svView[2]);

	StaticVectorView<int> svViewImplicit = sv4;
	BT_ASSERT_EQ (svView, svViewImplicit);

	StaticVector<int, 8> sv8;
	sv8.emplace_back (1);
	sv8.emplace_back (2);
	sv8.emplace_back (3);
	BT_ASSERT_EQ (sv4.view(), sv8.view());

	BT_SUCCESS;
}

consteval int test_sv_consteval () {
	StaticVector<int, 4> sv;
	sv.emplace_back (1);
	sv.emplace_back (2);
	return sv[0] + sv.back();
}

consteval bool test_sv_consteval_view () {
	StaticVector<int, 4> sv4 {1, 2, 3};
	StaticVector<int, 8> sv8 {1, 2, 3};
	return sv4.view() == sv8.view();
}

static_assert (std::default_initializable <StaticVector<Foo, 4>>);
static_assert (std::copy_constructible <StaticVector<Foo, 4>>);
static_assert (std::move_constructible <StaticVector<Foo, 4>>);
static_assert (std::equality_comparable_with <
	StaticVector<int, 8>,
	StaticVector<int, 4>&
>);
static_assert (std::equality_comparable <StaticVectorView<int>>);

static_assert (test_sv_consteval() == 3);
static_assert (test_sv_consteval_view());

int main () {
	return Suite ("StaticVector", tests (
		BT_SUITE_SCENARIO (test_sv_create_empty),
		BT_SUITE_SCENARIO (test_sv_create_ctad),
		BT_SUITE_SCENARIO (test_sv_clear),
		BT_SUITE_SCENARIO (test_sv_modify_step),
		BT_SUITE_SCENARIO (test_sv_modify_sum),
		BT_SUITE_SCENARIO (test_sv_copy_ctor),
		BT_SUITE_SCENARIO (test_sv_move_ctor),
		BT_SUITE_SCENARIO (test_sv_copy_ass),
		BT_SUITE_SCENARIO (test_sv_move_ass),
		BT_SUITE_SCENARIO (test_sv_copy_self),
		BT_SUITE_SCENARIO (test_sv_move_self),
		BT_SUITE_SCENARIO (test_sv_from_vector),
		BT_SUITE_SCENARIO (test_sv_equal),
		BT_SUITE_SCENARIO (test_sv_view)
	)).run_rc();	
}
