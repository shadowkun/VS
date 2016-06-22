//
//  shadow_struct_block.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/16.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_struct_block_h
#define shadow_struct_block_h


#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <vector>

namespace shadow {
    struct struct_full: public std::runtime_error{ struct_full(const std::string& s):std::runtime_error(s){}};

    class SBlock
    {
    public:
        SBlock() {_header = NULL;_bits_header = NULL; _data = NULL;}
        ~SBlock() {_header = NULL;_bits_header = NULL; _data = NULL;}
        
        static unsigned long getSize(unsigned datanum,unsigned datasize)
        {
            return sizeof(Header)+(sizeof(unsigned)+datasize)*(datanum+1);
        }
        static unsigned long getHeadSize(unsigned datanum)
        {
            return sizeof(Header)+sizeof(unsigned)*(datanum+1);
        }
        
        void attach(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error);
        

        void backupHead(char *pBuf) { memcpy(pBuf,(char *)_header,getHeadSize(_header->iDataNum)); }
        /*
         void attach(char* pBufHead,unsigned long iHeadSize,char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error);
         void create(char* pBufHead,unsigned long iHeadSize,char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error);
         */

        
        char * operator [](unsigned pos);

        
        unsigned alloc() throw(struct_full);

        
        void malloc(unsigned num,std::vector<unsigned>& poss) throw(struct_full);

        
        void realloc(unsigned pos,unsigned num,std::vector<unsigned>& poss) throw(struct_full);

        unsigned get(unsigned pos,std::vector<unsigned>& poss);

        
        unsigned free(unsigned pos);
        
    public:
        bool empty() const {return _header->iUseNum == 0;}
        bool full() const {return _header->iUseNum == _header->iDataNum;}
        unsigned used() const {return _header->iUseNum;}
        unsigned idle() const {return _header->iDataNum - _header->iUseNum;}
        
    private:
        struct Header
        {
            unsigned long iBufSize;
            unsigned iDataSize;
            unsigned iDataNum;
            unsigned iUseNum;
            unsigned iIdleBegin;
            unsigned iIdleEnd;
        };
        
        Header *_header;
        unsigned * _bits_header;
        char *_data;
    };
    
    inline void SBlock::attach(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error)
    {
        assert(datanum>0 && datanum<0x7FFFFFFF && datasize>0);
        if(iBufSize != getSize(datanum, datasize)) {
            throw std::runtime_error("SBlock::attach fail: iBufSize!=getSize");
        }
        _header = (Header *)pBuf;
        _bits_header = (unsigned *)(pBuf+sizeof(Header));
        _data = pBuf+sizeof(Header)+sizeof(unsigned)*(datanum+1);
        
        if(_header->iBufSize != iBufSize) {
            throw std::runtime_error("SBlock::attach fail: iBufSize!=iBufSize");
        }
        if(_header->iDataSize != datasize) {
            throw std::runtime_error("SBlock::attach fail: iDataSize!=datasize");
        }
        if(_header->iDataNum != datanum) {
            throw std::runtime_error("SBlock::attach fail: iDataNum!=realnum");
        }
        if(_header->iUseNum > datanum) {
            throw std::runtime_error("SBlock::attach fail: iUseNum>datanum");
        }
        if(_header->iIdleBegin > datanum) {
            throw std::runtime_error("SBlock::attach fail: iIdleBegin>datanum");
        }
        if(_header->iIdleEnd >= datanum) {
            throw std::runtime_error("SBlock::attach fail: iIdleEnd>datanum");
        }
    }
    
