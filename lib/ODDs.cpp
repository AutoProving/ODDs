#include <ODDs.h>

namespace ODDs {

ODD::Transition::Transition(ODD::State fromState, ODD::Symbol bySymbol, ODD::State stateTo)
    : from(fromState)
    , symbol(bySymbol)
    , to(stateTo)
{}

bool ODD::Transition::operator==(const ODD::Transition& rhs) const {
    return from == rhs.from
        && symbol == rhs.symbol
        && to == rhs.to;
}

bool ODD::Transition::operator<(const ODD::Transition& rhs) const {
    if (from != rhs.from)
        return from < rhs.from;
    if (symbol != rhs.symbol)
        return symbol < rhs.symbol;
    return to < rhs.to;
}

}
