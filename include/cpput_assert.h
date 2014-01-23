#ifndef cpput_assert_h
#define cpput_assert_h

#include <string>
#include <sstream>

namespace cpput
{

class Failure : public std::exception
{
public:
    explicit Failure(const char *msg) : message(msg)
    {
    }

    virtual ~Failure() throw()
    {
    }

    const char* what() const throw()
    {
        return message;
    }

private:
    const char *message;
};

inline void EXPECT(bool cond)
{
    if (!cond)
    {
        throw Failure("Assert Failure!");
    }
}

inline void EXPECT(bool cond, const char *msg)
{
    if (!cond)
    {
        throw Failure(msg);
    }
}

inline void EXPECT_NOT(bool cond)
{
    EXPECT(!cond);
}

inline void EXPECT_NOT(bool cond, const char *msg)
{
    EXPECT(!cond, msg);
}

}

#endif
