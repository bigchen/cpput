#ifndef cpput_registry_h
#define cpput_registry_h

#include <map>
#include <string>
#include <algorithm>

namespace cpput
{

class GroupBase;
typedef void (GroupBase::*method)();

class GroupBase
{
public:
	typedef std::map<int, method> TestMap;
	explicit GroupBase(const std::string &name) : group_name(name), dummy_test(false) {}
	virtual ~GroupBase() {}

	const TestMap & GetTests() const
	{
		return tests;
	}

	void Init()
	{
		dummy_test = false;
	}

	bool IsDummy()
	{
		return dummy_test;
	}

    void SetDummy(bool dummy)
    {
        dummy_test = dummy;
    }

	const std::string & GetGroupName()
	{
		return group_name;
	}

protected:
	const std::string group_name;
	TestMap           tests;
    bool              dummy_test;
};

class TestRegistry
{
public:
	typedef std::map<std::string, GroupBase*> GroupMap;

	static TestRegistry * Instance()
	{
		static TestRegistry reg;
		return &reg;
	}

	void RegisterGroup(const std::string& name, GroupBase* gr)
    {
		std::string pure_name = name;
		pure_name.erase(std::remove_if(pure_name.begin(), pure_name.end(), isspace), pure_name.end());
		groups[pure_name] = gr;
    }

	const GroupMap & GetGroups() const
	{
		return groups;
	}

private:
	GroupMap groups;

};

}

#endif

