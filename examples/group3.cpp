#include "cpput.h"

int my_sum3(int a, int b)
{
    return a + b;
}

namespace cpput
{

GROUP(Group3, "Group3 Test")

GROUP_BEGIN(Group3)
{
    //do something
}

GROUP_END(Group3)
{
    //do something
}

TEST_BEGIN(Group3)
{
    //do something
}

TEST_END(Group3)
{
    //do something
}

TEST(Group3, 1)
{
    EXPECT(9 == my_sum3(5, 4), "9 == my_sum3(5, 4) error!");
}

TEST(Group3, 2)
{
    //etc.
}

}
