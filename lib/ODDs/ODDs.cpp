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

#include <ODDs/ODDs.h>

namespace ODDs {

ODD::Transition::Transition(ODD::State fromState, ODD::Symbol bySymbol, ODD::State stateTo)
    : from(fromState)
    , symbol(bySymbol)
    , to(stateTo)
{}

bool ODD::Transition::operator==(const ODD::Transition& rhs) const {
    return from == rhs.from
        && symbol == rhs.symbol
        && to == rhs.to;
}

bool ODD::Transition::operator<(const ODD::Transition& rhs) const {
    if (from != rhs.from)
        return from < rhs.from;
    if (symbol != rhs.symbol)
        return symbol < rhs.symbol;
    return to < rhs.to;
}

bool ODD::TransitionKey::operator<(const ODD::TransitionKey& rhs) const {
    if (from != rhs.from)
        return from < rhs.from;
    return symbol < rhs.symbol;
}

bool ODD::TransitionKey::operator==(const ODD::TransitionKey& rhs) const {
    return from == rhs.from
        && symbol == rhs.symbol;
}

using TCIterator = ODD::TransitionContainer::Iterator;

bool TCIterator::operator==(const TCIterator& rhs) const {
    return it_ == rhs.it_;
}

bool TCIterator::operator!=(const TCIterator& rhs) const {
    return it_ != rhs.it_;
}

TCIterator::reference TCIterator::operator*() {
    updateStored_();
    return stored_;
}

TCIterator::pointer TCIterator::operator->() {
    updateStored_();
    return &stored_;
}

TCIterator& TCIterator::operator++() {
    ++it_;
    return *this;
}

TCIterator TCIterator::operator++(int) {
    TCIterator ret(it_);
    ++it_;
    return ret;
}

TCIterator& TCIterator::operator--() {
    --it_;
    return *this;
}

TCIterator TCIterator::operator--(int) {
    TCIterator ret(it_);
    --it_;
    return ret;
}

TCIterator::Iterator(ODD::TransitionContainer::BaseIterator it)
    : it_(it)
{}

void TCIterator::updateStored_() {
    stored_.from = it_->first.from;
    stored_.symbol = it_->first.symbol;
    stored_.to = it_->second;
}

using TC = ODD::TransitionContainer;

TC::Range::Range(Iterator from, Iterator to)
    : b_(from)
    , e_(to)
{}

TCIterator TC::Range::begin() const {
    return b_;
}

TCIterator TC::Range::end() const {
    return e_;
}

TC::TransitionContainer(std::initializer_list<Transition> elements) {
    for (const Transition& transition : elements)
        insert(transition);
}

TCIterator TC::begin() const {
    return TCIterator(m_.begin());
}

TCIterator TC::end() const {
    return TCIterator(m_.end());
}

void TC::insert(const Transition& transition) {
    auto it = p_.lower_bound(transition);
    if (it != p_.end() && *it == transition)
        return;
    p_.insert(it, transition);

    Base::value_type element = {
        {transition.from, transition.symbol},
        transition.to
    };
    m_.insert(std::move(element));
}

TC::Range TC::proceed(const ODD::TransitionKey& key) const {
    auto range = m_.equal_range(key);
    return {range.first, range.second};
}

TC::Range TC::proceed(State from, Symbol symbol) const {
    return proceed({from, symbol});
}

ODD::State TC::go(const ODD::TransitionKey& key) const {
    return m_.lower_bound(key)->second;
}

ODD::State TC::go(ODD::State from, ODD::Symbol symbol) const {
    return go({from, symbol});
}

std::size_t TC::size() const {
    return m_.size();
}

bool TC::operator==(const TC& rhs) const {
    return m_ == rhs.m_;
}

bool TC::hasKey(const ODD::TransitionKey& key) const {
    return m_.count(key);
}

bool TC::hasKey(State from, Symbol symbol) const {
    return hasKey({from, symbol});
}

TC::Range TC::proceedFrom(State from) const {
    const auto min = std::numeric_limits<ODD::Symbol>::min();
    const auto max = std::numeric_limits<ODD::Symbol>::max();
    return Range(m_.lower_bound({from, min}), m_.upper_bound({from, max}));
}

int ODD::AlphabetMap::addSymbol(const std::string& symbol) {
    auto it = s2n_.lower_bound(symbol);
    if (it != s2n_.end() && it->first == symbol) {
        return it->second;
    }
    int newId = n2s_.size();
    s2n_.insert(it, std::make_pair(symbol, newId));
    n2s_.push_back(symbol);
    return newId;
}

const std::string& ODD::AlphabetMap::numberToSymbol(ODD::Symbol number) const {
    return n2s_[number];
}

ODD::Symbol ODD::AlphabetMap::symbolToNumber(const std::string& symbol) const {
    auto it = s2n_.find(symbol);
    if (it == s2n_.end())
        return -1;
    return it->second;
}

const std::vector<std::string>& ODD::AlphabetMap::symbols() const {
    return n2s_;
}

bool ODD::AlphabetMap::operator==(const ODD::AlphabetMap& rhs) const {
    return n2s_ == rhs.n2s_;
}

bool ODD::AlphabetMap::contains(const std::string& symbol) const {
    return s2n_.count(symbol);
}

int ODD::Layer::width() const {
    return std::max<int>(leftStates, rightStates);
}

bool ODD::Layer::checkSanity() const {
    if (!isInitial && !initialStates.empty())
        return false;
    if (!isFinal && !finalStates.empty())
        return false;
    return true;
}

ODD::ODD() = default;

int ODD::countLayers() const {
    return layers_.size();
}

const ODD::Layer& ODD::getLayer(int i) const {
    return layers_[i];
}

const ODD::StateContainer& ODD::initialStates() const {
    return getLayer(0).initialStates;
}

const ODD::StateContainer& ODD::finalStates() const {
    return getLayer(countLayers() - 1).finalStates;
}

namespace {

bool intersects(const ODD::StateContainer& lhs,
                const ODD::StateContainer& rhs) {
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

}

bool ODD::accepts(const std::vector<std::string>& string) const {
    assert((int)string.size() == countLayers());
    StateContainer curStates = initialStates();
    for (int i = 0; i < countLayers(); i++) {
        Symbol symbol = getLayer(i).alphabet.symbolToNumber(string[i]);
        StateContainer nextStates;
        for (const State& cur : curStates) {
            TransitionKey key = {cur, symbol};
            for (const Transition& t : getLayer(i).transitions.proceed(key)) {
                nextStates.insert(t.to);
            }
        }
        curStates = nextStates;
    }
    return intersects(curStates, finalStates());
}

class ODDBuilder::Impl {
public:
    Impl(int firstLayerCount)
        : stateCounts_({firstLayerCount})
    {}

