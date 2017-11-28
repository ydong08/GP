#ifndef __CONSLOE_H_
#define __CONSLOE_H_

// C 89 header files
#include <assert.h>
#include <errno.h>
#include <strings.h>

// C++ 98 header files
#include <list>
#include <string>
#include <sstream>
#include <iostream>

// Rain header files
#include "TtyColor.h"

namespace stdx {

#define STDX_LOG_OUTPUT(osscolor, statement) \
    do                                  \
    {                                   \
        std::ostringstream oss;         \
        oss << osscolor;                \
        oss << statement;               \
        oss << stdx::endcolor;          \
        oss << std::endl;               \
        std::cerr << oss.str();         \
    } while (false)

#define STDX_LOG_DEBUG(statement)    STDX_LOG_OUTPUT(stdx::endcolor, statement)
#define STDX_LOG_INFO(statement)     STDX_LOG_OUTPUT(stdx::green, statement)
#define STDX_LOG_NOTICE(statement)   STDX_LOG_OUTPUT(stdx::white, statement)
#define STDX_LOG_WARNING(statement)  STDX_LOG_OUTPUT(stdx::yellow, statement)
#define STDX_LOG_ERROR(statement)    STDX_LOG_OUTPUT(stdx::red, statement)
#define STDX_LOG_CRIT(statement)     STDX_LOG_OUTPUT(stdx::red, statement)
#define STDX_LOG_ALERT(statement)    STDX_LOG_OUTPUT(stdx::red, statement)
#define STDX_LOG_EMERG(statement)    STDX_LOG_OUTPUT(stdx::red, statement)


#define LOG_FORMAT(format, ...) \
    do                                                  \
    {                                                   \
        int n = snprintf(NULL, 0, format, __VA_ARGS__); \
        if (n > 0)                                      \
        {                                               \
            char buf[n + 1];                            \
            snprintf(buf, n + 1, format, __VA_ARGS__);  \
            std::cerr << buf << std::endl;              \
        }                                               \
    } while (false)

#define STDX_LOG_PERROR(statement) \
    do                                          \
    {                                           \
        std::string strerr = strerror(errno);   \
        LOG_ERROR(statement << ": " << strerr); \
    } while (false)

class call_stack
{
private:
    typedef std::list<std::string>  __stack_type;
    __stack_type m_calls;

public:
    void push(const std::string& strcall)
    {
        m_calls.push_back(strcall);
    }

    void pop()
    {
        m_calls.pop_back();
    }

    std::string names() const
    {
        std::string strcalls;
        for (__stack_type::const_iterator it = m_calls.begin();
                it != m_calls.end(); ++it)
        {
            strcalls += " --> " + *it + "()";
        }
        return strcalls;
    }
};

class call_trace
{
private:
    call_stack& m_calls;

public:
    call_trace(call_stack& calls, const std::string& name) : m_calls(calls)
    {
        m_calls.push(name);
        STDX_LOG_DEBUG("STACK " << std::string(10,'+') << m_calls.names());
    }

    ~call_trace()
    {
        STDX_LOG_DEBUG("STACK " << std::string(10,'-') << m_calls.names());
        m_calls.pop();
    }
};


} // namespace stdx


#endif // __STDX_CONSOLE_H
