//
//  fdstream.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/3.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef fdstream_h
#define fdstream_h

#include <streambuf>
#include <unistd.h>

namespace shadow
{
    /** a streambuf wrap with a file descriptors
     */
    template<class charT, class traits = std::char_traits<charT> >
    class basic_fdstreambuf: public std::basic_streambuf<charT, traits>
    {
        int _fd;					///< file descriptors
        charT *_out;				///< out buffer
        std::streamsize _out_len;	///< out buffer length
        charT *_in;					///< in buffer
        std::streamsize _in_len;	///< in buffer length
        bool _auto_close;			///< auto close file descriptors on destructor
        
        basic_fdstreambuf(const basic_fdstreambuf&);
        basic_fdstreambuf& operator=(const basic_fdstreambuf&);
        
        typedef std::basic_streambuf<charT, traits>	BaseT;
        using BaseT::setg;
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
        typedef typename BaseT::char_type	char_type;
        typedef typename BaseT::int_type	int_type;
        typedef typename BaseT::pos_type	pos_type;
        typedef typename BaseT::off_type	off_type;
        typedef traits						traits_type;
        
        explicit basic_fdstreambuf(int fd, bool auto_close = true, int out_buffer_len = 512, int in_buffer_len = 512);
        
        ~basic_fdstreambuf();
        
        /** reset the file descriptors
         * \param fd	new file descriptors
         */
        void reset(int fd);
        
        /// set auto close flag
        void set_auto_close(bool auto_close = true)
        {
            _auto_close = auto_close;
        }
        
        /// close the streambuf
        void close();
        
        /** test streambuf is read available
         * \return	if a something read available in buffer or fd, or fd closed, return true, otherwise return false
         */
        bool read_available();
    private:
        void init_out_buf();
        void free_out_buf();
        void init_in_buf();
        void free_in_buf();
        
        int read(charT * buf, int len);
        int write(const charT * buf, int len);
    protected:
        virtual int_type overflow(int_type c);
        virtual int_type underflow();
        virtual int sync();
    };
    
    template<class charT, class traits = std::char_traits<charT> >
    class basic_ofdstream: public std::basic_ostream<charT, traits>
    {
        typedef std::basic_ostream<charT, traits>	BaseT;
        basic_fdstreambuf<charT, traits>	m_streambuf;
    public:
        explicit basic_ofdstream(int fd, bool auto_close = true, int buf_len = 512)
        : BaseT(&m_streambuf)
        , m_streambuf(fd, auto_close, buf_len, 0)
        {}
        
        /// close the streambuf
        void close()	{ m_streambuf.close(); }
        
        /** reset the file descriptors
         * \param fd	new file descriptors
         */
        void reset(int fd)	{ m_streambuf.reset(fd); }
        
        /// set auto close flag
        void set_auto_close(bool auto_close = true)	{ m_streambuf.set_auto_close(auto_close); }
    };
    
    template<class charT, class traits = std::char_traits<charT> >
    class basic_ifdstream: public std::basic_istream<charT, traits>
    {
        typedef std::basic_istream<charT, traits>	BaseT;
        basic_fdstreambuf<charT, traits>	m_streambuf;
    public:
        explicit basic_ifdstream(int fd, bool auto_close = true, int buf_len = 512)
        : BaseT(&m_streambuf)
        , m_streambuf(fd, auto_close, 0, buf_len)
        {}
        
        /** test streambuf is read available
         * \return	if a something read available in buffer or fd, or fd closed, return true, otherwise return false
         */
        bool read_available()	{ return m_streambuf.read_available(); }
        
        /// close the streambuf
        void close()	{ m_streambuf.close(); }
        
        /** reset the file descriptors
         * \param fd	new file descriptors
         */
        void reset(int fd)	{ m_streambuf.reset(fd); }
        
        /// set auto close flag
        void set_auto_close(bool auto_close = true)	{ m_streambuf.set_auto_close(auto_close); }
    };
    
    template<class charT, class traits = std::char_traits<charT> >
    class basic_iofdstream: public std::basic_iostream<charT, traits>
    {
        typedef std::basic_iostream<charT, traits>	BaseT;
        basic_fdstreambuf<charT, traits>	m_streambuf;
    public:
        explicit basic_iofdstream(int fd, bool auto_close = true, int out_buffer_len = 512, int in_buffer_len = 512)
        : BaseT(&m_streambuf)
        , m_streambuf(fd, auto_close, out_buffer_len, in_buffer_len)
        {}
        
        /** test streambuf is read available
         * \return	if a something read available in buffer or fd, or fd closed, return true, otherwise return false
         */
        bool read_available()	{ return m_streambuf.read_available(); }
        
        /// close the streambuf
        void close()	{ m_streambuf.close(); }
        
        /** reset the file descriptors
         * \param fd	new file descriptors
         */
        void reset(int fd)	{ m_streambuf.reset(fd); }
        
        /// set auto close flag
        void set_auto_close(bool auto_close = true)	{ m_streambuf.set_auto_close(auto_close); }
    };
    
