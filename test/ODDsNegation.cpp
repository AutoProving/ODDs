#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

#include <Common/DivisionODDs.h>

#include <string>

namespace {

void doTest(std::string argDesc, std::string expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramNegation(arg);
    ASSERT_EQ(ODDs::JSONDump(expected), ODDs::JSONDump(result));
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
