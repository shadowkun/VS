//
//  regex_util.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef regex_util_h
#define regex_util_h

#include <regex.h>
#include <stdlib.h>
#include <string>
#include <vector>

namespace shadow
{
    
    /** split string with a regexp separator, to a output iterator
     *  \param str			string to split
     *  \param sep_exp		separator's regexp
     *  \param o			output iterator to store result
     *  \return				when success, return true; otherwise return false
     */
    template < typename OI >
    bool split_string_o(const std::string& str, regex_t * sep_exp, OI o)
    {
        if(str.empty())
            return true;
        regmatch_t matches[1];
        const char * cstr = str.c_str();
        int pos1 = 0;
        int pos2 = 0;
        int posBegin = 0;
        for(; pos2 >= 0; ){
            if(::regexec(sep_exp, cstr + pos1, 1, matches, 0) == 0){
                if(matches[0].rm_eo == 0){
                    if((size_t)++pos1 > str.size()){
                        if((size_t)posBegin < str.size()){
                            std::string s(cstr + posBegin);
                            *o = s;
                        }
                        break;
                    }
                }else{
                    pos2 = pos1 + matches[0].rm_so;
                    std::string s(cstr + posBegin, cstr + pos2);
                    *o = s;
                    pos1 += matches[0].rm_eo;
                    posBegin = pos1;
                }
            }else{
                pos2 = -1;
                if((size_t)posBegin < str.size()){
                    std::string s(cstr + posBegin);
                    *o = s;
                }
            }
        }
        
        return true;
    }
    
    /** split string with a regexp separator
     *  \param str			string to split
     *  \param sep_exp		separator's regexp
     *  \param con			result container
     *  \return				when success, return true; otherwise return false
     */
    template < typename ContainerT >
    inline bool split_string(const std::string& str, regex_t * sep_exp, ContainerT& con)
    {
        con.clear();
        return split_string_o(str, sep_exp, std::back_inserter(con));
    }
    
    /** split string with a regexp separator
     *  \param str			string to split
     *  \param sep_exp		separator's regexp
     *  \param con			result container
     *  \return				when success, return true; otherwise return false
     */
    template < typename ContainerT >
    inline bool split_string(const std::string& str, const std::string& sep_exp, ContainerT& con)
    {
        regex_t reg_sep;
        if(::regcomp(&reg_sep, sep_exp.c_str(), REG_NEWLINE | REG_EXTENDED) != 0)
            return false;
        bool r = split_string(str, &reg_sep, con);
        ::regfree(&reg_sep);
        return r;
    }
    
    /** split string into a int container with a regexp separator
     *  \param str			string to split
     *  \param sep_exp		separator's regexp
     *  \param con			result container
     *  \param def			when some field not a number, it will use def instead
     *  \param base			converts string to a integer value according to the given  base
     *  \return				when some string field not a number, return false; otherwise return true
     */
    template < typename ContainerT >
    bool split_int(const std::string& str, regex_t * sep_exp, ContainerT& con, int def = 0, int base = 10)
    {
        con.clear();
        std::vector<std::string> vstr;
        if(!split_string(str, sep_exp, vstr))
            return false;
        typedef std::vector<std::string>::iterator	VIT;
        for(VIT i = vstr.begin(); i != vstr.end(); ++i){
            const char * s = i->c_str();
            char * send;
            int n = ::strtol(s, &send, base);
            if(*send != '\0')
                n = def;
            con.push_back(n);
        }
        return true;
    }
    
    /** split string into a int container with a regexp separator
     *  \param str			string to split
     *  \param sep_exp		separator's regexp
     *  \param con			result container
     *  \param def			when some field not a number, it will use def instead
     *  \param base			converts string to a integer value according to the given  base
     *  \return				when some string field not a number, return false; otherwise return true
     */
    template < typename ContainerT >
    inline bool split_int(const std::string& str, const std::string& sep_exp, ContainerT& con, int def = 0, int base = 10)
    {
        regex_t reg_sep;
        if(::regcomp(&reg_sep, sep_exp.c_str(), REG_NEWLINE | REG_EXTENDED) != 0)
            return false;
        bool r = split_int(str, &reg_sep, con, def, base);
        ::regfree(&reg_sep);
        return r;
    }
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    int replace_all(std::string& str, regex_t * regex, const char * szNew);
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    inline int replace_all(std::string& str, regex_t * regex, const std::string& szNew)
    {
        return replace_all(str, regex, szNew.c_str());
    }
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    int replace_all(std::string& str, const char * regex, const char * szNew);
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    inline int replace_all(std::string& str, const char * regex, const std::string& szNew)
    {
        return replace_all(str, regex, szNew.c_str());
    }
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    inline int replace_all(std::string& str, const std::string& regex, const std::string& szNew)
    {
        return replace_all(str, regex.c_str(), szNew);
    }
    
    /** replaces each substring of str that matches the given regular expression with the szNew.
     *  \param str			the string to replace
     *  \param regex		the regular expression to which this string is to be matched
     *  \param szNew		the replacement string
     *  \return				count of substring matches and replaced
     */
    inline int replace_all(std::string& str, const std::string& regex, const char * szNew)
    {
        return replace_all(str, regex.c_str(), szNew);
    }
    
    /** removes each substring of str thatmatches the given regular expression
     *  \param str			the string to remove
     *  \param regex		the regular expression to which this string is to be matched
     *  \return				count of substring matches and replaced
     */
    int remove_all(std::string& str, regex_t * regex);
    
    /** removes each substring of str thatmatches the given regular expression
     *  \param str			the string to remove
     *  \param regex		the regular expression to which this string is to be matched
     *  \return				count of substring matches and replaced
     */
    int remove_all(std::string& str, const char * regex);
    
    /** removes each substring of str thatmatches the given regular expression
     *  \param str			the string to remove
     *  \param regex		the regular expression to which this string is to be matched
     *  \return				count of substring matches and replaced
     */
    inline int remove_all(std::string& str, const std::string& regex)
    {
        return remove_all(str, regex.c_str());
    }
    
}



#endif /* regex_util_h */
