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
        const std::string& numberToSymbol(int number) const;

        /**
         * @brief  Get the symbol id.
         * @return Symbol id, or -1 in case the symbol is not mapped.
         */
        int symbolToNumber(const std::string& symbol) const;

    private:
        std::vector<std::string> n2s_;
        std::map<std::string, int> s2n_;
    };

    /**
     * @brief A type for layers.
     */
    struct Layer {
        AlphabetMap alphabet;
        StateContainer *leftStates; // Owned by the ODD
        StateContainer initialStates;
        StateContainer *rightStates; // Owned by the ODD
        StateContainer finalStates;
        TransitionContainer transitions;
        bool isInitial, isFinal;

        /**
         * @brief Width of the layer.
         *
         * Equal to `max(leftStates.size(), rightStates.size())`.
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
    std::vector<StateContainer> states_;
    std::vector<Layer> layers_;

    friend class ODDBuilder;
};

/**
 * @brief A builder for ODDs.
 *
 * Simple alternative to a monsterous constructor. Guarantees by design that
 * the built ODD is correct.
 * 
 * TODO: Provide an example.
 */
class ODDBuilder {
public:
    /**
     * @brief Construct from the leftmost set of states.
     */
    ODDBuilder(const ODD::StateContainer& firstLayerLeft);

    ~ODDBuilder() = default;
    ODDBuilder(const ODDBuilder&) = delete;
    ODDBuilder& operator=(const ODDBuilder&) = delete;
    ODDBuilder(ODDBuilder&&) = default;
    ODDBuilder& operator=(ODDBuilder&&) = default;

    /**
     * @brief Add a new layer to the right.
     */
    void addLayer(const ODD::AlphabetMap& alphabet,
                  const ODD::TransitionContainer& transitions,
                  const ODD::StateContainer& rightStates);

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
 * TODO: Provide format description.
 */
std::istream& operator>>(std::istream& is, ODD& odd);

/**
 * @brief Writes ODD description to an output stream.
 *
 * TODO: Provide format description.
 */
std::ostream& operator<<(std::ostream& os, const ODD& odd);

}
