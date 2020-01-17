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

namespace {

std::string glueSymbolPair(const std::string& a, const std::string& b) {
    return a + "," + b;
}

ODD::AlphabetMap alphabetCrossProduct(const ODD::AlphabetMap& lhs,
                                      const ODD::AlphabetMap& rhs) {
    ODD::AlphabetMap ret;
    for (const std::string& a : lhs.symbols())
        for (const std::string& b : rhs.symbols())
            ret.addSymbol(glueSymbolPair(a, b));
    return ret;
}

ODD::TransitionContainer tensorProductTransitions(const ODD::TransitionContainer& lt,
                                                  const ODD::AlphabetMap& la,
                                                  const ODD::TransitionContainer& rt,
                                                  const ODD::AlphabetMap& ra,
                                                  int rlsz, int rrsz,
                                                  const ODD::AlphabetMap& ja) {
    ODD::TransitionContainer transitions;
    for (const ODD::Transition& ltt : lt) {
        for (const ODD::Transition& rtt : rt) {
            ODD::Symbol symbol = ja.symbolToNumber(
                glueSymbolPair(
                    la.numberToSymbol(ltt.symbol),
                    ra.numberToSymbol(rtt.symbol)
                )
            );
            transitions.insert({
                ltt.from * rlsz + rtt.from,
                symbol,
                ltt.to * rrsz + rtt.to
            });
        }
    }
    return transitions;
}

}

