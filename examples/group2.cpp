#include "cpput.h"

int my_sum2(int a, int b)
{
    return a + b;
}

namespace cpput
{
struct Group2 {};

GROUP(Group2, "Group2 Test")

GROUP_BEGIN(Group2)
{
	//do something
}

GROUP_END(Group2)
{
	//do something
}

TEST(Group2, 1)
{
    EXPECT(7 == my_sum2(3, 4), "7 == my_sum2(3, 4) error!");
}

TEST(Group2, 2)
{
	//etc.
}

}
