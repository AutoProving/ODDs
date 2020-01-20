// Copyright (c) 2019-2020 Mateus de Oliveira Oliveira and Contributors. 
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

TEST(ODDsStructuresTest, transitionEqualityTrivial) {
    EXPECT_EQ(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 2));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 0, 0) == ODDs::ODD::Transition(1, 1, 1));
}

TEST(ODDsStructuresTest, trainsitionComparatorTrivial) {
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 3));
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 3), ODDs::ODD::Transition(0, 2, 1));
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 3));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 1, 2) < ODDs::ODD::Transition(0, 1, 2));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 1, 3) < ODDs::ODD::Transition(0, 1, 2));
}

TEST(ODDsStructuresTest, alphabetMapTrivial) {
    ODDs::ODD::AlphabetMap alphabet;
    ODDs::ODD::Symbol a = alphabet.addSymbol("symb_a");
    ODDs::ODD::Symbol b = alphabet.addSymbol("symb_b");
    EXPECT_EQ(a, alphabet.symbolToNumber("symb_a"));
    EXPECT_EQ(b, alphabet.symbolToNumber("symb_b"));
    EXPECT_EQ(-1, alphabet.symbolToNumber("symb_c"));
    EXPECT_EQ("symb_a", alphabet.numberToSymbol(a));
    EXPECT_EQ("symb_b", alphabet.numberToSymbol(b));
}

TEST(ODDsStructuresTest, alphabetMapEmpty) {
    ODDs::ODD::AlphabetMap alphabet;
    EXPECT_EQ(-1, alphabet.symbolToNumber("a"));
    ODDs::ODD::Symbol a = alphabet.addSymbol("a");
    EXPECT_EQ(a, alphabet.symbolToNumber("a"));
}

TEST(ODDsStructuresTest, alphabetMapExistingSymbol) {
    ODDs::ODD::AlphabetMap alphabet;
    ODDs::ODD::Symbol a1 = alphabet.addSymbol("a");
    ODDs::ODD::Symbol a2 = alphabet.addSymbol("a");
    ASSERT_EQ(a1, a2);
}

TEST(ODDsStructuresTest, alphabetMapEqualityTrivial) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");
    a.addSymbol("b");
    ODDs::ODD::AlphabetMap b = a;
    ASSERT_EQ(a, b);
    b.addSymbol("c");
    ASSERT_FALSE(a == b);
}

TEST(ODDsStructuresTest, alphabetMapOrder) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");
    a.addSymbol("b");
    ODDs::ODD::AlphabetMap b;
    b.addSymbol("b");
    b.addSymbol("a");
    ASSERT_FALSE(a == b);
}

TEST(ODDsStructuresTest, alphabetMapSymbols) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("b");
    a.addSymbol("a");
    a.addSymbol("b");
    std::vector<std::string> expected = {"b", "a"};
    ASSERT_EQ(expected, a.symbols());
}

TEST(ODDsStructuresTest, alphabetMapContains) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");
    a.addSymbol("c");
    ASSERT_TRUE(a.contains("a"));
    ASSERT_FALSE(a.contains("b"));
    ASSERT_TRUE(a.contains("c"));
}

TEST(ODDsStructuresTest, layerWidthLeft) {
    ODDs::ODD::Layer layer {
        {},
        3,
        {},
        2,
        {},
        {},
        false, false
    };
    ASSERT_EQ(3, layer.width());
}

TEST(ODDsStructuresTest, layerWidthRight) {
    ODDs::ODD::Layer layer {
        {},
        2,
        {},
        3,
        {},
        {},
        false, false
    };
    ASSERT_EQ(3, layer.width());
}

TEST(ODDsStructuresTest, emptyLayerWidth) {
    ODDs::ODD::Layer layer {
        {},
        0,
        {},
        0,
        {},
        {},
        false, false
    };
    ASSERT_EQ(0, layer.width());
}

TEST(ODDsStructuresTest, layerSanity) {
    ODDs::ODD::StateContainer empty, nonEmpty;
    nonEmpty.insert(0);
    ODDs::ODD::Layer layerEEFF {{},5,empty,5,empty,{},false,false};
    EXPECT_TRUE(layerEEFF.checkSanity());
    ODDs::ODD::Layer layerEEFT {{},5,empty,5,empty,{},false,true};
    EXPECT_TRUE(layerEEFT.checkSanity());
    ODDs::ODD::Layer layerEETF {{},5,empty,5,empty,{},true,false};
    EXPECT_TRUE(layerEETF.checkSanity());
    ODDs::ODD::Layer layerEETT {{},5,empty,5,empty,{},true,true};
    EXPECT_TRUE(layerEETT.checkSanity());
    ODDs::ODD::Layer layerENFF {{},5,empty,5,nonEmpty,{},false,false};
    EXPECT_FALSE(layerENFF.checkSanity());
    ODDs::ODD::Layer layerENFT {{},5,empty,5,nonEmpty,{},false,true};
    EXPECT_TRUE(layerENFT.checkSanity());
    ODDs::ODD::Layer layerENTF {{},5,empty,5,nonEmpty,{},true,false};
    EXPECT_FALSE(layerENTF.checkSanity());
    ODDs::ODD::Layer layerENTT {{},5,empty,5,nonEmpty,{},true,true};
    EXPECT_TRUE(layerENTT.checkSanity());
    ODDs::ODD::Layer layerNEFF {{},5,nonEmpty,5,empty,{},false,false};
    EXPECT_FALSE(layerNEFF.checkSanity());
    ODDs::ODD::Layer layerNEFT {{},5,nonEmpty,5,empty,{},false,true};
    EXPECT_FALSE(layerNEFT.checkSanity());
    ODDs::ODD::Layer layerNETF {{},5,nonEmpty,5,empty,{},true,false};
    EXPECT_TRUE(layerNETF.checkSanity());
    ODDs::ODD::Layer layerNETT {{},5,nonEmpty,5,empty,{},true,true};
    EXPECT_TRUE(layerNETT.checkSanity());
    ODDs::ODD::Layer layerNNFF {{},5,nonEmpty,5,nonEmpty,{},false,false};
    EXPECT_FALSE(layerNNFF.checkSanity());
    ODDs::ODD::Layer layerNNFT {{},5,nonEmpty,5,nonEmpty,{},false,true};
    EXPECT_FALSE(layerNNFT.checkSanity());
    ODDs::ODD::Layer layerNNTF {{},5,nonEmpty,5,nonEmpty,{},true,false};
    EXPECT_FALSE(layerNNTF.checkSanity());
    ODDs::ODD::Layer layerNNTT {{},5,nonEmpty,5,nonEmpty,{},true,true};
    EXPECT_TRUE(layerNNTT.checkSanity());
}

