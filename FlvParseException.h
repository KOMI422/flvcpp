#ifndef _FLV_PARSE_EXCEPTION_H_
#define _FLV_PARSE_EXCEPTION_H_

#include <exception>
#include <string>

class FlvParseException : public std::exception
{
public:
    FlvParseException(const std::string& exceptionHint) noexcept
        : m_exceptionHint(exceptionHint) {}
    const char* what() const noexcept { return m_exceptionHint.c_str(); }
private:
    const std::string m_exceptionHint;
};

#endif