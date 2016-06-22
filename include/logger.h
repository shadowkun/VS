//
//  logger.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef logger_h
#define logger_h



#include <streambuf>
#include <stdarg.h>
#include <ostream>


#include "pthread_util.h"
#include "log_writer.h"

namespace shadow
{
    
    /** a streambuf wrap with a file descriptors
     */
    class logstreambuf: public std::basic_streambuf<char>
    {
        char *			_out;			///< out buffer
        std::streamsize	_out_len;		///< out buffer length
        log_writer *	_writer;		///< log writer
        std::string		_timeformat;	///< time format
        
        logstreambuf(const logstreambuf&);
        logstreambuf& operator=(const logstreambuf&);
        
        typedef std::basic_streambuf<char>	BaseT;
        using BaseT::setp;
        using BaseT::pptr;
        using BaseT::sputc;
        using BaseT::egptr;
        using BaseT::eback;
        using BaseT::pbase;
        using BaseT::gptr;
        using BaseT::epptr;
        using BaseT::pbump;
    public:
        typedef BaseT::char_type				char_type;
        typedef BaseT::int_type					int_type;
        typedef BaseT::pos_type					pos_type;
        typedef BaseT::off_type					off_type;
        typedef std::char_traits<char_type>		traits;
        
        logstreambuf();
        explicit logstreambuf(log_writer * writer, std::streamsize out_buffer_len = 1024);
        logstreambuf(log_writer * writer, const std::string& timeformat, std::streamsize out_buffer_len = 1024);
        ~logstreambuf();
        
        /** initialize logstreambuf
         *  \param writer			log writer
         *  \param timeformat		timeformat to log time
         *  \param out_buffer_len	out buffer length
         *  \return					if success return true, otherwise return false
         */
        bool initialize(log_writer * writer, const std::string& timeformat, std::streamsize out_buffer_len = 1024);
        
        /// reserve buffer
        void reserve(std::streamsize n);
        
        /** log the head info of message.
         *  including time info and priority name
         *  \param priority		log priority
         *  \param separator	field separator
         *  \return				the number of characters written
         */
        int logmsg_head(int priority, char separator = ' ');
        
        /// write '\n' when the last written character is not '\n'
        void logmsg_end();
        
        int logmsg(const char_type * fmt, va_list ap, std::streamsize max_len);
    protected:
        virtual int_type overflow(int_type c);
        virtual int_type underflow();
        virtual int sync();
    };
    
    /** a stream wraper for write log
     *  it will check priority
     */
    class logstream
    {
        mutable Mutex *				_mutex;	///< mutex to unlock stream
        mutable std::ostream *		_stream;	///< stream write
    private:
        explicit logstream(int priority = 0, Mutex * mutex = NULL,
                           std::ostream * stream = NULL);
        
        logstream& operator=(const logstream& o);
        
        friend class logger;
        
        template < typename T >
        friend const logstream& operator<<(const logstream& f, const T& v);
    public:
        logstream(const logstream& o);
        ~logstream();
        
        /// return valid flag, only a valid stream will write log actually
        bool is_valid() const	{ return _stream != NULL; }
        
        const logstream& operator<<(std::ostream& (*pf)(std::ostream&)) const;
        const logstream& operator<<(std::basic_ios<char>& (*pf)(std::basic_ios<char>&)) const;
        const logstream& operator<<(std::ios_base& (*pf)(std::ios_base&)) const;
    };
    
    template < typename T >
    inline const logstream& operator<<(const logstream& f, const T& v)
    {
        if(f.is_valid())
            (*f._stream) << v;
        return f;
    }
    
    /** a simple stream wraper for write log
     */
    class logstream_simple
    {
        mutable Mutex *				_mutex;		///< mutex to unlock stream
        mutable std::ostream *		_stream;	///< stream write
    private:
        explicit logstream_simple(Mutex& mutex, std::ostream& stream);
        
        logstream_simple& operator=(const logstream_simple& o);
        
        friend class logger;
        
        template < typename T >
        friend const logstream_simple& operator<<(const logstream_simple& f, const T& v);
    public:
        logstream_simple(const logstream_simple& o);
        ~logstream_simple()	{ _mutex->Unlock(); }
        
