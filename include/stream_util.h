//
//  stream_util.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/3.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef stream_util_h
#define stream_util_h

#include <unistd.h>
#include <ostream>
#include <errno.h>
#include <netinet/in.h>

struct timezone;
struct timeval;
struct tm;
struct timespec;
struct timeb;

#if defined(__GNUC__)
#	define WBL_CURRENT_FUNCTION	__PRETTY_FUNCTION__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#	define WBL_CURRENT_FUNCTION	__func__
#else
#	define WBL_CURRENT_FUNCTION	"(unknown)"
#endif

#define TRACE_INFO()	"[" << __FILE__ << ":" << __LINE__ << "|" << WBL_CURRENT_FUNCTION << "] "

namespace shadow
{
    /** for socket info output
     */
    struct socket_info_helper1
    {
        int _sock;
        
        explicit socket_info_helper1(int sock)
        : _sock(sock)
        {}
    };
    
    /** for sockaddr_in info output
     */
    struct sockaddr_in_info_helper1
    {
        const sockaddr_in& _in;
        
        explicit sockaddr_in_info_helper1(const sockaddr_in& in)
        : _in(in)
        {}
    };
    
    /** for in_addr info output
     */
    struct in_addr_info_helper1
    {
        in_addr _in;
        
        explicit in_addr_info_helper1(in_addr in)
        : _in(in)
        {}
        
        explicit in_addr_info_helper1(unsigned long addr)
        {
            _in.s_addr = addr;
        }
    };
    
    /** for time info output
     */
    struct time_info_helper1
    {
        time_t _time;
        
        explicit time_info_helper1(time_t t)
        : _time(t)
        {}
    };
    
    /** for time info output
     */
    struct time_info_helper2
    {
        time_t _time;
        const char * _format;
        
        explicit time_info_helper2(time_t t, const char * format)
        : _time(t)
        , _format(format)
        {}
    };
    
    /** for string output, encode non-printable character
     */
    struct encoded_string_info_helper1
    {
        const char * _str;
        
        encoded_string_info_helper1(const char * str)
        : _str(str)
        {}
    };
    
    /** for string output, encode non-printable character
     */
    struct encoded_string_info_helper2
    {
        const char *	_str;
        size_t			_len;
        
        encoded_string_info_helper2(const char * str, size_t len)
        : _str(str)
        , _len(len)
        {}
    };
    
    /** for string output, output hex code
     */
    struct hex_string_info_helpper1
    {
        const char * _str;
        
        hex_string_info_helpper1(const char * str)
        : _str(str)
        {}
    };
    
    /** for string output, output hex code
     */
    struct hex_string_info_helpper2
    {
        const char *	_str;
        size_t			_len;
        
        hex_string_info_helpper2(const char * str, size_t len)
        : _str(str)
        , _len(len)
        {}
    };
    
    /** for string output, output hex code
     */
    struct hex_string_info_helpper3
    {
        const char *	_str;
        const char		_sep;
    };
    
    /** for string output, output hex code
     */
    struct hex_string_info_helpper4
    {
        const char *	_str;
        size_t			_len;
        const char		_sep;
    };
    
    /** for string output, output hex code & string
     */
    struct debug_string_info_helpper1
    {
        const char *	_str;
        const char *	_head_str;
    };
    
    /** for string output, output hex code & string
     */
    struct debug_string_info_helpper2
    {
        const char *	_str;
        size_t			_len;
        const char *	_head_str;
    };
    
    /** for write buffer
     */
    struct buffer_info_helper
    {
        const char *    _buf;
        size_t          _len;
    };
    
    /** for output a range
     */
    template < typename FI >
    struct range_info_helper
    {
        FI	first;
        FI	last;
        const char * separator;
    };
    
    /** for output a range
     */
    template < typename FI, typename FN >
    struct range_info_helper2
    {
        FI	first;
        FI	last;
        const char * separator;
        FN	fn;
    };
    