    typedef basic_fdstreambuf<char>	fdstreambuf;
    typedef basic_ofdstream<char>	ofdstream;
    typedef basic_ifdstream<char>	ifdstream;
    typedef basic_iofdstream<char>	iofdstream;
    
    typedef basic_fdstreambuf<wchar_t>	wfdstreambuf;
    typedef basic_ofdstream<wchar_t>	wofdstream;
    typedef basic_ifdstream<wchar_t>	wifdstream;
    typedef basic_iofdstream<wchar_t>	wiofdstream;
    
    ///////////////////////////////////////////////////////////////////
    // implemention
    namespace fdstream_detail
    {
        int read(int fd, void * buf, int buf_len);
        int write(int fd, const void * buf, int len);
        bool read_available(int fd);
    }
    
    template<class charT, class traits>
    inline basic_fdstreambuf<charT, traits>::basic_fdstreambuf(int fd, bool auto_close, int out_buffer_len, int in_buffer_len)
    : _fd(fd)
    , _out(0)
    , _out_len(out_buffer_len)
    , _in(0)
    , _in_len(in_buffer_len)
    , _auto_close(auto_close)
    {
        setg(_in, _in, _in);
        setp(_out, _out);
    }
    
    template<class charT, class traits>
    inline basic_fdstreambuf<charT, traits>::~basic_fdstreambuf()
    {
        sync();
        if(_auto_close && _fd > 0)
            ::close(_fd);
        free_out_buf();
        free_in_buf();
    }
    
    template<class charT, class traits>
    inline void basic_fdstreambuf<charT, traits>::reset(int fd)
    {
        close();
        _fd = fd;
    }
    
    template<class charT, class traits>
    inline void basic_fdstreambuf<charT, traits>::close()
    {
        setp(_out,_out + _out_len);
        setg(_in, _in, _in);
        sync();
        ::close(_fd);
        _fd = -1;
    }
    
    template<class charT, class traits>
    inline bool basic_fdstreambuf<charT, traits>::read_available()
    {
        return egptr() - gptr() > 0
        || fdstream_detail::read_available(_fd);
    }
    
    template<class charT, class traits>
    inline int basic_fdstreambuf<charT, traits>::read(charT * buf, int len)
    {
        return fdstream_detail::read(_fd, buf, len * sizeof(charT));
    }
    
    template<class charT, class traits>
    inline int basic_fdstreambuf<charT, traits>::write(const charT * buf, int len)
    {
        return fdstream_detail::write(_fd, buf, len * sizeof(charT));
    }
    
    template<class charT, class traits>
    void basic_fdstreambuf<charT, traits>::init_out_buf()
    {
        _out = new charT[_out_len];
        setp(_out,_out + _out_len);
    }
    
    template<class charT, class traits>
    void basic_fdstreambuf<charT, traits>::free_out_buf()
    {
        delete [] _out;
        _out = 0;
        setp(NULL, NULL);
    }
    
    template<class charT, class traits>
    void basic_fdstreambuf<charT, traits>::init_in_buf()
    {
        _in = new charT[_in_len];
        setg(_in, _in, _in);
    }
    
    template<class charT, class traits>
    void basic_fdstreambuf<charT, traits>::free_in_buf()
    {
        delete [] _in;
        _in = 0;
        setg(_in, _in, _in);
    }
    
    template<class charT, class traits>
    typename basic_fdstreambuf<charT, traits>::int_type basic_fdstreambuf<charT, traits>::overflow(typename basic_fdstreambuf<charT, traits>::int_type c)
    {
        sync();
        if(traits::eq_int_type(c,traits::eof()) )
            return traits::not_eof(c);
        else
        {
            if(pptr() == NULL)
                init_out_buf();
            return sputc(c);
        }
    }
    
    template<class charT, class traits>
    typename basic_fdstreambuf<charT, traits>::int_type basic_fdstreambuf<charT, traits>::underflow()
    {
        if(egptr() == 0)
            init_in_buf();
        if(egptr() == _in + _in_len)
            setg(_in, _in, _in);
        charT * const egptr1 = egptr();
        int bufLeft = _in + _in_len - egptr1;
        int n = read(egptr1, bufLeft);
        if(n <= 0)
            return traits::eof();
        setg(eback(), egptr1, egptr1 + n);
        return traits::to_int_type(*gptr());
    }
    
    template<class charT, class traits>
    int basic_fdstreambuf<charT, traits>::sync()
    {
        charT * const pptr1 = pptr();
        charT * const pbase1 = pbase();
        if(pptr1 > pbase1){
            int len = pptr1 - pbase1;
            int n = write(pbase1, len);
            if(n < 1)
                return -1;
            if(n < len)
            {
                memcpy(pbase1, pbase1 + n, len - n);
                setp(pbase1, epptr());
                pbump(n);
            }else
                setp(pbase1, epptr());  
        }
        return 0;
    }
    
}



#endif /* fdstream_h */
