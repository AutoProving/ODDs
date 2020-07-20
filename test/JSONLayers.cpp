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

ODDs::ODD::Layer testLayer() {
    ODDs::ODD::AlphabetMap alphabet;
    ODDs::ODD::Symbol a = alphabet.addSymbol("a");
    ODDs::ODD::Symbol b = alphabet.addSymbol("b");

    ODDs::ODD::TransitionContainer transitions;
    transitions.insert({0, a, 1});
    transitions.insert({0, a, 2});
    transitions.insert({0, b, 0});
    transitions.insert({1, a, 2});
    transitions.insert({1, b, 0});

    return {
        alphabet,
        2,
        {},
        3,
        {},
        transitions,
        false,
        false
    };
}

std::string dumpVector(const std::vector<int>& vec) {
    std::ostringstream ss;
    ss << "[";
    bool first = true;
    for (int u : vec) {
        if (!first)
            ss << ",";
        ss << u;
        first = false;
    }
    ss << "]";
    return ss.str();
}

std::string boolToString(bool b) {
    return b ? "true" : "false";
}

std::string expectedDump(const std::vector<int>& initialStates,
                         const std::vector<int>& finalStates) {
    return
        "{"
            R"("alphabet":["a","b"],)"
            R"("leftLayerStates":2,)"
            R"("initialStates":)" + dumpVector(initialStates) + ","
            R"("rightLayerStates":3,)"
            R"("finalStates":)" + dumpVector(finalStates) + ","
            R"("transitions":[)"
                R"({"from":0,"symbol":"a","to":1},)"
                R"({"from":0,"symbol":"a","to":2},)"
                R"({"from":0,"symbol":"b","to":0},)"
                R"({"from":1,"symbol":"a","to":2},)"
                R"({"from":1,"symbol":"b","to":0})"
            R"(],)"
            R"("isInitial":)" + boolToString(!initialStates.empty()) + ","
            R"("isFinal":)" + boolToString(!finalStates.empty()) +
        "}";
}

void testWrite(std::vector<int> initialStates,
               std::vector<int> finalStates) {
    auto layer = testLayer();

    layer.isInitial = !initialStates.empty();
    for (int u : initialStates)
        layer.initialStates.insert(u);

    layer.isFinal = !finalStates.empty();
    for (int u : finalStates)
        layer.finalStates.insert(u);

    std::string expected = expectedDump(initialStates, finalStates);

    std::ostringstream ss;
    ODDs::writeJSON(ss, layer);

    EXPECT_EQ(expected, ss.str());
}

void testRead(std::vector<int> initialStates,
              std::vector<int> finalStates) {
    auto layer = testLayer();

    layer.isInitial = !initialStates.empty();
    for (int u : initialStates)
        layer.initialStates.insert(u);

    layer.isFinal = !finalStates.empty();
    for (int u : finalStates)
        layer.finalStates.insert(u);

    std::string dump = expectedDump(initialStates, finalStates);

    std::istringstream ss(dump);
    auto actual = ODDs::readLayerJSON(ss);

    EXPECT_EQ(layer.alphabet, actual.alphabet);
    EXPECT_EQ(layer.leftStates, actual.leftStates);
    EXPECT_EQ(layer.initialStates, actual.initialStates);
    EXPECT_EQ(layer.rightStates, actual.rightStates);
    EXPECT_EQ(layer.finalStates, actual.finalStates);
    EXPECT_EQ(layer.transitions, actual.transitions);
    EXPECT_EQ(layer.isInitial, actual.isInitial);
    EXPECT_EQ(layer.isFinal, actual.isFinal);
}

}

TEST(ODDsJSONLayersTest, firstLayerWrite) {
    testWrite({0, 1}, {});
}

TEST(ODDsJSONLayersTest, intermediateLayerWrite) {
    testWrite({}, {});
}

TEST(ODDsJSONLayersTest, lastLayerWrite) {
    testWrite({}, {1, 2});
}

TEST(ODDsJSONLayersTest, onlyLayerWrite) {
    testWrite({0}, {1, 2});
}

TEST(ODDsJSONLayersTest, firstLayerRead) {
    testRead({0, 1}, {});
}

TEST(ODDsJSONLayersTest, intermediateLayerRead) {
    testRead({}, {});
}

TEST(ODDsJSONLayersTest, lastLayerRead) {
    testRead({}, {1, 2});
}

TEST(ODDsJSONLayersTest, onlyLayerRead) {
    testRead({0}, {1, 2});
}