    /** for pair output
     */
    template < typename FT, typename ST >
    struct pair_info_helper
    {
        const std::pair<FT, ST>&	pr;
    };
    
    /** for c standard error info output
     */
    struct c_error_info_helper
    {
        int	errnum;
    };
    
    inline socket_info_helper1 socket_info(int sockfd)
    {
        return socket_info_helper1(sockfd);
    }
    
    /// output socket info
    std::ostream& operator<<(std::ostream& ost, socket_info_helper1 si);
    
    inline sockaddr_in_info_helper1 sockaddr_in_info(const sockaddr_in& in)
    {
        return sockaddr_in_info_helper1(in);
    }
    
    inline in_addr_info_helper1 sockaddr_in_info(in_addr in)
    {
        return in_addr_info_helper1(in);
    }
    
    inline in_addr_info_helper1 sockaddr_in_info(unsigned long addr)
    {
        return in_addr_info_helper1(addr);
    }
    
    /// output sockaddr_in info
    std::ostream& operator<<(std::ostream& ost, sockaddr_in_info_helper1 si);
    
    std::ostream& operator<<(std::ostream& ost, in_addr_info_helper1 si);
    
    inline time_info_helper1 time_info(time_t t = ::time(NULL))
    {
        return time_info_helper1(t);
    }
    
    inline time_info_helper2 time_info(time_t t, const char * format)
    {
        return time_info_helper2(t, format);
    }
    
    inline time_info_helper2 time_info(time_t t, const std::string& format)
    {
        return time_info(t, format.c_str());
    }
    
    /// output time info
    std::ostream& operator<<(std::ostream& ost, time_info_helper1 ti);
    
    /// output time info
    std::ostream& operator<<(std::ostream& ost, time_info_helper2 ti);
    
    inline encoded_string_info_helper1 encoded_string_info(const char * str)
    {
        return encoded_string_info_helper1(str);
    }
    
    inline encoded_string_info_helper2 encoded_string_info(const char * str, size_t len)
    {
        return encoded_string_info_helper2(str, len);
    }
    
    inline encoded_string_info_helper2 encoded_string_info(const std::string& str)
    {
        return encoded_string_info_helper2(str.data(), str.size());
    }
    
    inline encoded_string_info_helper2 encoded_string_info(const std::string& str, size_t len)
    {
        len = str.size() > len ? len : str.size();
        return encoded_string_info_helper2(str.data(), len);
    }
    
    std::ostream& operator<<(std::ostream& ost, encoded_string_info_helper1 s);
    std::ostream& operator<<(std::ostream& ost, encoded_string_info_helper2 s);
    
    inline hex_string_info_helpper1 hex_string_info(const char * str)
    {
        return hex_string_info_helpper1(str);
    }
    
    inline hex_string_info_helpper2 hex_string_info(const char * str, size_t len)
    {
        return hex_string_info_helpper2(str, len);
    }
    
    inline hex_string_info_helpper2 hex_string_info(const std::string& str)
    {
        return hex_string_info_helpper2(str.data(), str.size());
    }
    
    inline hex_string_info_helpper2 hex_string_info(const std::string& str, size_t len)
    {
        len = str.size() > len ? len : str.size();
        return hex_string_info_helpper2(str.data(), len);
    }
    
    inline hex_string_info_helpper3 hex_string_info(const char * str, char separator)
    {
        hex_string_info_helpper3 hi = {str, separator};
        return hi;
    }
    
    inline hex_string_info_helpper4 hex_string_info(const char * str, size_t len, char separator)
    {
        hex_string_info_helpper4 hi = {str, len, separator};
        return hi;
    }
    
    inline hex_string_info_helpper4 hex_string_info(const std::string& str, char separator)
    {
        return hex_string_info(str.data(), str.size(), separator);
    }
    
    inline hex_string_info_helpper4 hex_string_info(const std::string& str, size_t len, char separator)
    {
        len = str.size() > len ? len : str.size();
        return hex_string_info(str.data(), len, separator);
    }
    
