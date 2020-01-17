#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

#include <Common/DivisionODDs.h>

#include <string>

namespace {

void doTest(std::string lhsDesc,
            std::string rhsDesc,
            std::string expectedDesc) {
    ODDs::ODD lhs = ODDs::readJSON(lhsDesc);
    ODDs::ODD rhs = ODDs::readJSON(rhsDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramTensorProduct(lhs, rhs);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

}

class ODDsTensorProductTest : public ::testing::Test {
protected:
    static std::string trivialLhsDesc;
    static std::string trivialRhsDesc;
    static std::string trivialExpectedDesc;
};

// {abc,abd}, not minimal
std::string ODDsTensorProductTest::trivialLhsDesc = R"(
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
      "alphabet": ["c", "d"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 0, "symbol": "d", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

// {cba,dba}, not minimal
std::string ODDsTensorProductTest::trivialRhsDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0, 1],
  "layers": [
    {
      "alphabet": ["c", "d"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 1, "symbol": "d", "to": 0}
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
      "alphabet": ["a"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 1
    }
  ],
  "finalStates": [0]
})";

std::string ODDsTensorProductTest::trivialExpectedDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0, 1],
  "layers": [
    {
      "alphabet": ["a,c", "a,d"],
      "transitions": [
        {"from": 0, "symbol": "a,c", "to": 0},
        {"from": 1, "symbol": "a,d", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["b,b"],
      "transitions": [
        {"from": 0, "symbol": "b,b",  "to":0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["c,a", "d,a"],
      "transitions": [
        {"from": 0, "symbol": "c,a", "to": 0},
        {"from": 0, "symbol": "d,a", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

TEST_F(ODDsTensorProductTest, trivial) {
    doTest(trivialLhsDesc, trivialRhsDesc, trivialExpectedDesc);
}