TEST(ODDsStructuresTest, transitionContainerOrder) {
    ODDs::ODD::TransitionContainer transitions;
    transitions.insert({0, 0, 1});
    transitions.insert({0, 1, 0});
    transitions.insert({1, 0, 1});
    transitions.insert({0, 0, 0});
    
    std::vector<ODDs::ODD::Transition> ord;
    for (const ODDs::ODD::Transition& transition : transitions) {
        ord.push_back(transition);
    }

    std::vector<ODDs::ODD::Transition> expected = {
        {0, 0, 1},
        {0, 0, 0},
        {0, 1, 0},
        {1, 0, 1}
    };

    ASSERT_EQ(expected, ord);
}

TEST(ODDsStructuresTest, transitionContainerOrderIList) {
    ODDs::ODD::TransitionContainer transitions = {
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
        {0, 0, 0}
    };

    std::vector<ODDs::ODD::Transition> ord;
    for (const ODDs::ODD::Transition& transition : transitions) {
        ord.push_back(transition);
    }

    std::vector<ODDs::ODD::Transition> expected = {
        {0, 0, 1},
        {0, 0, 0},
        {0, 1, 0},
        {1, 0, 1}
    };

    ASSERT_EQ(expected, ord);
}

TEST(ODDsStructuresTest, transitionContainerIteratorDecrement) {
    ODDs::ODD::Transition transition0(0, 0, 1);
    ODDs::ODD::Transition transition1(0, 0, 0);
    ODDs::ODD::Transition transition2(0, 1, 0);
    ODDs::ODD::Transition transition3(1, 0, 1);

    ODDs::ODD::TransitionContainer transitions = {
        transition0,
        transition3,
        transition2,
        transition1
    };

    auto it = transitions.begin();
    ASSERT_EQ(transition0, *it);
    it++;
    ASSERT_EQ(transition1, *it);
    std::advance(it, 2);
    ASSERT_EQ(transition3, *it);
    --it;
    ASSERT_EQ(transition2, *it);
    std::advance(it, -2);
    ASSERT_EQ(transition0, *it);
}

TEST(ODDsStructuresTest, transitionContainerProceed) {
    ODDs::ODD::Transition transition0(0, 0, 1);
    ODDs::ODD::Transition transition1(0, 0, 0);
    ODDs::ODD::Transition transition2(0, 1, 0);
    ODDs::ODD::Transition transition3(1, 0, 1);

    ODDs::ODD::TransitionContainer transitions = {
        transition0,
        transition1,
        transition2,
        transition3
    };

    auto range00 = transitions.proceed({0, 0});
    std::vector<ODDs::ODD::Transition> expected = {
        transition0,
        transition1
    };
    std::vector<ODDs::ODD::Transition> actual;
    std::copy(range00.begin(), range00.end(), std::back_inserter(actual));
    ASSERT_EQ(expected, actual);

    expected = {transition2};
    actual.clear();
    for (const ODDs::ODD::Transition& transition : transitions.proceed(0, 1)) {
        actual.push_back(transition);
    }
    ASSERT_EQ(expected, actual);
}

TEST(ODDsStructuresTest, transitionContainerGo) {
    ODDs::ODD::TransitionContainer transitions = {
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
        {0, 0, 0}
    };

    ASSERT_EQ(1, transitions.go(0, 0));
    ASSERT_EQ(0, transitions.go(0, 1));
}

TEST(ODDsStructuresTest, transitionContainerDuplicates) {
    ODDs::ODD::TransitionContainer transitions = {
        {0, 0, 0},
        {0, 0, 1},
        {0, 0, 0}, 
        {0, 0, 2}
    };

    std::vector<ODDs::ODD::Transition> actual;
    for (const ODDs::ODD::Transition& transition : transitions) {
        actual.emplace_back(transition);
    }

    std::vector<ODDs::ODD::Transition> expected = {
        {0, 0, 0},
        {0, 0, 1},
        {0, 0, 2}
    };

    ASSERT_EQ(expected, actual);
}

TEST(ODDsStructuresTest, transitionContainerHasKey) {
    ODDs::ODD::TransitionContainer transitions = {
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 1},
        {0, 0, 0}
    };

    EXPECT_TRUE(transitions.hasKey(0, 1));
    EXPECT_TRUE(transitions.hasKey({0, 0}));
    EXPECT_FALSE(transitions.hasKey(1, 1));
}
