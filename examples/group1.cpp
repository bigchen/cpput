#include "cpput.h"

int my_sum(int a, int b)
{
    return a + b;
}

namespace cpput
{
struct Group1 {};

GROUP(Group1, "Group1 Test")

TEST(Group1, 1)
{
    EXPECT(3 == my_sum(1, 2), "3 == my_sum(1, 2) error!");
}

TEST(Group1, 2)
{
    EXPECT(4 == my_sum(1, 2), "4 == my_sum(1, 2) error!");
}

}


