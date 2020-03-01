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

/**
 * @brief Construct a minimal determinate complete diagram with same language.
 *
 * If the automation is not complete or non-determinate, first performs
 * diagramLazyPowerSet.
 */
ODD minimize(ODD odd);

}
