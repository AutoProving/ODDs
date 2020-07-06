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

#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace ODDs {

/**
 * @brief An Ordered Decision Diagram.
 *
 * An ODD can be stored in memory or on disk. When it is stored on disk, at
 * most one layer is stored in the memory at a time, but operations are much
 * slower. In disk mode the ODD is stored in some files in the directory
 * provided to ODDBuilder. Normally, the directory is deleted after the ODD
 * object is destroyed; one should call ODD::detachDir to prevent this. An
 * ODD can be restored from the directory where it was stored by calling
 * ODDs::readFromDirectory.
 *
 * @warning Since disk mode ODD dynamically loads and unloads layers, make
 * sure you don't work with two layers simultaneously with undefined operation
 * order. For instance, the following assertion might fail:
 * @code
 * // Assuming odd is a valid ODD with at least two layers.
 * assert(odd.getLayer(0).rightStates == odd.getLayer(1).leftStates);
 * @endcode
 * The reason is that layer 0 may be unloaded by the time the number of its
 * right states is requested. A correct way to do the same assertion is as
 * follows:
 * @code
 * int leftStates = odd.getLayer(0).rightStates;
 * int rightStates = odd.getLayer(1).leftStates;
 * assert(leftStates == rightStates);
 * @endcode
 */
class ODD {
public:
    /**
     * @brief A type for symbol indices.
     *
     * Since this type is not a likely subject to change, this should be defined
     * as a type synonim rather then a template parameter for the ODD class.
     */
    using Symbol = int;

    /**
     * @brief A type for state indices.
     *
     * Since this type is not a likely subject to change, this should be defined
     * as a type synonim rather then a template parameter for the ODD class.
     */
    using State = int;

    /**
     * @brief A type for transitions.
     */
    struct Transition {
        State from;
        Symbol symbol;
        State to;

        // Needed by TransitionContainer::Iterator
        Transition() = default;

        Transition(State stateFrom, Symbol bySymbol, State stateTo);

        /**
         * @brief Compares two transitions on equality.
         *
         * Compares triples (`from`, `symbol`, `to`) lexicographically.
         */
        bool operator==(const Transition& rhs) const;

        /**
         * @brief Compares two transitions.
         *
         * Compares triples (`from`, `symbol`, `to`) lexicographically. Needed
         * by `std::set`.
         */
        bool operator<(const Transition& rhs) const;
    };

    /**
     * @brief A type for state containers.
     *
     * The `std::set` implementation might be a subject to library-wide change.
     * Yet the current implementation may rely on `std::set` features.
     */
    using StateContainer = std::set<State>;

    /**
     * @brief A key for TransitionContainer.
     */
    struct TransitionKey {
        State from;
        Symbol symbol;

        /**
         * @brief Compares two transition keys lexicographically.
         *
         * Needed by multimap in TransitionContainer.
         */
        bool operator<(const TransitionKey& rhs) const;

        /**
         * @brief Compares two transition keys on equality,
         *
         * Needed by multimap in TransitionContainer.
         */
        bool operator==(const TransitionKey& rhs) const;
    };

    /**
     * @brief A type for transition containers.
     *
     * A wrapper around std::multimap.
     */
    class TransitionContainer {
        using Base = std::multimap<TransitionKey, State>;
        using BaseIterator = Base::const_iterator;
    public:
        /**
         * @brief A wrapper around constant multimap iterator.
         *
         * Needed for value_type being Transition.
         */
        class Iterator {
        public:
            // Iterator traits (for range-based loops, for instance)
            using difference_type   = BaseIterator::difference_type;
            using value_type        = Transition;
            using pointer           = const Transition*;
            using reference         = const Transition&;
            using iterator_category = std::bidirectional_iterator_tag;

            Iterator() = default;
            Iterator(const Iterator&) = default;
            Iterator& operator=(const Iterator&) = default;
            Iterator(Iterator&&) = default;
            Iterator& operator=(Iterator&&) = default;
            ~Iterator() = default;

            bool operator==(const Iterator& rhs) const;
            bool operator!=(const Iterator& rhs) const;

            reference operator*();
            pointer operator->();

            Iterator& operator++();
            Iterator operator++(int);
            Iterator& operator--();
            Iterator operator--(int);