    inline debug_string_info_helpper1 debug_string_info(const char * str, const char * head_str = "")
    {
        debug_string_info_helpper1 hi = {str, head_str};
        return hi;
    }
    
    inline debug_string_info_helpper2 debug_string_info(const char * str, size_t len, const char * head_str = "")
    {
        debug_string_info_helpper2 hi = {str, len, head_str};
        return hi;
    }
    
    inline debug_string_info_helpper2 debug_string_info(const std::string& str, const char * head_str = "")
    {
        return debug_string_info(str.data(), str.size(), head_str);
    }
    
    inline debug_string_info_helpper2 debug_string_info(const std::string& str, size_t len, const char * head_str = "")
    {
        len = str.size() > len ? len : str.size();
        return debug_string_info(str.data(), len, head_str);
    }
    
    std::ostream& operator<<(std::ostream& ost, hex_string_info_helpper1 s);
    std::ostream& operator<<(std::ostream& ost, hex_string_info_helpper2 s);
    std::ostream& operator<<(std::ostream& ost, hex_string_info_helpper3 s);
    std::ostream& operator<<(std::ostream& ost, hex_string_info_helpper4 s);
    
    std::ostream& operator<<(std::ostream& ost, debug_string_info_helpper1 s);
    std::ostream& operator<<(std::ostream& ost, debug_string_info_helpper2 s);
    
    
    inline buffer_info_helper buffer_info(const char * buf, size_t len)
    {
        buffer_info_helper b = {buf, len};
        return b;
    }
    
    std::ostream& operator<<(std::ostream& ost, buffer_info_helper bi);
    
    template < typename FI >
    inline range_info_helper<FI> range_info(FI first, FI last, const char * separator)
    {
        range_info_helper<FI> ri = {first, last, separator};
        return ri;
    }
    
    template < typename FI >
    inline range_info_helper<FI> range_info(FI first, FI last, const std::string& separator)
    {
        return range_info(first, last, separator.c_str());
    }
    
    template < typename FI >
    inline range_info_helper<FI> range_info(FI first, FI last)
    {
        return range_info(first, last, ", ");
    }
    
    template < typename ContainerT >
    inline range_info_helper<typename ContainerT::const_iterator> container_info(const ContainerT& con)
    {
        return range_info(con.begin(), con.end());
    }
    
    template < typename ContainerT >
    inline range_info_helper<typename ContainerT::const_iterator> container_info(const ContainerT& con, const char * separator)
    {
        return range_info(con.begin(), con.end(), separator);
    }
    
    template < typename ContainerT >
    inline range_info_helper<typename ContainerT::const_iterator> container_info(const ContainerT& con, const std::string& separator)
    {
        return range_info(con.begin(), con.end(), separator.c_str());
    }
    
    template < typename T >
    inline range_info_helper<const T *> array_info(const T * addr, size_t length, const char * separator)
    {
        return range_info(addr, addr + length, separator);
    }
    
    template < typename T >
    inline range_info_helper<const T *> array_info(const T * addr, size_t length, const std::string& separator)
    {
        return range_info(addr, addr + length, separator.c_str());
    }
    
    template < typename T >
    inline range_info_helper<const T *> array_info(const T * addr, size_t length)
    {
        return array_info(addr, length, ", ");
    }
    
    template < typename FI >
    std::ostream& operator<<(std::ostream& ost, range_info_helper<FI> ri)
    {
        ost << '[';
        if(ri.first != ri.last){
            ost << *ri.first;
            ++ri.first;
        }
        for( ; ri.first != ri.last; ++ri.first){
            ost << ri.separator << *ri.first;
        }
        ost << ']';
        return ost;
    }
    
    template < typename FI, typename FN >
    inline range_info_helper2<FI, FN> range_info2(FI first, FI last, const char * separator, FN fn)
    {
        range_info_helper2<FI, FN> ri = {first, last, separator, fn};
        return ri;
    }
    
