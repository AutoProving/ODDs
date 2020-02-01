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

#include <ODDs/Operations.h>

#include <algorithm>
#include <cassert>
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

namespace {

using StateSubsetMap = std::map<ODD::StateContainer, int>;
using StateSubsetList = std::vector<ODD::StateContainer>;

StateSubsetList dlpsAddLayer(ODDBuilder& builder,
                             const ODD::Layer& layer,
                             const StateSubsetList& prev) {
    StateSubsetMap toId;
    StateSubsetList toSet;
    int alphabetSize = layer.alphabet.symbols().size();
    ODD::TransitionContainer transitions;
    for (int i = 0; i < (int)prev.size(); i++) {
        const ODD::StateContainer& from = prev[i];
        for (int symbol = 0; symbol < alphabetSize; symbol++) {
            ODD::StateContainer to;
            for (ODD::State state : from) {
                auto range = layer.transitions.proceed(state, symbol);
                for (const ODD::Transition& t : range) {
                    to.insert(t.to);
                }
            }
            int next;
            auto it = toId.lower_bound(to);
            if (it != toId.end() && it->first == to) {
                next = it->second;
            } else {
                next = toSet.size();
                toId.insert(it, {to, next});
                toSet.emplace_back(std::move(to));
            }
            transitions.insert({i, symbol, next});
        }
    }
    builder.addLayer(
        layer.alphabet,
        transitions,
        toSet.size()
    );
    return toSet;
}

bool intersects(const ODD::StateContainer& lhs, const ODD::StateContainer& rhs) {
    auto it = lhs.begin();
    auto jt = rhs.begin();
    while (it != lhs.end() && jt != rhs.end()) {
        if (*it == *jt)
            return true;
        if (*it < *jt)
            it++;
        else
            jt++;
    }
    return false;
}

ODD::StateContainer dlpsFinalStates(const ODD::Layer& rightLayer,
                                    const StateSubsetList& prev) {
    ODD::StateContainer ret;
    for (int i = 0; i < (int)prev.size(); i++) {
        const ODD::StateContainer& state = prev[i];
        if (intersects(state, rightLayer.finalStates))
            ret.insert(i);
    }
    return ret;
}

}

ODD diagramLazyPowerSet(const ODD& odd) {
    ODDBuilder builder(1);
    builder.setInitialStates({0});
    StateSubsetList next = {odd.initialStates()};
    for (int i = 0; i < odd.countLayers(); i++) {
        next = dlpsAddLayer(builder, odd.getLayer(i), next);
    }
    builder.setFinalStates(
        dlpsFinalStates(
            odd.getLayer(odd.countLayers() - 1),
            next
        )
    );
    return builder.build();
}

