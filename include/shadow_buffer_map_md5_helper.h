//
//  shadow_buffer_map_md5_helper.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_buffer_map_md5_helper_h
#define shadow_buffer_map_md5_helper_h

#include "shadow_buffer_block.h"
#include <vector>
#include <sys/time.h>

namespace shadow {
    
 
    class CBufferMapMd5Helper
    {
    public:
        CBufferMapMd5Helper() {_header = NULL; _hashkeys = NULL;}
        ~CBufferMapMd5Helper() {_header = NULL; _hashkeys = NULL;}
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
            bool SetString(const std::string &k); // assert(k.length()>=32);
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
            bool SetString(const std::string &k); // assert(k.length()>=32);
            
            unsigned long v;
            unsigned long v2;
            unsigned long v3;
            unsigned long v4;
            
            bool operator==(const KEY& r) {return r.v == v && r.v2 == v2 && r.v2 == v3 && r.v4 == v4;}
            bool operator!=(const KEY& r){return r.v != v || r.v2 != v2 || r.v3 != v3 || r.v4 != v4;}
#endif
            std::string str() const;
        };
        static KEY NULLKEY;
        
 
        void attach(char* pBuf, unsigned long iBufSize) throw (std::runtime_error);
        
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
        void GetHash(const KEY &key);
        void ShowMem(unsigned long pos,unsigned len);
        void ShowMemX(unsigned long pos);
        
        void ShowHead();
        void ShowHash(unsigned long pos);
        void ShowIndex(unsigned long pos);
        void ShowData(unsigned long pos);
        void Show(const KEY &key);
        void Show(const std::string &key);
        
        void ScanHash();
        void ScanIndex();
        void ScanData();
        
        void SaveData();
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
            KEY key;
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
        
        struct Block_Header
        {
            unsigned long iBufSize;
            unsigned long iDataSize;
            unsigned long iDataNum;
            unsigned long iUseNum;
            unsigned long iIdleBegin;
            unsigned long iIdleEnd;
        };
        struct Block_Data
        {
            unsigned long prefix;
            unsigned long next;
        };
        
    private:
        unsigned long hash(const KEY &key);
 
        Index * GetIndex(const KEY &key,unsigned long &preidx,unsigned long &thisidx);
        
        Header *_header;
        unsigned long *_hashkeys;
        CBufferBlock _indexs;
        CBufferBlock _data;
        char * _index_header;
        char * _data_header;
    };
}

#endif /* shadow_buffer_map_md5_helper_h */
