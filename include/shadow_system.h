//
//  shadow_system.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/16.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_system_h
#define shadow_system_h



#include "shadow_comm.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>

namespace shadow {
    

    
    void init_rand();
    

    
    struct fifo_fail: public std::runtime_error{ fifo_fail(const std::string& s):std::runtime_error(s){}};
    struct fifo_delay: public std::runtime_error{ fifo_delay(const std::string& s):std::runtime_error(s){}};
    class CFifo {
    public:
        CFifo();
        ~CFifo();
        
    public:

        
        void open(const char *pathname,bool rorw, mode_t mode=0777) throw(fifo_fail);

        
        void close();
        int fd() { return _fd;}

        
        int read(char * buffer,size_t max_size) throw(fifo_fail,fifo_delay);

        
        int write(const char *buffer,size_t buflen) throw(fifo_fail,fifo_delay);
        
    private:
        std::string _pathname;
        int _fd;
        bool _rorw;
    };

    
    struct file_locked: public std::runtime_error{ file_locked(const std::string& s):std::runtime_error(s){}};
    class CNBFileLock
    {
    public:
        CNBFileLock() {_fd = -1;}
        ~CNBFileLock(){unLock();}
        
    public:

        void init(const std::string& filename) throw (std::runtime_error)
        {
            if (filename.empty()) throw std::runtime_error("wbl::CNBFileLock init: filename.empty");
                _fd = open(filename.c_str(), O_RDWR|O_CREAT, 0660);
                if (_fd<0) throw std::runtime_error(std::string("wbl::CNBFileLock init: open file failed:")+filename);
                    }
        

        void lock() throw (std::runtime_error,file_locked)
        {
            if (flock(_fd, LOCK_EX | LOCK_NB)) {
                if (errno == EWOULDBLOCK) throw file_locked("wbl::CNBFileLock lock file is locked by other");
                    throw std::runtime_error(std::string("wbl::CNBFileLock lock locking failed")+strerror(errno));
                    }
        }

        void unLock() {if (_fd>0) flock(_fd, LOCK_UN);}
    private:
        int _fd;
    };
    
}



#endif /* shadow_system_h */
