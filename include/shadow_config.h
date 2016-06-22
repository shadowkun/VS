//
//  shadow_config.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_config_h
#define shadow_config_h


#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace shadow {
    
    struct conf_load_error: public std::runtime_error{ conf_load_error(const std::string& s):std::runtime_error(s){}};
    struct conf_not_find: public std::runtime_error{ conf_not_find(const std::string& s):std::runtime_error(s){}};
 
    class CConfig
    {
    public:
        CConfig(){}
        CConfig(const CConfig&);
        virtual ~CConfig(){}
        
    public:
 
        virtual void Load()=0;
        
 
        virtual const std::string& operator [](const std::string& name) const = 0;
        
 
        virtual const std::map<std::string,std::string>& GetPairs(const std::string& path) const = 0;
 
        virtual const std::vector<std::string>& GetDomains(const std::string& path) const = 0;
 
        virtual const std::vector<std::string>& GetSubPath(const std::string& path) const = 0;
    };
    
}


#endif /* shadow_config_h */
