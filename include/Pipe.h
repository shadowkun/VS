//
//  Pipe.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef Pipe_h
#define Pipe_h



#if defined(_WIN32)
#	include <io.h>
#	include <winsock2.h>
#else
#	include <fcntl.h>
#	include <unistd.h>
#	include <sys/select.h>
#endif

namespace shadow
{
    /** pipe wrapper
     */
    class Pipe
    {
        int m_handles[2];
        
        Pipe(const Pipe&);
        Pipe& operator=(const Pipe&);
    public:
        Pipe()
        {
            open();
        }
        
        ~Pipe()
        {
            close();
        }
    public:
        /// open a pipe
        bool open()
        {
#if defined(_WIN32)
            if(0 != ::_pipe(&m_handles[0], 10240, _O_TEXT))
#else
                if(0 != ::pipe(&m_handles[0]))
#endif
                {
                    m_handles[0] = -1;
                    m_handles[1] = -1;
                    return false;
                }
            return true;
        }
        
        /// get read handle
        int read_handle() const		{ return m_handles[0]; }
        /// get write handle
        int write_handle() const	{ return m_handles[1]; }
        
        /// read from pipe
        size_t read(void *buf, size_t count)
        {
            return ::read(read_handle(), buf, count);
        }
        
        /// write to pipe
        size_t write(const void *buf, size_t count)
        {
            return ::write(write_handle(), buf, count);
        }
        
        /** wait for read available
         *  \param timeout	wait timeout
         *  \return			if pipe read available return true; if timeout return false
         */
        bool wait_for_read(timeval * timeout) const
        {
            fd_set rfds;
            FD_SET(read_handle(), &rfds);
            int r = ::select(read_handle() + 1, &rfds, NULL, NULL, timeout);
            return r > 0;
        }
        
        /** wait for read available
         *  \return			always return true
         */
        bool wait_for_read() const
        {
            return wait_for_read(NULL);
        }
        
        /** wait for write available
         *  \param timeout	wait timeout
         *  \return			if pipe write available return true; if timeout return false
         */
        bool wait_for_write(timeval * timeout) const
        {
            fd_set wfds;
            FD_SET(write_handle(), &wfds);
            int r = ::select(write_handle() + 1, NULL, &wfds, NULL, timeout);
            return r > 0;
        }
        
        /** wait for write available
         *  \return			always return true
         */
        bool wait_for_write() const
        {
            return wait_for_write(NULL);
        }
        
        /// close read handle
        void close_read()
        {
            if(-1 != read_handle())
            {
                ::close(m_handles[0]);
                m_handles[0] = -1;
            }
        }
        
        /// close write handle
        void close_write()
        {
            if(-1 != write_handle())
            {
                ::close(m_handles[1]);
                m_handles[1] = -1;
            }
        }
        
        /// close pipe
        void close()
        {
            close_write();
            close_read();
        }
        
#if !defined(_WIN32)
        /// set nonblocking
        bool set_nonblocking()
        {
            return set_nonblocking(read_handle()) 
            && set_nonblocking(write_handle());
        }
        
        /// set nonblocking
        static bool set_nonblocking(int sock)
        {
            int opts = fcntl(sock, F_GETFL);
            if(opts < 0)
                return false;
            opts = opts|O_NONBLOCK;
            return fcntl(sock, F_SETFL, opts) != -1;
        }
#endif
    };
    
}


#endif /* Pipe_h */
