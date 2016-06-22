//
//  shadow_buffer_map2.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_map2_h
#define shadow_buffer_map2_h


#include "shadow_buffer_block2.h"
#include <vector>
#include <sys/time.h>

namespace shadow {
 
    class CBufferMap2
    {
    public:
        CBufferMap2() {_header = NULL; _hashkeys = NULL;}
        ~CBufferMap2() {_header = NULL; _hashkeys = NULL;}
        enum SortBy
        {
            By_CreateTime = 0,
            By_AccessTime
        };
        struct KEY {
#if __WORDSIZE == 64
            KEY():v(0),v2(0){}
            KEY(const char * k) { Set(k);}
            void Set(const char *k) {
                memcpy(&v,k,8);
                memcpy(&v2,k+8,8);
            }
            bool SetString(const std::string &k);
            unsigned long v;
            unsigned long v2;
            
            bool operator==(const KEY& r) {return r.v == v && r.v2 == v2;}
            bool operator!=(const KEY& r){return r.v != v || r.v2 != v2;}
#else
            KEY():v(0),v2(0),v3(0),v4(0){}
            KEY(const char * k) { Set(k);}
            void Set(const char *k) {
                memcpy(&v,k,4);
                memcpy(&v2,k+4,4);
                memcpy(&v3,k,4);
                memcpy(&v4,k+4,4);
            }
            bool SetString(const std::string &k);
            
            unsigned long v;
            unsigned long v2;
            unsigned long v3;
            unsigned long v4;
            
            bool operator==(const KEY& r) {return r.v == v && r.v2 == v2 && r.v2 == v3 && r.v4 == v4;}
            bool operator!=(const KEY& r){return r.v != v || r.v2 != v2 || r.v3 != v3 || r.v4 != v4;}
#endif
        };
        static KEY NULLKEY;
        
 
        void attach(char* pBuf, unsigned long iBufSize,unsigned keynum,unsigned indexnum,unsigned datanum,unsigned datasize,SortBy sorttype=By_CreateTime) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize,unsigned keynum,unsigned indexnum,unsigned datanum,unsigned datasize,SortBy sorttype=By_CreateTime) throw (std::runtime_error);
        
        static unsigned long  getSize(unsigned keynum,unsigned indexnum,unsigned datanum,unsigned datasize)
        {
            return sizeof(Header)
            +(unsigned long)keynum*sizeof(unsigned)
            +CBufferBlock2::getSize(indexnum,sizeof(Index))
            +CBufferBlock2::getSize(datanum,datasize);
        }
        
        unsigned long size_head() const {return sizeof(Header);}
        unsigned long size_hash() const {return _header->iKeyLen;}
        unsigned long size_index() const {return _header->iIndexLen;}
        unsigned long size_data() const {return _header->iDataLen;}
    public:
 
        bool get(const KEY &key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
 
        bool get(const KEY &key,time_t &create,std::vector<char> &value);
        
 
        bool peek(const KEY &key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
        bool peek(const KEY &key,time_t &create,std::vector<char> &value);
        
 
        bool touch(const KEY &key,time_t &create);
        
 
        bool find(const KEY &key);
 
        bool find(const KEY &key,char **value,unsigned &valuesize);
        
 
        bool dequeue(KEY &key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
        bool dequeue(KEY &key,time_t &create,std::vector<char> &value);
        
 
        void set(const KEY &key,char *value,unsigned valuesize) throw (buffer_full);
        
 
        void set_secret(const KEY &key,char *value,unsigned valuesize) throw (buffer_full);
        
 
        bool erase(const KEY &key);
        
    public:
        unsigned totalIndex() const {return _header->iIndexNum;}
        unsigned totalData() const {return _header->iDataNum;}
        unsigned usedIndex() const {return _indexs.used();}
        unsigned usedData() const {return _data.used();}
        unsigned usedLogicData() const {return _data.usedLogic();}
        
        bool isEmpty() const {assert(_indexs.isEmpty() == _data.isEmpty()); return _indexs.isEmpty();}
        bool isFull() const {return _indexs.isFull() || _data.isFull();}
    public:
 
        unsigned recycleByTime(unsigned timeout);
 
        unsigned recycleByNum(unsigned num);
        
    public:
        class const_iterator;
        class Pair
        {
            friend class CBufferMap2::const_iterator;
        public:
            KEY key() const;
            void value(std::vector<char> &value) const;
            time_t create() const;
            time_t access() const;
            
        private:
            CBufferMap2 *_map;
            unsigned pos;
        };
        
 
        class const_iterator {
            friend class CBufferMap2;
        public:
            const_iterator(const const_iterator& r):pos(r.pos),_map(r._map) {}
            const_iterator& operator=(const const_iterator& r) {pos=r.pos;_map=r._map; return *this;}
            bool operator==(const const_iterator& r) { return (_map == r._map && pos == r.pos);}
            bool operator!=(const const_iterator& r) { return (_map != r._map || pos != r.pos);}
            const Pair operator*();
            const_iterator& operator++(int)
            {
                if(pos == 0) return *this;
                assert(_map->_indexs[pos]);
                pos = ((Index *)(_map->_indexs[pos]))->post; //
                if(pos == _map->_header->iBeginIndex) {pos = 0; return *this;}
                return *this;
            }
            
        private:
            unsigned pos;
            
        private:
            const_iterator();
            const_iterator(unsigned p,CBufferMap2* list):pos(p),_map(list) {}
            CBufferMap2* _map;
        };
        const_iterator begin() { return const_iterator(_header->iBeginIndex,this);}
        const_iterator end() {return const_iterator(0,this);}
        friend class const_iterator;
        friend class Pair;
        
    private:
        struct Header
        {
            unsigned iKeyNum;
            unsigned iIndexNum;
            unsigned iDataNum;
            unsigned iDataSize;
            unsigned iBeginIndex;
            unsigned iEndIndex;
            long SortType;
            unsigned long iReserve;
            unsigned long iKeyLen;
            unsigned long iIndexLen;
            unsigned long iDataLen;
            unsigned long iTotalLen;
        };
        struct Index
        {
            KEY key;
            //unsigned long nextIndex;
            unsigned pre;
            unsigned post;
            time_t create;
            time_t access;
            unsigned datapos;
            unsigned datalen;
        };
        struct Data
        {
            KEY key;
            unsigned datalen;
            char content[1];
        };
        
    private:
        const static unsigned DataHeadSize = sizeof(KEY) + sizeof(unsigned);
        unsigned hash(const KEY &key);
 
        Index * GetIndex(const KEY &key,unsigned &preidx,unsigned &thisidx);
        
        Header *_header;
        unsigned *_hashkeys;
        CBufferBlock2 _indexs;
        CBufferBlock2 _data;
        char * _data_header;
    };
}

#endif /* shadow_buffer_map2_h */
