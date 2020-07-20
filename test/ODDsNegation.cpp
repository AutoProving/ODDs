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

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

#include <Common/DivisionODDs.h>

#include <filesystem>
#include <string>

namespace {

void doTest(std::string argDesc, std::string expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramNegation(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doDiskTest(std::string argDesc, std::string expectedDesc) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD result = ODDs::diagramNegation(arg, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

}

class ODDsNegationTest : public ::testing::Test {
protected:
    static std::string trivialArgDesc;
    static std::string trivialExpectedDesc;
};

// 
std::string ODDsNegationTest::trivialArgDesc = R"(
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
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 1, "symbol": "a", "to": 1},
        {"from": 1, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [1]
})";

//
std::string ODDsNegationTest::trivialExpectedDesc = R"(
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
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 1, "symbol": "a", "to": 1},
        {"from": 1, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0]
})";

TEST_F(ODDsNegationTest, trivial) {
    doTest(trivialArgDesc, trivialExpectedDesc);
}


TEST_F(ODDsNegationTest, trivialDisk) {
    doDiskTest(trivialArgDesc, trivialExpectedDesc);
}


TEST_F(ODDsNegationTest, div2) {
    auto pred = [](int n) -> bool {
        return n % 2 == 1;
    };
    ODDs::ODD odd = TestCommon::div2(10);
    ASSERT_TRUE(TestCommon::checkPredicate(~odd, pred));
}

TEST_F(ODDsNegationTest, div3) {
    auto pred = [](int n) -> bool {
        return n % 3 != 0;
    };
    ODDs::ODD odd = TestCommon::div3(10);
    ASSERT_TRUE(TestCommon::checkPredicate(~odd, pred));
}

TEST_F(ODDsNegationTest, threeModuloSix) {
    auto pred = [](int n) -> bool {
        return n % 6 == 3;
    };
    ODDs::ODD div2 = TestCommon::div2(10);
    ODDs::ODD div3 = TestCommon::div3(10);
    ASSERT_TRUE(TestCommon::checkPredicate(div3 & ~div2, pred));
}