    inline void SBlock::create(char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error)// attach and init
    {
        assert(datanum>0 && datanum<0x7FFFFFFF && datasize>0);
        if(iBufSize != getSize(datanum, datasize)) {
            throw std::runtime_error("SBlock::create fail: iBufSize!=getSize");
        }
        _header = (Header *)pBuf;
        _bits_header = (unsigned *)(pBuf+sizeof(Header));
        _data = pBuf+sizeof(Header)+sizeof(unsigned)*(datanum+1);
        
        _header->iBufSize = iBufSize;
        _header->iDataSize = datasize;
        _header->iDataNum = datanum;
        _header->iUseNum = 0;
        _header->iIdleBegin = 1; // data[0] ±£¡Ù,∂º==0 ±±Ì æ¬˙
        _header->iIdleEnd = datanum;
        _bits_header[0] = 0;
        for(unsigned i=1;i<=datanum;i++) {
            if(i==datanum) {
                _bits_header[i] = 0;
            } else {
                _bits_header[i] = i+1;
            }
        }
    }
    /*
     inline void SBlock::attach(char* pBufHead,unsigned long iHeadSize,char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error)
     {
     assert(datanum>0 && datanum<0x7FFFFFFF && datasize>0);
     if(iHeadSize != getHeadSize(datanum)) {
     throw std::runtime_error("SBlock::attach fail: iHeadSize!=getHeadSize");
     }
     if(iBufSize != getSize(datanum, datasize)-getHeadSize(datanum)) {
     throw std::runtime_error("SBlock::attach fail: iBufSize!=getSize");
     }
     _header = (Header *)pBufHead;
     _bits_header = (unsigned *)(pBufHead+sizeof(Header));
     _data = pBuf;
     
     if(_header->iBufSize != iBufSize) {
     throw std::runtime_error("SBlock::attach fail: iBufSize!=iBufSize");
     }
     if(_header->iDataSize != datasize) {
     throw std::runtime_error("SBlock::attach fail: iDataSize!=datasize");
     }
     if(_header->iDataNum != datanum) {
     throw std::runtime_error("SBlock::attach fail: iDataNum!=realnum");
     }
     if(_header->iUseNum > datanum) {
     throw std::runtime_error("SBlock::attach fail: iUseNum>datanum");
     }
     if(_header->iIdleBegin > datanum) {
     throw std::runtime_error("SBlock::attach fail: iIdleBegin>datanum");
     }
     if(_header->iIdleEnd >= datanum) {
     throw std::runtime_error("SBlock::attach fail: iIdleEnd>datanum");
     }
     }
     
     inline void SBlock::create(char* pBufHead,unsigned long iHeadSize,char* pBuf, unsigned long iBufSize,unsigned datanum,unsigned datasize) throw (std::runtime_error)// attach and init
     {
     assert(datanum>0 && datanum<0x7FFFFFFF && datasize>0);
     if(iHeadSize != getHeadSize(datanum)) {
     throw std::runtime_error("SBlock::attach fail: iHeadSize!=getHeadSize");
     }
     if(iBufSize != getSize(datanum, datasize)-getHeadSize(datanum)) {
     throw std::runtime_error("SBlock::attach fail: iBufSize!=getSize");
     }
     
     _header = (Header *)pBufHead;
     _bits_header = (unsigned *)(pBufHead+sizeof(Header));
     _data = pBuf;
     
     _header->iBufSize = iBufSize;
     _header->iDataSize = datasize;
     _header->iDataNum = datanum;
     _header->iUseNum = 0;
     _header->iIdleBegin = 1; // data[0] ±£¡Ù,∂º==0 ±±Ì æ¬˙
     _header->iIdleEnd = datanum;
     _bits_header[0] = 0;
     for(unsigned i=1;i<=datanum;i++) {
     if(i==datanum) {
     _bits_header[i] = 0;
     } else {
     _bits_header[i] = i+1;
     }
     }
     }
     */
    inline char * SBlock::operator [](unsigned pos)
    {
        assert(pos > 0 && pos <= _header->iDataNum);
        if((_bits_header[pos] & 0x80000000) == 0) return NULL;
        return _data+_header->iDataSize*pos;
    }
    
    inline unsigned SBlock::alloc() throw(struct_full)
    {
        if(full()) throw struct_full("SBlock::alloc full");
        
        assert(_header->iIdleBegin != 0);
        assert(_header->iIdleEnd != 0);
        
        unsigned pos = _header->iIdleBegin;
        assert((_bits_header[pos] & 0x80000000) == 0);
        unsigned next = (_bits_header[pos] & 0x7FFFFFFF);
        _bits_header[pos] = 0x80000000;
        _header->iIdleBegin = next;
        _header->iUseNum++;
        if(next == 0) {
            assert(_header->iIdleEnd == pos);
            _header->iIdleEnd = 0;
            assert(full());
        }
        return pos;
    }
    
