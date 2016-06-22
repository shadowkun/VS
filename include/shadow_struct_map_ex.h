//
//  shadow_struct_map_ex.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/16.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_struct_map_ex_h
#define shadow_struct_map_ex_h


#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include "shadow_struct_block.h"
#include "functor.h"


namespace shadow {
    
    class SMapEx
    {
    public:
        SMapEx() {_header = NULL;_bits_key = NULL; _index_header = NULL; _data = NULL;}
        ~SMapEx() {_header = NULL;_bits_key = NULL; _index_header = NULL;_data = NULL;}
        enum SortBy
        {
            By_UpdateTime = 0,
            By_AccessTime
        };
        
        static unsigned long getSize(unsigned keynum,unsigned datanum,unsigned datasize)
        {
            assert(keynum>0 && keynum<0x7FFFFFFF);
            assert(datanum>0 && datanum<0x7FFFFFFF);
            assert(datasize>0);
            return sizeof(Header)+sizeof(unsigned)*keynum*2+SBlock::getSize(keynum, sizeof(Index))+SBlock::getSize(datanum, datasize);
        }
        
        static unsigned long getHeadSize(unsigned keynum,unsigned datanum)
        {
            assert(keynum>0 && keynum<0x7FFFFFFF);
            assert(datanum>0 && datanum<0x7FFFFFFF);
            return sizeof(Header)+sizeof(unsigned)*keynum*2+SBlock::getSize(keynum, sizeof(Index))+SBlock::getHeadSize(datanum);
        }
        
        void attach(char* pBuf, unsigned long iBufSize,unsigned keynum,unsigned datanum,unsigned datasize,int sorttype=By_AccessTime) throw (std::runtime_error)
        {
            assert(keynum>0);
            assert(datanum>0);
            assert(datasize>0);
            if(iBufSize != getSize(keynum,datanum,datasize)) {
                throw std::runtime_error("SMapEx::attach fail: iBufSize!=getSize");
            }
            _header = (Header *)pBuf;
            _bits_key = (unsigned *)(pBuf+sizeof(Header));
            _index_header = pBuf+sizeof(Header)+sizeof(unsigned)*keynum*2;
            _data = pBuf+sizeof(Header)+sizeof(unsigned)*keynum*2+SBlock::getSize(keynum, sizeof(Index));
            
            if(_header->iVersion != VERSION) {
                throw std::runtime_error("SMapEx::attach fail: iVersion!=VERSION");
            }
            if(_header->iIndexNum != keynum) {
                throw std::runtime_error("SMapEx::attach fail: iIndexNum!=keynum");
            }
            if(_header->iDataNum != datanum) {
                throw std::runtime_error("SMapEx::attach fail: iDataNum!=datanum");
            }
            if(_header->iDataSize != datasize) {
                throw std::runtime_error("SMapEx::attach fail: iDataSize!=datasize");
            }
            if(_header->iBeginIndex > datanum) {
                throw std::runtime_error("SMapEx::attach fail: iBeginIndex>datanum");
            }
            if(_header->iEndIndex != datanum) {
                throw std::runtime_error("SMapEx::attach fail: iEndIndex>datanum");
            }
            _header->SortType = sorttype;
            
            _block_index.attach(_index_header, SBlock::getSize(keynum, sizeof(Index)), keynum, sizeof(Index));
            _block_data.attach(_data, SBlock::getSize(datanum, datasize), datanum, datasize);
        }
        
        void create(char* pBuf, unsigned long iBufSize,unsigned keynum,unsigned datanum,unsigned datasize,int sorttype=By_AccessTime) throw (std::runtime_error)
        {
            assert(keynum>0);
            assert(datanum>0);
            assert(datasize>0);
            if(iBufSize != getSize(keynum,datanum,datasize)) {
                throw std::runtime_error("SMapEx::attach fail: iBufSize!=getSize");
            }
            _header = (Header *)pBuf;
            _bits_key = (unsigned *)(pBuf+sizeof(Header));
            _index_header = pBuf+sizeof(Header)+sizeof(unsigned)*keynum*2;
            _data = pBuf+sizeof(Header)+sizeof(unsigned)*keynum*2+SBlock::getSize(keynum, sizeof(Index));
            
            _header->iVersion = VERSION;
            _header->iIndexNum = keynum;
            _header->iDataNum = datanum;
            _header->iDataSize = datasize;
            _header->iBeginIndex = 0;
            _header->iEndIndex = 0;
            _header->SortType = sorttype;
            
            memset(_bits_key,0,sizeof(unsigned)*keynum*2);
            _block_index.create(_index_header, SBlock::getSize(keynum, sizeof(Index)), keynum, sizeof(Index));
            _block_data.create(_data, SBlock::getSize(datanum, datasize), datanum, datasize);
        }
        