    template < typename FI, typename FN >
    inline range_info_helper2<FI, FN> range_info2(FI first, FI last, const std::string& separator, FN fn)
    {
        return range_info2(first, last, separator.c_str(), fn);
    }
    
    template < typename FI, typename FN >
    inline range_info_helper2<FI, FN> range_info2(FI first, FI last, FN fn)
    {
        return range_info2(first, last, ", ", fn);
    }
    
    template < typename ContainerT, typename FN >
    inline range_info_helper2<typename ContainerT::const_iterator, FN> container_info2(const ContainerT& con, FN fn)
    {
        return range_info2(con.begin(), con.end(), fn);
    }
    
    template < typename ContainerT, typename FN >
    inline range_info_helper2<typename ContainerT::const_iterator, FN> container_info2(const ContainerT& con, const char * separator, FN fn)
    {
        return range_info2(con.begin(), con.end(), separator, fn);
    }
    
    template < typename ContainerT, typename FN >
    inline range_info_helper2<typename ContainerT::const_iterator, FN> container_info2(const ContainerT& con, const std::string& separator, FN fn)
    {
        return range_info2(con.begin(), con.end(), separator.c_str(), fn);
    }
    
    template < typename FI, typename FN >
    std::ostream& operator<<(std::ostream& ost, range_info_helper2<FI, FN> ri)
    {
        ost << '[';
        if(ri.first != ri.last){
            ost << ri.fn(*ri.first);
            ++ri.first;
        }
        for( ; ri.first != ri.last; ++ri.first){
            ost << ri.separator << ri.fn(*ri.first);
        }
        ost << ']';
        return ost;
    }
    
    template < typename FT, typename ST >
    inline pair_info_helper<FT, ST> pair_info(const std::pair<FT, ST>& pr)
    {
        pair_info_helper<FT, ST> pi = {pr};
        return pi;
    }
    
    template < typename FT, typename ST >
    inline std::ostream& operator<<(std::ostream& ost, pair_info_helper<FT, ST> pr)
    {
        ost << '(' << pr.pr.first << ", " << pr.pr.second << ')';
        return ost;
    }
    
    inline c_error_info_helper c_error_info(int errnum = errno)
    {
        c_error_info_helper ei = {errnum};
        return ei;
    }
    
    std::ostream& operator<<(std::ostream& ost, c_error_info_helper ei);
    
    struct timeval_info_helper
    {
        const struct timeval * tv;
    };
    
    std::ostream& operator<<(std::ostream& ost, timeval_info_helper ti);
    
    inline timeval_info_helper timeval_info(const timeval& tv)
    {
        timeval_info_helper ti = {&tv};
        return ti;
    }
    
    struct timezone_info_helper
    {
        const struct timezone * tz;
    };
    
    std::ostream& operator<<(std::ostream& ost, timezone_info_helper ti);
    
    inline timezone_info_helper timezone_info(const struct timezone& tz)
    {
        timezone_info_helper ti = {&tz};
        return ti;
    }
    
    struct tm_info_helper
    {
        const struct tm * t;
    };
    
    std::ostream& operator<<(std::ostream& ost, tm_info_helper ti);
    
    inline tm_info_helper tm_info(const struct tm& t)
    {
        tm_info_helper ti = {&t};
        return ti;
    }
    
    struct timespec_info_helper
    {
        const struct timespec * t;
    };
    
    std::ostream& operator<<(std::ostream& ost, timespec_info_helper ti);
    
    inline timespec_info_helper timespec_info(const struct timespec& t)
    {
        timespec_info_helper ti = {&t};
        return ti;
    }
    
    struct timeb_info_helper
    {
        const struct timeb * t;
    };
    
    std::ostream& operator<<(std::ostream& ost, timeb_info_helper ti);
    
    inline timeb_info_helper timeb_info(const struct timeb& t)
    {
        timeb_info_helper ti = {&t};
        return ti;
    }
    
}



#endif /* stream_util_h */
