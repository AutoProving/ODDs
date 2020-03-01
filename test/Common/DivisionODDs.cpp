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

#include <Common/DivisionODDs.h>

namespace TestCommon {

ODDs::ODD div2(int length) {
    ODDs::ODD::AlphabetMap binary;
    int zero = binary.addSymbol("0");
    int one = binary.addSymbol("1");

    ODDs::ODDBuilder builder(1);
    builder.setInitialStates({0});
    for (int i = 0; i < length - 1; i++) {
        builder.addLayer(binary, {{0, zero, 0}, {0, one, 0}}, 1);
    }
    builder.addLayer(binary, {{0, zero, 0}, {0, one, 1}}, 2);
    builder.setFinalStates({0});
    return builder.build();
}

ODDs::ODD div2NM(int length) {
    ODDs::ODD::AlphabetMap binary;
    int zero = binary.addSymbol("0");
    int one = binary.addSymbol("1");

    ODDs::ODDBuilder builder(2);
    builder.setInitialStates({0});
    for (int i = 0; i < length; i++) {
        ODDs::ODD::TransitionContainer transitions = {
            {0, zero, 0},
            {0, one, 1},
            {1, zero, 0},
            {1, one, 1}
        };
        builder.addLayer(binary, transitions, 1);
    }
    builder.setFinalStates({0});
    return builder.build();
}

ODDs::ODD div3(int length) {
    ODDs::ODD::AlphabetMap binary;
    int zero = binary.addSymbol("0");
    int one = binary.addSymbol("1");

    ODDs::ODDBuilder builder(3);
    builder.setInitialStates({0});
    for (int i = 0; i < length; i++) {
        ODDs::ODD::TransitionContainer transitions = {
            {0, zero, 0},
            {0, one, 1},
            {1, zero, 2},
            {1, one, 0},
            {2, zero, 1},
            {2, one, 2}
        };
        builder.addLayer(binary, transitions, 3);
    }
    builder.setFinalStates({0});
    return builder.build();
}

namespace {

std::vector<std::string> toInput(int n, int length) {
    std::vector<std::string> ret;
    for (int i = 0; i < length; i++)
        ret.push_back((n >> i) & 1 ? "1" : "0");
    std::reverse(ret.begin(), ret.end());
    return ret;
}

}

bool checkPredicate(const ODDs::ODD& odd, std::function<bool(int)> predicate) {
    int n = odd.countLayers();
    for (int i = 0; i < (1 << n); i++) {
        if (odd.accepts(toInput(i, n)) != predicate(i))
            return false;
    }
    return true;
}

}