        bool find(const std::string &key) {return (GetIndex(key) != NULL);}
        
        bool peek(const std::string &key,char *value,unsigned &valuesize) throw (struct_full)
        {
            Index * index = GetIndex(key);
            if(index) {
                assert(index->keylen<_header->iDataSize);
                if(valuesize<index->datalen) throw struct_full("SMapEx::peek valuesize is too small");
                    valuesize = index->datalen;
                if(index->datapos>0) {
                    assert(index->datalen > 0);
                    std::vector<unsigned> dataposs;
                    _block_data.get(index->datapos,  dataposs);
                    copydata(value,dataposs,index->keylen+1,valuesize);
                }
                return true;
            }
            return false;
        }
        
        bool get(const std::string &key,char *value,unsigned &valuesize) throw (struct_full)
        {
            unsigned hashpos = 0;
            unsigned prepos = 0;
            unsigned pos;
            Index * index = GetIndex(key,hashpos,prepos,pos);
            if(index) {
                assert(index->keylen<_header->iDataSize);
                if(valuesize<index->datalen) throw struct_full("SMapEx::peek valuesize is too small");
                    valuesize = index->datalen;
                if(index->datapos>0) {
                    assert(index->datalen > 0);
                    std::vector<unsigned> dataposs;
                    _block_data.get(index->datapos,  dataposs);
                    copydata(value,dataposs,index->keylen+1,valuesize);
                }
                if(_header->SortType == By_AccessTime)
                    updateindex(pos);
                    return true;
            }
            return false;
        }
        
        /**
         * @return true-insert succ false-exist
         */
        bool insert_ignore(const std::string &key,char *value,unsigned valuesize) throw (struct_full)
        {
            unsigned hashpos = 0;
            unsigned prepos = 0;
            unsigned datapos = 0;
            Index * index = GetIndex(key,hashpos,prepos,datapos);
            if(index == NULL) {
                if(_block_index.full()) throw struct_full("SMapEx::insert_ignore can not alloc more index");
                    unsigned realsize = key.length()+1+valuesize;
                unsigned datanum = realsize%_header->iDataSize == 0?realsize/_header->iDataSize:realsize/_header->iDataSize+1;
                if(datanum > _block_data.idle()) throw struct_full("SMapEx::insert_ignore can not alloc more data");
                    std::vector<unsigned> dataposs;
                _block_data.malloc(datanum,dataposs);
                assert(dataposs.size() == datanum);
                datapos = dataposs[0];
                copydata(dataposs,key,value,valuesize);
                
                unsigned indexpos = _block_index.alloc();
                index = (Index *)_block_index[indexpos];
                index->datapos = datapos;
                index->keylen = key.length();
                index->datalen = valuesize;
                index->next = 0;
                insertindex(hashpos,prepos,indexpos);
                return true;
            }
            return false;
        }
        
        void insert_replace(const std::string &key,char *value,unsigned valuesize) throw (struct_full)
        {
            unsigned hashpos = 0;
            unsigned prepos = 0;
            unsigned datapos = 0;
            unsigned indexpos = 0;
            Index * index = GetIndex(key,hashpos,prepos,indexpos);
            if(index == NULL) {
                insert_ignore(key,value,valuesize);
                return;
            }
            if(_header->SortType == By_UpdateTime) updateindex(indexpos);
                unsigned realsize = key.length()+1+valuesize;
            unsigned datanum = realsize%_header->iDataSize == 0?realsize/_header->iDataSize:realsize/_header->iDataSize+1;
            if(datanum > _block_data.idle()) throw struct_full("SMapEx::insert_replace can not alloc more data");
                std::vector<unsigned> dataposs;
            _block_data.realloc(index->datapos,datanum,dataposs);
            assert(dataposs.size() == datanum);
            copydata(dataposs,key,value,valuesize);
            index->datalen = valuesize;
            index->datapos = dataposs[0];
        }
        
