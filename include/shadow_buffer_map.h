//
//  shadow_buffer_map.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_map_h
#define shadow_buffer_map_h


#include "shadow_buffer_block.h"
#include <vector>
#include <sys/time.h>

namespace shadow {
    
 
    class CBufferMap
    {
    public:
        CBufferMap() {_header = NULL; _hashkeys = NULL;}
        ~CBufferMap() {_header = NULL; _hashkeys = NULL;}
        enum SortBy
        {
            By_CreateTime = 0,
            By_AccessTime
        };
        
 
        void attach(char* pBuf, unsigned long iBufSize,unsigned long keynum,unsigned long indexnum,unsigned long datanum,unsigned long datasize,SortBy sorttype=By_CreateTime) throw (std::runtime_error);
        void create(char* pBuf, unsigned long iBufSize,unsigned long keynum,unsigned long indexnum,unsigned long datanum,unsigned long datasize,SortBy sorttype=By_CreateTime) throw (std::runtime_error); // attach and init
        
        static unsigned long  getSize(unsigned long keynum,unsigned long indexnum,unsigned long datanum,unsigned long datasize)
        {
            return sizeof(Header)
            +keynum*sizeof(unsigned long)
            +CBufferBlock::getSize(indexnum,sizeof(Index))
            +CBufferBlock::getSize(datanum,datasize+sizeof(unsigned long)*2);
        }
        
        unsigned long size_head() const {return sizeof(Header);}
        unsigned long size_hash() const {return _header->iKeyLen;}
        unsigned long size_index() const {return _header->iIndexLen;}
        unsigned long size_data() const {return _header->iDataLen;}
    public:
 
        bool get(unsigned long key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
 
        bool get(unsigned long key,time_t &create,std::vector<char> &value);
 
        bool peek(unsigned long key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
        bool peek(unsigned long key,time_t &create,std::vector<char> &value);
 
        bool touch(unsigned long key,time_t &create);
 
        bool find(unsigned long key);
 
        bool dequeue(unsigned long &key,time_t &create,char *value,unsigned &valuesize) throw (buffer_full);
        bool dequeue(unsigned long &key,time_t &create,std::vector<char> &value);
        
 
        void set(unsigned long key,char *value,unsigned valuesize) throw (buffer_full);
        
 
        void set_secret(unsigned long key,char *value,unsigned valuesize) throw (buffer_full);
        
 
        bool erase(unsigned long key);
        
    public:
        unsigned long totalIndex() const {return _header->iIndexNum;}
        unsigned long totalData() const {return _header->iDataNum;}
        unsigned long usedIndex() const {return _indexs.used();}
        unsigned long usedData() const {return _data.used();}
        
        bool isEmpty() const {assert(_indexs.isEmpty() == _data.isEmpty()); return _indexs.isEmpty();}
        bool isFull() const {return _indexs.isFull() || _data.isFull();}
    public:
 
        unsigned long recycleByTime(unsigned long timeout);
 
        unsigned long recycleByNum(unsigned long num);
        
    public:
        class const_iterator;
        class Pair
        {
            friend class CBufferMap::const_iterator;
        public:
            unsigned long key() const;
            void value(std::vector<char> &value) const;
            time_t create() const;
            time_t access() const;
            
        private:
            CBufferMap *_map;
            unsigned long pos;
        };
        
 
        class const_iterator {
            friend class CBufferMap;
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
            unsigned long pos;
            
        private:
            const_iterator();
            const_iterator(unsigned long p,CBufferMap* list):pos(p),_map(list) {}
            CBufferMap* _map;
        };
        const_iterator begin() { return const_iterator(_header->iBeginIndex,this);}
        const_iterator end() {return const_iterator(0,this);}
        friend class const_iterator;
        friend class Pair;
        
    private:
        const static unsigned long MAX_BLOCK_ONCE = 8192;
        struct Header
        {
            unsigned long iKeyNum;
            unsigned long iIndexNum;
            unsigned long iDataNum;
            unsigned long iDataSize;
            unsigned long iBeginIndex;
            unsigned long iEndIndex;
            long SortType;
            unsigned long iReserve;
            unsigned long iKeyLen;
            unsigned long iIndexLen;
            unsigned long iDataLen;
            unsigned long iTotalLen;
        };
        struct Index
        {
            unsigned long key;
            unsigned long nextIndex;
            unsigned long pre;
            unsigned long post;
            time_t create;		// 
            time_t access;
            unsigned long datapos;
        };
        struct Data
        {
            //unsigned short total;
            //unsigned short index;
            unsigned long next;
            unsigned long len;
            char content[1];
        };
        
    private:
        unsigned long hash(unsigned long key);
 
        Index * GetIndex(unsigned long key,unsigned long &preidx,unsigned long &thisidx);
        void Check(int line);
        
        Header *_header;
        unsigned long *_hashkeys;
        CBufferBlock _indexs;
        CBufferBlock _data;
    };
}

#endif /* shadow_buffer_map_h */
