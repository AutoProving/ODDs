#include <gtest/gtest.h>
#include <Common/DivisionODDs.h>

namespace {

bool divsBy(int n, int m) {
    return n % m == 0;
}

auto divsBy2 = std::bind(divsBy, std::placeholders::_1, 2);
auto divsBy3 = std::bind(divsBy, std::placeholders::_1, 3);
auto divsBy6 = std::bind(divsBy, std::placeholders::_1, 6);

}

TEST(DivisionODDsTest, testDiv2) {
    ODDs::ODD odd = TestCommon::div2(10);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy2));
    ASSERT_FALSE(TestCommon::checkPredicate(odd, divsBy6));
}

TEST(DivisionODDsTest, testDiv2_1) {
    ODDs::ODD odd = TestCommon::div2(1);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy2));
}

TEST(DivisionOddsTest, testDiv2NM) {
    ODDs::ODD odd = TestCommon::div2NM(10);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy2));
}

TEST(DivisionOddsTest, testDiv2NM_1) {
    ODDs::ODD odd = TestCommon::div2NM(1);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy2));
}

TEST(DivisionODDsTest, testDiv3) {
    ODDs::ODD odd = TestCommon::div3(10);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy3));
}

TEST(DivisionODDsTest, testDiv3_1) {
    ODDs::ODD odd = TestCommon::div3(1);
    ASSERT_TRUE(TestCommon::checkPredicate(odd, divsBy3));
}
