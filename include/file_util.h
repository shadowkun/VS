//
//  file_util.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef file_util_h
#define file_util_h

#include <string>

#ifdef _WIN32
#	include <direct.h>
#	include <windows.h>
#else
#	include <unistd.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#endif

namespace shadow
{
    /** get current execute file's path
     *  \return     the current execute file's full path
     */
    std::string get_exec_file_path();
    
    /** get exe file's folder's path
     */
    std::string get_exec_base_path();
    
    /** mkdir recursive
     *  \param pathname     directory to create
     *  \param mode         specifies the permissions to use. It is modified by the process?[m~@~Ys umask in the usual way: thh
     e permissions of the created directory are (mode & ~umask & 0777).  Other mode bits of the created directory depend on tt
     he operating system.
     *  \return             when success, return true; otherwise return false
     */
    bool mkdir_recursive(const std::string& pathname, mode_t mode);
}

#endif /* file_util_h */
