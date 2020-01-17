#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

#include <Common/DivisionODDs.h>

class ODDsPowerSetTest : public ::testing::Test {
protected:
    static std::string trivialArgDesc;
    static std::string trivialExpectedDesc;
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
        {"from": 0, "symbol": "b", "to": 1},
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
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [1]
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

#if false
TEST_F(ODDsPowerSetTest, trivial) {
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::diagramPowerSet(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, detComplete) {
    ODDs::ODD odd = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::diagramPowerSet(odd);
    ASSERT_EQ(ODDs::writeJSON(odd), ODDs::writeJSON(actual));
}

TEST_F(ODDsPowerSetTest, incomplete) {
    ODDs::ODD odd = ODDs::readJSON(incompleteDesc);
    ODDs::ODD actual = ODDs::diagramPowerSet(odd);
    ASSERT_TRUE(isComplete(actual));
}
#endif
