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

/**
 * @file ODDs/JSONDump.h
 * JSON descriptions for ODDs.
 *
 * This file provides a more human-readable but more space-consuming format for
 * ODDs.
 *
 * @section Example
 *
 * For instance, the example from ODDBuilder documentation is written as
 *
 * ```
 * {
 *   "leftLayerStates": 3,
 *   "initialStates": [2],
 *   "layers": [
 *      {
 *        "alphabet": ["a", "b"],
 *        "transitions": [
 *          {"from": 0, "symbol": "a", "to": 0},
 *          {"from": 0, "symbol": "b", "to": 1},
 *          {"from": 1, "symbol": "a", "to": 0},
 *          {"from": 1, "symbol": "b", "to": 1},
 *          {"from": 2, "symbol": "a", "to": 1},
 *          {"from": 2, "symbol": "b", "to": 0}
 *        ],
 *        "rightLayerStates": 2
 *      },
 *      {
 *        "alphabet": ["c", "d"],
 *        "transitions": [
 *          {"from": 0, "symbol": "c", "to": 0},
 *          {"from": 0, "symbol": "d", "to": 1},
 *          {"from": 1, "symbol": "c", "to": 0},
 *          {"from": 1, "symbol": "d", "to": 2},
 *          {"from": 1, "symbol": "d", "to": 0}
 *        ],
 *        "rightLayerStates": 3
 *      }
 *   ],
 *   "finalStates": [2]
 * }
 * ```
 */

#include <ODDs/ODDs.h>

#include <stdexcept>

namespace ODDs {

/**
 * @brief Write a prettyprinted JSON description of an ODD to an output stream.
 *
 * Uses 2 spaces for indentaion.
 */
void writeJSON(std::ostream& os, const ODD& odd);

/**
 * @brief Write a non-prettyprinted JSON description of an ODD layer to an
 * output stream.
 */
void writeJSON(std::ostream& os, const ODD::Layer& layer);

/**
 * @brief Get a prettyprinted JSON description of an ODD.
 *
 * Uses 2 spaces for indentaion.
 */
std::string writeJSON(const ODD& odd);

/**
 * @brief JSON parse error.
 */
class JSONParseError : public std::runtime_error {
public:
    JSONParseError(std::string message);
    virtual ~JSONParseError() = default;
};

/**
 * @brief Read a JSON description from an input stream.
 *
 * @throws JSONParseError in case of malformated input.
 */
ODD readJSON(std::istream& is);

/**
 * @brief Read a JSON layer description from an input stream.
 *
 * @throws JSONParseError in case of malformed input.
 */
ODD::Layer readLayerJSON(std::istream& is);

/**
 * @brief Read a JSON description from a string.
 *
 * @throws JSONParseError in case of malformated input.
 */
ODD readJSON(const std::string& description);

}
