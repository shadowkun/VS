//
//  shadow_regex_match.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_regex_match_h
#define shadow_regex_match_h


#include <string>
#include <map>
#include <vector>
#include <regex.h>
#include <stdexcept>
using namespace std;

namespace shadow {
    
    struct regular_is_invalid: public logic_error { regular_is_invalid(const string& s);};
    

    struct CRuleItem {
        vector<string> options;
        vector<string> values;
    };
    

    class CRegexMatch
    {
    public:
        CRegexMatch();
        /**
         * no implementation
         */
        CRegexMatch(const CRegexMatch&);
        ~CRegexMatch();
        
    public:

        void Init(const vector<CRuleItem>& rules
                  ,const unsigned option_num
                  ,const unsigned value_num)
        throw(regular_is_invalid);

        bool Match(const vector<string>& options,vector<string>& values);

        size_t MatchAll(const vector<string>& options,vector<vector<string> >& all_values);
        
    public:

        struct ad_match {
            virtual bool matched(const string& rule,const string& value)=0;
            virtual ~ad_match(){}
        };

        bool AdvancedMatch(const vector<string>& options,const vector<ad_match *>& matchfun,vector<string>& values);
        

        size_t AdvancedMatchAll(const vector<string>& options,const vector<ad_match *>& matchfun,vector<vector<string> >& all_values);
        
    protected:
        bool MatchRegex(const string& rule,const string& value);
        void InitRegex(const string& s);
        void ReleaseRegex();
        
    protected:
        static unsigned Min(unsigned a,unsigned b) {return a>b?b:a;}
        
    protected:
        unsigned _option_num;
        unsigned _value_num;
        
        vector<CRuleItem> _allrules;
        map<string,regex_t> _str_to_regex;
    };
    

    struct ad_match_great:public CRegexMatch::ad_match
    {
        virtual bool matched(const string& rule,const string& value) {return value>rule;}
        virtual ~ad_match_great(){}
    };
    

    struct ad_match_equal:public CRegexMatch::ad_match
    {
        virtual bool matched(const string& rule,const string& value) {return value==rule;}
        virtual ~ad_match_equal(){}
    };
    

    struct ad_match_less:public CRegexMatch::ad_match 
    {
        virtual bool matched(const string& rule,const string& value) {return value<rule;}
        virtual ~ad_match_less(){}
    };
    

    struct ad_match_ge:public CRegexMatch::ad_match 
    {
        virtual bool matched(const string& rule,const string& value) {return value>=rule;}
        virtual ~ad_match_ge(){}
    };
    

    struct ad_match_le:public CRegexMatch::ad_match 
    {
        virtual bool matched(const string& rule,const string& value) {return value<=rule;}
        virtual ~ad_match_le(){}
    };
    
}


#endif /* shadow_regex_match_h */
