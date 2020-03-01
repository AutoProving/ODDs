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

#include <sstream>

namespace {

void testLayersSanity(const ODDs::ODD& odd) {
    for (int i = 0; i < odd.countLayers(); i++) {
        ASSERT_TRUE(odd.getLayer(i).checkSanity());
        ASSERT_EQ(i == 0, odd.getLayer(i).isInitial);
        ASSERT_EQ(i + 1 == odd.countLayers(), odd.getLayer(i).isFinal);
        if (!odd.getLayer(i).isFinal) {
            ASSERT_EQ(odd.getLayer(i).rightStates, odd.getLayer(i + 1).leftStates);
        }
    }
}

void checkLayer(const ODDs::ODD::Layer& layer,
                const ODDs::ODD::AlphabetMap& alphabet,
                int leftStates, int rightStates,
                const ODDs::ODD::TransitionContainer& transitions,
                bool isInitial, bool isFinal) {
    ASSERT_EQ(alphabet, layer.alphabet);
    ASSERT_EQ(leftStates, layer.leftStates);
    ASSERT_EQ(rightStates, layer.rightStates);
    ASSERT_EQ(transitions, layer.transitions);
    ASSERT_EQ(isInitial, layer.isInitial);
    ASSERT_EQ(isFinal, layer.isFinal);
}

}

class ODDBuilderTest : public ::testing::Test {
protected:
    static std::string description;

    static void setDescription() {
        description =
            "2\n"         // Number of layers
            "1\n2 \n"     // Initial states
            "3\n"         // Leftmost layer states
            "2\na b \n"   // First layer alphabet
            "6\n"         // Six transitions of the first layer
            "0 a 0\n"
            "0 b 1\n"
            "1 a 0\n"
            "1 b 1\n"
            "2 a 1\n"
            "2 b 0\n"
            "2\n"         // Middle layer states
            "2\nc d \n"   // Second layer alphabet
            "5\n"         // Five transitions of the second layer
            "0 c 0\n"
            "0 d 1\n"
            "1 c 0\n"
            "1 d 2\n"
            "1 d 0\n"     // Non-deterministic
            "3\n"         // Rightmost layer states
            "1\n2 \n";    // Final states
    }

    static ODDs::ODD::StateContainer      initialStates, finalStates;
    static int                            states0, states1, states2;
    static ODDs::ODD::AlphabetMap         alphabet0, alphabet1;
    static ODDs::ODD::TransitionContainer transitions0, transitions1;

    static void setODDBuilderPrerequisites() {
        initialStates = {2};

        states0 = 3;

        ODDs::ODD::Symbol a0a = alphabet0.addSymbol("a");
        ODDs::ODD::Symbol a0b = alphabet0.addSymbol("b");
        transitions0.insert({0, a0a, 0});
        transitions0.insert({0, a0b, 1});
        transitions0.insert({1, a0a, 0});
        transitions0.insert({1, a0b, 1});
        transitions0.insert({2, a0a, 1});
        transitions0.insert({2, a0b, 0});

        states1 = 2;

        ODDs::ODD::Symbol a1c = alphabet1.addSymbol("c");
        ODDs::ODD::Symbol a1d = alphabet1.addSymbol("d");

        transitions1.insert({0, a1c, 0});
        transitions1.insert({0, a1d, 1});
        transitions1.insert({1, a1c, 0});
        transitions1.insert({1, a1d, 2});
        transitions1.insert({1, a1d, 0});

        states2 = 3;

        finalStates = {2};
    }

    static std::string jsonDescription;

    static void SetUpTestSuite() {
        setDescription();
        setODDBuilderPrerequisites();
    }

    virtual void checkODD(const ODDs::ODD& odd) {
        testLayersSanity(odd);
        ASSERT_EQ(2, odd.countLayers());
        checkLayer(odd.getLayer(0), alphabet0, states0, states1, transitions0, true, false);
        checkLayer(odd.getLayer(1), alphabet1, states1, states2, transitions1, false, true);
        ASSERT_EQ(initialStates, odd.getLayer(0).initialStates);
        ASSERT_EQ(finalStates, odd.getLayer(1).finalStates);
    }