    void addLayer(const ODD::AlphabetMap& alphabet,
                  const ODD::TransitionContainer& transitions,
                  int rightStateCount) {
        alphabets_.push_back(alphabet);
        transitions_.push_back(transitions);
        stateCounts_.push_back(rightStateCount);
    }

    void setInitialStates(const ODD::StateContainer& initialStates) {
        initialStates_ = initialStates;
    }

    void setFinalStates(const ODD::StateContainer& finalStates) {
        finalStates_ = finalStates;
    }

    ODD build() {
        // We try to use std::move whereever possible.
        ODD ret;
        ret.layers_.resize(alphabets_.size());
        for (int i = 0; i < (int)alphabets_.size(); i++) {
            ret.layers_[i].alphabet = std::move(alphabets_[i]);
            ret.layers_[i].leftStates = stateCounts_[i];
            ret.layers_[i].rightStates = stateCounts_[i + 1];
            ret.layers_[i].transitions = std::move(transitions_[i]);
            ret.layers_[i].isInitial = i == 0;
            ret.layers_[i].isFinal = i + 2 == (int)stateCounts_.size();
        }
        if (!ret.layers_.empty()) {
            ret.layers_[0].initialStates = std::move(initialStates_);
            ret.layers_.back().finalStates = std::move(finalStates_);
        }
        return ret;
    }

private:
    std::vector<int> stateCounts_;
    std::vector<ODD::AlphabetMap> alphabets_;
    std::vector<ODD::TransitionContainer> transitions_;
    ODD::StateContainer initialStates_;
    ODD::StateContainer finalStates_;
};

ODDBuilder::ODDBuilder(int leftStateCount)
    : impl_(std::make_unique<ODDBuilder::Impl>(leftStateCount))
{}

ODDBuilder::~ODDBuilder() = default;

void ODDBuilder::addLayer(const ODD::AlphabetMap& alphabet,
                          const ODD::TransitionContainer& transitions,
                          int rightStateCount) {
    impl_->addLayer(alphabet, transitions, rightStateCount);
}

void ODDBuilder::setInitialStates(const ODD::StateContainer& initialStates) {
    impl_->setInitialStates(initialStates);
}

void ODDBuilder::setFinalStates(const ODD::StateContainer& finalStates) {
    impl_->setFinalStates(finalStates);
}

ODD ODDBuilder::build() {
    return impl_->build();
}

namespace {

ODD::StateContainer readStateSet(std::istream& is) {
    int size;
    is >> size;
    ODD::StateContainer states;
    for (int i = 0; i < size; i++) {
        int state;
        is >> state;
        states.insert(state);
    }
    return states;
}

ODD::AlphabetMap readAlphabetMap(std::istream& is) {
    int size;
    is >> size;
    ODD::AlphabetMap alphabet;
    for (int i = 0; i < size; i++) {
        std::string symbol;
        is >> symbol;
        alphabet.addSymbol(symbol);
    }
    return alphabet;
}

ODD::TransitionContainer readTransitionSet(const ODD::AlphabetMap& alphabet, std::istream& is) {
    int size;
    is >> size;
    ODD::TransitionContainer transitions;
    for (int i = 0; i < size; i++) {
        int from, to;
        std::string symbol;
        is >> from >> symbol >> to;
        transitions.insert({from, alphabet.symbolToNumber(symbol), to});
    }
    return transitions;
}

}

ODD readFromIStream(std::istream& is) {
    int layers;
    is >> layers;
    ODD::StateContainer initialStates = readStateSet(is);
    int leftStates;
    is >> leftStates;
    ODDBuilder builder(leftStates);
    for (int layer = 0; layer < layers; layer++) {
        ODD::AlphabetMap alphabet = readAlphabetMap(is);
        ODD::TransitionContainer transitions = readTransitionSet(alphabet, is);
        int rightStates;
        is >> rightStates;
        builder.addLayer(alphabet, transitions, rightStates);
    }
    builder.setInitialStates(initialStates);
    builder.setFinalStates(readStateSet(is));
    return builder.build();
}

namespace {

void writeStateSet(std::ostream& os, const ODD::StateContainer& states) {
    os << states.size() << std::endl;
    for (int state : states)
        os << state << " ";
    os << std::endl;
}

void writeAlphabetMap(std::ostream& os, const ODD::AlphabetMap& alphabet) {
    os << alphabet.symbols().size() << std::endl;
    for (const std::string& symbol :  alphabet.symbols())
        os << symbol << " ";
    os << std::endl;
}

void writeTransitionSet(std::ostream& os,
                        const ODD::AlphabetMap& alphabet,
                        const ODD::TransitionContainer& transitions) {
    os << transitions.size() << std::endl;
    for (const ODD::Transition& transition : transitions) {
        os << transition.from << " "
           << alphabet.numberToSymbol(transition.symbol) << " "
           << transition.to << std::endl;
    }
}

}

void writeToOStream(std::ostream& os, const ODD& odd) {
    os << odd.countLayers() << std::endl;
    writeStateSet(os, odd.getLayer(0).initialStates);
    os << odd.getLayer(0).leftStates << std::endl;
    for (int layer = 0; layer < odd.countLayers(); layer++) {
        const auto& alphabet = odd.getLayer(layer).alphabet;
        writeAlphabetMap(os, alphabet);
        writeTransitionSet(os, alphabet, odd.getLayer(layer).transitions);
        os << odd.getLayer(layer).rightStates << std::endl;
    }
    writeStateSet(os, odd.getLayer(odd.countLayers() - 1).finalStates);
}

}