namespace {

using LayerReachabilityMap = std::vector<bool>;
using ReachabilityMap = std::vector<LayerReachabilityMap>;

ReachabilityMap reachabilityMap(const ODD& odd) {
    ReachabilityMap ret(odd.countLayers() + 1);
    ret[0].resize(odd.getLayer(0).leftStates, false);
    for (int u : odd.initialStates())
        ret[0][u] = true;
    for (int i = 0; i < odd.countLayers(); i++) {
        const auto& layer = odd.getLayer(i);
        int alphabetSize = layer.alphabet.symbols().size();
        ret[i + 1].resize(layer.rightStates);
        for (int j = 0; j < layer.leftStates; j++) {
            if (!ret[i][j]) {
                continue;
            }
            // Now j-th state of i-th layer is reachable from initial states.
            for (int symbol = 0; symbol < alphabetSize; symbol++) {
                for (const auto& t : layer.transitions.proceed(j, symbol)) {
                    ret[i + 1][t.to] = true;
                }
            }
        }
    }
    return ret;
}

/**
 * For each vertex of a layer of vertices, stores either an id of its component
 * or -1 in case the vertex isn't reachable from initial states.
 */
using ComponentMap = std::vector<int>;

/**
 * @param reachable Reachability map for the rightmost layer of vertices.
 */
ComponentMap glueRightLayer(const ODD::Layer& layer,
                            const LayerReachabilityMap& reachable) {
    ComponentMap ret(layer.rightStates);
    for (int i = 0; i < layer.rightStates; i++)
        ret[i] = reachable[i] ? 1 : -1;
    for (int u : layer.finalStates)
        ret[u] = 0;
    if (layer.finalStates.empty()) {
        for (int& u : ret) {
            if (u == 1) {
                u--;
            }
        }
    }
    return ret;
}

/**
 * For each symbol a component of state to which a state has transition by this
 * symbol.
 */
using TransitionMap = std::vector<int>;

TransitionMap stateTransitionMap(const ODD::Layer& layer,
                                 int state,
                                 const ComponentMap& next) {
    int alphabetSize = layer.alphabet.symbols().size();
    TransitionMap ret(alphabetSize);
    for (int symbol = 0; symbol < alphabetSize; symbol++) {
        ret[symbol] = next[layer.transitions.go(state, symbol)];
    }
    return ret;
}

using ComponentStateMap = std::map<TransitionMap, int>;

/**
 * @param reachable Reachability map for the left layer of vertices.
 * @param next      Component map for the right layer of vertices.
 */
ComponentMap glueLayer(const ODD::Layer& layer,
                       const LayerReachabilityMap& reachable,
                       const ComponentMap& next) {
    ComponentMap ret(layer.leftStates, -1);
    ComponentStateMap comps;
    for (int i = 0; i < layer.leftStates; i++) {
        if (!reachable[i])
            continue;
        auto tm = stateTransitionMap(layer, i, next);
        auto it = comps.lower_bound(tm);
        if (it != comps.end() && it->first == tm) {
            ret[i] = it->second;
        } else {
            ret[i] = comps.size();
            comps.insert(it, {tm, comps.size()});
        }
    }
    return ret;
}

int countClasses(const ComponentMap& map) {
    return (*std::max_element(map.begin(), map.end())) + 1;
}

/**
 * For each class an ID of some of its states or -1 in case the class is empty
 * (empty classes can occur in case of empty language of given ODD).
 */
using ExemplarList = std::vector<int>;

ExemplarList exemplarList(const ComponentMap& map) {
    ExemplarList ret(countClasses(map), -1);
    for (int i = 0; i < (int)map.size(); i++) {
        if (map[i] == -1)
            continue;
        if (ret[map[i]] == -1)
            ret[map[i]] = i;
    }
    return ret;
}

void minAddLayer(ODDBuilder& builder,
                 const ODD::Layer& oldLayer,
                 const ComponentMap& leftMap,
                 const ComponentMap& rightMap) {
    int alphabetSize = oldLayer.alphabet.symbols().size();
    ExemplarList some = exemplarList(leftMap);
    ODD::TransitionContainer transitions;
    for (int i = 0; i < (int)some.size(); i++) {
        if (some[i] == -1)
            continue;
        for (int symbol = 0; symbol < alphabetSize; symbol++) {
            int to = rightMap[oldLayer.transitions.go(some[i], symbol)];
            transitions.insert({i, symbol, to});
        }
    }
    builder.addLayer(
        oldLayer.alphabet,
        transitions,
        countClasses(rightMap)
    );
}

}

ODD minimize(ODD odd) {
    if (!isDeterministic(odd) || !isComplete(odd)) {
        odd = diagramLazyPowerSet(odd);
    }

    int n = odd.countLayers();
    ReachabilityMap reachable = reachabilityMap(odd);
    std::vector<ComponentMap> maps(odd.countLayers() + 1);
    maps.back() = glueRightLayer(odd.getLayer(n - 1), reachable.back());
    for (int i = n - 1; i >= 0; i--) {
        maps[i] = glueLayer(odd.getLayer(i), reachable[i], maps[i + 1]);
    }

    ODDBuilder builder(1);
    builder.setInitialStates({0});
    for (int i = 0; i < n; i++) {
        minAddLayer(builder, odd.getLayer(i), maps[i], maps[i + 1]);
    }
    if (!odd.finalStates().empty()) {
        // The ID of final state of the rightmost layer is 0.
        builder.setFinalStates({0});
    } else {
        builder.setFinalStates({});
    }

    return builder.build();
}

}