        private:
            Iterator(BaseIterator it);

            void updateStored_();

            BaseIterator it_;
            Transition stored_;

            friend class TransitionContainer;
        };

        /**
         * @brief Represents a range of transitions.
         *
         * Needed for iterating through edges of same key.
         */
        class Range {
        public:
            Range(Iterator from, Iterator to);

            ~Range() = default;
            Range(const Range&) = default;
            Range& operator=(const Range&) = default;
            Range(Range&&) = default;
            Range& operator=(Range&&) = default;

            Iterator begin() const;
            Iterator end() const;

        private:
            Iterator b_, e_;
        };

        /**
         * @brief Construct an empty container.
         */
        TransitionContainer() = default;

        /**
         * @brief Construct container with given elements.
         */
        TransitionContainer(std::initializer_list<Transition> elements);

        TransitionContainer(const TransitionContainer&) = default;
        TransitionContainer& operator=(const TransitionContainer&) = default;
        TransitionContainer(TransitionContainer&&) = default;
        TransitionContainer& operator=(TransitionContainer&&) = default;

        /**
         * @brief Iterator to the beginning.
         */
        Iterator begin() const;

        /**
         * @brief Iterator to the end.
         */
        Iterator end() const;

        /**
         * @brief Insert a new transition into the container.
         *
         * No effect in case given transition is already in the container.
         */
        void insert(const Transition& transition);

        /**
         * @brief Number of stored transitions.
         */
        std::size_t size() const;

        /**
         * @brief Compares two transition containers on equality.
         *
         * Needed for tests.
         */
        bool operator==(const TransitionContainer& rhs) const;

        /**
         * @brief Range of transitions with the same key.
         *
         * Thanks to the Range class, we can use it as
         * @code
         * for (const Transition& t : ts.proceed(key)) {
         *     // Do something
         * }
         * @endcode
         */
        Range proceed(const TransitionKey& key) const;

        /**
         * @brief Range of transitions with the same key.
         *
         * A slightly more usable overload.
         */
        Range proceed(State from, Symbol symbol) const;

        /**
         * @brief Returns first transition with given key.
         *
         * Useful for determinate ODDs.
         * Behaviour is undefined in case no such edge exists.
         */
        State go(const TransitionKey& key) const;

        /**
         * @brief Returns first transition with given key.
         *
         * A slightly more usable overload.
         */
        State go(State from, Symbol symbol) const;

        /**
         * @brief Checks if container contains a transition with given key.
         */
        bool hasKey(const TransitionKey& key) const;

        /**
         * @brief Checks if container contains a transition with given key.
         *
         * A slightly more usable overload.
         */
        bool hasKey(State from, Symbol symbol) const;

        /**
         * @breif Returns a range of transitions from given started state.
         */
        Range proceedFrom(State from) const;

    private:
        Base m_;
        std::set<Transition> p_;
    };

    /**
     * @brief A mapping between the alphabet and the state.
     */
    class AlphabetMap {
    public:
        /**
         * @brief Expand the alphabet.
         *
         * @return The new symbol id if {@param symbol} was not in alphabet,
         * and its id if it was.
         */
        int addSymbol(const std::string& symbol);

        /**
         * @brief  Get the symbol.
         *
         * The behaviour in case the number is not in alphabet is undefined.
         */
        const std::string& numberToSymbol(Symbol number) const;

        /**
         * @brief  Get the symbol id.
         * @return Symbol id, or -1 in case the symbol is not mapped.
         */
        Symbol symbolToNumber(const std::string& symbol) const;

        /**
         * @brief Get all symbols added to the alphabet.
         *
         * The symbols are ordered by the time they were added.
         */
        const std::vector<std::string>& symbols() const;

        /**
         * @brief Compares two alphabet maps on equality.
         *
         * Needed for tests.
         */
        bool operator==(const AlphabetMap& rhs) const;

        /**
         * @brief Checks if alphabet map contains a symbol.
         */
        bool contains(const std::string& symbol) const;
    private:
        std::vector<std::string> n2s_;
        std::map<std::string, int> s2n_;
    };

    /**
     * @brief A type for layers.
     */
    struct Layer {
        AlphabetMap alphabet;
        int leftStates; // Owned by the ODD
        StateContainer initialStates;
        int rightStates; // Owned by the ODD
        StateContainer finalStates;
        TransitionContainer transitions;
        bool isInitial, isFinal;