    virtual ODDs::ODD buildODD() {
        ODDs::ODDBuilder builder(states0);
        builder.addLayer(alphabet0, transitions0, states1);
        builder.addLayer(alphabet1, transitions1, states2);
        builder.setInitialStates(initialStates);
        builder.setFinalStates(finalStates);
        return builder.build();
    }
};

std::string                    ODDBuilderTest::description;
ODDs::ODD::StateContainer      ODDBuilderTest::initialStates, ODDBuilderTest::finalStates;
int                            ODDBuilderTest::states0, ODDBuilderTest::states1, ODDBuilderTest::states2;
ODDs::ODD::AlphabetMap         ODDBuilderTest::alphabet0, ODDBuilderTest::alphabet1;
ODDs::ODD::TransitionContainer ODDBuilderTest::transitions0, ODDBuilderTest::transitions1;

std::string ODDBuilderTest::jsonDescription = R"({
  "leftLayerStates": 3,
  "initialStates": [
    2
  ],
  "layers": [
    {
      "alphabet": [
        "a",
        "b"
      ],
      "transitions": [
        {
          "from": 0,
          "symbol": "a",
          "to": 0
        },
        {
          "from": 0,
          "symbol": "b",
          "to": 1
        },
        {
          "from": 1,
          "symbol": "a",
          "to": 0
        },
        {
          "from": 1,
          "symbol": "b",
          "to": 1
        },
        {
          "from": 2,
          "symbol": "a",
          "to": 1
        },
        {
          "from": 2,
          "symbol": "b",
          "to": 0
        }
      ],
      "rightLayerStates": 2
    },
    {
      "alphabet": [
        "c",
        "d"
      ],
      "transitions": [
        {
          "from": 0,
          "symbol": "c",
          "to": 0
        },
        {
          "from": 0,
          "symbol": "d",
          "to": 1
        },
        {
          "from": 1,
          "symbol": "c",
          "to": 0
        },
        {
          "from": 1,
          "symbol": "d",
          "to": 2
        },
        {
          "from": 1,
          "symbol": "d",
          "to": 0
        }
      ],
      "rightLayerStates": 3
    }
  ],
  "finalStates": [
    2
  ]
})";

TEST_F(ODDBuilderTest, trivial) {
    ODDs::ODD odd = buildODD();
    checkODD(odd);
}

TEST_F(ODDBuilderTest, noLayers) {
    ODDs::ODD::StateContainer localInitialStates = {1, 2};
    ODDs::ODD::StateContainer localFinalStates = {0, 1};
    ODDs::ODDBuilder builder(3);
    builder.setInitialStates(localInitialStates);
    builder.setFinalStates(localFinalStates);
    ODDs::ODD odd = builder.build();
    testLayersSanity(odd);
}

TEST_F(ODDBuilderTest, input) {
    std::istringstream is(description);
    ODDs::ODD odd = ODDs::readFromIStream(is);
    checkODD(odd);
}

TEST_F(ODDBuilderTest, output) {
    ODDs::ODD odd = buildODD();
    std::stringstream ss;
    writeToOStream(ss, odd);
    ASSERT_EQ(description, ss.str());
}

TEST_F(ODDBuilderTest, jsonRead) {
    ODDs::ODD odd = ODDs::readJSON(jsonDescription);
    checkODD(odd);
}

TEST_F(ODDBuilderTest, jsonReadStream) {
    std::istringstream is(jsonDescription);
    ODDs::ODD odd = ODDs::readJSON(is);
    checkODD(odd);
}

TEST_F(ODDBuilderTest, jsonWrite) {
    ODDs::ODD odd = buildODD();
    ASSERT_EQ(jsonDescription, writeJSON(odd));
}

TEST_F(ODDBuilderTest, jsonWriteStream) {
    ODDs::ODD odd = buildODD();
    std::stringstream ss;
    writeJSON(ss, odd);
    ASSERT_EQ(jsonDescription, ss.str());
}

TEST_F(ODDBuilderTest, accepts) {
    ODDs::ODD odd = buildODD();
    EXPECT_FALSE(odd.accepts({"a", "c"}));
    EXPECT_TRUE(odd.accepts({"a", "d"}));
}