    inline void SBlock::malloc(unsigned num,std::vector<unsigned>& poss) throw(struct_full)
    {
        if(idle() < num) throw struct_full("SBlock::malloc full");
        
        //poss.clear();
        
        unsigned pos = 0;
        unsigned next = 0;
        for(unsigned i=0;i<num;i++){
            assert(_header->iIdleBegin != 0);
            assert(_header->iIdleEnd != 0);
            pos = _header->iIdleBegin;
            assert((_bits_header[pos] & 0x80000000) == 0);
            next = (_bits_header[pos] & 0x7FFFFFFF);
            if(i == num-1)
                _bits_header[pos] = 0x80000000;
            else
                _bits_header[pos] = (next | 0x80000000);
            _header->iIdleBegin = next;
            _header->iUseNum++;
            if(next == 0) {
                assert(_header->iIdleEnd == pos);
                _header->iIdleEnd = 0;
                assert(full());
                assert(i==num-1);
            }
            poss.push_back(pos);
        }
    }
    
    inline void SBlock::realloc(unsigned pos,unsigned num,std::vector<unsigned>& poss) throw(struct_full)
    {
        assert(pos > 0 && pos <= _header->iDataNum);
        //poss.clear();
        
        unsigned i=0;
        unsigned next = 0;
        unsigned last = 0;
        unsigned do_num = 0;
        unsigned old_num = 0;
        for(;i<_header->iDataNum;i++) {
            assert((_bits_header[pos] & 0x80000000) != 0);
            poss.push_back(pos);
            next = (_bits_header[pos] & 0x7FFFFFFF);
            if(next == 0) {
                last = pos;
                break;
            }
            pos = next;
        }
        assert(i<_header->iDataNum);
        assert(last>0);
        old_num = poss.size();
        if(old_num == num) {
            return;
        } else if(old_num < num) {
            do_num = num-old_num;
            malloc(do_num,  poss);
            assert(poss.size() == num);
            _bits_header[last] = (poss[old_num] | 0x80000000);
        }else {
            do_num = old_num-num;
            _bits_header[poss[num-1]] = 0x80000000;
            unsigned f_num = free(poss[num]);
            assert(f_num == do_num);
            poss.resize(num);
        }
    }
    
    inline unsigned SBlock::get(unsigned pos,std::vector<unsigned>& poss)
    {
        assert(pos > 0 && pos <= _header->iDataNum);
        poss.clear();
        if((_bits_header[pos] & 0x80000000) == 0) return 0;
        
        unsigned i=0;
        unsigned next = 0;
        for(;i<_header->iDataNum;i++) {
            assert((_bits_header[pos] & 0x80000000) != 0);
            poss.push_back(pos);
            next = (_bits_header[pos] & 0x7FFFFFFF);
            if(next == 0) {
                break;
            }
            pos = next;
        }
        assert(i<_header->iDataNum);
        return poss.size();
    }
    
    inline unsigned SBlock::free(unsigned pos)
    {
        assert(pos > 0 && pos <= _header->iDataNum);
        if((_bits_header[pos] & 0x80000000) == 0) return 0;
        
        unsigned i=0;
        unsigned next = 0;
        unsigned num = 0;
        for(;i<_header->iDataNum;i++) {
            assert((_bits_header[pos] & 0x80000000) != 0);
            num++;
            next = (_bits_header[pos] & 0x7FFFFFFF);
            if(_header->iIdleBegin == 0) {
                assert(_header->iIdleEnd == 0);
                assert(_header->iUseNum == _header->iDataNum);
                _bits_header[pos] = 0;
                _header->iIdleBegin = pos;
                _header->iIdleEnd = pos;
                _header->iUseNum--;
            } else {
                _bits_header[pos] =  _header->iIdleBegin;
                _header->iIdleBegin = pos;
                _header->iUseNum--;
            }
            pos = next;
            if(pos == 0) break;
        }
        assert(i<_header->iDataNum);
        return num;
    }
    
}

#endif /* shadow_struct_block_h */
