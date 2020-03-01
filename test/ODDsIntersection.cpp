// Copyright (c) 2019-2020 Vasily Alferov, Mateus de Oliveira Oliveira and Contributors. 
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

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

namespace {

void doTest(std::string lhsDesc,
            std::string rhsDesc,
            std::string expectedDesc) {
    ODDs::ODD lhs = ODDs::readJSON(lhsDesc);
    ODDs::ODD rhs = ODDs::readJSON(rhsDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramIntersection(lhs, rhs);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

}

class ODDsIntersectionTest : public ::testing::Test {
protected:
    static std::string trivialLhsDesc;
    static std::string trivialRhsDesc;
    static std::string trivialExpectedDesc;
};

// {abd,abe,acd,ace}
std::string ODDsIntersectionTest::trivialLhsDesc = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["b", "c"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "c", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["d", "e"],
      "transitions": [
        {"from": 0, "symbol": "d", "to": 0},
        {"from": 0, "symbol": "e", "to": 1},
        {"from": 1, "symbol": "d", "to": 0},
        {"from": 1, "symbol": "e", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

// {abd,acd,bbd,bcd}
std::string ODDsIntersectionTest::trivialRhsDesc = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["b", "c"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "c", "to": 1},
        {"from": 1, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "c", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["d"],
      "transitions": [
        {"from": 0, "symbol": "d", "to": 0},
        {"from": 1, "symbol": "d", "to": 0}
      ],
      "rightLayerStates": 1
    }
  ],
  "finalStates": [0]
})";

// {abd,acd}, not even all states are reachable from initial
std::string ODDsIntersectionTest::trivialExpectedDesc = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["b", "c"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "c", "to": 3},
        {"from": 1, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "c", "to": 3}
      ],
      "rightLayerStates": 4
    },
    {
      "alphabet": ["d"],
      "transitions": [
        {"from": 0, "symbol": "d", "to": 0},
        {"from": 1, "symbol": "d", "to": 0},
        {"from": 2, "symbol": "d", "to": 0},
        {"from": 3, "symbol": "d", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

TEST_F(ODDsIntersectionTest, trivial) {
    doTest(trivialLhsDesc, trivialRhsDesc, trivialExpectedDesc);
}

TEST_F(ODDsIntersectionTest, div2div3) {
    ODDs::ODD odd = TestCommon::div2(10) & TestCommon::div3(10);
    auto pred = [](int n) -> bool {
        return n % 6 == 0;
    };
    ASSERT_TRUE(TestCommon::checkPredicate(odd, pred));
}
