//
//  scope_helper.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef scope_helper_h
#define scope_helper_h



#include "meta.h"
#include "runnable.h"

#define UNIQUE_VAR_NAME	WBL_JOIN(_scope_helper_, __LINE__)

#define SCOPE_EXIT_FUN   wbl::scope_helper UNIQUE_VAR_NAME; UNIQUE_VAR_NAME = make_fun_runnable
#define SCOPE_EXIT_MEMFUN   wbl::scope_helper UNIQUE_VAR_NAME; UNIQUE_VAR_NAME = make_memfun_runnable

namespace shadow
{
    class scope_helper
    {
        runnable * _run;
        
        scope_helper(const scope_helper& s);
        scope_helper& operator=(const scope_helper&);
    public:
        explicit scope_helper(runnable * run = NULL)
        : _run(run)
        {}
        
        const scope_helper& operator=(runnable * run)
        {
            dismiss();
            _run = run;
            return *this;
        }
        
        void dismiss()
        {
            delete _run;
            _run = NULL;
        }
        
        ~scope_helper()
        {
            if(_run != NULL){
                try{
                    _run->run();
                    delete _run;
                }catch(...){
                }
            }
        }
    };
}

#endif /* scope_helper_h */
