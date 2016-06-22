//
//  log_writer.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef log_writer_h
#define log_writer_h


#include <stdint.h>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "file_util.h"
#include "pthread_util.h"

namespace shadow
{
    /** log writer
     */
    class log_writer
    {
    public:
        virtual ~log_writer(){}
        
        /** write data
         *    if you return a positive number, you should ensure all the data is
         *  written, otherwise some data will be ignore.
         *  \param data		data buffer to write
         *  \param len		length of data buffer
         *  \return			if fail return a negative, otherwise return a positive
         */
        virtual int write(const void * data, size_t len) = 0;
    };
    
    /** console writer
     *  write log to console
     */
    class console_writer : public log_writer
    {
        int _fd;	///< STDOUT_FILENO or STDERR_FILENO
    public:
        explicit console_writer(int fd = STDOUT_FILENO)
        : _fd(fd)
        {}
        
        int write(const void * data, size_t len)
        {
            return ::write(_fd, data, len);
        }
    };
    
    /** do not write anything
     */
    struct non_writer : public log_writer
    {
        int write(const void * data, size_t len)
        {
            return len;
        }
    };
    
    /** cut file never
     */
    struct cut_never
    {
        std::string get_new_name(const std::string& filename, size_t writing_len)
        {
            return "";
        }
    };
    
    /** cut file by file size
     */
    class cut_by_size
    {
        uint64_t	_cut_size;			///< cut size
        uint64_t	_written_bytes;	///< current file size
        int			_id;				///< current id
    public:
        explicit cut_by_size(size_t cut_size = 100 * 1024 * 1024, int id = -1);
        std::string get_new_name(const std::string& filename, size_t writing_len);
    };
    
    /** cut file by day
     */
    class cut_by_day
    {
        size_t	_days;				///< days
        time_t	_next_cut_time;	///< time of next cut file
    public:
        explicit cut_by_day(size_t days = 1);
        std::string get_new_name(const std::string& filename, size_t writing_len);
    };
    
    /** cut file by time
     */
    class cut_by_time
    {
        size_t	_minute;				///< time in minute
        time_t	_next_cut_time;	///< time of next cut file
    public:
        explicit cut_by_time(size_t minute = 60);
        std::string get_new_name(const std::string& filename, size_t writing_len);
    };
    
    /** roll file by file size
     */
    class roll_by_size
    {
        uint64_t	_roll_size;			///< roll size
        uint64_t	_written_bytes;	///< current file size
        int			_count;				///< count
        int			_id;				///< current id
    public:
        explicit roll_by_size(size_t roll_size = 100 * 1024 * 1024, int count = 10, int id = -1);
        std::string get_new_name(const std::string& filename, size_t writing_len);
    };
    
    /** roll file by time
     */
    class roll_by_time
    {
        size_t	_minute;		///< time in minute
        time_t	_next_cut_time;	///< time of next roll
        int		_count;			///< count of file
        int		_id;			///< current id
    public:
        explicit roll_by_time(size_t minute = 60, int count = 10, int id = -1);
        std::string get_new_name(const std::string& filename, size_t writing_len);
    };
    
    /** file writer
     */
    template < typename FileCutterT >
    class file_writer : public log_writer
    {
        std::string	_filename;
        int			_fd;
        FileCutterT	_fc;
        
        file_writer(const file_writer&);
        file_writer& operator=(const file_writer&);
    public:
        file_writer(const std::string& filename, const FileCutterT& fc);
        ~file_writer() { ::close(_fd); }
        int write(const void * data, size_t len);
    };
    
    template < typename FileCutterT >
    inline log_writer * make_file_writer(const std::string& filename, const FileCutterT& fc)
    {
        return new file_writer<FileCutterT>(filename, fc);
    }
    
    /** command pipe writer
     */
    template < typename FileCutterT >
    class cmd_pipe_writer : public log_writer
    {
        std::string _cmd;		///< command
        std::string	_filename;	///< log filename
        int			_fd;		///< file descriptors
        FileCutterT	_fc;		///< file cutter
        
        cmd_pipe_writer(const cmd_pipe_writer&);
        cmd_pipe_writer& operator=(const cmd_pipe_writer&);
    public:
        cmd_pipe_writer(const std::string& cmd, const std::string& filename, const FileCutterT& fc)
        : _cmd(cmd)
        , _filename(filename)
        , _fd(-1)
        , _fc(fc)
        {
            open();
        }
        
        ~cmd_pipe_writer()
        {
            ::close(_fd);
        }
        
        int write(const void * data, size_t len);
    private:
        bool open();
    };
    
    template < typename FileCutterT >
    inline log_writer * make_cmd_pipe_writer(const std::string& cmd, const std::string& filename, const FileCutterT& fc)
    {
        return new cmd_pipe_writer<FileCutterT>(cmd, filename, fc);
    }
    
    struct async_writer_info
    {
        virtual ~async_writer_info(){}
        virtual int write(const void * data, size_t len) = 0;
        virtual void cut() = 0;
        virtual void on_destory_writer() = 0;
        virtual void write_log() = 0;
        virtual bool in_use() const = 0;
    };
    
