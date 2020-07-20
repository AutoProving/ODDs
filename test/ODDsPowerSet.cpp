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

class ODDsPowerSetTest : public ::testing::Test {
protected:
    static std::string trivialArgDesc;
    static std::string trivialExpectedDesc;
    static std::string trivialNLExpectedDesc;
    static std::string initialDesc;
    static std::string leftDesc;
    static std::string incompleteDesc;
};

std::string ODDsPowerSetTest::trivialArgDesc = R"(
{
  "leftLayerStates": 3,
  "initialStates": [0, 2],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 1, "symbol": "a", "to": 2},
        {"from": 1, "symbol": "b", "to": 1},
        {"from": 2, "symbol": "b", "to": 2},
        {"from": 2, "symbol": "a", "to": 1},
        {"from": 2, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 3
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 1, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 0},
        {"from": 2, "symbol": "a", "to": 0},
        {"from": 2, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [1]
})";

std::string ODDsPowerSetTest::trivialExpectedDesc = R"(
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
        {"from": 1, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0]
})";

std::string ODDsPowerSetTest::trivialNLExpectedDesc = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 3},
        {"from": 0, "symbol": "b", "to": 5}
      ],
      "rightLayerStates": 8
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "a", "to": 3},
        {"from": 1, "symbol": "b", "to": 1},
        {"from": 2, "symbol": "a", "to": 1},
        {"from": 2, "symbol": "b", "to": 1},
        {"from": 3, "symbol": "a", "to": 3},
        {"from": 3, "symbol": "b", "to": 1},
        {"from": 4, "symbol": "a", "to": 1},
        {"from": 4, "symbol": "b", "to": 1},
        {"from": 5, "symbol": "a", "to": 3},
        {"from": 5, "symbol": "b", "to": 1},
        {"from": 6, "symbol": "a", "to": 1},
        {"from": 6, "symbol": "b", "to": 1},
        {"from": 7, "symbol": "a", "to": 3},
        {"from": 7, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 4
    }
  ],
  "finalStates": [2, 3]
})";

std::string ODDsPowerSetTest::initialDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0, 1],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

std::string ODDsPowerSetTest::leftDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

std::string ODDsPowerSetTest::incompleteDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0, 1],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

TEST_F(ODDsPowerSetTest, isDeterministicTrue) {
    ODDs::ODD odd = ODDs::readJSON(trivialExpectedDesc);
    ASSERT_TRUE(ODDs::isDeterministic(odd));
}

TEST_F(ODDsPowerSetTest, isDeterministicFalse) {
    ODDs::ODD odd = ODDs::readJSON(trivialArgDesc);
    ASSERT_FALSE(ODDs::isDeterministic(odd));
}

TEST_F(ODDsPowerSetTest, isDeterministicDiv3) {
    ASSERT_FALSE(ODDs::isDeterministic(TestCommon::div3(10)));
}

TEST_F(ODDsPowerSetTest, isDeterministicInitial) {
    ODDs::ODD odd = ODDs::readJSON(initialDesc);
    ASSERT_FALSE(ODDs::isDeterministic(odd));
}

TEST_F(ODDsPowerSetTest, isDeterministicLeft) {
    ODDs::ODD odd = ODDs::readJSON(leftDesc);
    ASSERT_FALSE(ODDs::isDeterministic(odd));
}

TEST_F(ODDsPowerSetTest, isCompleteTrue) {
    ODDs::ODD odd = ODDs::readJSON(trivialExpectedDesc);
    ASSERT_TRUE(ODDs::isComplete(odd));
}

TEST_F(ODDsPowerSetTest, isCompleteFalse) {
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    ASSERT_FALSE(ODDs::isComplete(odd));
}

TEST_F(ODDsPowerSetTest, isCompleteDiv2) {
    ASSERT_TRUE(ODDs::isComplete(TestCommon::div2(10)));
}

TEST_F(ODDsPowerSetTest, trivial) {
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialNLExpectedDesc);
    ODDs::ODD actual = ODDs::diagramPowerSet(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, trivialDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialNLExpectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::diagramPowerSet(arg, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, incomplete) {
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    ODDs::ODD actual = ODDs::diagramPowerSet(odd);
    ASSERT_TRUE(isComplete(actual));
}

TEST_F(ODDsPowerSetTest, incompleteDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::diagramPowerSet(odd, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_TRUE(isComplete(actual));
}

TEST_F(ODDsPowerSetTest, lazyTrivial) {
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, lazyTrivialDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(arg, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, lazyDetComplete) {
    ODDs::ODD odd = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(odd);
    ASSERT_EQ(ODDs::writeJSON(odd), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, lazyDetCompleteDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD odd = ODDs::readJSON(trivialExpectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(odd, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(odd), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, lazyIncomplete) {
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(odd);
    ASSERT_TRUE(isComplete(actual));
}

TEST_F(ODDsPowerSetTest, lazyIncompleteDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::diagramLazyPowerSet(odd, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_TRUE(isComplete(actual));
}
