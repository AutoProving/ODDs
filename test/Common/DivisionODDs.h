#include <ODDs/ODDs.h>

#include <functional>

namespace TestCommon {

ODDs::ODD div2(int length);
ODDs::ODD div2NM(int length);
ODDs::ODD div3(int length);

bool checkPredicate(const ODDs::ODD& odd, std::function<bool(int)> predicate);

}