ODD diagramTensorProduct(const ODD& lhs, const ODD& rhs) {
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
            alphabetCrossProduct(
                lhs.getLayer(i).alphabet,
                rhs.getLayer(i).alphabet
            );
        ODD::TransitionContainer transitions =
            tensorProductTransitions(
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

namespace {

ODD::StateContainer negateStates(const ODD::StateContainer& states, int n) {
    auto it = states.begin();
    ODD::StateContainer ret;
    for (int state = 0; state < n; state++) {
        if (it != states.end() && state == *it)
            it++;
        else
            ret.insert(ret.end(), state);
    }
    return ret;
}

}

ODD diagramNegation(const ODD& odd) {
    ODDBuilder builder(odd.getLayer(0).leftStates);
    builder.setInitialStates(odd.initialStates());
    for (int i = 0; i < odd.countLayers(); i++) {
        builder.addLayer(
            odd.getLayer(i).alphabet,
            odd.getLayer(i).transitions,
            odd.getLayer(i).rightStates
        );
    }
    builder.setFinalStates(
        negateStates(
            odd.finalStates(),
            odd.getLayer(odd.countLayers() - 1).rightStates
        )
    );
    return builder.build();
}

namespace {

ODD::AlphabetMap alphabetMapping(const AlphabetMapping& g,
                                 const ODD::AlphabetMap& alphabet) {
    ODD::AlphabetMap ret;
    for (const std::string& symbol : alphabet.symbols())
        ret.addSymbol(g(symbol));
    return ret;
}

ODD::TransitionContainer transitionMapping(const AlphabetMapping& g,
                                           const ODD::TransitionContainer& ts,
                                           const ODD::AlphabetMap& oldAlphabet,
                                           const ODD::AlphabetMap& newAlphabet) {
    ODD::TransitionContainer ret;
    for (const ODD::Transition& transition : ts) {
        std::string symbol = oldAlphabet.numberToSymbol(transition.symbol);
        ret.insert({
            transition.from,
            newAlphabet.symbolToNumber(g(symbol)),
            transition.to
        });
    }
    return ret;
}

}

ODD diagramMapping(const std::vector<AlphabetMapping>& gs, const ODD& odd) {
    assert((int)gs.size() == odd.countLayers());
    ODDBuilder builder(odd.getLayer(0).leftStates);
    builder.setInitialStates(odd.initialStates());
    for (int i = 0; i < odd.countLayers(); i++) {
        ODD::AlphabetMap oldAlphabet = odd.getLayer(i).alphabet;
        ODD::AlphabetMap newAlphabet = alphabetMapping(gs[i], oldAlphabet);
        ODD::TransitionContainer transitions = transitionMapping(
            gs[i],
            odd.getLayer(i).transitions,
            oldAlphabet,
            newAlphabet
        );
        int rightStates = odd.getLayer(i).rightStates;
        builder.addLayer(newAlphabet, transitions, rightStates);
    }
    builder.setFinalStates(odd.finalStates());
    return builder.build();
}

namespace {

using InvMap = std::map<std::string, std::vector<std::string>>;

InvMap alphabetInvMap(const AlphabetMapping& g, const ODD::AlphabetMap& a) {
    InvMap ret;
    for (std::string symbol : a.symbols())
        ret[g(symbol)].emplace_back(symbol);
    return ret;
}

ODD::TransitionContainer transitionInvMapping(const AlphabetMapping& g,
                                              const ODD::TransitionContainer& ts,
                                              const ODD::AlphabetMap& oldAlphabet,
                                              const ODD::AlphabetMap& newAlphabet) {
    InvMap invMap = alphabetInvMap(g, newAlphabet);
    ODD::TransitionContainer ret;
    for (const ODD::Transition& transition : ts) {
        std::string oldSymbol = oldAlphabet.numberToSymbol(transition.symbol);
        for (const std::string& symbol: invMap[oldSymbol]) {
            ret.insert({
                transition.from,
                newAlphabet.symbolToNumber(symbol),
                transition.to
            });
        }
    }
    return ret;
}

}

ODD diagramInverseMapping(const std::vector<AlphabetMapping>& gs,
                          const std::vector<ODD::AlphabetMap>& newAlphabets,
                          const ODD& odd) {
    assert((int)gs.size() == odd.countLayers());
    assert((int)newAlphabets.size() == odd.countLayers());
    ODDBuilder builder(odd.getLayer(0).leftStates);
    builder.setInitialStates(odd.initialStates());
    for (int i = 0; i < odd.countLayers(); i++) {
        ODD::AlphabetMap oldAlphabet = odd.getLayer(i).alphabet;
        const ODD::AlphabetMap& newAlphabet = newAlphabets[i];
        ODD::TransitionContainer transitions = transitionInvMapping(
            gs[i],
            odd.getLayer(i).transitions,
            oldAlphabet,
            newAlphabet
        );
        int rightStates = odd.getLayer(i).rightStates;
        builder.addLayer(newAlphabet, transitions, rightStates);
    }
    builder.setFinalStates(odd.finalStates());
    return builder.build();
}

bool isDeterministic(const ODD& odd) {
    const ODD::Layer& leftLayer = odd.getLayer(0);
    if (leftLayer.initialStates.size() != 1)
        return false;
    if (leftLayer.leftStates != 1)
        return false;

    for (int i = 0; i < odd.countLayers(); i++) {
        const ODD::Layer& layer = odd.getLayer(i);
        ODD::Transition prev;
        bool first = true;
        for (const ODD::Transition& transition : layer.transitions) {
            if (!first
                && prev.from == transition.from
                && prev.symbol == transition.symbol) {
                return false;
            }
            prev = transition;
            first = false;
        }
    }
    return true;
}

bool isComplete(const ODD& odd) {
    for (int i = 0; i < odd.countLayers(); i++) {
        const ODD::Layer& layer = odd.getLayer(i);
        int alphabetSize = layer.alphabet.symbols().size();
        for (int state = 0; state < layer.leftStates; state++) {
            for (int symbol = 0; symbol < alphabetSize; symbol++) {
                if (!layer.transitions.hasKey(state, symbol)) {
                    return false;
                }
            }
        }
    }
    return true;
}

namespace {

ODD::TransitionContainer dpsLeftTransitions(const ODD::Layer& leftLayer) {
    int leftAlphabetSize = leftLayer.alphabet.symbols().size();
    ODD::TransitionContainer leftTransitions;
    for (int symbol = 0; symbol < leftAlphabetSize; symbol++) {
        int go = 0;
        for (ODD::State state : leftLayer.initialStates) {
            auto range = leftLayer.transitions.proceed(state, symbol);
            for (const ODD::Transition& t : range) {
                go |= 1 << t.to;
            }
        }
        leftTransitions.insert({0, symbol, go});
    }
    return leftTransitions;
}

void dpsAddLayer(ODDBuilder& builder, const ODD::Layer& layer) {
    int alphabetSize = layer.alphabet.symbols().size();
    ODD::TransitionContainer transitions;
    for (int from = 0; from < (1 << layer.leftStates); from++) {
        for (int symbol = 0; symbol < alphabetSize; symbol++) {
            int to = 0;
            for (int state = 0; state < layer.leftStates; state++) {
                if (!((from >> state) & 1))
                    continue;
                auto range = layer.transitions.proceed(state, symbol);
                for (const ODD::Transition& t : range)
                    to |= 1 << t.to;
            }
            transitions.insert({from, symbol, to});
        }
    }
    builder.addLayer(
        layer.alphabet,
        transitions,
        1 << layer.rightStates
    );
}

ODD::StateContainer dpsFinalStates(const ODD::Layer& rightLayer) {
    int finalStatesMask = 0;
    for (int state : rightLayer.finalStates)
        finalStatesMask |= 1 << state;
    ODD::StateContainer ret;
    for (int i = 0; i < (1 << rightLayer.rightStates); i++) {
        if (i & finalStatesMask)
            ret.insert(i);
    }
    return ret;
}

}

ODD diagramPowerSet(const ODD& odd) {
    ODDBuilder builder(1);
    builder.setInitialStates({0});
    builder.addLayer(
        odd.getLayer(0).alphabet,
        dpsLeftTransitions(odd.getLayer(0)),
        1 << odd.getLayer(0).rightStates
    );
    for (int i = 1; i < odd.countLayers(); i++) {
        dpsAddLayer(builder, odd.getLayer(i));
    }
    builder.setFinalStates(
        dpsFinalStates(
            odd.getLayer(
                odd.countLayers() - 1
            )
        )
    );
    return builder.build();
}

}
