#include <Common/DivisionODDs.h>

namespace TestCommon {

ODDs::ODD div2(int length) {
    ODDs::ODD::AlphabetMap binary;
    int zero = binary.addSymbol("0");
    int one = binary.addSymbol("1");

    ODDs::ODDBuilder builder(1);
    builder.setInitialStates({0});
    for (int i = 0; i < length - 1; i++) {
        builder.addLayer(binary, {{0, zero, 0}, {0, one, 0}}, 1);
    }
    builder.addLayer(binary, {{0, zero, 0}, {0, one, 1}}, 2);
    builder.setFinalStates({0});
    return builder.build();
}

ODDs::ODD div3(int length) {
    ODDs::ODD::AlphabetMap binary;
    int zero = binary.addSymbol("0");
    int one = binary.addSymbol("1");

    ODDs::ODDBuilder builder(3);
    builder.setInitialStates({0});
    for (int i = 0; i < length; i++) {
        ODDs::ODD::TransitionContainer transitions = {
            {0, zero, 0},
            {0, one, 1},
            {1, zero, 2},
            {1, one, 0},
            {2, zero, 1},
            {2, one, 2}
        };
        builder.addLayer(binary, transitions, 3);
    }
    builder.setFinalStates({0});
    return builder.build();
}

namespace {

std::vector<std::string> toInput(int n, int length) {
    std::vector<std::string> ret;
    for (int i = 0; i < length; i++)
        ret.push_back((n >> i) & 1 ? "1" : "0");
    std::reverse(ret.begin(), ret.end());
    return ret;
}

}

bool checkPredicate(const ODDs::ODD& odd, std::function<bool(int)> predicate) {
    int n = odd.countLayers();
    for (int i = 0; i < (1 << n); i++) {
        if (odd.accepts(toInput(i, n)) != predicate(i))
            return false;
    }
    return true;
}

}
