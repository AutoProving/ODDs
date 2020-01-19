#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/JSONDump.h>
#include <ODDs/Operations.h>

#include <Common/DivisionODDs.h>

class ODDsMinimizationTest : public ::testing::Test {
protected:
    static std::string trivialArgDesc;
    static std::string trivialExpectedDesc;
    static std::string everyAccepted;
};

std::string ODDsMinimizationTest::trivialArgDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 0, "symbol": "a", "to": 2},
        {"from": 1, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 3
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 1, "symbol": "a", "to": 2},
        {"from": 1, "symbol": "b", "to": 3},
        {"from": 2, "symbol": "a", "to": 0},
        {"from": 2, "symbol": "a", "to": 2},
        {"from": 2, "symbol": "b", "to": 1},
        {"from": 2, "symbol": "b", "to": 3}
      ],
      "rightLayerStates": 4
    }
  ],
  "finalStates": [0, 2]
})";

std::string ODDsMinimizationTest::trivialExpectedDesc = R"(
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
        {"from": 1, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [1]
})";

std::string ODDsMinimizationTest::everyAccepted = R"(
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
      "rightLayerStates": 3
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1},
        {"from": 1, "symbol": "a", "to": 2},
        {"from": 1, "symbol": "b", "to": 3}
      ],
      "rightLayerStates": 5
    }
  ],
  "finalStates": [0, 1, 2, 3]
})";

TEST_F(ODDsMinimizationTest, trivial) {
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::minimize(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsMinimizationTest, everyAccepted) {
    ODDs::ODD arg = ODDs::readJSON(everyAccepted);
    ODDs::ODD odd = ODDs::minimize(arg);
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(1, odd.getLayer(i).width());
}

TEST_F(ODDsMinimizationTest, div3) {
    ASSERT_TRUE(
        ODDs::isDeterministic(
            ODDs::minimize(
                TestCommon::div3(10)
            )
        )
    );
}

TEST_F(ODDsMinimizationTest, div6) {
    using namespace TestCommon;
    ODDs::ODD odd = ODDs::minimize(div2NM(10) & (div3(10) | div2NM(10)));
    auto pred = [](int n) -> bool {
        return n % 6 == 0;
    };
    EXPECT_TRUE(checkPredicate(odd, pred));
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(6, odd.getLayer(i).width());
}

TEST_F(ODDsMinimizationTest, div6DoubleDiv2) {
    using namespace TestCommon;
    ODDs::ODD odd = ODDs::minimize(div2NM(10) & div2NM(10) & div3(10));
    auto pred = [](int n) -> bool {
        return n % 6 == 0;
    };
    EXPECT_TRUE(checkPredicate(odd, pred));
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(6, odd.getLayer(i).width());
}
