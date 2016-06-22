//
//  shadow_buffer_queue.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_queue_h
#define shadow_buffer_queue_h


#include <vector>
#include <sys/time.h>

#include "shadow_buffer_block.h"

namespace shadow {
    
 
    class CBufferQueue
    {
    public:
        CBufferQueue() {_header = NULL; _data = NULL;}
        ~CBufferQueue() {_header = NULL; _data = NULL;}
        
 
        void attach(char* pBuf, unsigned long iBufSize) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize) throw (std::runtime_error); // attach and init
        
 
        bool dequeue(char *buffer,unsigned & buffersize) throw(buffer_full);
 
        bool dequeue(char *buffer1,unsigned & buffersize1,char *buffer2,unsigned & buffersize2) throw(buffer_full);
        
 
        bool peek(char *buffer,unsigned & buffersize) throw(buffer_full);
        bool peek(char *buffer1,unsigned & buffersize1,char *buffer2,unsigned & buffersize2) throw(buffer_full);
        
 
        void enqueue(const char *buffer,unsigned len) throw(buffer_full);
        void enqueue(const char *buffer1,unsigned len1,const char *buffer2,unsigned len2) throw(buffer_full);
        
        bool isEmpty() const {return _header->iBegin == _header->iEnd;}
        bool isFull(unsigned long len) const;
    private:
        unsigned long GetLen(char *buf) {unsigned long u; memcpy((void *)&u,buf,sizeof(unsigned long)); return u;}
        void SetLen(char *buf,unsigned long u) {memcpy(buf,(void *)&u,sizeof(unsigned long));}
        
    private:
        const static unsigned long ReserveLen = 8;
        typedef struct Header
        {
            unsigned long iBufSize;
            unsigned long iReserveLen; // must be 8
            unsigned long iBegin;
            unsigned long iEnd;
        };
        
        Header *_header;
        char *_data;
    };
}
#endif /* shadow_buffer_queue_h */
