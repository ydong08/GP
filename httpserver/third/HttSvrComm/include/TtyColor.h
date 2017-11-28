#ifndef __STDX_TTYCOLOR_H
#define __STDX_TTYCOLOR_H


// Posix header files
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

// C 89 header files
#include <assert.h>
#include <errno.h>

// C++ 98 header files
#include <string>
#include <sstream>
#include <iomanip>


namespace stdx {

const char* const _Text_endcolor     = "\033[0m";
const char* const _Text_black        = "\033[22;30m";
const char* const _Text_red          = "\033[22;31m";
const char* const _Text_green        = "\033[22;32m";
const char* const _Text_brown        = "\033[22;33m";
const char* const _Text_blue         = "\033[22;34m";
const char* const _Text_magenta      = "\033[22;35m";
const char* const _Text_cyan         = "\033[22;36m";
const char* const _Text_gran         = "\033[22;37m";
const char* const _Text_dark         = "\033[01;30m";
const char* const _Text_lightred     = "\033[01;31m";
const char* const _Text_lightgreen   = "\033[01;32m";
const char* const _Text_yellow       = "\033[01;33m";
const char* const _Text_lightblue    = "\033[01;34m";
const char* const _Text_lightmagenta = "\033[01;35m";
const char* const _Text_lightcyan    = "\033[01;36m";
const char* const _Text_white        = "\033[01;37m";
const char* const _Back_black        = "\033[22;40m";
const char* const _Back_red          = "\033[22;41m";
const char* const _Back_green        = "\033[22;42m";
const char* const _Back_brown        = "\033[22;43m";
const char* const _Back_blue         = "\033[22;44m";
const char* const _Back_magenta      = "\033[22;45m";
const char* const _Back_cyan         = "\033[22;46m";
const char* const _Back_gran         = "\033[22;47m";
const char* const _Back_dark         = "\033[01;40m";
const char* const _Back_lightred     = "\033[01;41m";
const char* const _Back_lightgreen   = "\033[01;42m";
const char* const _Back_yellow       = "\033[01;43m";
const char* const _Back_lightblue    = "\033[01;44m";
const char* const _Back_lightmagenta = "\033[01;45m";
const char* const _Back_lightcyan    = "\033[01;46m";
const char* const _Back_white        = "\033[01;47m";


struct _Log_color { std::string m_color; };

inline _Log_color
setcolor(const std::string& clr)
{
    _Log_color __x;
    __x.m_color = clr;
    return __x;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, _Log_color __f)
{
    __os << __f.m_color << std::flush;
    return __os;
}


template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
endcolor(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_endcolor << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
black(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_black << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
red(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_red << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
green(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_green << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
brown(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_brown << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
blue(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_blue << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
magenta(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_magenta << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
cyan(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_cyan << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
gran(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_gran << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
dark(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_dark << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
lightred(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_lightred << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
lightgreen(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_lightgreen << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
yellow(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_yellow << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
lightblue(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_lightblue << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
lightmagenta(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_lightmagenta << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
lightcyan(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_lightcyan << std::flush;
    return __os;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
white(std::basic_ostream<_CharT, _Traits>& __os)
{
    __os << _Text_white << std::flush;
    return __os;
}

struct _Log_dump { const void* _M_addr; int _M_len; };

inline _Log_dump
dump(const void* __addr, int __len)
{
    _Log_dump __x;
    __x._M_addr = __addr;
    __x._M_len = __len;
    return __x;
}

template <typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, _Log_dump __f)
{
    const std::string __table("0123456789ABCDEF");

    const int __line = 32;
    const int __interval = 4;

    int __hexlen = __f._M_len * 2; // one byte == two hex character

    const char* __p = static_cast<const char*>(__f._M_addr);
    while (__hexlen > 0)
    {
        // print memory address
        int __count = 0;
        if ((__count % __line) == 0)
        {
            std::ostringstream __ostrm;
            __ostrm << std::setw((sizeof __p)*2) << std::setfill('0');
            __ostrm << std::hex << reinterpret_cast<unsigned long>(__p);
            __os << white;
            __os << __ostrm.str() + ":";
            __os << endcolor;
        }

        // print memory as hex
        int __min = std::min<int>(__hexlen, __line);
        std::string __chardump(__p, __p + __min/2);

        __count = 0;
        while (__count < __min)
        {
            if ((__count % __interval) == 0)
                __os << ' ';

            unsigned char __ch = *__p++;
            std::ostringstream __ostrm;
            __ostrm << std::setw(2) << std::setfill('0');
            __ostrm << std::hex << std::noshowbase << static_cast<unsigned>(__ch);
            __os << __ostrm.str();

            __count += 2;
        }
        __hexlen -= __min;
        __os << std::string(__line - __min + (__line - __min)/__interval, ' ');

        // print memory as character
        __os << yellow;
        __os << "  ";
        for (std::string::size_type i = 0; i < __chardump.size(); ++i)
        {
            if (isprint(__chardump[i]))
            {
                __os << yellow;
                __os << __chardump[i];
            }
            else
            {
                __os << lightmagenta;
                __os << '.';
            }
        //  if (!isprint(__chardump[i]))
        //      __chardump[i] = '.';
        }
    //  __os << "  " + __chardump;
        __os << endcolor;

        // newline
        __os << '\n';
    }
    return __os;
}
} // namespace stdx

#endif // __STDX_TTYCOLOR_H