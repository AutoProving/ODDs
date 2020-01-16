#include <gtest/gtest.h>

#include <Common/DivisionODDs.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

namespace {

void doTest(std::function<std::string(std::string)> g,
            std::string argDesc,
            std::string expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramMapping(g, arg);
    ASSERT_EQ(ODDs::JSONDump(expected), ODDs::JSONDump(result));
}

}

class ODDsMappingTest : public ::testing::Test {
protected:
    static std::string trivialArg;
    static std::string trivialExpected;
};

namespace {

std::string swapMap(std::string s) {
    if (s == "a")
        return "b";
    return "a";
}

}

// {aba, abb, abc}
std::string ODDsMappingTest::trivialArg = R"(
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
      "alphabet": ["b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["b", "a", "c"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 0, "symbol": "c", "to": 2}
      ],
      "rightLayerStates": 3
    }
  ],
  "finalStates": [0, 1, 2]
})";

// {bab, baa}
std::string ODDsMappingTest::trivialExpected = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["a"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 0, "symbol": "a", "to": 2}
      ],
      "rightLayerStates": 3
    }
  ],
  "finalStates": [0, 1, 2]
})";

TEST_F(ODDsMappingTest, trivial) {
    doTest(swapMap, trivialArg, trivialExpected);
}

TEST_F(ODDsMappingTest, div3) {
    auto pred = [](int n) -> bool {
        int m = ~n & ~1023;
        return m % 3 == 0;
    };
    ODDs::ODD odd = TestCommon::div3(10);
    ASSERT_TRUE(TestCommon::checkPredicate(diagramMapping(swapMap, odd), pred));
}
