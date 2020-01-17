#include <gtest/gtest.h>

#include <Common/DivisionODDs.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>
#include <ODDs/JSONDump.h>

namespace {

void doTest(std::vector<ODDs::AlphabetMapping> gs,
            std::string argDesc,
            std::string expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramMapping(gs, arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doTestInverse(const std::vector<ODDs::AlphabetMapping>& gs,
                   const std::vector<ODDs::ODD::AlphabetMap>& as,
                   std::string argDesc,
                   std::string expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramInverseMapping(gs, as, arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

}

class ODDsMappingTest : public ::testing::Test {
protected:
    static std::string trivialArg;
    static std::string trivialExpected;
    static std::string trivialInverseExpected;
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

// {aba, abb, abc}
std::string ODDsMappingTest::trivialInverseExpected = R"(
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
        {"from": 0, "symbol": "c", "to": 1},
        {"from": 0, "symbol": "b", "to": 2},
        {"from": 0, "symbol": "c", "to": 2}
      ],
      "rightLayerStates": 3
    }
  ],
  "finalStates": [0, 1, 2]
})";

TEST_F(ODDsMappingTest, trivial) {
    std::vector<ODDs::AlphabetMapping> maps = {swapMap, swapMap, swapMap};
    doTest(maps, trivialArg, trivialExpected);
}

TEST_F(ODDsMappingTest, div3) {
    auto pred = [](int n) -> bool {
        int m = ~n & ~1023;
        return m % 3 == 0;
    };
    ODDs::ODD odd = TestCommon::div3(10);
    std::vector<ODDs::AlphabetMapping> maps;
    for (int i = 0; i < 10; i++)
        maps.emplace_back(swapMap);
    ASSERT_TRUE(TestCommon::checkPredicate(diagramMapping(maps, odd), pred));
}

TEST_F(ODDsMappingTest, inverseTrivial) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");

    ODDs::ODD::AlphabetMap b;
    b.addSymbol("b");

    ODDs::ODD::AlphabetMap bac;
    bac.addSymbol("b");
    bac.addSymbol("a");
    bac.addSymbol("c");

    std::vector<ODDs::ODD::AlphabetMap> alphabets = {a, b, bac};
    std::vector<ODDs::AlphabetMapping> maps = {swapMap, swapMap, swapMap};
    doTestInverse(maps, alphabets, trivialExpected, trivialInverseExpected);
}

TEST_F(ODDsMappingTest, inverseDiv3) {
    ODDs::ODD::AlphabetMap binary;
    binary.addSymbol("0");
    binary.addSymbol("1");

    auto pred = [](int n) -> bool {
        int m = ~n & ~1023;
        return m % 3 == 0;
    };
    ODDs::ODD odd = TestCommon::div3(10);
    std::vector<ODDs::AlphabetMapping> maps;
    for (int i = 0; i < 10; i++)
        maps.emplace_back(swapMap);
    std::vector<ODDs::ODD::AlphabetMap> as;
    for (int i = 0; i < 10; i++)
        as.emplace_back(binary);
    auto res = diagramInverseMapping(maps, as, odd);
    ASSERT_TRUE(TestCommon::checkPredicate(res, pred));
}
