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

void doTest(const std::string& lhsDesc,
            const std::string& rhsDesc,
            const std::string& expectedDesc) {
    ODDs::ODD lhs = ODDs::readJSON(lhsDesc);
    ODDs::ODD rhs = ODDs::readJSON(rhsDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    ODDs::ODD result = ODDs::diagramUnion(lhs, rhs);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
}

void doDiskTest(const std::string& lhsDesc,
                const std::string& rhsDesc,
                const std::string& expectedDesc) {
    namespace fs = std::filesystem;
    ODDs::ODD lhs = ODDs::readJSON(lhsDesc);
    ODDs::ODD rhs = ODDs::readJSON(rhsDesc);
    ODDs::ODD expected = ODDs::readJSON(expectedDesc);
    std::string dirName = std::tmpnam(nullptr);
    ODDs::ODD result = ODDs::diagramUnion(lhs, rhs, dirName);
    EXPECT_TRUE(fs::exists(dirName));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(result));
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

TEST_F(ODDsUnionTest, trivialDisk) {
    doDiskTest(trivialLhsDesc, trivialRhsDesc, trivialExpectedDesc);
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
        {"from": 2, "symbol": "a", "to": 3},
        {"from": 2, "symbol": "a", "to": 2}
      ],
      "rightLayerStates": 4
    }
  ],
  "finalStates": [0, 1, 3]
})";

TEST_F(ODDsUnionTest, nonDet) {
    doTest(nonDetLhsDesc, nonDetRhsDesc, nonDetExpectedDesc);
}

TEST_F(ODDsUnionTest, nonDetDisk) {
    doDiskTest(nonDetLhsDesc, nonDetRhsDesc, nonDetExpectedDesc);
}

TEST_F(ODDsUnionTest, div2div3) {
    auto odd = TestCommon::div2(10) | TestCommon::div3(10);
    auto pred = [](int n) -> bool {
        return n % 2 == 0 || n % 3 == 0;
    };
    ASSERT_TRUE(TestCommon::checkPredicate(odd, pred));
}

TEST_F(ODDsUnionTest, div2div3Disk) {
    namespace fs = std::filesystem;
    std::string dirName = std::tmpnam(nullptr);
    auto odd = ODDs::diagramUnion(TestCommon::div2(10),
                                  TestCommon::div3(10),
                                  dirName);
    EXPECT_TRUE(fs::exists(dirName));
    auto pred = [](int n) -> bool {
        return n % 2 == 0 || n % 3 == 0;
    };
    ASSERT_TRUE(TestCommon::checkPredicate(odd, pred));
}