        /**
         * @brief Width of the layer.
         *
         * Equal to `max(leftStates, rightStates)`.
         */
        int width() const;

        /**
         * @brief Checks correctness of boolean flags.
         *
         * Checks that values of isInitial and isFinal correspond with
         * initialStates and finalStates.
         *
         * @return True if flags are set correctly, false otherwise.
         */
        bool checkSanity() const;
    };

    enum class Mode {
        Memory,
        Disk
    };

private:
    ODD(); // Use ODDBuilder to construct ODDs

    ODD(const std::string& dirName); // Use ODDBuilder to construct ODDs

public:
    /**
     * @brief Destroys the ODD on disk if nessessary.
     */
    ~ODD();

    /**
     * Direct copying of ODDs is prohibited because they might incapsulate
     * ownership of directories.
     */
    ODD(const ODD&) = delete;
    ODD& operator=(const ODD&) = delete;

    /**
     * @brief Move constructor.
     */
    ODD(ODD&& other);

    /**
     * @brief Move-assignment operator.
     */
    ODD& operator=(ODD&& rhs);

    /**
     * @brief Returns number of layers.
     *
     * This is the number of transition sets, not state sets.
     */
    int countLayers() const;

    /**
     * @brief Constant reference to the layer by its id.
     */
    const Layer& getLayer(int i) const;

    /**
     * @brief Initial states of the leftmost state layer.
     */
    const StateContainer& initialStates() const;

    /**
     * @brief Final states of the rightmost state layer.
     */
    const StateContainer& finalStates() const;

    /**
     * @brief Checks if an ODD accepts a string.
     */
    bool accepts(const std::vector<std::string>& string) const;

    /**
     * @brief Withdraws ownership for the directory.
     *
     * If the ODD is in disk mode, this implies that the directory will not be
     * removed after the object is destroyed. No effect if the ODD is in memory
     * mode.
     */
    void detachDir();

    /**
     * @brief Reduces amount of space used by disk-mode ODD.
     *
     * Unloads loaded layer, if there is such layer.
     */
    void unload() const;

private:
    std::vector<Layer> layers_;

    Mode mode_;
    mutable int loaded_;
    std::string dirName_;
    bool detached_ = false;
    int countLayers_;
    mutable Layer loadedLayer_;

    friend class ODDBuilder;
    friend std::optional<ODD> readFromDirectory(const std::string&);
};

/**
 * @brief A builder for ODDs.
 *
 * Simple alternative to a monsterous constructor. Guarantees by design that
 * the built ODD is correct.
 * 
 * @section Example
 * @code
 *  ODDs::ODD::StateContainer initialStates = {2};
 *
 *  ODDs::ODD::AlphabetMap alphabet0;
 *  ODDs::ODD::Symbol a0a = alphabet0.addSymbol("a");
 *  ODDs::ODD::Symbol a0b = alphabet0.addSymbol("b");
 *  ODDs::ODD::TransitionContainer transitions0;
 *  transitions0.insert({0, a0a, 0});
 *  transitions0.insert({0, a0b, 1});
 *  transitions0.insert({1, a0a, 0});
 *  transitions0.insert({1, a0b, 1});
 *  transitions0.insert({2, a0a, 1});
 *  transitions0.insert({2, a0b, 0});
 *
 *  ODDs::ODD::AlphabetMap alphabet1;
 *  ODDs::ODD::Symbol a1c = alphabet1.addSymbol("c");
 *  ODDs::ODD::Symbol a1d = alphabet1.addSymbol("d");
 *  ODDs::ODD::TransitionContainer transitions1;
 *  transitions1.insert({0, a1c, 0});
 *  transitions1.insert({0, a1d, 1});
 *  transitions1.insert({1, a1c, 0});
 *  transitions1.insert({1, a1d, 2});
 *  // Non-deterministic, why not?
 *  transitions1.insert({1, a1d, 0});
 *
 *  ODDs::ODD::StateContainer finalStates = {2};
 *
 *  ODDs::ODDBuilder builder(3);
 *  builder.addLayer(alphabet0, transitions0, 2);
 *  builder.addLayer(alphabet1, transitions1, 3);
 *  builder.setInitialStates(initialStates);
 *  builder.setFinalStates(finalStates);
 *  ODDs::ODD odd = builder.build();
 * @endcode
 */
