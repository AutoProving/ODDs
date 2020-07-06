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
 * @brief Construct a disk union of the diagrams.
 *
 * Same as diagramUnion(const ODD&, const ODD&), but stores the result to disk.
 */
ODD diagramUnion(const ODD& lhs, const ODD& rhs, const std::string& dirName);

/**
 * @brief Shorthand for diagramUnion(const ODD&, const ODD&).
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
 * @brief Construct a disk intersection of the diagrams.
 *
 * Same as diagramIntersection(const ODD&, const ODD&), but stores the result to disk.
 */
ODD diagramIntersection(const ODD& lhs, const ODD& rhs, const std::string& dirName);

/**
 * @brief Shorthand for diagramIntersection(const ODD&, const ODD&).
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
 * @brief Construct a disk tensor product of the diagrams.
 *
 * Same as diagramTensorProduct(const ODD&, const ODD&), but stores the result to disk.
 */
ODD diagramTensorProduct(const ODD& lhs, const ODD& rhs, const std::string& dirName);

/**
 * @brief Shorthand for diagramTensorProduct(const ODD&, const ODD&).
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
 * @brief Construct disk negation of a *complete deterministic diagram.
 *
 * Same as diagramNegation(const ODD&), but stores the result to disk.
 */
ODD diagramNegation(const ODD& odd, const std::string& dirName);

/**
 * @brief Shorthand for diagramNegation(const ODD&).
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
 * @brief Maps an alphabet of an ODD with given functions.
 *
 * Same as diagramMapping(const std::vector<AlphabetMapping>&, const ODD&), but
 * stores the result to disk.
 */
ODD diagramMapping(const std::vector<AlphabetMapping>& mappings,
                   const ODD& odd,
                   const std::string& dirName);

/**
 * @brief Performs an inverse mapping of ODD alphabet with given functions.
 */
ODD diagramInverseMapping(const std::vector<AlphabetMapping>& mappings,
                          const std::vector<ODD::AlphabetMap>& newAlphabets,
                          const ODD& odd);

/**
 * @brief Performs an inverse mapping of ODD alphabet with given functions.
 *
 * Same as
 * diagramInverseMapping(const std::vector<AlphabetMapping>&, const std::vector<ODD::AlphabetMap>&, const ODD&),
 * but stores result to the disk.
 */
ODD diagramInverseMapping(const std::vector<AlphabetMapping>& mappings,
                          const std::vector<ODD::AlphabetMap>& newAlphabets,
                          const ODD& odd,
                          const std::string& dirName);

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
 * @brief Construct a disk power set of the ODD.
 *
 * Same as diagramPowerSet(const ODD&), but stores result to the disk.
 */
ODD diagramPowerSet(const ODD& odd, const std::string& dirName);

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
 * @brief Construct a disk power set of the ODD
 *
 * Same as diagramLazyPowerSet(const ODD& odd), but stores result to the disk.
 */
ODD diagramLazyPowerSet(const ODD& odd, const std::string& dirName);

/**
 * @brief Construct a minimal determinate complete diagram with same language.
 *
 * If the automation is not complete or non-determinate, first performs
 * diagramLazyPowerSet.
 */
ODD minimize(const ODD& odd);

/**
 * @brief Construct a mininal deteminate complete diagram with same language.
 *
 * Same as minimize(const ODD&), but stores result to the disk.
 *
 * Needs additional directory to store the power set. It is deleted after
 * the execution finishes.
 */
ODD minimize(const ODD& odd,
             const std::string& dirName,
             const std::string& tempDirName);

}
