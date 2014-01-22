#ifndef cpput_runner_h
#define cpput_runner_h

#include "cpput_registry.h"
#include "cpput_reporter.h"

namespace cpput
{

class TestRunner
{
public:
	TestRunner() : reporter(0), registry(TestRegistry::Instance()) {}
    explicit TestRunner(TestReporter *rep) : reporter(rep), registry(TestRegistry::Instance()) {}
    ~TestRunner() {}

	void SetReporter(TestReporter* rep)
	{
        reporter = rep;
	}

	static TestRunner * Instance()
	{
		static TestRunner runner;
		return &runner;
	}

    void RunGroup(const std::string &group_name)
    {
        TestRegistry::GroupMap::const_iterator group_iter = registry->GetGroups().find(group_name);
        TestRegistry::GroupMap::const_iterator group_iter_end = registry->GetGroups().end();

        reporter->RunBegin();

        if(group_iter != group_iter_end)
            RunGroup_(group_iter);

        reporter->RunEnd();
    }

    void RunAllGroups()
    {
        TestRegistry::GroupMap::const_iterator group_iter = registry->GetGroups().begin();
        TestRegistry::GroupMap::const_iterator group_iter_end = registry->GetGroups().end();

		reporter->RunBegin();

        for(; group_iter != group_iter_end; ++group_iter)
            RunGroup_(group_iter);

		reporter->RunEnd();
    }

private:
    void RunGroupBegin(const TestRegistry::GroupMap::const_iterator &group_iter,
                      const GroupBase::TestMap::const_iterator &test_iter,
                      TestResult &tr)
    {
        tr.test_no = test_iter->first;
        tr.message.clear();
        tr.test_result = TestResult::ok;

        try
        {
            (group_iter->second->*(test_iter->second))();
        }
        catch(const Failure &ex)
		{
            tr.group_result = TestResult::ex_group_ctor;
            tr.test_result = TestResult::fail;
			tr.message = ex.what();
		}
		catch(const std::exception &ex)
		{
            tr.group_result = TestResult::ex_group_ctor;
            tr.test_result = TestResult::ex;
			tr.message = ex.what();
		}
		catch(...)
		{
            tr.group_result = TestResult::ex_group_ctor;
            tr.test_result = TestResult::ex_unknow;

		}

    }

    void RunGroupEnd(const TestRegistry::GroupMap::const_iterator &group_iter,
                    const GroupBase::TestMap::const_iterator &test_iter,
                    TestResult &tr)
    {
        tr.test_no = test_iter->first;
        tr.message.clear();
        tr.test_result = TestResult::ok;

        try
        {
            (group_iter->second->*(test_iter->second))();
        }
        catch(const Failure &ex)
		{

            if(tr.group_result == TestResult::fail)
                tr.group_result = TestResult::fail_and_ex_group_dtor;
            else
                tr.group_result = TestResult::ex_group_dtor;
            tr.test_result = TestResult::fail;
			tr.message = ex.what();

		}
		catch(const std::exception &ex)
		{

            if(tr.group_result == TestResult::fail)
                tr.group_result = TestResult::fail_and_ex_group_dtor;
            else
                tr.group_result = TestResult::ex_group_dtor;
            tr.test_result = TestResult::ex;
			tr.message = ex.what();

		}
		catch(...)
		{
            if(tr.group_result == TestResult::fail)
                tr.group_result = TestResult::fail_and_ex_group_dtor;
            else
                tr.group_result = TestResult::ex_group_dtor;
            tr.test_result = TestResult::ex_unknow;

		}

    }

    void RunTest(const TestRegistry::GroupMap::const_iterator &group_iter,
                 const GroupBase::TestMap::const_iterator &test_iter,
                 const GroupBase::TestMap::const_iterator &test_begin_iter,
                 const GroupBase::TestMap::const_iterator &test_end_iter,
                 TestResult &tr,
                 bool &is_dummy)
    {
        tr.test_no = test_iter->first;
        tr.message.clear();
        tr.test_result = TestResult::ok;

        try
        {
            (group_iter->second->*(test_begin_iter->second))();  //TEST_BEGIN
            is_dummy = false;
            group_iter->second->SetDummy(false);
            (group_iter->second->*(test_iter->second))();
            is_dummy = group_iter->second->IsDummy();
            (group_iter->second->*(test_end_iter->second))();    //TEST_END
        }
        catch(const Failure &ex)
		{
            tr.group_result = TestResult::fail;
            tr.test_result = TestResult::fail;
			tr.message = ex.what();

		}
		catch(const std::exception &ex)
		{
            tr.group_result = TestResult::fail;
            tr.test_result = TestResult::ex;
			tr.message = ex.what();

		}
		catch(...)
		{
            tr.group_result = TestResult::fail;
            tr.test_result = TestResult::ex_unknow;

		}

        if(tr.test_no > 4)
            tr.test_no -= 4; //tricky!
    }

    void RunGroup_(TestRegistry::GroupMap::const_iterator &group_iter)
    {
        if(group_iter->second->GetTests().size() < 4)
            return;

		GroupBase::TestMap::const_iterator test_iter = group_iter->second->GetTests().begin();  //GROUP_BEGIN
		GroupBase::TestMap::const_iterator test_iter_end = group_iter->second->GetTests().end();

        const std::string group_name = group_iter->second->GetGroupName();
        reporter->GroupBegin(group_name);

        TestResult tr;
        tr.group_name = group_name;
		tr.group_result = TestResult::ok;

        RunGroupBegin(group_iter, test_iter, tr);
        if(tr.group_result == TestResult::ex_group_ctor)
        {
            reporter->GroupEnd(group_name, tr);
			return;
        }

        GroupBase::TestMap::const_iterator group_end_iter = ++test_iter;  //GROUP_END
        GroupBase::TestMap::const_iterator test_begin_iter = ++test_iter;  //TEST_BEGIN
        GroupBase::TestMap::const_iterator test_end_iter = ++test_iter;  //TEST_END
        ++test_iter;

        group_iter->second->Init();

        for(; test_iter != test_iter_end; ++test_iter)
		{
            reporter->TestBegin(test_iter->first);
            bool is_dummy = true;
            RunTest(group_iter, test_iter, test_begin_iter, test_end_iter, tr, is_dummy);
            if(is_dummy)
                break;
            reporter->TestEnd(test_iter->first, tr);

		}

        //group dtor
        RunGroupEnd(group_iter, group_end_iter, tr);

        reporter->GroupEnd(group_name, tr);

    }

private:
    TestReporter  *reporter;
	TestRegistry  *registry;

};

inline void RunAllGroups()
{
	ConsoleReporter reporter;
	TestRunner *runner = TestRunner::Instance();
	runner->SetReporter(&reporter);
	runner->RunAllGroups();
}

inline void RunGroup(const std::string &group_name)
{
	ConsoleReporter reporter;
	TestRunner *runner = TestRunner::Instance();
	runner->SetReporter(&reporter);
	runner->RunGroup(group_name);
}

}

#endif
