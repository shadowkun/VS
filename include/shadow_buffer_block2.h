//
//  shadow_buffer_block2.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_block2_h
#define shadow_buffer_block2_h



#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <vector>

namespace shadow {
    struct buffer_full: public std::runtime_error{ buffer_full(const std::string& s):std::runtime_error(s){}};
 
    class CBufferBlock2
    {
    public:
        CBufferBlock2() {_header = NULL;_bits_header = NULL; _data = NULL;}
        ~CBufferBlock2() {_header = NULL;_bits_header = NULL; _data = NULL;}
 
        void attach(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error); // attach and init
        
        static unsigned long getSize(unsigned datanum,unsigned datasize)
        {
            assert(datanum>0);
            assert(datasize>0);
            unsigned realnum = datanum+1; // Œª÷√0±£¡Ù
            return sizeof(Header)+(datasize+sizeof(Bits))*realnum;
        }
 
        char * operator [](unsigned pos);
 
        unsigned getLink(unsigned pos,std::vector<unsigned>& poss);
 
        bool getData(unsigned pos,char *value,unsigned &valuesize) throw(buffer_full);
 
        void alloc(unsigned num,std::vector<unsigned>& poss) throw(buffer_full);
 
        void alloc(unsigned pos,unsigned num,std::vector<unsigned>& poss) throw(buffer_full);
 
        unsigned free(unsigned pos);
 
        bool erase(unsigned pre,unsigned& pos);
    public:
        bool isEmpty() const {return _header->iUseNum == 0;}
        bool isFull() const {return _header->iUseNum == _header->iDataNum;}
        unsigned used() const {return _header->iUseNum;}
        unsigned usedLogic() const {return _header->iLogicNum;}
        unsigned idle() const {return _header->iDataNum - _header->iUseNum;}
        unsigned long data_offset() const {return (unsigned long)sizeof(Header)+(unsigned long)sizeof(Bits)*(_header->iDataNum+1);}
        
    private:
 
        struct Header
        {
            unsigned long iBufSize;
            unsigned iDataSize;
            unsigned iDataNum;
            unsigned iUseNum;
            unsigned iLogicNum;
            unsigned iIdleBegin;
            unsigned iIdleEnd;
        };
 
        struct Bits
        {
            unsigned stat;
            unsigned next;
        };
        
        Header *_header;
        Bits * _bits_header;
        char *_data;
    };
}

#endif /* shadow_buffer_block2_h */
