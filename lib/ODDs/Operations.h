#pragma once

#include <ODDs/ODDs.h>

#include <functional>

namespace ODDs {

/**
 * @brief Construct union of the diagrams.
 *
 * Construct a diagram that accepts exactly the union of languages of two given
 * diagrams.
 */
ODD diagramUnion(const ODD& lhs, const ODD& rhs);

/**
 * @brief Shorthand for diagramUnion.
 */
inline ODD operator|(const ODD& lhs, const ODD& rhs) {
    return diagramUnion(lhs, rhs);
}

/**
 * @brief Construct intersection of the diagrams.
 *
 * Construct a diagram that accpets exactly the intersection of languages of
 * two given diagrams.
 */
ODD diagramIntersection(const ODD& lhs, const ODD& rhs);

/**
 * @brief Shorthand for diagramIntersection.
 */
inline ODD operator&(const ODD& lhs, const ODD& rhs) {
    return diagramIntersection(lhs, rhs);
}

/**
 * @brief Construct tensor product of the diagrams.
 *
 * Alphabet symbols are connected with a comma between.
 */
ODD diagramTensorProduct(const ODD& lhs, const ODD& rhs);

/**
 * @brief Shorthand for diagramTensorProduct.
 */
inline ODD operator*(const ODD& lhs, const ODD& rhs) {
    return diagramTensorProduct(lhs, rhs);
}

/**
 * @brief Construct negation of a *complete deterministic* diagram.
 *
 * Construct a diagram that accepts exactly the complement of language of given
 * diagram.
 *
 * @warning In case the diagram is non-deterministic or not complete, constructs
 * a valid diagram that *is not a negation*.
 */
ODD diagramNegation(const ODD& odd);

/**
 * @brief Shorthand for diagramNegation.
 */
inline ODD operator~(const ODD& odd) {
    return diagramNegation(odd);
}

/**
 * @brief A mapping from one alphabet to another.
 */
using AlphabetMapping = std::function<std::string(std::string)>;

/**
 * @brief Maps an alphabet of an ODD with given functions.
 */
ODD diagramMapping(const std::vector<AlphabetMapping>& mappings, const ODD& odd);

/**
 * @brief Performs an inverse mapping of ODD alphabet with given functions.
 */
ODD diagramInverseMapping(const std::vector<AlphabetMapping>& mappings,
                          const std::vector<ODD::AlphabetMap>& newAlphabets,
                          const ODD& odd);

/**
 * @brief Checks if ODD is deterministic.
 */
bool isDeterministic(const ODD& odd);

/**
 * @brief Checks if ODD is complete.
 */
bool isComplete(const ODD& odd);

/**
 * @brief Construct a power set of the ODD.
 *
 * Construct a deterministic, complete ODD with same language as the given.
 * This is basic version, that replaces each layer of vertices except the
 * leftmost with its powerset.
 *
 * @warning May consume a lot of unnecessary time and memory. In many cases you
 * need diagramLazyPowerSet.
 */
ODD diagramPowerSet(const ODD& odd);

/**
 * @brief Construct a power set of the ODD.
 *
 * Construct a deterministic, complete ODD with same language as the given.
 * This is an optimized version that stores only reachable state sets. For
 * example, if an automation is already deterministic and complete, it does
 * nothing.
 */
ODD diagramLazyPowerSet(const ODD& odd);

}
