//
//  shadow_buffer_block.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_block_h
#define shadow_buffer_block_h

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>

namespace shadow {
    struct buffer_full: public std::runtime_error{ buffer_full(const std::string& s):std::runtime_error(s){}};
 
    class CBufferBlock
    {
    public:
        CBufferBlock() {_header = NULL; _data = NULL;}
        ~CBufferBlock() {_header = NULL; _data = NULL;}
 
        void attach(char* pBuf, unsigned long iBufSize,unsigned long datanum,unsigned long datasize) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize,unsigned long datanum,unsigned long datasize) throw (std::runtime_error); // attach and init
        
        static unsigned long getSize(unsigned long datanum,unsigned long datasize)
        {
            assert(datanum>2);
            assert(datasize>0);
            unsigned long realnum = datanum+1;
            unsigned long realsize = (datasize%sizeof(unsigned long)==0?datasize+sizeof(unsigned long):(datasize/sizeof(unsigned long)+2)*sizeof(unsigned long));
            return sizeof(Header)+(realsize+sizeof(unsigned long))*realnum;
        }

        
        char * operator [](unsigned long index);

        
        unsigned long alloc() throw(buffer_full);

        bool free(unsigned long index);
        
    public:
        bool isEmpty() const {return _header->iUseNum == 0;}
        bool isFull() const {return _header->iUseNum == _header->iDataNum-1;}
        unsigned long used() const {return _header->iUseNum;}
        unsigned long idle() const {return _header->iDataNum - _header->iUseNum - 1;}
        
    private:
 
        struct Header
        {
            unsigned long iBufSize;
            unsigned long iDataSize;
            unsigned long iDataNum;
            unsigned long iUseNum;
            unsigned long iIdleBegin;
            unsigned long iIdleEnd;
        };
        
        struct DataIdle
        {
            unsigned long prefix;
            unsigned long next;
        };
        
        Header *_header;
        char *_data;
    };
}

#endif /* shadow_buffer_block_h */
