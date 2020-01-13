#include <ODDs.h>

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

const std::string& ODD::AlphabetMap::numberToSymbol(int number) const {
    return n2s_[number];
}

int ODD::AlphabetMap::symbolToNumber(const std::string& symbol) const {
    auto it = s2n_.find(symbol);
    if (it == s2n_.end())
        return -1;
    return it->second;
}

int ODD::Layer::width() const {
    return std::max<int>(leftStates->size(), rightStates->size());
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
    Impl(const ODD::StateContainer& firstLayerLeft)
        : states_({firstLayerLeft})
    {}

    void addLayer(const ODD::AlphabetMap& alphabet,
                  const ODD::TransitionContainer& transitions,
                  const ODD::StateContainer& rightStates) {
        alphabets_.push_back(alphabet);
        transitions_.push_back(transitions);
        states_.push_back(rightStates);
    }

    void setInitialStates(const ODD::StateContainer& initialStates) {
        initialStates_ = initialStates;
    }

    void setFinalStates(const ODD::StateContainer& finalStates) {
        finalStates_ = finalStates;
    }

    ODD build() {
        ODD ret;
        ret.states_ = std::move(states_);
        ret.layers_.resize(alphabets_.size());
        for (int i = 0; i < (int)alphabets_.size(); i++) {
            ret.layers_[i].alphabet = std::move(alphabets_[i]);
            ret.layers_[i].leftStates = &ret.states_[i];
            ret.layers_[i].rightStates = &ret.states_[i + 1];
            ret.layers_[i].transitions = transitions_[i];
            ret.layers_[i].isInitial = i == 0;
            ret.layers_[i].isFinal = i + 1 == (int)ret.states_.size();
        }
        ret.layers_[0].initialStates = std::move(initialStates_);
        ret.layers_.back().finalStates = std::move(finalStates_);
        return ret;
    }

private:
    std::vector<ODD::StateContainer> states_;
    std::vector<ODD::AlphabetMap> alphabets_;
    std::vector<ODD::TransitionContainer> transitions_;
    ODD::StateContainer initialStates_;
    ODD::StateContainer finalStates_;
};

ODDBuilder::ODDBuilder(const ODD::StateContainer& firstLayerLeft)
    : impl_(std::make_unique<ODDBuilder::Impl>(firstLayerLeft))
{}

void ODDBuilder::addLayer(const ODD::AlphabetMap& alphabet,
                          const ODD::TransitionContainer& transitions,
                          const ODD::StateContainer& rightStates) {
    impl_->addLayer(alphabet, transitions, rightStates);
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

}
