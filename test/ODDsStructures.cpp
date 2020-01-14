#include <gtest/gtest.h>
#include <ODDs.h>

TEST(ODDsStructures, transitionEqualityTrivial) {
    EXPECT_EQ(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 2));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 0, 0) == ODDs::ODD::Transition(1, 1, 1));
}

TEST(ODDsStructures, trainsitionComparatorTrivial) {
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 3));
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 3), ODDs::ODD::Transition(0, 2, 1));
    EXPECT_LT(ODDs::ODD::Transition(0, 1, 2), ODDs::ODD::Transition(0, 1, 3));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 1, 2) < ODDs::ODD::Transition(0, 1, 2));
    EXPECT_FALSE(ODDs::ODD::Transition(0, 1, 3) < ODDs::ODD::Transition(0, 1, 2));
}

TEST(ODDsStructures, alphabetMapTrivial) {
    ODDs::ODD::AlphabetMap alphabet;
    int a = alphabet.addSymbol("symb_a");
    int b = alphabet.addSymbol("symb_b");
    EXPECT_EQ(a, alphabet.symbolToNumber("symb_a"));
    EXPECT_EQ(b, alphabet.symbolToNumber("symb_b"));
    EXPECT_EQ(-1, alphabet.symbolToNumber("symb_c"));
    EXPECT_EQ("symb_a", alphabet.numberToSymbol(a));
    EXPECT_EQ("symb_b", alphabet.numberToSymbol(b));
}

TEST(ODDsStructures, alphabetMapEmpty) {
    ODDs::ODD::AlphabetMap alphabet;
    EXPECT_EQ(-1, alphabet.symbolToNumber("a"));
    int a = alphabet.addSymbol("a");
    EXPECT_EQ(a, alphabet.symbolToNumber("a"));
}

TEST(ODDsStructures, alphabetMapExistingSymbol) {
    ODDs::ODD::AlphabetMap alphabet;
    int a1 = alphabet.addSymbol("a");
    int a2 = alphabet.addSymbol("a");
    ASSERT_EQ(a1, a2);
}

TEST(ODDsStructures, alphabetMapEqualityTrivial) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");
    a.addSymbol("b");
    ODDs::ODD::AlphabetMap b = a;
    ASSERT_EQ(a, b);
    b.addSymbol("c");
    ASSERT_FALSE(a == b);
}

TEST(ODDsStructures, alphabetMapOrder) {
    ODDs::ODD::AlphabetMap a;
    a.addSymbol("a");
    a.addSymbol("b");
    ODDs::ODD::AlphabetMap b;
    b.addSymbol("b");
    b.addSymbol("a");
    ASSERT_FALSE(a == b);
}

TEST(ODDsStructures, layerWidthLeft) {
    ODDs::ODD::StateContainer leftStates, rightStates;
    leftStates.insert(1);
    leftStates.insert(2);
    leftStates.insert(3);
    rightStates.insert(1);
    rightStates.insert(2);
    ODDs::ODD::Layer layer {
        {},
        &leftStates,
        {},
        &rightStates,
        {},
        {},
        false, false
    };
    ASSERT_EQ(3, layer.width());
}

TEST(ODDsStructures, layerWidthRight) {
    ODDs::ODD::StateContainer leftStates, rightStates;
    rightStates.insert(1);
    rightStates.insert(2);
    rightStates.insert(3);
    leftStates.insert(1);
    leftStates.insert(2);
    ODDs::ODD::Layer layer {
        {},
        &leftStates,
        {},
        &rightStates,
        {},
        {},
        false, false
    };
    ASSERT_EQ(3, layer.width());
}

TEST(ODDsStructures, emptyLayerWidth) {
    ODDs::ODD::StateContainer leftStates, rightStates;
    ODDs::ODD::Layer layer {
        {},
        &leftStates,
        {},
        &rightStates,
        {},
        {},
        false, false
    };
    ASSERT_EQ(0, layer.width());
}

TEST(ODDsStructures, layerSanity) {
    ODDs::ODD::StateContainer empty, nonEmpty;
    nonEmpty.insert(0);
    ODDs::ODD::Layer layerEEFF {{},&empty,empty,&empty,empty,{},false,false};
    EXPECT_TRUE(layerEEFF.checkSanity());
    ODDs::ODD::Layer layerEEFT {{},&empty,empty,&empty,empty,{},false,true};
    EXPECT_TRUE(layerEEFT.checkSanity());
    ODDs::ODD::Layer layerEETF {{},&empty,empty,&empty,empty,{},true,false};
    EXPECT_TRUE(layerEETF.checkSanity());
    ODDs::ODD::Layer layerEETT {{},&empty,empty,&empty,empty,{},true,true};
    EXPECT_TRUE(layerEETT.checkSanity());
    ODDs::ODD::Layer layerENFF {{},&empty,empty,&empty,nonEmpty,{},false,false};
    EXPECT_FALSE(layerENFF.checkSanity());
    ODDs::ODD::Layer layerENFT {{},&empty,empty,&empty,nonEmpty,{},false,true};
    EXPECT_TRUE(layerENFT.checkSanity());
    ODDs::ODD::Layer layerENTF {{},&empty,empty,&empty,nonEmpty,{},true,false};
    EXPECT_FALSE(layerENTF.checkSanity());
    ODDs::ODD::Layer layerENTT {{},&empty,empty,&empty,nonEmpty,{},true,true};
    EXPECT_TRUE(layerENTT.checkSanity());
    ODDs::ODD::Layer layerNEFF {{},&empty,nonEmpty,&empty,empty,{},false,false};
    EXPECT_FALSE(layerNEFF.checkSanity());
    ODDs::ODD::Layer layerNEFT {{},&empty,nonEmpty,&empty,empty,{},false,true};
    EXPECT_FALSE(layerNEFT.checkSanity());
    ODDs::ODD::Layer layerNETF {{},&empty,nonEmpty,&empty,empty,{},true,false};
    EXPECT_TRUE(layerNETF.checkSanity());
    ODDs::ODD::Layer layerNETT {{},&empty,nonEmpty,&empty,empty,{},true,true};
    EXPECT_TRUE(layerNETT.checkSanity());
    ODDs::ODD::Layer layerNNFF {{},&empty,nonEmpty,&empty,nonEmpty,{},false,false};
    EXPECT_FALSE(layerNNFF.checkSanity());
    ODDs::ODD::Layer layerNNFT {{},&empty,nonEmpty,&empty,nonEmpty,{},false,true};
    EXPECT_FALSE(layerNNFT.checkSanity());
    ODDs::ODD::Layer layerNNTF {{},&empty,nonEmpty,&empty,nonEmpty,{},true,false};
    EXPECT_FALSE(layerNNTF.checkSanity());
    ODDs::ODD::Layer layerNNTT {{},&empty,nonEmpty,&empty,nonEmpty,{},true,true};
    EXPECT_TRUE(layerNNTT.checkSanity());
}