class ODDBuilder {
public:
    /**
     * Implementation-defined inner class.
     */
    class Impl;

    /**
     * @brief Construct a memory builder given the number of vertices in the
     * first layer.
     */
    ODDBuilder(int leftStateCount);

    /**
     * @brief Construct a disk builder given the number of vertices in the
     * first layer and directory name.
     */
    ODDBuilder(int leftStateCount, const std::string& dirName);

    ~ODDBuilder();
    ODDBuilder(const ODDBuilder&) = delete;
    ODDBuilder& operator=(const ODDBuilder&) = delete;
    ODDBuilder(ODDBuilder&&) = default;
    ODDBuilder& operator=(ODDBuilder&&) = default;

    /**
     * @brief Add a new layer to the right.
     */
    void addLayer(const ODD::AlphabetMap& alphabet,
                  const ODD::TransitionContainer& transitions,
                  int rightStateCount);

    /**
     * @brief Set the initial states set for the leftmost layer.
     */
    void setInitialStates(const ODD::StateContainer& initialStates);

    /**
     * @brief Set the final states set for the rightmost layer.
     */
    void setFinalStates(const ODD::StateContainer& finalStates);

    /**
     * @brief Build the ODD.
     *
     * @warning ODDBuilder isn't supposed to be used after this function
     * is called. It may leave the builder in unconsistent state.
     */
    ODD build();

private:
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief Creates a memory copy of given ODD.
 *
 * A memory analogue of copy constructor.
 */
ODD copyODD(const ODD& odd);

/**
 * @brief Creates a disk copy of given ODD.
 *
 * A disk analogue of copy constructor.
 */
ODD copyODD(const ODD& odd, const std::string& dirName);

/**
 * @brief Tries to load an ODD from directory.
 *
 * @return Disk-mode ODD if provided dir contains a correct ODD, nothing
 * otherwise.
 */
std::optional<ODD> readFromDirectory(const std::string& dirName);

/**
 * @brief Reads ODD description from an input stream.
 *
 * The format is the same as in writeToOStream. This function is insensitive to
 * indentation and EOLs.
 */
ODD readFromIStream(std::istream& is);

/**
 * @brief Writes ODD description to an output stream.
 *
 * @section Format
 *
 * Prints the ODD in the following way:
 *
 * ```
 * <number of layers>
 * <initial states>
 * <first state set size>
 * <first layer alphabet>
 * <first layer transitions>
 * <second state set size>
 * <second layer alphabet>
 * <second layer transitions>
 * <third state set size>
 * ...
 * <last layer alphabet>
 * <last layer transitions>
 * <last state set size>
 * <final states>
 * ```
 *
 * The state sets are printed in the following way:
 *
 * ```
 * <number of states>
 * <space-separated list of states>
 * ```
 *
 * There might be an additional space in the end of the last line.
 * The states are ordered by their IDs.
 *
 * The alphabets are printed in the following way:
 *
 * ```
 * <number of symbols>
 * <space-separated list of symbols (strings)>
 * ```
 *
 * There might be an additional space in the end of the last line.
 * The symbols are ordered by the time they were added.
 *
 * The transition sets are printed in the following way:
 *
 * ```
 * <number of transitions>
 * <first left end> <first symbol> <first right end>
 * <second left end> <second symbol> <second right end>
 * ...
 * ```
 *
 * The transitions are ordered lexicographically by first two elements and by
 * order of insertion by the last element.
 *
 * @subsection Example
 *
 * For instance, the ODD from the ODDBuilder class example is printed as
 * ```
 * 2
 * 1
 * 2 
 * 3
 * 2
 * a b 
 * 6
 * 0 a 0
 * 0 b 1
 * 1 a 0
 * 1 b 1
 * 2 a 1
 * 2 b 0
 * 2
 * 2
 * c d 
 * 5
 * 0 c 0
 * 0 d 1
 * 1 c 0
 * 1 d 2
 * 1 d 0
 * 3
 * 1
 * 2 
 * ```
 */
void writeToOStream(std::ostream& os, const ODD& odd);

}
