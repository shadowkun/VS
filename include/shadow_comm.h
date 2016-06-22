//
//  shadow_comm.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/16.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_comm_h
#define shadow_comm_h


#include <string>
#include <time.h>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cctype>
#include <sstream>
#include <utility>
#include <stdexcept>

using namespace std;

namespace shadow {
    
    struct comm_error: public logic_error{ comm_error(const string& s);};
    

    void Daemon();
    

    inline bool is_digits(const std::string& s)
    {
        if(s.length() == 0) return false;
        for (size_t i = 0; i < s.length(); i++){
            if (!isdigit(s.c_str()[i])) return false;
        }
        return true;
    }

    inline bool is_xdigits(const std::string& s)
    {
        if(s.length() == 0) return false;
        for (size_t i = 0; i < s.length(); i++){
            if (!isxdigit(s.c_str()[i])) return false;
        }
        return true;
    }
    

    std::string trim_left(const std::string &s,const std::string& filt=" ");
    

    std::string trim_right(const std::string &s,const std::string& filt=" ");
    

    inline std::string trim(const std::string &s,const std::string& filt=" ")
    {
        return trim_right(trim_left(s, filt),filt);
    }
    

    inline string upper(const string &s)
    {
        string s2;
        for(size_t i=0;i<s.length();i++) {
            s2 += ((unsigned char)toupper(s.c_str()[i]));
        }
        return s2;
    }
    

    inline string lower(const string &s)
    {
        string s2;
        for(size_t i=0;i<s.length();i++) {
            s2 += ((unsigned char)tolower(s.c_str()[i]));
        }
        return s2;
    }
    

    enum TimeField {Year,Month,Day,Hour,Minute,Second};
    

    inline time_t time_add(time_t t,TimeField f,int i)
    {
        struct tm curr;
        switch(f) {
            case Second:
                return t+i;
            case Minute:
                return t+i*60;
            case Hour:
                return t+i*60*60;
            case Day:
                return t+i*60*60*24;
            case Month:
                curr = *localtime(&t);
                curr.tm_year += (curr.tm_mon-1+i)/12;
                curr.tm_mon = (curr.tm_mon-1+i) %12+1;
                return mktime(&curr);
            case Year:
                curr = *localtime(&t);
                curr.tm_year+=i;
                return mktime(&curr);
            default:
                return 0;
        }
        
        return 0;
    }
    

    template<typename T> std::string tostr(const T& t)
    {
        std::ostringstream s;
        s << t;
        return s.str();
    }
    

    template<typename F, typename S> std::string tostr(const std::pair<F, S>& it)
    {
        std::ostringstream s;
        s << "["+(it.first)+"="+(it.second)+"]";
        return s.str();
    }
    
    template <typename I> std::string tostr(I itbegin,I itend)
    {
        std::string s;
        I it = itbegin;
        for(I it=itbegin;it!=itend;it++) {
            s.append(tostr(*it));
            s.append(" ");
        }
        return s;
    }
    
    template<typename T> T strto(const std::string& s)
    {
        std::istringstream is(s);
        T t;
        is >> t;
        return t;
    }
    
    inline string u2s(const unsigned u)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%u", u);
        return string(sTmp);
    }

    inline string ul2s(const unsigned long u)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%lu", u);
        return string(sTmp);
    }
    
    inline string u2sx(const unsigned u)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%x", u);
        return string(sTmp);
    }

    inline string ul2sx(const unsigned long u)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%lx", u);
        return string(sTmp);
    }
    
    inline string i2s(const int i)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%d", i);
        return string(sTmp);
    }
    
    inline string l2s(const long i)
    {
        char sTmp[64] ={0};
        sprintf(sTmp, "%ld", i);
        return string(sTmp);
    }
    
    inline string c2s(const char c)
    {
        if(c==0) return string("");
        return string()+c;
    }
    
    string b2s(const char *b,size_t len,const unsigned block = 8);
    
    inline string t2s(const time_t t,const string& format="%Y-%m-%d %H:%M:%S")
    {
        struct tm curr;
        curr = *localtime(&t);
        char sTmp[1024];
        strftime(sTmp,sizeof(sTmp),format.c_str(),&curr);
        return string(sTmp);
    }
    
    inline string now(const string& format="%Y-%m-%d %H:%M:%S")
    {
        return t2s(time(0),format);
    }
    
    unsigned s2u(const std::string &s) throw (comm_error);
    
    unsigned s2u(const std::string &s,unsigned defaultvalue);
    
    unsigned long s2ul(const std::string &s) throw (comm_error);

    unsigned long s2ul(const std::string &s,unsigned long defaultvalue);
    
    unsigned sx2u(const std::string &s) throw (comm_error);

    unsigned long sx2ul(const std::string &s) throw (comm_error);
    
    unsigned sx2u(const std::string &s,unsigned defaultvalue);

    unsigned long sx2ul(const std::string &s,unsigned long defaultvalue);
    

    int s2i(const std::string &s) throw (comm_error);
    

    int s2i(const std::string &s,int defaultvalue);
    

    long s2l(const std::string &s) throw (comm_error);
    

    long s2l(const std::string &s,long defaultvalue);
    

    char s2c(const std::string &s);
    

    char s2c(const std::string &s,char defaultvalue);
    

    size_t s2b(const std::string &s,char *b,size_t maxlen) throw (comm_error);
    

    time_t s2t(const std::string &s,const std::string& format="%Y-%m-%d %H:%M:%S") throw (comm_error);
    

    string trim_head(string& s,const string& split=" \t");
    
    inline bool is_qq(unsigned q)
    {
        return (q>10000);
    }
    
    inline bool is_qq(const std::string& s)
    {
        return (s2u(s,0)>10000);
    }
    
    inline bool is_cellphone(const std::string& s)
    {
        if(!is_digits(s)) return false;
        // 13xxx 15xxx
        if(s.c_str()[0] == '1' && (s.c_str()[1] == '3' || s.c_str()[1] == '5')) {
            return s.length() == 11;
        }
        // 013xxx 015xxx
        if(s.c_str()[0] == '0' && s.c_str()[1] == '1' && (s.c_str()[2] == '3' || s.c_str()[2] == '5')) {
            return s.length() == 12;
        }
        // 8613xxx 8615xxx
        if(s.c_str()[0] == '8' && s.c_str()[1] == '6' && s.c_str()[2] == '1' && (s.c_str()[3] == '3' || s.c_str()[3] == '5')) {
            return s.length() == 13;
        }
        return false;
    }
    
    inline bool is_telephone(const std::string& s)
    {
        if(!is_digits(s)) return false;
        // 0755xxx
        if(s.c_str()[0] == '0') {
            return (s.length() >= 10 && s.length() <= 12);
        }
        // 86755xxx
        if(s.c_str()[0] == '8' && s.c_str()[1] == '6') {
            if(s.c_str()[2] == '1' && (s.c_str()[3] == '3' || s.c_str()[3] == '5')) return false;
            return (s.length() >= 11 && s.length() <= 13);
        }
        return false;
    }
    
    inline bool is_phone(const std::string& s)
    {
        return (is_cellphone(s) || is_telephone(s));
    }
    
}

#endif /* shadow_comm_h */
