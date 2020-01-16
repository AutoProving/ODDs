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
    builder.setInitialStates(stateSetUnion(lhs.initialStates(),
                                           rhs.initialStates(),
                                           lhs.getLayer(0).leftStates));
    builder.setFinalStates(stateSetUnion(lhs.finalStates(),
                                         rhs.finalStates(),
                                         lhs.getLayer(n - 1).rightStates));

    return builder.build();
}

namespace {

ODD::AlphabetMap alphabetIntersection(const ODD::AlphabetMap& lhs,
                                      const ODD::AlphabetMap& rhs) {
    ODD::AlphabetMap ret;
    for (const std::string& symbol : lhs.symbols()) {
        if (rhs.contains(symbol))
            ret.addSymbol(symbol);
    }
    return ret;
}

using SymbolTransitions = std::vector<std::vector<ODD::Transition>>;

SymbolTransitions transitionsBySymbol(const ODD::TransitionContainer& transitions,
                                      const ODD::AlphabetMap& alphabet) {
    SymbolTransitions ans(alphabet.symbols().size());
    for (const ODD::Transition& transition : transitions) {
        ans[transition.symbol].push_back(transition);
    }
    return ans;
}

ODD::TransitionContainer intersectionTransitions(const ODD::TransitionContainer& lt,
                                                 const ODD::AlphabetMap& la,
                                                 const ODD::TransitionContainer& rt,
                                                 const ODD::AlphabetMap& ra,
                                                 int rlsz, int rrsz,
                                                 const ODD::AlphabetMap& ja) {
    ODD::TransitionContainer ans;
    SymbolTransitions lst = transitionsBySymbol(lt, la);
    SymbolTransitions rst = transitionsBySymbol(rt, ra);
    for (const std::string& symbol : ja.symbols()) {
        ODD::Symbol li = la.symbolToNumber(symbol);
        ODD::Symbol ri = ra.symbolToNumber(symbol);
        ODD::Symbol ji = ja.symbolToNumber(symbol);
        for (const ODD::Transition& ltr : lst[li]) {
            for (const ODD::Transition& rtr : rst[ri]) {
                int fromId = rlsz * ltr.from + rtr.from;
                int toId = rrsz * ltr.to  + rtr.to;
                ans.insert({fromId, ji, toId});
            }
        }
    }
    return ans;
}

ODD::StateContainer crossProduct(const ODD::StateContainer& lhs,
                                 const ODD::StateContainer& rhs,
                                 int rightSize) {
    ODD::StateContainer ans;
    for (const ODD::State& ls : lhs)
        for (const ODD::State& rs : rhs)
            ans.insert(ls * rightSize + rs);
    return ans;
}

}

ODD diagramIntersection(const ODD& lhs, const ODD& rhs) {
    assert(lhs.countLayers() == rhs.countLayers());

    ODDBuilder builder(lhs.getLayer(0).leftStates * rhs.getLayer(0).leftStates);
    builder.setInitialStates(
        crossProduct(
            lhs.initialStates(),
            rhs.initialStates(),
            rhs.getLayer(0).leftStates
        )
    );
    for (int i = 0; i < lhs.countLayers(); i++) {
        ODD::AlphabetMap alphabet =
            alphabetIntersection(
                lhs.getLayer(i).alphabet,
                rhs.getLayer(i).alphabet
            );
        ODD::TransitionContainer transitions =
            intersectionTransitions(
                lhs.getLayer(i).transitions,
                lhs.getLayer(i).alphabet,
                rhs.getLayer(i).transitions,
                rhs.getLayer(i).alphabet,
                rhs.getLayer(i).leftStates,
                rhs.getLayer(i).rightStates,
                alphabet
            );
        int rightSize =
            lhs.getLayer(i).rightStates * rhs.getLayer(i).rightStates;
        builder.addLayer(alphabet, transitions, rightSize);
    }
    int n = lhs.countLayers();
    builder.setFinalStates(
        crossProduct(
            lhs.finalStates(),
            rhs.finalStates(),
            rhs.getLayer(n - 1).rightStates
        )
    );
    return builder.build();
}

}
