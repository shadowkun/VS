//
//  log_client.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef log_client_h
#define log_client_h


#include <vector>
#include <string>
#include <arpa/inet.h>


#include "log_writer.h"
#include "logger.h"
#include "stream_util.h"

namespace shadow
{
    class log_server_writer_info : public async_writer_info
    {
        typedef std::vector<std::string>		LogQueueT;
        std::string			_name;		///< logger name
        Mutex				_mutex;			///< mutex
        LogQueueT			_vLogs[2];	///< log buffers
        volatile int		_nCurLog;	///< current log buffer's index
        volatile bool		_in_use;	///< in use?
        int					_fd;		///< socket fd to log server
        sockaddr_in 		_addr;		///< address of log server
        logger *			_logger;		///< logger
        bool				_own_logger;	///< own logger?
    public:
        /** constructor
         *  \param log_name		logger name
         *  \param addr			address of log server
         *  \param port			port of log server
         *  \param fail_logger		logger when write to log server fail
         *  \param own_logger	own the logger, if true, log_server_writer_info will free it when destory
         */
        log_server_writer_info(std::string log_name, const char * addr, int port, logger * fail_logger, bool own_logger = true)
        : _name(log_name)
        , _nCurLog(0)
        , _in_use(true)
        , _fd(-1)
        , _logger(fail_logger)
        , _own_logger(own_logger)
        {
            memset(&_addr, 0, sizeof(_addr));
            _addr.sin_family = AF_INET;
            _addr.sin_addr.s_addr = inet_addr(addr);
            _addr.sin_port = htons(port);
        }
        
        ~log_server_writer_info()
        {
            close_fd();
            if(_own_logger)
                delete _logger;
        }
        
        void cut() { }
        void on_destory_writer() { _in_use = false; }
        bool in_use() const { return _in_use; }
        
        void write_log()
        {
            _mutex.Lock();
            _nCurLog = !_nCurLog;
            _mutex.Unlock();
            
            LogQueueT& v = _vLogs[!_nCurLog];
            if(v.empty())
                return;
            if(_fd == -1)
                connect();
            const size_t N = 10;
            std::vector<iovec> vec;
            std::vector<char> head;
            for(size_t i = 0; i < v.size(); i += N){
                size_t n = v.size() - i;
                n = n > N ? N : n;
                vec.resize(n * 2 + 2);
                
                pack(head, n);
                
                uint32_t packet_length = n * sizeof(uint32_t);
                std::vector<uint32_t> log_length(n);
                for(size_t j = 0; j < n; ++j){
                    packet_length += v[i + j].size();
                    log_length[j] = htonl(v[i + j].size());
                    vec[j * 2 + 2].iov_base = (void *) &log_length[j];
                    vec[j * 2 + 2].iov_len = sizeof(uint32_t);
                    vec[j * 2 + 1 + 2].iov_base = (void *) v[i + j].data();
                    vec[j * 2 + 1 + 2].iov_len = v[i + j].size();
                }
                packet_length += sizeof(packet_length) + head.size();
                packet_length = htonl(packet_length);
                vec[0].iov_base = &packet_length;
                vec[0].iov_len = sizeof(packet_length);
                vec[1].iov_base = &head[0];
                vec[1].iov_len = head.size();
                
                int r = writev(_fd, &vec[0], vec.size());
                if(r == -1){
                    connect();
                    r = writev(_fd, &vec[0], vec.size());
                }
                if(r == -1 && _logger != NULL)
                {
                    write_log_to_local(*_logger, v.begin() + i, v.begin() + i + n);
                    //write_log_to_local(*_logger, &v[0], v.size());
                }
                vec.clear();
                head.clear();
            }
            
            v.clear();
            /*
             std::vector<char> head;
             pack(head, nLogsCount);
             uint32_t len = htonl(sizeof(len) + head.size() + v.size());
             iovec vec[3];
             vec[0].iov_base = &len;
             vec[0].iov_len = sizeof(len);
             vec[1].iov_base = &head[0];
             vec[1].iov_len = head.size();
             vec[2].iov_base = &v[0];
             vec[2].iov_len = v.size();
             
             int n = writev(_fd, vec, sizeof(vec) / sizeof(vec[0]));
             if(n == -1){
             connect();
             n = writev(_fd, vec, sizeof(vec) / sizeof(vec[0]));
             }
             if(n == -1 && _logger != NULL)
             {
             write_log_to_local(*_logger, &v[0], v.size());
             }
             _nLogsCount[!_nCurLog] = 0;
             v.clear();
             */
        }
        
        int write(const void * data, size_t len)
        {
            MutexLocker lock(_mutex);
            LogQueueT& v = _vLogs[_nCurLog];
            const char * p = static_cast<const char *>(data);
            if(len > 0 && p[len - 1] == '\n')
                --len;
            v.push_back(std::string(p, p + len));
            return len;
        }
    private:
        template < typename IT >
        static void write_log_to_local(logger& log, IT first, IT last)
        {
            for( ; first != last; ++first)
                log.stream_simple() << *first << std::endl;
        }
        /*
         static void write_log_to_local(logger& log, const char * buf, size_t len)
         {
         const char * end = buf + len;
         for(; buf < end; ){
         uint32_t n = read_int(buf);
         log.stream_simple() << buffer_info(buf, n) << std::endl;
         buf += n;
         }
         }
         */
        
        static uint32_t read_int(const char *& buf)
        {
            const uint32_t * p = (const uint32_t *) buf;
            buf += sizeof(uint32_t);
            return ntohl(*p);
        }
        
        static void append_int(std::vector<char>& r, int n)
        {
            uint32_t num = htonl(n);
            const char * p = (const char *)(&num);
            r.insert(r.end(), p, p + sizeof(num));
        }
        
        static void append_short(std::vector<char>& r, short n)
        {
            uint16_t num = htons(n);
            const char * p = (const char *)(&num);
            r.insert(r.end(), p, p + sizeof(num));
        }
        
        void pack(std::vector<char>& r, int log_count)
        {
            append_short(r, 100); // version
            append_short(r, 2);	// command
            append_int(r, time(NULL));	// time
            append_short(r, _name.size());
            r.insert(r.end(), _name.begin(), _name.end());
            append_short(r, log_count);
        }
        
        void close_fd()
        {
            ::close(_fd);
            _fd = -1;
        }
        
        bool connect()
        {
            if(_fd > 0)
                close_fd();
            _fd = ::socket(AF_INET, SOCK_STREAM, 0);
            if(_fd == -1)
                return false;
            int ret = ::connect(_fd, (sockaddr *)&_addr, sizeof(_addr));
            return ret == 0;
        }
    };
}


#endif /* log_client_h */