        bool erase(const std::string &key)
        {
            unsigned hashpos = 0;
            unsigned prepos = 0;
            unsigned pos = 0;
            Index * index = GetIndex(key,hashpos,prepos,pos);
            if(index) {
                _block_data.free(index->datapos);
                deleteindex(hashpos,prepos,pos);
                _block_index.free(pos);
                return true;
            }
            return false;
        }
        
    public:
        unsigned totalIndex() const {return _header->iIndexNum;}
        unsigned totalData() const {return _header->iDataNum;}
        unsigned usedIndex() const {return _block_index.used();}
        unsigned usedData() const {return _block_data.used();}
        
        bool empty() const {return _block_index.empty();}
        bool full() const {return _block_index.full() || _block_data.full();}
        
        unsigned recycleByTime(unsigned timeout)
        {
            assert(timeout>0);
            time_t tnow = time(0);
            unsigned deled = 0;
            for(unsigned i=0;i<_header->iIndexNum;i++) {
                if(_header->iBeginIndex == 0) {
                    assert(_header->iEndIndex == 0);
                    assert(_block_index.empty());
                    assert(_block_data.empty());
                    return deled;
                }
                Index *index = (Index *)_block_index[_header->iBeginIndex];
                if(tnow-(time_t)index->create < (time_t)timeout) break;
                erase(_block_data[index->datapos]);
                deled++;
            }
            
            return deled;
        }
        unsigned recycleByNum(unsigned num)
        {
            assert(num>0 && num<=_header->iIndexNum);
            unsigned deled = 0;
            while(deled < num) {
                if(_header->iBeginIndex == 0) {
                    assert(_header->iEndIndex == 0);
                    assert(_block_index.empty());
                    assert(_block_data.empty());
                    return deled;
                }
                Index *index = (Index *)_block_index[_header->iBeginIndex];
                assert(index->datapos);
                erase(_block_data[index->datapos]);
                deled++;
            }
            
            return deled;
        }
    public:
        class const_iterator;
        class Pair
        {
            friend class SMapEx::const_iterator;
        public:
            std::string key() const {
                if(pos == 0) return "";
                SMapEx::Index * index = (SMapEx::Index *)_map->_block_index[pos];
                return index->key;
            }
            void value(char *v,unsigned &vsize) const {
                if(pos == 0) return NULL;
                SMapEx::Index * index = (SMapEx::Index *)_map->_block_index[pos];
                _map->peek(index->key, v, vsize);
            }
            time_t create() const {
                if(pos == 0) return 0;
                SMapEx::Index * index = (SMapEx::Index *)_map->_block_index[pos];
                return index->create;
            }
            
        private:
            SMapEx *_map;
            unsigned pos;
        };
        
        class const_iterator {
            friend class CBufferMap;
        public:
            const_iterator(const const_iterator& r):pos(r.pos),_map(r._map) {}
            const_iterator& operator=(const const_iterator& r) {pos=r.pos;_map=r._map; return *this;}
            bool operator==(const const_iterator& r) { return (_map == r._map && pos == r.pos);}
            bool operator!=(const const_iterator& r) { return (_map != r._map || pos != r.pos);}
            const Pair operator*() {
                SMapEx::Pair p;
                p._map = _map;
                p.pos = pos;
                return p;
            }
            const_iterator& operator++(int)
            {
                if(pos == 0) return *this;
                assert(_map->_indexs[pos]);
                pos = ((Index *)(_map->_block_index[pos]))->next; //
                if(pos == _map->_header->iBeginIndex) {pos = 0; return *this;}
                return *this;
            }
            
        private:
            unsigned pos;
            
        private:
            const_iterator();
            const_iterator(unsigned p,SMapEx *list):pos(p),_map(list) {}
            SMapEx *_map;
        };
        const_iterator begin() { return const_iterator(_header->iBeginIndex,this);}
        const_iterator end() {return const_iterator(0,this);}
        friend class const_iterator;
        friend class Pair;
        
