#include <gtest/gtest.h>

#include <ODDs/ODDs.h>
#include <ODDs/Operations.h>

#include <string>

namespace {

ODDs::ODD readFromString(const std::string& desc) {
    std::istringstream is(desc);
    return ODDs::readFromIStream(is);
}

std::string writeDesc(const ODDs::ODD odd) {
    std::ostringstream os;
    ODDs::writeToOStream(os, odd);
    return os.str();
}

void doTest(std::string lhsDesc, std::string rhsDesc, std::string expected) {
    ODDs::ODD lhs = readFromString(lhsDesc);
    ODDs::ODD rhs = readFromString(rhsDesc);
    ODDs::ODD result = ODDs::diagramUnion(lhs, rhs);
    ASSERT_EQ(expected, writeDesc(result));
}

}

class ODDsUnionTest : public ::testing::Test {
protected:
    static std::string trivialLhsDesc;
    static std::string trivialRhsDesc;
    static std::string trivialExpectedDesc;
};

// {abc,abd}, not minimal
std::string ODDsUnionTest::trivialLhsDesc = R"(3
1
0
1
1
a
1
0 a 0
1
1
b
1
0 b 0
1
2
c d
2
0 c 0
0 d 1
2
2
0 1
 )";

// {cba,dba}, not minimal
std::string ODDsUnionTest::trivialRhsDesc = R"(3
2
0 1
2
2
c d
2
0 c 0
1 d 0
1
1
b
1
0 b 0
1
1
a
1
0 a 0
1
1
0
 )";

// {abc,abd,cba,dba}, not minimal
std::string ODDsUnionTest::trivialExpectedDesc = R"(3
3
0 1 2 
3
3
a c d 
3
0 a 0
1 c 1
2 d 1
2
1
b 
2
0 b 0
1 b 1
2
3
c d a 
3
0 c 0
0 d 1
1 a 2
3
3
0 1 2 
)";

TEST_F(ODDsUnionTest, trivial) {
    doTest(trivialLhsDesc, trivialRhsDesc, trivialExpectedDesc);
}