    /** thread to write log asynchronous
     */
    class async_writer_thread
    {
    public:
        //class async_writer_info;
        typedef std::list<async_writer_info*>	InfoQueueT;
    private:
        InfoQueueT			_infos;			///< all writer info
        Mutex				_mutex;			///< mutex
        pthread_t			_tid;			///< thread id
        volatile bool		_stop_thread;	///< thread stop flag
        unsigned long		_sleep_between_write;	///< sleep between write
        int					_pipe[2];		///< pipe
    public:
        explicit async_writer_thread(unsigned long sleep_between_write = 1000000);
        ~async_writer_thread();
        
        /// set sleep between write
        void set_sleep_between_write(unsigned long sleep_between_write = 1000000)
        {
            _sleep_between_write = sleep_between_write;
        }
        
        /** get an asynchronous writer
         *  \param writer	the actual writer
         *  \return			the asynchronous writer associate with input writer
         */
        log_writer * async(log_writer * writer);
        
        log_writer * async(async_writer_info * info);
        
        /** get an asynchronous file writer
         *  \param filename	filename of log
         *  \param fc		the file cutter
         *  \return			the asynchronous writer associate with the file
         */
        template < typename FileCutterT >
        log_writer * async_file(const std::string& filename, const FileCutterT& fc);
        /*
         {
         return async(make_file_writer(filename, fc));
         }
         */
        
        /** start the write log thread
         *  \return if success, return true, otherwise return false
         */
        bool start();
    private:
        static void * write_proc(void * para);
        void do_write_log();
    };
    
    /** asynchronous writer
     */
    class async_writer : public log_writer
    {
        //typedef async_writer_thread::async_writer_info	async_writer_info;
        async_writer_info *		_info;		///< writer info
        
        friend class shadow::async_writer_thread;
        
        explicit async_writer(async_writer_info * info)
        : _info(info)
        {}
    public:
        ~async_writer();
        void cut();
        int write(const void * data, size_t len);
    };
    
    /** asynchronous writer with cutter
     */
    template < typename CutterT >
    class async_writer_with_cutter : public log_writer
    {
        async_writer *		_info;		///< writer info
        CutterT	_fc;
        
        friend class shadow::async_writer_thread;
        
        async_writer_with_cutter(async_writer * info,  CutterT fc)
        : _info(info)
        , _fc(fc)
        {}
    public:
        ~async_writer_with_cutter()
        {
            delete _info;
        }
        
        int write(const void * data, size_t len);
    };
    
    //////////////////////////////////////////////////////////////////////////////
    // implements
    
    namespace detail
    {
        void get_cut_filename_from_old(std::string& fn);
        int open_cmd_pipe(const std::string& cmd, const std::string& filename);
        int file_writer_open(const std::string& filename);
        int file_writer_write_all(int fd, const void * data, size_t len);
    }
    
    template < typename FileCutterT >
    inline log_writer * async_writer_thread::async_file(const std::string& filename, const FileCutterT& fc)
    {
        async_writer * p = (async_writer *) async(make_file_writer(filename, fc));
        return new async_writer_with_cutter<FileCutterT>(p, fc);
    }
    
    template < typename CutterT >
    inline int async_writer_with_cutter<CutterT>::write(const void * data, size_t len)
    {
        std::string fn = _fc.get_new_name("", len);
        if(fn.size() > 0){
            _info->cut();
        }
        return _info->write(data, len);
    }
    
    //////////////////////////////////////////////////////////////////////////////
    // file_writer
    template < typename FileCutterT >
    inline file_writer<FileCutterT>::file_writer(const std::string& filename, const FileCutterT& fc)
    : _filename(filename)
    , _fc(fc)
    {
        _fd = detail::file_writer_open(filename);
    }
    
    template < typename FileCutterT >
    int file_writer<FileCutterT>::write(const void * data, size_t len)
    {
        if(_fd <= 0)
            return -1;
        
        // cut file
        std::string fn = _fc.get_new_name(_filename, len);
        if(fn.size() > 0 && fn != _filename){
            detail::get_cut_filename_from_old(fn);
            ::close(_fd);
            ::rename(_filename.c_str(), fn.c_str());
            _fd = ::open(_filename.c_str(), O_WRONLY|O_APPEND|O_CREAT, 0666);
        }
        
        return ::write(_fd, data, len);
    }
    
    template < typename FileCutterT >
    int cmd_pipe_writer<FileCutterT>::write(const void * data, size_t len)
    {
        if(_fd <= 0)
            return -1;
        std::string fn = _fc.get_new_name(_filename, len);
        if(fn.size() > 0 && fn != _filename){
            detail::get_cut_filename_from_old(fn);
            ::close(_fd);
            ::rename(_filename.c_str(), fn.c_str());
            open();
        }
        return detail::file_writer_write_all(_fd, data, len);
    }
    
    template < typename FileCutterT >
    inline bool cmd_pipe_writer<FileCutterT>::open()
    {
        int fd = detail::open_cmd_pipe(_cmd, _filename);
        if(fd < 0)
            return false;
        _fd = fd;
        return true;
    }
    
}


#endif /* log_writer_h */
