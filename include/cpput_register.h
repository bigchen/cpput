#ifndef cpput_register_h
#define cpput_register_h

#include "cpput_registry.h"

namespace cpput
{

const int MAX_TESTS_IN_GROUP = 20;

template<class GROUP, int N>
struct GroupRegister
{
    static void Register(GROUP &g)
    {
		g.Register(N, static_cast<method>(&GROUP::template Test<N>));
        GroupRegister<GROUP, N-1>::Register(g);
    }
};

template<class GROUP>
struct GroupRegister<GROUP, 0>
{
    static void Register(GROUP& g)
    {
    }
};

template<class CONTEXT, int MAX_TESTS = MAX_TESTS_IN_GROUP>
class TestGroup : public GroupBase, public CONTEXT
{
public:
    TestGroup(const std::string &name) : GroupBase(name)
    {
		TestRegistry::Instance()->RegisterGroup(group_name, this);
        GroupRegister<TestGroup, MAX_TESTS>::Register(*this);
    }

    void Register(int n, method m)
    {
        this->tests[n] = m;
    }

    template<int n>
    void Test()
    {
        this->dummy_test = true;
    }

};


#define GROUP(CONTEXT,NAME) cpput::TestGroup<CONTEXT> CONTEXT##group(NAME);
#define TEST(CONTEXT,N) template<> template<> \
        void TestGroup<CONTEXT>::Test<(N+4)>()
#define GROUP_BEGIN(CONTEXT) template<> template<> \
        void TestGroup<CONTEXT>::Test<1>()
#define GROUP_END(CONTEXT) template<> template<> \
        void TestGroup<CONTEXT>::Test<2>()
#define TEST_BEGIN(CONTEXT) template<> template<> \
        void TestGroup<CONTEXT>::Test<3>()
#define TEST_END(CONTEXT) template<> template<> \
        void TestGroup<CONTEXT>::Test<4>()

}

#endif

