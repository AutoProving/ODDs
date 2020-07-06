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

#include <filesystem>

namespace {

void doTest(const std::vector<ODDs::AlphabetMapping>& gs,
            const std::string& argDesc,
            const std::string& expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramMapping(gs, arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doDiskTest(const std::vector<ODDs::AlphabetMapping>& gs,
                const std::string& argDesc,
                const std::string& expectedDesc) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD result = ODDs::diagramMapping(gs, arg, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doTestInverse(const std::vector<ODDs::AlphabetMapping>& gs,
                   const std::vector<ODDs::ODD::AlphabetMap>& as,
                   const std::string& argDesc,
                   const std::string& expectedDesc) {
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramInverseMapping(gs, as, arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doDiskTestInverse(const std::vector<ODDs::AlphabetMapping>& gs,
                       const std::vector<ODDs::ODD::AlphabetMap>& as,
                       const std::string& argDesc,
                       const std::string& expectedDesc) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(argDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD result = ODDs::diagramInverseMapping(gs, as, arg, dirName);
    EXPECT_TRUE(fs::exists(dirName));
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

TEST_F(ODDsMappingTest, trivialDisk) {
    std::vector<ODDs::AlphabetMapping> maps = {swapMap, swapMap, swapMap};
    doDiskTest(maps, trivialArg, trivialExpected);
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

TEST_F(ODDsMappingTest, inverseTrivialDisk) {
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
    doDiskTestInverse(maps, alphabets, trivialExpected, trivialInverseExpected);
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
