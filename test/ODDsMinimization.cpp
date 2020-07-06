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
#include <ODDs/JSONDump.h>
#include <ODDs/Operations.h>

#include <Common/DivisionODDs.h>

#include <filesystem>

class ODDsMinimizationTest : public ::testing::Test {
protected:
    static std::string trivialArgDesc;
    static std::string trivialExpectedDesc;
    static std::string everyAccepted;
    static std::string noneAccepted;
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
        {"from": 0, "symbol": "b", "to": 0}
      ],
      "rightLayerStates": 1
    },
    {
      "alphabet": ["a", "b"],
      "transitions": [
        {"from": 0, "symbol": "a", "to": 0},
        {"from": 0, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": [0]
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

std::string ODDsMinimizationTest::noneAccepted = R"(
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
        {"from": 1, "symbol": "b", "to": 1}
      ],
      "rightLayerStates": 2
    }
  ],
  "finalStates": []
})";

TEST_F(ODDsMinimizationTest, trivial) {
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    ODDs::ODD actual = ODDs::minimize(arg);
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}


TEST_F(ODDsMinimizationTest, trivialDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(trivialArgDesc);
    ODDs::ODD expected = ODDs::readJSON(trivialExpectedDesc);
    std::string dirName = std::tmpnam(nullptr), temp = std::tmpnam(nullptr);
    ODDs::ODD actual = ODDs::minimize(arg, dirName, temp);
    EXPECT_TRUE(fs::exists(dirName));
    EXPECT_FALSE(fs::exists(temp));
    ASSERT_EQ(ODDs::writeJSON(expected), ODDs::writeJSON(actual));
}

TEST_F(ODDsMinimizationTest, everyAccepted) {
    ODDs::ODD arg = ODDs::readJSON(everyAccepted);
    ODDs::ODD odd = ODDs::minimize(arg);
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(1, odd.getLayer(i).width());
}

TEST_F(ODDsMinimizationTest, everyAcceptedDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(everyAccepted);
    std::string dirName = std::tmpnam(nullptr), temp = std::tmpnam(nullptr);
    ODDs::ODD odd = ODDs::minimize(arg, dirName, temp);
    EXPECT_TRUE(fs::exists(dirName));
    EXPECT_FALSE(fs::exists(temp));
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(1, odd.getLayer(i).width());
}

TEST_F(ODDsMinimizationTest, noneAccepted) {
    ODDs::ODD arg = ODDs::readJSON(noneAccepted);
    ODDs::ODD odd = ODDs::minimize(arg);
    for (int i = 0; i < odd.countLayers(); i++)
        EXPECT_EQ(1, odd.getLayer(i).width());
}

TEST_F(ODDsMinimizationTest, noneAcceptedDisk) {
    namespace fs = std::filesystem;
    ODDs::ODD arg = ODDs::readJSON(noneAccepted);
    std::string dirName = std::tmpnam(nullptr), temp = std::tmpnam(nullptr);
    ODDs::ODD odd = ODDs::minimize(arg, dirName, temp);
    EXPECT_TRUE(fs::exists(dirName));
    EXPECT_FALSE(fs::exists(temp));
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

TEST_F(ODDsMinimizationTest, div3Disk) {
    ASSERT_TRUE(
        ODDs::isDeterministic(
            ODDs::minimize(
                TestCommon::div3(10),
                std::tmpnam(nullptr),
                std::tmpnam(nullptr)
            )
        )
    );
}

TEST_F(ODDsMinimizationTest, div2) {
    using namespace TestCommon;
    ODDs::ODD odd = ODDs::minimize(div2NM(10) & (div3(10) | div2NM(10)));
    auto pred = [](int n) -> bool {
        return n % 2 == 0;
    };
    EXPECT_TRUE(checkPredicate(odd, pred));
    for (int i = 0; i < odd.countLayers() - 1; i++)
        EXPECT_EQ(1, odd.getLayer(i).width());
    EXPECT_EQ(2, odd.getLayer(odd.countLayers() - 1).width());
}

TEST_F(ODDsMinimizationTest, div6) {
    using namespace TestCommon;
    ODDs::ODD odd = ODDs::minimize(div2NM(10) & div2NM(10) & div3(10));
    auto pred = [](int n) -> bool {
        return n % 6 == 0;
    };
    EXPECT_TRUE(checkPredicate(odd, pred));
    EXPECT_EQ(2, odd.getLayer(0).width());
    for (int i = 1; i < odd.countLayers() - 1; i++)
        EXPECT_EQ(3, odd.getLayer(i).width());
    EXPECT_EQ(2, odd.getLayer(odd.countLayers() - 1).width());
}
