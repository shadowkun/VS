//
//  PathUtility.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef PathUtility_h
#define PathUtility_h

#include <string>

namespace shadow
{
    
    /** get file name from a path
     *  \param szFullPath	full path of file
     *  \return				name of file
     */
    std::string GetFileNameFromFullPath(const std::string& szFullPath);
    
    /** get file name without extension
     *  \param szFile	full name of file
     *  \return			name of file exclude extension name
     */
    std::string GetFileNameWithoutExt(const std::string& szFile);
    
    /** get file extension name
     *  \param szFile	full name of file
     *  \return			extension name of file
     */
    std::string GetFileExt(const std::string& szFile);
    
    /** get base path from a path
     *  \param szFullPath	full path of file
     *  \return				base path
     *  etc: GetBasePath("/bin/bash") result is "/bin
     */
    std::string GetBasePath(const std::string& szFullPath);
    
    /** trim path string, and convert the path include "../" to a absolute path
     *  \param szPath		path to trim
     */
    void TrimPath(std::string& szPath);
    
    /** get full path of the file
     *  \param szPath	relative path
     *  \return			full path
     */
    std::string GetFullPath(const std::string& szPath);
    
    /** compare file name
     *  \param szFile1		first filename
     *  \param szFile2		second filename
     *  \return				if szFile1 > szFile2, return a positive, if szFile1 < szFile2, return a negative, else return zero
     */
    int CompareFileName(const std::string& szFile1, const std::string& szFile2);
    
}



#endif /* PathUtility_h */