    private:
        void copydata(char *value,std::vector<unsigned> &datapos,unsigned offset,unsigned valuesize)
        {
            unsigned num = (offset+valuesize)%_header->iDataSize==0?(offset+valuesize)/_header->iDataSize:(offset+valuesize)/_header->iDataSize+1;
            unsigned len = 0;
            unsigned thislen = 0;
            assert(datapos.size() == num);
            
            for(unsigned i=0;i<num;i++) {
                if(i == 0) {
                    if(i == num-1) {
                        thislen = valuesize;
                    } else {
                        thislen = _header->iDataSize-offset;
                    }
                    memcpy(value,_block_data[datapos[i]]+offset,thislen);
                } else {
                    if(i == num-1) {
                        thislen = valuesize-len;
                    } else {
                        thislen = _header->iDataSize;
                    }
                    memcpy(value+len,_block_data[datapos[i]],thislen);
                }
            }
        }
        
        void copydata(std::vector<unsigned> &datapos,const std::string& key,char *value,unsigned valuesize)
        {
            unsigned num = (key.length()+1+valuesize)%_header->iDataSize==0?(key.length()+1+valuesize)/_header->iDataSize:(key.length()+1+valuesize)/_header->iDataSize+1;
            unsigned len = key.length();
            unsigned thislen = 0;
            assert(datapos.size() == num);
            
            memcpy(_block_data[datapos[0]],key.c_str(),len);
            _block_data[datapos[0]][len] = '\0';
            len++;
            
            for(unsigned i=0;i<num;i++) {
                if(i == 0) {
                    if(i == num-1) {
                        thislen = valuesize;
                    } else {
                        thislen = _header->iDataSize;
                    }
                    memcpy(_block_data[datapos[i]]+len,value,thislen);
                } else {
                    if(i == num-1) {
                        thislen = valuesize+key.length()+1-len;
                    } else {
                        thislen = _header->iDataSize;
                    }
                    memcpy(_block_data[datapos[i]],value+len-key.length()-1,thislen);
                }
            }
        }
        
        unsigned hash(const std::string &key)
        {
            register unsigned h;
            register const char *p = key.c_str();
            for(h=0; *p ; p++)
                h = 31 * h + (unsigned char)(*p);
            
            return h % (_header->iIndexNum*2);
        }
        
    private:
        struct Header
        {
            unsigned iVersion;
            unsigned iIndexNum;
            unsigned iDataNum;
            unsigned iDataSize;
            unsigned iBeginIndex;
            unsigned iEndIndex;
            int SortType;
        };
        
        struct Index
        {
            unsigned next;
            unsigned pre;
            unsigned post;
            unsigned datapos;
            unsigned datalen;
            unsigned keylen;
            time_t create;
            //KEY key;			//
        };
        
    private:
        Index * GetIndex(const std::string &key)
        {
            //hash<std::string> h;
            unsigned pos = _bits_key[hash(key)%(_header->iIndexNum*2)];
            if(pos == 0) return NULL;
            Index * index = (Index *)_block_index[pos];
            unsigned i;
            for(i=0;i<_header->iIndexNum;i++) {
                //if(index->keylen != key.length()) continue;
                if(key == std::string(_block_data[index->datapos])) break;
                if(index->next == 0) {
                    return NULL;
                }
                index = (Index *)_block_index[index->next];
            }
            assert(i<_header->iIndexNum);
            return index;
        }
        
        Index * GetIndex(const std::string &key,unsigned &hashpos,unsigned &pre,unsigned &pos)
        {
            //hash<std::string> h;
            pre = 0;
            hashpos = hash(key)%(_header->iIndexNum*2);
            pos = _bits_key[hashpos];
            if(pos == 0) return NULL;
            Index * index = (Index *)_block_index[pos];
            unsigned i;
            for(i=0;i<_header->iIndexNum;i++) {
                if(key == std::string(_block_data[index->datapos])) 
                    break;
                pre = pos;
                if(index->next == 0) {
                    pos = 0;
                    return NULL;
                }
                pos = index->next;
                index = (Index *)_block_index[pos];
            }
            assert(i<_header->iIndexNum);
            return index;
        }
        
