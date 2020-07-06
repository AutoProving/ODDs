// Copyright (c) 2019-2020 Vasily Alferov, Mateus de Oliveira Oliveira and Contributors. 
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
#include <ODDs/JSONDump.h>

#include <cassert>
#include <filesystem>
#include <fstream>

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

ODD::ODD()
    : layers_()
    , mode_(Mode::Memory)
{}

ODD::ODD(const std::string& dirName)
    : layers_()
    , mode_(Mode::Disk)
    , loaded_(-1)
    , dirName_(dirName)
{}

ODD::~ODD() {
    if (mode_ == Mode::Disk && !detached_) {
        namespace fs = std::filesystem;
        fs::remove_all(fs::path(dirName_));
    }
}

ODD::ODD(ODD&& other) {
    mode_ = other.mode_;
    loaded_ = other.loaded_;
    dirName_ = std::move(other.dirName_);
    layers_ = std::move(other.layers_);
    detached_ = other.detached_;
    other.detached_ = true;
}

ODD& ODD::operator=(ODD&& rhs) {
    std::swap(mode_, rhs.mode_);
    std::swap(loaded_, rhs.loaded_);
    std::swap(dirName_, rhs.dirName_);
    std::swap(layers_, rhs.layers_);
    std::swap(detached_, rhs.detached_);
    return *this;
}

int ODD::countLayers() const {
    return countLayers_;
}

namespace {

void unloadLayer(ODD::Layer& layer) {
    layer.alphabet = {};
    layer.initialStates.clear();
    layer.finalStates.clear();
    layer.transitions = {};
}

std::string layerFileName(int i) {
    return "layer" + std::to_string(i) + ".json";
}

void loadLayer(ODD::Layer& layer, const std::string& dirName, int i) {
    namespace fs = std::filesystem;
    fs::path path = fs::path(dirName) / layerFileName(i);
    std::ifstream fin(path);
    layer = readLayerJSON(fin);
}

void writeLayer(const ODD::Layer& layer, const std::string& dirName, int i) {
    namespace fs = std::filesystem;
    fs::path path = fs::path(dirName) / layerFileName(i);
    std::ofstream fout(path);
    writeJSON(fout, layer);
}

}

const ODD::Layer& ODD::getLayer(int i) const {
    if (mode_ == Mode::Memory) {
        return layers_[i];
    }
    // Else mode_ == Mode::Disk
    if (loaded_ != i) {
        if (loaded_ != -1) {
            unloadLayer(loadedLayer_);
        }
        loadLayer(loadedLayer_, dirName_, i);
        loaded_ = i;
    }
    return loadedLayer_;
}

const ODD::StateContainer& ODD::initialStates() const {
    return getLayer(0).initialStates;
}

const ODD::StateContainer& ODD::finalStates() const {
    return getLayer(countLayers() - 1).finalStates;
}

void ODD::detachDir() {
    detached_ = true;
}

void ODD::unload() const {
    if (loaded_ != -1) {
        unloadLayer(loadedLayer_);
        loaded_ = -1;
    }
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
    Impl()
        : mode_(ODD::Mode::Memory)
        , dirName_("")
        , layers_()
    {}

    Impl(const std::string& dirName)
        : mode_(ODD::Mode::Disk)
        , dirName_(dirName)
        , layers_()
    {}

    virtual ~Impl() = default;

    void setInitialStates(const ODD::StateContainer& initialStates) {
        initialStates_ = initialStates;
    }

    void setFinalStates(const ODD::StateContainer& finalStates) {
        finalStates_ = finalStates;
    }

    virtual void addLayer(const ODD::AlphabetMap& alphabet,
                          const ODD::TransitionContainer& transitions,
                          int rightStateCount) = 0;

    virtual ODD build() = 0;

protected:
    ODD doBuild(int countLayers) const {
        ODD ret;
        ret.layers_ = std::move(layers_);
        ret.mode_ = mode_;
        ret.loaded_ = -1;
        ret.dirName_ = std::move(dirName_);
        ret.countLayers_ = countLayers;
        return ret;
    }

    ODD::Mode mode_;
    std::string dirName_;
    std::vector<ODD::Layer> layers_;
    ODD::StateContainer initialStates_;
    ODD::StateContainer finalStates_;
};

namespace {

class MemoryBuilder : public ODDBuilder::Impl {
public:
    MemoryBuilder(int firstLayerCount)
        : stateCounts_({firstLayerCount})
    {}

    virtual void addLayer(const ODD::AlphabetMap& alphabet,
                          const ODD::TransitionContainer& transitions,
                          int rightStateCount) override {
        alphabets_.push_back(alphabet);
        transitions_.push_back(transitions);
        stateCounts_.push_back(rightStateCount);
    }

