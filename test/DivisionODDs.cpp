// Copyright (c) 2019-2020 Mateus de Oliveira Oliveira and Contributors. 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
