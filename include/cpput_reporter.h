#ifndef cpput_reporter_h
#define cpput_reporter_h

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace cpput
{

struct TestResult
{
    TestResult() : test_no(0), group_result(ok), test_result(ok)
    {
    }

    int test_no;
    std::string group_name;
    std::string test_name;

    enum ResultType
    {
        ok,
        fail,
        ex,
        ex_group_ctor,
        ex_group_dtor,
        fail_and_ex_group_dtor,
        ex_unknow
    };

    ResultType  group_result;
    ResultType  test_result;
    std::string message;

};

class TestReporter
{
public:
    TestReporter() {}
    virtual ~TestReporter() {}

    void Init()
    {
        total_count = 0;
        ok_count = 0;
        exception_count = 0;
        failure_count = 0;
        ex_unknow_count = 0;
        group_count = 0;
        group_ok_count = 0;
        group_failure_count = 0;
        group_ex_ctor_count = 0;
        group_ex_dtor_count = 0;
        group_fail_and_ex_dtor_count = 0;
    }

    void RunBegin()
    {
        Init();
        DoRunBegin();
    }

    void RunEnd()
    {
        DoRunEnd();
    }

    void GroupBegin(const std::string &name)
    {
        DoGroupBegin(name);
    }

    void GroupEnd(const std::string &name, const TestResult& group_tr)
    {
        group_count++;
        switch(group_tr.group_result)
        {
        case TestResult::ok:
            group_ok_count++;
            break;
        case TestResult::fail:
            group_failure_count++;
            break;
        case TestResult::ex_group_ctor:
            group_ex_ctor_count++;
            break;
        case TestResult::ex_group_dtor:
            group_ex_dtor_count++;
            break;
        case TestResult::fail_and_ex_group_dtor:
            group_fail_and_ex_dtor_count++;
            break;
        default:
            break;
        }

        if(group_tr.group_result != TestResult::ok)
            not_passed_group_list.push_back(group_tr);

        DoGroupEnd(name, group_tr);
    }

    void TestBegin(int n)
    {
        DoTestBegin(n);
    }


    void TestEnd(int n, const TestResult& tr)
    {
        total_count++;
        switch(tr.test_result)
        {
        case TestResult::ok:
            ok_count++;
            break;
        case TestResult::fail:
            failure_count++;
            break;
        case TestResult::ex:
            exception_count++;
            break;
        case TestResult::ex_unknow:
            ex_unknow_count++;
            break;
        default:
            break;
        }

        if(tr.test_result != TestResult::ok)
            not_passed_list.push_back(tr);

        DoTestEnd(n, tr);
    }

    virtual void DoRunBegin() {}
    virtual void DoRunEnd() {}
    virtual void DoGroupBegin(const std::string &name) {}
    virtual void DoGroupEnd(const std::string &name, const TestResult &tr) {}
    virtual void DoTestBegin(int n) {}
    virtual void DoTestEnd(int n, const TestResult &tr) {}

protected:
    int total_count;
    int ok_count;
    int exception_count;
    int failure_count;
    int ex_unknow_count;

    int group_count;
    int group_ok_count;
    int group_failure_count;
    int group_ex_ctor_count;
    int group_ex_dtor_count;
    int group_fail_and_ex_dtor_count;
    std::vector<TestResult> not_passed_group_list;
    std::vector<TestResult> not_passed_list;
};

class ConsoleReporter : public TestReporter
{
public:
    void DoRunBegin()
    {
        std::cout << "C/C++ Unit Testing Framework. [V0.1]. " << "\n";
        std::cout << "Author: Chen Jiazhou" << "\n\n";
    }

    void ShowResult()
    {
        //group stat
        std::cout << "\n";
        std::cout << "The statistics for test group:" << "\n";
        std::cout << "        Groups = " << group_count << ", Succ = " << group_ok_count
            << ", Fail = " << (group_count - group_ok_count) << "\n";

        std::cout << "The statistics for failure group:" << "\n";
        std::cout << "        CF = " << group_ex_ctor_count << ", DF = " << group_ex_dtor_count
            << ", GF = " << group_failure_count
            << ", GF&DF = " << group_fail_and_ex_dtor_count << "\n\n";

        //test case stat
        std::cout << "The statistics for test case:" << "\n";
        std::cout << "        Test Cases = " << total_count << ", Succ = " << ok_count
            << ", Fail = " << (total_count - ok_count) << "\n";

        std::cout << "The statistics for failure test case:" << "\n";
        std::cout << "        AF = " << failure_count << ", SE = " << exception_count
            << ", UE = " << ex_unknow_count << "\n";


    }

    void ShowDetail()
    {
        TestResult tr;
        std::vector<TestResult>::const_iterator group_iter = not_passed_group_list.begin();
        std::vector<TestResult>::const_iterator group_iter_end = not_passed_group_list.end();

        for(; group_iter != group_iter_end; ++group_iter)
        {
            std::cout << std::endl;
            tr = *group_iter;
            std::cout << "Group[" << tr.group_name << "] has error(s).";

            switch(tr.group_result)
            {
            case TestResult::fail:
                std::cout << " -> [GF]" << "\n";
                break;
            case TestResult::ex_group_ctor:
                std::cout << " -> [CF]" << "\n";
                break;
            case TestResult::ex_group_dtor:
                std::cout << " -> [DF]" << "\n";
                break;
            case TestResult::fail_and_ex_group_dtor:
                std::cout << " -> [GF&DF]" << "\n";
                break;
            default:
                break;
            }

        }

        std::vector<TestResult>::const_iterator test_iter = not_passed_list.begin();
        std::vector<TestResult>::const_iterator test_iter_end = not_passed_list.end();

        for(; test_iter != test_iter_end; ++test_iter)
        {
            std::cout << std::endl;
            tr = *test_iter;
            std::cout << "Group[" << tr.group_name << "] -> Test[" << std::setw(2) << tr.test_no << "] has error -> ";
            switch(tr.test_result)
            {
            case TestResult::fail:
                std::cout << "AF.";
                break;
            case TestResult::ex:
                std::cout << "SE.";
                break;
            case TestResult::ex_unknow:
                std::cout << "UE.";
                break;
            default:
                break;
            }
            if(!tr.message.empty())
                    std::cout << "[" << tr.message.c_str() << "]" << std::endl;
            else
                std::cout << "\n";
        }
        std::cout << "\n";
        std::cout << "AF: Assert Failure." << "\n";
        std::cout << "SE: Std Exception." << "\n";
        std::cout << "UE: Unknown Exception." << "\n";
        std::cout << "CF: (Group) Constructor Failure." << "\n";
        std::cout << "DF: (Group) Destructor Failure." << "\n";
        std::cout << "GF: Group Failure (AF|SE|UE)." << "\n";
        std::cout << std::endl;
    }

    void DoRunEnd()
    {
        ShowResult();
        ShowDetail();
    }

    void DoGroupBegin(const std::string &name)
    {
    }

    void DoGroupEnd(const std::string &name, const TestResult& group_tr)
    {
    }

    void DoTestBegin(int n)
    {
    }

    void DoTestEnd(int n, const TestResult& tr)
    {
    }


};

}

#endif