        const logstream_simple& operator<<(std::ostream& (*pf)(std::ostream&)) const;
        const logstream_simple& operator<<(std::basic_ios<char>& (*pf)(std::basic_ios<char>&)) const;
        const logstream_simple& operator<<(std::ios_base& (*pf)(std::ios_base&)) const;
    };
    
    template < typename T >
    inline const logstream_simple& operator<<(const logstream_simple& f, const T& v)
    {
        (*f._stream) << v;
        return f;
    }
    
#define LOGGER_V_LOGMSG(x)			\
va_list ap;						\
va_start(ap, fmt);				\
vlogmsg(x, fmt, ap);			\
va_end(ap);
    
#define LOGGER_V_LOGMSG2(x, fd)		\
va_list ap;						\
va_start(ap, fmt);				\
vlogmsg(x, fd, fmt, ap);		\
va_end(ap);
    
    /** logger
     */
    class logger
    {
        logstreambuf	_buf;			///< log stream buffer
        std::ostream	_stream;		///< log stream
        Mutex			_mutex;		///< mutex
        int				_priority;		///< log level
        char			_separator;		///< separator
        
        logger(const logger&);
        logger& operator=(const logger&);
    public:
        logger(char separator = ' ');
        
        /** constructor
         *  \param priority		log priority
         *  \param writer		log writer
         *  \param separator	field separator
         */
        logger(int priority, log_writer * writer, char separator = ' ');
        
        /** constructor
         *  \param priority		log priority
         *  \param writer		log writer
         *  \param timeformat	timeformat, see man page of strftime
         *  \param separator	field separator
         */
        logger(int priority, log_writer * writer, const std::string& timeformat, char separator = ' ');
        
        /** initialize logger object
         *  \param priority		log priority
         *  \param writer		log writer
         *  \param timeformat	timeformat to log time
         *  \return				if initialize succ, return true, otherwise return false
         */
        bool initialize(int priority, log_writer * writer, const std::string& timeformat = "");
        
        enum ELogPriority
        {
            eLogDebug = 0,
            eLogInfo = 1,
            eLogWarning = 2,
            eLogError = 3,
            eLogFatalError = 4,
        };
        
        /// get log priority
        ELogPriority get_priority() const	{ return static_cast<ELogPriority>(_priority); }
        /// set log priority
        void set_priority(ELogPriority e)	{ _priority = e; }
        
        /// get separator
        char get_separator() const	{ return _separator; }
        /// set separator
        void set_separator(char separator = ' ')	{ _separator = separator; }
        
        /// get the name of priority
        const std::string& get_priority_name() const;
        
        /// log a format message
        void logmsg(int priority, const char * fmt, ...);
        
        /// log socket info and a format message
        void logmsg(int priority, int sockfd, const char * fmt, ...);
        
        /// same as logmsg, except that they are called with a va_list instead of a variable number of arguments
        void vlogmsg(int priority, const char * fmt, va_list ap);
        
        /// same as logmsg, except that they are called with a va_list instead of a variable number of arguments
        void vlogmsg(int priority, int sockfd, const char * fmt, va_list ap);
        
        /** get a logstream to log something
         *  it's thread safe, and it will log time info and priority info in the head of line automatically
         *  \param priority		log level, if this value less than value of this logger, it will not log anything
         *  \return				the temporarily logstream
         */
        logstream stream(int priority = eLogInfo);
        
        /// get a debug stream
        logstream debug()	{ return stream(eLogDebug); }
        /// get a info stream
        logstream info()	{ return stream(eLogInfo); }
        /// get a warning stream
        logstream warning()	{ return stream(eLogWarning); }
        /// get a error stream
        logstream error()	{ return stream(eLogError); }
        /// get a fatal error stream
        logstream fatal()	{ return stream(eLogFatalError); }
        
        /// @name log message function
        //@{
        void debug(const char * fmt, ...)	{ LOGGER_V_LOGMSG(eLogDebug); }
        void info(const char * fmt, ...)	{ LOGGER_V_LOGMSG(eLogInfo); }
        void warning(const char * fmt, ...)	{ LOGGER_V_LOGMSG(eLogWarning); }
        void error(const char * fmt, ...)	{ LOGGER_V_LOGMSG(eLogError); }
        void fatal(const char * fmt, ...)	{ LOGGER_V_LOGMSG(eLogFatalError); }
        