    virtual ODD build() override {
        // We try to use std::move whereever possible.
        layers_.resize(alphabets_.size());
        for (int i = 0; i < (int)alphabets_.size(); i++) {
            layers_[i].alphabet = std::move(alphabets_[i]);
            layers_[i].leftStates = stateCounts_[i];
            layers_[i].rightStates = stateCounts_[i + 1];
            layers_[i].transitions = std::move(transitions_[i]);
            layers_[i].isInitial = i == 0;
            layers_[i].isFinal = i + 2 == (int)stateCounts_.size();
        }
        if (!layers_.empty()) {
            layers_[0].initialStates = std::move(initialStates_);
            layers_.back().finalStates = std::move(finalStates_);
        }
        return doBuild(layers_.size());
    }

private:
    std::vector<int> stateCounts_;
    std::vector<ODD::AlphabetMap> alphabets_;
    std::vector<ODD::TransitionContainer> transitions_;
};

class DiskBuilder : public ODDBuilder::Impl {
public:
    DiskBuilder(int firstLayerCount, const std::string& diskName)
        : ODDBuilder::Impl(diskName)
    {
        currentLayer_.leftStates = firstLayerCount;
        currentLayer_.isInitial = false;
        currentLayer_.isFinal = false;

        namespace fs = std::filesystem;
        fs::create_directories(fs::path(dirName_));
    }

    virtual void addLayer(const ODD::AlphabetMap& alphabet,
                          const ODD::TransitionContainer& transitions,
                          int rightStateCount) override {
        currentLayer_.alphabet = alphabet;
        currentLayer_.transitions = transitions;
        currentLayer_.rightStates = rightStateCount;
        writeLayer(currentLayer_, dirName_, i_);
        shift();
    }

    virtual ODD build() override {
        {
            // Set initial states
            ODD::Layer initialLayer;
            loadLayer(initialLayer, dirName_, 0);
            initialLayer.isInitial = true;
            initialLayer.initialStates = std::move(initialStates_);
            writeLayer(initialLayer, dirName_, 0);
        }
        {
            // Set final states
            ODD::Layer finalLayer;
            loadLayer(finalLayer, dirName_, i_ - 1);
            finalLayer.isFinal = true;
            finalLayer.finalStates = std::move(finalStates_);
            writeLayer(finalLayer, dirName_, i_ - 1);
        }
        return doBuild(i_);
    }

private:
    void shift() {
        currentLayer_.alphabet = {};
        currentLayer_.leftStates = currentLayer_.rightStates;
        currentLayer_.transitions = {};
        i_++;
    }

    ODD::Layer currentLayer_;
    int i_ = 0;
};

}

ODDBuilder::ODDBuilder(int leftStateCount)
    : impl_(std::make_unique<MemoryBuilder>(leftStateCount))
{}

ODDBuilder::ODDBuilder(int leftStateCount, const std::string& dirName)
    : impl_(std::make_unique<DiskBuilder>(leftStateCount, dirName))
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

ODD doCopyODD(const ODD& odd, ODDBuilder& builder) {
    builder.setInitialStates(odd.initialStates());
    for (int i = 0; i < odd.countLayers(); i++) {
        builder.addLayer(
            odd.getLayer(i).alphabet,
            odd.getLayer(i).transitions,
            odd.getLayer(i).rightStates
        );
    }
    builder.setFinalStates(odd.finalStates());
    return builder.build();
}

}

ODD copyODD(const ODD& odd) {
    ODDBuilder builder(odd.getLayer(0).leftStates);
    return doCopyODD(odd, builder);
}

ODD copyODD(const ODD& odd, const std::string& dirName) {
    ODDBuilder builder(odd.getLayer(0).leftStates, dirName);
    return doCopyODD(odd, builder);
}

namespace {

int countLayers(const std::string& dirName) {
    namespace fs = std::filesystem;
    for (int ret = 0; ; ret++) {
        fs::path path = fs::path(dirName) / layerFileName(ret);
        if (!fs::exists(path)) {
            return ret;
        }
    }
}

}

std::optional<ODD> readFromDirectory(const std::string& dirName) {
    int layers = countLayers(dirName);
    if (layers == 0) {
        return {};
    }
    int lastRight = -1;
    for (int i = 0; i < layers; i++) {
        namespace fs = std::filesystem;
        fs::path path = fs::path(dirName) / layerFileName(i);
        std::ifstream in(path);
        if (!in) {
            return {};
        }
        ODD::Layer layer;
        try {
            layer = readLayerJSON(in);
        } catch (JSONParseError&) {
            return {};
        }
        if (!layer.checkSanity()) {
            return {};
        }
        if (lastRight != -1 && lastRight != layer.leftStates) {
            return {};
        }
        lastRight = layer.rightStates;
    }

    ODD ret;
    ret.mode_ = ODD::Mode::Disk;
    ret.loaded_ = -1;
    ret.dirName_ = dirName;
    ret.countLayers_ = layers;
    return {std::move(ret)};
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
