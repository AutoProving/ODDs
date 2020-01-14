#include <gtest/gtest.h>
#include <ODDs.h>

namespace {

void testLayersSanity(const ODDs::ODD& odd) {
    for (int i = 0; i < odd.countLayers(); i++) {
        ASSERT_TRUE(odd.getLayer(i).checkSanity());
        ASSERT_EQ(i == 0, odd.getLayer(i).isInitial);
        ASSERT_EQ(i + 1 == odd.countLayers(), odd.getLayer(i).isFinal);
        if (!odd.getLayer(i).isFinal)
            ASSERT_EQ(odd.getLayer(i).rightStates, odd.getLayer(i + 1).leftStates);
    }
}

void checkLayer(const ODDs::ODD::Layer& layer,
                const ODDs::ODD::AlphabetMap& alphabet,
                const ODDs::ODD::StateContainer& leftStates,
                const ODDs::ODD::StateContainer& rightStates,
                const ODDs::ODD::TransitionContainer& transitions,
                bool isInitial, bool isFinal) {
    ASSERT_EQ(alphabet, layer.alphabet);
    ASSERT_EQ(leftStates, *layer.leftStates);
    ASSERT_EQ(rightStates, *layer.rightStates);
    ASSERT_EQ(transitions, layer.transitions);
    ASSERT_EQ(isInitial, layer.isInitial);
    ASSERT_EQ(isFinal, layer.isFinal);
}

}

TEST(ODDBuilder, trivial) {
    ODDs::ODD::StateContainer initialStates = {2};

    ODDs::ODD::StateContainer states0 = {0, 1, 2};

    ODDs::ODD::AlphabetMap alphabet0;
    int a0a = alphabet0.addSymbol("a");
    int a0b = alphabet0.addSymbol("b");
    ODDs::ODD::TransitionContainer transitions0;
    transitions0.insert({0, a0a, 0});
    transitions0.insert({0, a0b, 1});
    transitions0.insert({1, a0a, 0});
    transitions0.insert({1, a0b, 1});
    transitions0.insert({2, a0a, 1});
    transitions0.insert({2, a0b, 0});

    ODDs::ODD::StateContainer states1 = {0, 1};

    ODDs::ODD::AlphabetMap alphabet1;
    int a1c = alphabet1.addSymbol("c");
    int a1d = alphabet1.addSymbol("d");
    ODDs::ODD::TransitionContainer transitions1;
    transitions1.insert({0, a1c, 0});
    transitions1.insert({0, a1d, 1});
    transitions1.insert({0, a1c, 0});
    transitions1.insert({0, a1d, 2});
    // Non-deterministic, why not?
    transitions1.insert({0, a1d, 0});

    ODDs::ODD::StateContainer states2 = {0, 1, 2};

    ODDs::ODD::StateContainer finalStates = {2};

    ODDs::ODDBuilder builder(states0);
    builder.addLayer(alphabet0, transitions0, states1);
    builder.addLayer(alphabet1, transitions1, states2);
    builder.setInitialStates(initialStates);
    builder.setFinalStates(finalStates);
    ODDs::ODD odd = builder.build();

    testLayersSanity(odd);
    ASSERT_EQ(2, odd.countLayers());
    checkLayer(odd.getLayer(0), alphabet0, states0, states1, transitions0, true, false);
    checkLayer(odd.getLayer(1), alphabet1, states1, states2, transitions1, false, true);
    ASSERT_EQ(initialStates, odd.getLayer(0).initialStates);
    ASSERT_EQ(finalStates, odd.getLayer(1).finalStates);
}

TEST(ODDBuilder, noLayers) {
    ODDs::ODD::StateContainer states = {0, 1, 2};
    ODDs::ODD::StateContainer initialStates = {1, 2};
    ODDs::ODD::StateContainer finalStates = {0, 1};
    ODDs::ODDBuilder builder(states);
    builder.setInitialStates(initialStates);
    builder.setFinalStates(finalStates);
    ODDs::ODD odd = builder.build();
    testLayersSanity(odd);
}
