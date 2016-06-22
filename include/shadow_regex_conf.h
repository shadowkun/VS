//
//  shadow_regex_conf.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_regex_conf_h
#define shadow_regex_conf_h


#include "shadow_regex_match_v.h"
#include "shadow_config.h"

namespace shadow {
    
    /**
     * Sample:<br>
     * "<"Macro">"<br>
     *   m1 = value1 <br>
     *   m2 = (value2|value3) <br>
     * "<"/Macro">"
     * "<"Rule">" <br>
     *   $(m1);${m2}|value4;÷µ1$(var1);$(va2)v2$(va1) <br>
     * "<"Rule">"
     */
    class CRegexConf
    {
    public:
        CRegexConf(const CConfig& conf);
        /**
         * no implementation
         */
        CRegexConf(const CRegexConf&);
        ~CRegexConf();
        
    public:
 
        void Init(const string& basepath
                  ,const unsigned option_num
                  ,const unsigned value_num)
        throw(conf_load_error);
        
 
        void Load() throw(conf_load_error);
 
        void SetVariable(const string& name,const string& value)
        {_regex.SetVariable(name, value);}
 
        void ClearVariable(const string& name="")
        {_regex.ClearVariable(name);} // "" -- clear all
        
 
        bool Match(const vector<string>& options,vector<string>& values,bool clearvar=true)
        {return _regex.Match(options, values, clearvar);}
        
 
        size_t MatchAll(const vector<string>& options,vector<vector<string> >& all_values,bool clearvar=true)
        {return _regex.MatchAll(options, all_values, clearvar);}
        
 
        const vector<CRuleItem>& GetRuleItem() const{return _rules;}
        
    public:
 
        bool AdvancedMatch(const vector<string>& options,const vector<CRegexMatch::ad_match *>& matchfun,vector<string>& values,bool clearvar=true)
        {return _regex.AdvancedMatch(options, matchfun, values, clearvar);}
        
 
        size_t AdvancedMatchAll(const vector<string>& options,const vector<CRegexMatch::ad_match *>& matchfun,vector<vector<string> >& all_values,bool clearvar=true)
        {return _regex.AdvancedMatchAll(options, matchfun, all_values, clearvar);}
        
    protected:
        string decode_option(const map<string,string>& macro,const string& value);
        
    protected:
        const CConfig& _conf;
        string _base_path;
        unsigned _option_num;
        unsigned _value_num;
        
        vector<CRuleItem> _rules;
        CRegexMatchEx _regex;
        
    };
    
}
#endif /* shadow_regex_conf_h */
