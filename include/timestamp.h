//
//  timestamp.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/1.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef timestamp_h
#define timestamp_h

#include <time.h>
#include <stdlib.h>
#include <stdio.h>


namespace shadow
{
    /* this routine 10 times faster than gmtime_r + sprintf */
    void gmtimestr(time_t tsrc, char *str);
    int gmtimestreq(time_t tsrc, const char *str);
    
    size_t get_std_time_str(time_t tsrc, char *str, size_t len);
}


#endif /* timestamp_h */
