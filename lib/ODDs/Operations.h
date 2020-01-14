#pragma once

#include <ODDs/ODDs.h>

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
 */
ODD diagramTensorProduct(const ODD& lhs, const ODD& rhs);

/**
 * @brief Shorthand for diagramTensorProduct.
 */
inline ODD operator*(const ODD& lhs, const ODD& rhs) {
    return diagramTensorProduct(lhs, rhs);
}

/**
 * @brief Construct negation of the diagram.
 *
 * Construct a diagram that accepts exactly the complement of language of
 * given diagram.
 */
ODD diagramNegation(const ODD& odd);

/**
 * @brief Shorthand for diagramNegation.
 */
inline ODD operator~(const ODD& odd) {
    return diagramNegation(odd);
}

}