        void insertindex(unsigned hashpos,unsigned pre,unsigned newpos)
        {
            //hash<std::string> h;
            Index * index = NULL;
            unsigned pos = _bits_key[hashpos];
            if(pos >0) {
                index = (Index *)_block_index[pre];
                assert(index->next == 0);
                index->next = newpos;
            } else {
                _bits_key[hashpos] = newpos;
            }
            pos = newpos;
            index = (Index *)_block_index[pos];
            
            index->create = time(0);
            index->next = 0;
            if(_header->iBeginIndex == 0) {
                assert(_header->iEndIndex == 0);
                assert(_block_index.used() == 1);
                _header->iBeginIndex = pos;
                _header->iEndIndex = pos;
                index->pre = pos;
                index->post = pos;
            } else if(_header->iBeginIndex == _header->iEndIndex) {
                assert(_block_index.used() ==2);
                assert(_header->iBeginIndex != pos);
                Index * index2 = (Index *)_block_index[_header->iBeginIndex];
                assert(index2->pre == _header->iBeginIndex);
                assert(index2->post == _header->iBeginIndex);
                index2->pre = pos;
                index2->post = pos;
                index->pre = _header->iBeginIndex;
                index->post = _header->iBeginIndex;
                _header->iEndIndex = pos;
            }
            else {
                assert(_block_index.used()>2);
                assert(_header->iBeginIndex != _header->iEndIndex);
                Index * indexHead = (Index *)_block_index[_header->iBeginIndex];
                Index * indexTail = (Index *)_block_index[_header->iEndIndex];
                indexHead->pre = pos;
                indexTail->post = pos;
                index->pre = _header->iEndIndex;
                index->post = _header->iBeginIndex;
                _header->iEndIndex = pos;
            }
        }
        
        void updateindex(unsigned pos)
        {
            Index * index = (Index *)_block_index[pos];
            
            index->create = time(0);
            assert(_header->iBeginIndex>0);
            assert(_header->iEndIndex>0);
            if(_header->iEndIndex == pos) return;
            if(index->pre == pos) {
                assert(index->post == pos);
                assert(_block_index.used() == 1);
                assert(_header->iBeginIndex == pos && _header->iEndIndex == pos);
            } else if(index->pre == index->post) {
                assert(_block_index.used() == 2);
                _header->iBeginIndex = index->pre;
                _header->iEndIndex = pos;
            } else {
                assert(_block_index.used()>2);
                if(_header->iBeginIndex == pos) {
                    _header->iBeginIndex = index->post;
                    _header->iEndIndex = pos;
                } else {
                    Index * indexPre = (Index *)_block_index[index->pre];
                    Index * indexPost = (Index *)_block_index[index->post];
                    Index * indexHead = (Index *)_block_index[_header->iBeginIndex];
                    Index * indexTail = (Index *)_block_index[_header->iEndIndex];
                    indexPre->post = index->post;
                    indexPost->pre = index->pre;
                    indexHead->pre = pos;
                    indexTail->post = pos;
                    index->pre = _header->iEndIndex;
                    index->post = _header->iBeginIndex;
                    _header->iEndIndex = pos;
                }
            }
        }
        
        void deleteindex(unsigned hashpos,unsigned pre,unsigned destpos)
        {
            //hash<std::string> h;
            unsigned pos = _bits_key[hashpos];
            Index * index = (Index *)_block_index[destpos];
            if(pos == destpos) {
                _bits_key[hashpos] = index->next;
            } else {
                Index *index2 = (Index *)_block_index[pre];
                index2->next = index->next;
            }
            
            assert(_header->iBeginIndex>0);
            assert(_header->iEndIndex>0);
            if(index->pre == pos) {
                assert(index->post == pos);
                assert(_block_index.used() == 1);
                assert(_header->iBeginIndex == pos && _header->iEndIndex == pos);
                _header->iBeginIndex = 0;
                _header->iEndIndex = 0;
            } else if(index->pre == index->post) {
                assert(_block_index.used() == 2);
                Index * indexPre = (Index *)_block_index[index->pre];
                indexPre->post = index->pre;
                indexPre->pre = index->pre;
                
                _header->iBeginIndex = index->pre;
                _header->iEndIndex = index->pre;
            } else {
                assert(_block_index.used()>2);
                Index * indexPre = (Index *)_block_index[index->pre];
                Index * indexPost = (Index *)_block_index[index->post];
                indexPre->post = index->post;
                indexPost->pre = index->pre;
                if(_header->iBeginIndex == pos) {
                    _header->iBeginIndex = index->post;
                }
                if(_header->iEndIndex == pos) {
                    _header->iEndIndex = index->pre;
                }
            }
        }
        
        const static unsigned VERSION = 0x0101;
        Header *_header;
        unsigned * _bits_key;
        char * _index_header;
        char * _data;
        SBlock _block_index;
        SBlock _block_data;
    };
    
}

#endif /* shadow_struct_map_ex_h */
