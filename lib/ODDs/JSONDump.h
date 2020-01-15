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
 * @brief A comparable class for ODDs.
 *
 * Useful for comparing ODDs on being structurally equivalent.
 */
class JSONDump {
public:
    JSONDump(const ODD& odd);
    ~JSONDump();

    bool operator==(const JSONDump& rhs) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief Write a prettyprinted JSON description of an ODD to an output stream.
 *
 * Uses 2 spaces for indentaion.
 */
void writeJSON(std::ostream& os, const ODD& odd);

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
 * @brief Read a JSON description from a string.
 *
 * @throws JSONParseError in case of malformated input.
 */
ODD readJSON(const std::string& description);

}
