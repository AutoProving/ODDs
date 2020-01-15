#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

#include <string>

namespace {

void doTest(std::string lhsDesc,
            std::string rhsDesc,
            std::string expectedDesc) {
    ODDs::ODD lhs = ODDs::readJSON(lhsDesc);
    ODDs::ODD rhs = ODDs::readJSON(rhsDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramUnion(lhs, rhs);
    ASSERT_EQ(ODDs::JSONDump(expected), ODDs::JSONDump(result));
}

}

class ODDsUnionTest : public ::testing::Test {
protected:
    static std::string trivialLhsDesc;
    static std::string trivialRhsDesc;
    static std::string trivialExpectedDesc;

    static std::string nonDetLhsDesc;
    static std::string nonDetRhsDesc;
    static std::string nonDetExpectedDesc;
};

// {abc,abd}, not minimal
std::string ODDsUnionTest::trivialLhsDesc = R"(
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
std::string ODDsUnionTest::trivialRhsDesc = R"(
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

// {abc,abd,cba,dba}, not minimal
std::string ODDsUnionTest::trivialExpectedDesc = R"(
{
  "leftLayerStates": 3,
  "initialStates": [0, 1, 2],
  "layers": [
    {
      "alphabet": ["a", "c", "d"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 1, "symbol": "c", "to": 1},
        {"from": 2, "symbol": "d", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["c", "d", "a"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 0, "symbol": "d", "to": 1},
        {"from": 1, "symbol": "a", "to": 2}
      ],
      "rightLayerStates": 3
    }
  ],
  "finalStates": [0, 1, 2]
})";

TEST_F(ODDsUnionTest, trivial) {
    doTest(trivialLhsDesc, trivialRhsDesc, trivialExpectedDesc);
}

//{abc,abd} non-deterministic
std::string ODDsUnionTest::nonDetLhsDesc = R"(
{
  "leftLayerStates": 1,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["a"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "a", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": ["c", "d"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 1, "symbol": "d", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0, 1]
})";

//{cba,dba} non-deterministic
std::string ODDsUnionTest::nonDetRhsDesc = R"(
{
  "leftLayerStates": 2,
  "initialStates": [0],
  "layers": [
    {
      "alphabet": ["c", "d", "a"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 0, "symbol": "d", "to": 0},
        {"from": 1, "symbol": "a", "to": 0}
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
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 0, "symbol": "a", "to": 0}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [1]
})";

//{abc,abd,cba,cbd} non-deterministic
std::string ODDsUnionTest::nonDetExpectedDesc = R"(
{
  "leftLayerStates": 3,
  "initialStates": [0, 1],
  "layers": [
    {
      "alphabet": ["a", "c", "d"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "a", "to": 1},
        {"from": 1, "symbol": "c", "to": 2},
        {"from": 1, "symbol": "d", "to": 2},
        {"from": 2, "symbol": "a", "to": 2}
      ],
      "rightLayerStates": 3
    },
    {
      "alphabet": ["b"],
      "transitions": [
        {"from": 0, "symbol": "b", "to": 0},
        {"from": 1, "symbol": "b", "to": 1},
        {"from": 2, "symbol": "b", "to": 2}
      ],
      "rightLayerStates": 3
    },
    {
      "alphabet": ["c", "d", "a"],
      "transitions": [
        {"from": 0, "symbol": "c", "to": 0},
        {"from": 1, "symbol": "d", "to": 1},
        {"from": 2, "symbol": "a", "to": 2},
        {"from": 2, "symbol": "a", "to": 3}
      ],
      "rightLayerStates": 4
    }
  ],
  "finalStates": [0, 1, 3]
})";

TEST_F(ODDsUnionTest, nonDet) {
    doTest(nonDetLhsDesc, nonDetRhsDesc, nonDetExpectedDesc);
}
