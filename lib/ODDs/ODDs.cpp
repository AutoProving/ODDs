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
