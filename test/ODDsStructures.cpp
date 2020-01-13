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
