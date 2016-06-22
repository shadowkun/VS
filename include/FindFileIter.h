//
//  FindFileIter.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef FindFileIter_h
#define FindFileIter_h

#include <iosfwd>
#include <iterator>
#include <string>
#include <limits>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


#include "string_utility.h"
#include "PathUtility.h"

namespace shadow
{
    class FindFileDataWrap
    {
        typedef std::string	StringT;
        typedef dirent	FindDataT;
        
        StringT m_basepath;
        FindDataT * m_dir;
        
        bool read_dir(DIR* dir)	{ return dir != NULL && (m_dir = ::readdir(dir)) != NULL; }
        friend class findfile_iterator;
    public:
        FindFileDataWrap() : m_dir(NULL) { }
        
        void SetBasePath(const StringT& basepath)
        {
            m_basepath = basepath;
            if(m_basepath.size() > 0 && m_basepath[m_basepath.size() - 1] == '/')
                m_basepath.erase(m_basepath.end() - 1);
        }
        
        dirent * GetFindDate() const	{ return m_dir; }
        
        bool IsDirectory() const
        {
            struct stat s;
            if(stat(GetFilePath().c_str(), &s) != 0)
                return 0;
            //return S_ISDIR(s.st_mode);
            return (((s.st_mode)&S_IFDIR)==S_IFDIR);
        }
        
        bool IsHidden() const
        {
            return m_dir->d_name[0] == '.' && m_dir->d_name[1] != '\0';
        }
        
        bool IsNormal() const
        {
            struct stat s;
            if(stat(GetFilePath().c_str(), &s) != 0)
                return 0;
            //return S_ISREG(s.st_mode);
            return (((s.st_mode)&S_IFREG)==S_IFREG);
        }
        
        bool IsLink() const
        {
            struct stat s;
            if(stat(GetFilePath().c_str(), &s) != 0)
                return 0;
            //return S_ISLINK(s.st_mode);
            return (((s.st_mode)&S_IFREG)==S_IFLNK);
        }
        
        bool IsTemporary() const
        {
            return StartsWith(m_basepath, "/tmp/") == 0;
        }
        
        bool IsDots() const
        {
            return strcmp(m_dir->d_name, ".") == 0
            || strcmp(m_dir->d_name, "..") == 0;
        }
        
        tm GetLastAccessTime() const
        {
            struct stat s;
            stat(GetFilePath().c_str(), &s);
            return *::gmtime(&s.st_atime);
        }
        
        tm GetLastWriteTime() const
        {
            struct stat s;
            stat(GetFilePath().c_str(), &s);
            return *::gmtime(&s.st_mtime);
        }
        
        off_t GetLength() const
        {
            struct stat s;
            stat(GetFilePath().c_str(), &s);
            return s.st_size;
        }
        
        StringT GetFilePath() const
        {
            return m_basepath + "/" + GetFileName();
        }
        
        StringT GetFileName() const
        {
            return m_dir->d_name;
        }
        
        const StringT& GetBasePath() const
        {
            return m_basepath;
        }
        
        StringT GetFileExt() const
        {
            const StringT& szFile = GetFileName();
            StringT::size_type pos = szFile.rfind('.');
            if(pos != StringT::npos)
                return szFile.substr(pos + 1);
            return StringT();
        }
    };
    
    inline std::ostream& operator<<(std::ostream& ost, const FindFileDataWrap& file)
    {
        ost << file.GetFileName();
        return ost;
    }
    

    class findfile_iterator
    : public std::iterator<std::input_iterator_tag, FindFileDataWrap >
    {
        DIR *			m_hFile;
        value_type *	m_pFindFileData;
        bool			m_bIgnoreDogs;
        size_t *		m_pCounter;
        
        typedef std::string	StringT;
        
        void add_counter()
        {
            if(m_pCounter)
                ++*m_pCounter;
        }
        
        void remove_counter()
        {
            if(m_pCounter && --*m_pCounter == 0){
                ::closedir(m_hFile);
                delete m_pFindFileData;
                delete m_pCounter;
            }
            m_hFile = 0;
            m_pFindFileData = 0;
            m_pCounter = 0;
        }
    public:
        explicit findfile_iterator(const StringT& filename, bool bIgnoreDogs = true)
        : m_hFile(0)
        , m_pFindFileData(new value_type)
        , m_bIgnoreDogs(bIgnoreDogs)
        , m_pCounter(new size_t(1))
        {
            find_file(filename);
        }
        
        findfile_iterator()
        : m_hFile(0)
        , m_pFindFileData(0)
        , m_bIgnoreDogs(false)
        , m_pCounter(0)
        {
        }
        
        findfile_iterator(const findfile_iterator& obj)
        : m_hFile(obj.m_hFile)
        , m_pFindFileData(obj.m_pFindFileData)
        , m_bIgnoreDogs(obj.m_bIgnoreDogs)
        , m_pCounter(obj.m_pCounter)
        {
            add_counter();
        }
        
        findfile_iterator& operator=(const findfile_iterator& obj)
        {
            remove_counter();
            m_hFile = obj.m_hFile;
            m_bIgnoreDogs = obj.m_bIgnoreDogs;
            m_pFindFileData = obj.m_pFindFileData;
            m_pCounter = obj.m_pCounter;
            add_counter();
            return *this;
        }
        
        ~findfile_iterator()
        {
            remove_counter();
        }
        
        findfile_iterator& operator++()
        {
            do {
                if (m_hFile != 0 && !m_pFindFileData->read_dir(m_hFile)){
                    close();
                    break;
                }
            } while(m_bIgnoreDogs && m_pFindFileData->IsDots());
            
            return *this;
        }
        
        bool operator==(const findfile_iterator& it) const
        {
            return m_hFile == it.m_hFile;
        }
        
        bool operator!=(const findfile_iterator& it) const
        {
            return !(*this == it);
        }
        
        value_type& operator*()
        {
            return *m_pFindFileData;
        }
        
        value_type* operator->()
        {
            return m_pFindFileData;
        }
        
        void close()
        {
            remove_counter();
        }
        
        void find_file(const StringT& filename)
        {
            remove_counter();
            m_pCounter = new size_t(1);
            m_pFindFileData = new value_type;
            
            m_hFile = ::opendir(filename.c_str());
            do {
                if (!m_pFindFileData->read_dir(m_hFile)){
                    close();
                    return;
                }
            } while(m_hFile != 0 && m_bIgnoreDogs && m_pFindFileData->IsDots());
            
            m_pFindFileData->SetBasePath(GetFullPath(filename));
        }
    };
    

    template < typename Fn >
    Fn for_each_in_folder(const std::string& szFolder, 
                          Fn fn,
                          bool bIncludeSubFloder = true,
                          bool bVisitFolder = false)
    {
        typedef findfile_iterator	IteratorT;
        IteratorT ib(szFolder), ie;
        for(; ib != ie; ++ib){
            if(ib->IsDirectory()){
                if(bVisitFolder)
                    fn(*ib);
                if(bIncludeSubFloder){

                    fn = for_each_in_folder(ib->GetFilePath(),
                                            fn,
                                            bIncludeSubFloder, 
                                            bVisitFolder);
                }
            }else{
                fn(*ib);
            }
        }
        return fn;
    }
    

    template < typename Fn >
    inline Fn for_each_in_folder(const char* szFolder, 
                                 Fn fn,
                                 bool bIncludeSubFloder = true,
                                 bool bVisitFolder = false)
    {
        return for_each_in_folder(std::string(szFolder),
                                  fn, 
                                  bIncludeSubFloder, 
                                  bVisitFolder);
    }
    
}



#endif /* FindFileIter_h */
