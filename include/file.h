//
//  file.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef file_h
#define file_h

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

namespace shadow
{
    
    class file
    {
        std::string	m_szFile;
        std::string m_szBasePath;
        int			m_fd;
        typedef file this_type;
    public:
        file()
        : m_fd(0)
        {}
        
        explicit file(const std::string& filename, int flag)
        : m_fd(0)
        {
            Open(filename, flag);
        }
        
        ~file() { Close(); }
        
        enum seekdir {beg = 0, cur = 1, end = 2};
        
        const std::string& GetBasePath() const
        {
            return m_szBasePath;
        }
        
        const std::string& GetFileName() const
        {
            return m_szFile;
        }
        
        std::string GetFullPath() const
        {
            return m_szBasePath + '\\' + m_szFile;
        }
        
        bool Open(const std::string& filename, int flag);
        
        bool Close()
        {
            if(m_fd < 0)
                return false;
            return 0 != ::close(m_fd);
        }
        
        off_t Tell()
        {
            return Seek(0L, cur);
        }
        
        off_t Seek(off_t offset, seekdir origin = beg)
        {
            return m_fd > 0 ? ::lseek(m_fd, offset, origin) : -1;
        }
        
        void SeekToBegin()
        {
            Seek(0L, beg);
        }
        
        void SeekToEnd()
        {
            Seek(0L, end);
        }
        
        bool Flush()
        {
            return m_fd > 0 && ::fsync(m_fd) == 0;
        }
        
        size_t Read(void *buffer, size_t size)
        {
            return m_fd > 0 ? ::read(m_fd, buffer, size) : 0;
        }
        
        size_t Write(const void *buffer, size_t size, size_t count)
        {
            return m_fd > 0 ? write(m_fd, (void*)buffer, size) : 0;
        }
        
        long GetLength() const
        {
            struct stat s;
            if(0 != fstat(m_fd, &s))
                return 0;
            return s.st_size;
        }
        
        bool operator!() const
        {
            return m_fd > 0;
        }
        
        // “˛ Ω◊™ªªµΩ bool
        typedef long (this_type::*unspecified_bool_type)() const;
        operator unspecified_bool_type() const
        {
            return m_fd == 0 ? 0 : &this_type::GetLength;
        }
        
        static bool Rename(const std::string& szOld, const std::string& szNew)
        {
            return Rename(szOld.c_str(), szNew.c_str());
        }
        
        static bool Rename(const char * szOld, const char * szNew)
        {
            return 0 == rename(szOld, szNew);
        }
        
        static bool RemoveFile(const std::string& szFile)
        {
            return RemoveFile(szFile.c_str());
        }
        
        static bool RemoveFile(const char * szFile)
        {
            return 0 == remove(szFile);
        }
        
        static bool CopyFile(const char* szExistFile, const char* szNewFile);
        
        static bool CopyFile(const std::string& szExistFile, const std::string& szNewFile)
        {
            return CopyFile(szExistFile.c_str(), szNewFile.c_str());
        }
    };
    
}



#endif /* file_h */
