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