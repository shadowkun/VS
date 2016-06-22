//
//  shadow_parsepara.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_parsepara_h
#define shadow_parsepara_h


#include <map>
#include <vector>
#include <string>
#include <stdexcept>

namespace shadow {
    
    struct parse_formaterr:public std::runtime_error { parse_formaterr(const std::string& s);};
 
    class CCgiPara {
    public:
        CCgiPara();
 
        CCgiPara(const CCgiPara&);
        ~CCgiPara();
        
    public:
 
        void Clear()
        {_paralist.clear();}
 
        void Decode(const std::string& s)
        throw(parse_formaterr);
        void Decode(const char *s)
        throw(parse_formaterr);
 
        std::string Encode();
 
        std::string& operator [](const std::string& name)
        { return _paralist[name];}
 
        const std::map<std::string,std::string>& GetPairs() const
        {return _paralist;}
        
    public:
 
        static std::string b2s(const char *b,size_t len); // 0-"%00" ; '%'-"%25" ; '='-"%3d" ; "\r"-"%0d" ; "\n"-"%0a" ...
 
        static std::string b2sx(const char *b,size_t len); // all - "%xx",not recommend
 
        static size_t s2b(const std::string &s,char *b,size_t maxlen);
        
    protected:
        inline static bool isBinaryString(const std::string &s);
        inline std::string FormatC2I(const std::string &s);
        inline std::string FormatI2C(const std::string &s);
    protected:
        std::map <std::string,std::string> _paralist;
        
    };
    
 
    class CSpacePara {
    public:
        CSpacePara();
 
        CSpacePara(const std::string& s);
 
        CSpacePara(const CSpacePara&);
        ~CSpacePara();
    public:
 
        void Decode(const std::string& s);
        void Decode(const char *s);
 
        void DecodeStrict(const std::string& s); //
        void DecodeStrict(const char *s); //
 
        const std::vector<std::string>& GetPairs() const 
        {return _paralist;}
        
    public: 
 
        void SetSplitChar(const std::string& s=" ,#\t\r\n") 
        {_split=s;}
        
    protected:
        bool isplit(unsigned char c,unsigned char c2);
    protected:
        std::string _split;
        std::vector<std::string> _paralist;
    };
    
    
    //inline unsigned parse_x2u(const std::string &s);
    
}

#endif /* shadow_parsepara_h */
