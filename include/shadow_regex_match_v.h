//
//  shadow_regex_match_v.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_regex_match_v_h
#define shadow_regex_match_v_h

#include "shadow_regex_match.h"

namespace shadow {
    
 
    class CRegexMatchEx
    {
    public:
        CRegexMatchEx();
        /**
         * no implementation
         */
        CRegexMatchEx(const CRegexMatchEx&);
        ~CRegexMatchEx();
        
    public:
 
        void Init(const vector<CRuleItem>& rules
                  ,const unsigned option_num
                  ,const unsigned value_num)
        throw(regular_is_invalid);
        
 
        void SetVariable(const string& name,const string& value) {_variable[name]=value;}

        void ClearVariable(const string& name=""){if(name=="") _variable.clear(); else _variable[name];} // "" -- clear all
        

        bool Match(const vector<string>& options,vector<string>& values,bool clearvar=true);
        

        size_t MatchAll(const vector<string>& options,vector<vector<string> >& all_values,bool clearvar=true);
        
    public:

        bool AdvancedMatch(const vector<string>& options,const vector<CRegexMatch::ad_match *>& matchfun,vector<string>& values,bool clearvar=true);
        

        size_t AdvancedMatchAll(const vector<string>& options,const vector<CRegexMatch::ad_match *>& matchfun,vector<vector<string> >& all_values,bool clearvar=true); 
        
    protected:
        void ReplaceVar(string& s);
        void CheckValue(const string& s);
        
    protected:
        map<string,string> _variable;
        CRegexMatch _regex;
        
    };
    
}
#endif /* shadow_regex_match_v_h */
