#include <ODDs/Operations.h>

#include <algorithm>
#include <iterator>

namespace ODDs {
namespace {

ODD::AlphabetMap alphabetUnion(const ODD::AlphabetMap& lhs,
                               const ODD::AlphabetMap& rhs) {
    ODD::AlphabetMap ret = lhs;
    for (const std::string& symbol : rhs.symbols()) {
        ret.addSymbol(symbol);
    }
    return ret;
}

void addTransitions(ODD::TransitionContainer& transitions,
                    const ODD::AlphabetMap& oldAlphabet,
                    const ODD::AlphabetMap& newAlphabet,
                    const ODD::TransitionContainer& from,
                    int leftIndexOffset,
                    int rightIndexOffset) {
    for (const ODD::Transition& transition : from) {
        const std::string& symb = oldAlphabet.numberToSymbol(transition.symbol);
        transitions.insert({
            transition.from + leftIndexOffset,
            newAlphabet.symbolToNumber(symb),
            transition.to + rightIndexOffset
        });
    }
}

ODD::StateContainer stateSetUnion(const ODD::StateContainer& lhs,
                                  const ODD::StateContainer& rhs,
                                  int rightIndexOffset) {
    ODD::StateContainer ans = lhs;
    for (ODD::State state : rhs)
        ans.insert(ans.end(), state + rightIndexOffset);
    return ans;
}

}

ODD diagramUnion(const ODD& lhs, const ODD& rhs) {
    assert(lhs.countLayers() == rhs.countLayers());

    ODDBuilder builder(lhs.getLayer(0).leftStates + rhs.getLayer(0).leftStates);
    for (int i = 0; i < lhs.countLayers(); i++) {
        ODD::AlphabetMap alphabet = alphabetUnion(lhs.getLayer(i).alphabet,
                                                  rhs.getLayer(i).alphabet);

        ODD::TransitionContainer transitions;
        addTransitions(transitions,
                       lhs.getLayer(i).alphabet,
                       alphabet,
                       lhs.getLayer(i).transitions,
                       0, 0);
        addTransitions(transitions,
                       rhs.getLayer(i).alphabet,
                       alphabet,
                       rhs.getLayer(i).transitions,
                       lhs.getLayer(i).leftStates, lhs.getLayer(i).rightStates);

        int rightStates = lhs.getLayer(i).rightStates
                        + rhs.getLayer(i).rightStates;

        builder.addLayer(alphabet, transitions, rightStates);
    }

    int n = lhs.countLayers();
    builder.setInitialStates(stateSetUnion(lhs.getLayer(0).initialStates,
                                           rhs.getLayer(0).initialStates,
                                           lhs.getLayer(0).leftStates));
    builder.setFinalStates(stateSetUnion(lhs.getLayer(n - 1).finalStates,
                                         rhs.getLayer(n - 1).finalStates,
                                         lhs.getLayer(n - 1).rightStates));

    return builder.build();
}

}