        void debug(int sockfd, const char * fmt, ...)	{ LOGGER_V_LOGMSG2(eLogDebug, sockfd); }
        void info(int sockfd, const char * fmt, ...)	{ LOGGER_V_LOGMSG2(eLogInfo, sockfd); }
        void warning(int sockfd, const char * fmt, ...)	{ LOGGER_V_LOGMSG2(eLogWarning, sockfd); }
        void error(int sockfd, const char * fmt, ...)	{ LOGGER_V_LOGMSG2(eLogError, sockfd); }
        void fatal(int sockfd, const char * fmt, ...)	{ LOGGER_V_LOGMSG2(eLogFatalError, sockfd); }
        //@}
        
        /** get an simple logstream
         *  this logstream will not log time info and priority info in the head of line, and will not check priority
         *  \return				the temporarily logstream
         */
        logstream_simple stream_simple();
        
        /// flush the logger
        void flush()	{ _stream.flush(); }
    };
    
#undef LOGGER_V_LOGMSG
#undef LOGGER_V_LOGMSG2
    
    //////////////////////////////////////////////////////////////////////////
    // implements
    
    ///////////////////////////////////////////////////////////////////////////
    // logstreambuf
    inline void logstreambuf::logmsg_end()
    {
        char_type * p = pptr();
        if(p <= _out || *(p-1) != '\n')
            sputc('\n');
        sync();
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // logstream
    inline logstream::logstream(int priority, Mutex * mutex,
                                std::ostream * stream)
    : _mutex(mutex)
    , _stream(stream)
    {
    }
    
    inline logstream::~logstream()
    {
        if(is_valid()){
            _mutex->Unlock();
        }
    }
    
    inline logstream::logstream(const logstream& o)
    {
        _mutex = o._mutex;
        _stream = o._stream;
        o._mutex = NULL;
        o._stream = NULL;
    }
    
    inline logstream& logstream::operator=(const logstream& o)
    {
        if(this != &o){
            _mutex = o._mutex;
            _stream = o._stream;
            o._mutex = NULL;
            o._stream = NULL;
        }
        return *this;
    }
    
    inline const logstream& logstream::operator<<(std::ostream& (*pf)(std::ostream&)) const
    {
        if(is_valid())
            (*_stream) << *pf;
        return *this;
    }
    
    inline const logstream& logstream::operator<<(std::basic_ios<char>& (*pf)(std::basic_ios<char>&)) const
    {
        if(is_valid())
            (*_stream) << pf;
        return *this;
    }
    
    inline const logstream& logstream::operator<<(std::ios_base& (*pf)(std::ios_base&)) const
    {
        if(is_valid())
            (*_stream) << pf;
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // logstream_simple
    inline logstream_simple::logstream_simple(Mutex& mutex, std::ostream& stream)
    : _mutex(&mutex)
    , _stream(&stream)
    {
        _mutex->Lock();
    }
    
    inline const logstream_simple& logstream_simple::operator<<(std::ostream& (*pf)(std::ostream&)) const
    {
        (*_stream) << pf;
        return *this;
    }
    
    inline const logstream_simple& logstream_simple::operator<<(std::basic_ios<char>& (*pf)(std::basic_ios<char>&)) const
    {
        (*_stream) << pf;
        return *this;
    }
    inline const logstream_simple& logstream_simple::operator<<(std::ios_base& (*pf)(std::ios_base&)) const
    {
        (*_stream) << pf;
        return *this;
    }
    
    inline logstream_simple::logstream_simple(const logstream_simple& o)
    {
        _mutex = o._mutex;
        _stream = o._stream;
        o._mutex = NULL;
        o._stream = NULL;
    }
    
    inline logstream_simple& logstream_simple::operator=(const logstream_simple& o)
    {
        if(this != &o){
            _mutex = o._mutex;
            _stream = o._stream;
            o._mutex = NULL;
            o._stream = NULL;
        }
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // logger
    inline logstream logger::stream(int priority)
    {
        std::ostream * ost;
        if(priority < _priority){
            ost = NULL;
        }else{
            _mutex.Lock();
            _buf.logmsg_head(priority, _separator);
            ost = &_stream;
        }
        return logstream(priority, &_mutex, ost);
    }
    
    inline logstream_simple logger::stream_simple()
    {
        return logstream_simple(_mutex, _stream);
    }
}


#endif /* logger_h */
