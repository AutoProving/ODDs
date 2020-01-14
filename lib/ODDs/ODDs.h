#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace ODDs {

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
     * @brief A type for transition containers.
     *
     * TODO: Make it a wrapper around `std::multimap`.
     */
    using TransitionContainer = std::set<Transition>;

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

private:
    ODD(); // Use ODDBuilder to construct ODDs

public:
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

private:
    std::vector<Layer> layers_;

    friend class ODDBuilder;
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
     * @brief Construct a builder given the number of vertices in the first
     * layer.
     */
    ODDBuilder(int leftStateCount);

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
    class Impl;
    std::unique_ptr<Impl> impl_;
};

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
 * The transitions are ordered lexicographically.
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
 * 1 d 0
 * 1 d 2
 * 3
 * 1
 * 2 
 * ```
 */
void writeToOStream(std::ostream& os, const ODD& odd);

}
