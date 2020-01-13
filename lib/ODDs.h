#pragma once

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
};

}
