//
//  shadow_struct_queue.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/16.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_struct_queue_h
#define shadow_struct_queue_h



#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include "shadow_struct_block.h"



namespace shadow {
    
    class SQueue
    {
    public:
        SQueue() {_header = NULL;_data = NULL;}
        ~SQueue() {_header = NULL;_data = NULL;}
        
        void attach(char* pBuf, unsigned long iBufSize) throw (std::runtime_error)
        {
            if(iBufSize <= sizeof(Header)+MarkLen+ReserveLen) {
                throw std::runtime_error("SQueue::attach fail:iBufSize is too small");
            }
            
            _header = (Header *)pBuf;
            _data = pBuf+sizeof(Header);
            if(_header->iBufSize != iBufSize - sizeof(Header))
                throw std::runtime_error("SQueue::attach fail: iBufSize != iBufSize - sizeof(Header);");
                if(_header->iReserveLen != ReserveLen)
                    throw std::runtime_error("SQueue::attach fail: iReserveLen != ReserveLen");
                    if(_header->iBegin >= _header->iBufSize)
                        throw std::runtime_error("SQueue::attach fail: iBegin > iBufSize - sizeof(Header);");
                        if(_header->iEnd > iBufSize - sizeof(Header))
                            throw std::runtime_error("SQueue::attach fail: iEnd > iBufSize - sizeof(Header);");
                            }
        
        void create(char* pBuf, unsigned long iBufSize) throw (std::runtime_error)
        {
            if(iBufSize <= sizeof(Header)+MarkLen+ReserveLen) {
                throw std::runtime_error("SQueue::create fail:iBufSize is too small");
            }
            
            _header = (Header *)pBuf;
            _data = pBuf+sizeof(Header);
            _header->iBufSize = iBufSize - sizeof(Header);
            _header->iReserveLen = ReserveLen;
            _header->iBegin = 0;
            _header->iEnd = 0;
        }
        

        bool dequeue(char *buffer,unsigned long & buffersize) throw(struct_full)
        {
            unsigned long iEnd=_header->iEnd;
            
            if(_header->iBegin == iEnd)
                return false;
            else if(_header->iBegin<iEnd) {
                assert(_header->iBegin+MarkLen < iEnd);
                unsigned long len = GetLen(_data+_header->iBegin);
                assert(_header->iBegin+MarkLen+len <= iEnd);
                if(len > buffersize) {
                    _header->iBegin += len+MarkLen;
                    throw struct_full("SQueue::dequeue data is too long to store in the buffer");
                }
                buffersize = len;
                memcpy(buffer,_data+_header->iBegin+MarkLen,len);
                _header->iBegin += len+MarkLen;
            } else {

                assert(iEnd+ReserveLen <= _header->iBegin);
                unsigned long len = 0;
                unsigned long new_begin = 0;
                char *data_from = NULL;
                char *data_to = NULL;
                assert(_header->iBegin < _header->iBufSize);

                if(_header->iBegin+MarkLen > _header->iBufSize) {
                    char tmp[16];
                    memcpy(tmp,_data+_header->iBegin,_header->iBufSize-_header->iBegin);
                    memcpy(tmp+_header->iBufSize-_header->iBegin,_data,_header->iBegin+MarkLen-_header->iBufSize);
                    len = GetLen(tmp);
                    data_from = _data+(_header->iBegin+MarkLen-_header->iBufSize); //
                    new_begin = _header->iBegin+MarkLen-_header->iBufSize+len;
                    assert(new_begin <= iEnd);
                } else {
                    len = GetLen(_data+_header->iBegin);
                    data_from = _data+_header->iBegin+MarkLen;
                    if(data_from == _data+_header->iBufSize) data_from = _data;
                    if(_header->iBegin+MarkLen+len < _header->iBufSize) {
                        new_begin = _header->iBegin+MarkLen+len;
                    } else {
                        new_begin = _header->iBegin+MarkLen+len-_header->iBufSize;
                        assert(new_begin <= iEnd);
                    }
                }
                data_to = _data+new_begin;
                
                if(len > buffersize) {
                    _header->iBegin = new_begin;
                    throw struct_full("SQueue::dequeue data is too long to store in the buffer");
                }
                buffersize = len;
                if(data_to > data_from) {
                    assert(data_to - data_from == (long)len);
                    memcpy(buffer,data_from,len);
                } else {
                    memcpy(buffer,data_from,_data+_header->iBufSize-data_from);
                    memcpy(buffer+(_data+_header->iBufSize-data_from),_data,data_to-_data);
                    assert(_header->iBufSize-(data_from-data_to)== len);
                }
                _header->iBegin = new_begin;
            }
            
            return true;
        }
        

        void enqueue(const char *buffer,unsigned long len) throw(struct_full)
        {
            if(len == 0) return;
            
            unsigned long iBegin = _header->iBegin;
            if(_header->iEnd == iBegin) {
                if(MarkLen+len+ReserveLen>_header->iBufSize)
                    throw struct_full("SQueue::enqueue full");
                    } else if(_header->iEnd > iBegin) {
                        assert(iBegin+MarkLen < _header->iEnd);
                        if(_header->iBufSize - _header->iEnd + iBegin < MarkLen+len+ReserveLen)
                            throw struct_full("SQueue::enqueue full");
                            } else {
                                assert(_header->iEnd+ReserveLen <= iBegin);
                                if(iBegin - _header->iEnd < MarkLen+len+ReserveLen)
                                    throw struct_full("SQueue::enqueue full");
                                    }
            
            
            if(_header->iEnd+MarkLen > _header->iBufSize) {
                char tmp[16]; SetLen(tmp,len);
                memcpy(_data+_header->iEnd,tmp,_header->iBufSize-_header->iEnd);
                memcpy(_data,tmp+_header->iBufSize-_header->iEnd,_header->iEnd+MarkLen-_header->iBufSize);
                memcpy(_data+_header->iEnd+MarkLen-_header->iBufSize,buffer,len);
                _header->iEnd = len+_header->iEnd+MarkLen-_header->iBufSize;
                assert(_header->iEnd+ReserveLen <= iBegin);
            }
            
            else if(_header->iEnd+MarkLen+len > _header->iBufSize){
                SetLen(_data+_header->iEnd,len);
                memcpy(_data+_header->iEnd+MarkLen,buffer,_header->iBufSize-_header->iEnd-MarkLen);
                memcpy(_data,buffer+_header->iBufSize-_header->iEnd-MarkLen,len-(_header->iBufSize-_header->iEnd-MarkLen));
                _header->iEnd = len-(_header->iBufSize-_header->iEnd-MarkLen);
                assert(_header->iEnd+ReserveLen <= iBegin);
            } else {
                SetLen(_data+_header->iEnd,len);
                memcpy(_data+_header->iEnd+MarkLen,buffer,len);
                _header->iEnd = (_header->iEnd+MarkLen+len)%_header->iBufSize;
            }
        }
        
        bool dequeue(char *buffer1,unsigned long &buffersize1,char *buffer2,unsigned long &buffersize2) throw(struct_full)
        {
            unsigned long buffersize = buffersize1+buffersize2;
            unsigned long iEnd=_header->iEnd;
            
            if(_header->iBegin == iEnd)
                return false;
            else if(_header->iBegin < iEnd) {
                assert(_header->iBegin+MarkLen < iEnd);
                unsigned long len = GetLen(_data+_header->iBegin);
                assert(_header->iBegin+MarkLen+len <= iEnd);
                if(len > buffersize) {
                    _header->iBegin += len+MarkLen;
                    throw struct_full("SQueue::dequeue data is too long to store in the buffer");
                }
                if(buffersize1 > len) {
                    buffersize1 = len;
                    buffersize2 = 0;
                    memcpy(buffer1,_data+_header->iBegin+MarkLen,len);
                } else {
                    buffersize2 = len-buffersize1;
                    memcpy(buffer1,_data+_header->iBegin+MarkLen,buffersize1);
                    memcpy(buffer2,_data+_header->iBegin+MarkLen+buffersize1,buffersize2);
                }
                _header->iBegin += len+MarkLen;
            } else {
                
                assert(iEnd+ReserveLen <= _header->iBegin);
                unsigned long len = 0;
                unsigned long new_begin = 0;
                char *data_from = NULL;
                char *data_to = NULL;
                assert(_header->iBegin < _header->iBufSize);
                
                if(_header->iBegin+MarkLen > _header->iBufSize) {
                    char tmp[16];
                    memcpy(tmp,_data+_header->iBegin,_header->iBufSize-_header->iBegin);
                    memcpy(tmp+_header->iBufSize-_header->iBegin,_data,_header->iBegin+MarkLen-_header->iBufSize);
                    len = GetLen(tmp);
                    data_from = _data+(_header->iBegin+MarkLen-_header->iBufSize);
                    new_begin = (_header->iBegin+MarkLen-_header->iBufSize)+len;
                    assert(new_begin <= iEnd);
                } else {
                    len = GetLen(_data+_header->iBegin);
                    data_from = _data+_header->iBegin+MarkLen;
                    if(data_from == _data+_header->iBufSize) data_from = _data;
                    if(_header->iBegin+MarkLen+len < _header->iBufSize) {
                        new_begin = _header->iBegin+MarkLen+len;
                    } else {
                        new_begin = _header->iBegin+MarkLen+len-_header->iBufSize;
                        assert(new_begin <= iEnd);
                    }
                }
                data_to = _data+new_begin;
                
                if(len > buffersize) {
                    _header->iBegin = new_begin;
                    throw struct_full("SQueue::dequeue data is too long to store in the buffer");
                }
                buffersize = len;
                if(data_to > data_from) {
                    assert(data_to - data_from == (long)len);
                    if(buffersize1 > len) {
                        buffersize1 = len;
                        buffersize2 = 0;
                        memcpy(buffer1,data_from,len);
                    } else {
                        buffersize2 = len-buffersize1;
                        memcpy(buffer1,data_from,buffersize1);
                        memcpy(buffer2,data_from+buffersize1,buffersize2);
                    }
                } else {
                    assert(_header->iBufSize-(data_from-data_to)== len);
                    if(buffersize1>len) {
                        buffersize1 = len;
                        buffersize2 = 0;
                        memcpy(buffer1,data_from,_data+_header->iBufSize-data_from);
                        memcpy(buffer1+(_data+_header->iBufSize-data_from),_data,data_to-_data);
                    } else if(buffersize1>_data-data_from+_header->iBufSize) {
                        buffersize2 = len-buffersize1;
                        memcpy(buffer1,data_from,_data+_header->iBufSize-data_from);
                        memcpy(buffer1+(_data+_header->iBufSize-data_from),_data,buffersize1-(_data+_header->iBufSize-data_from));
                        memcpy(buffer2,data_from+buffersize1-_header->iBufSize,buffersize2);
                    } else {
                        buffersize2 = len-buffersize1;
                        memcpy(buffer1,data_from,buffersize1);
                        memcpy(buffer2,data_from+buffersize1,_data+_header->iBufSize-data_from-buffersize1);
                        memcpy(buffer2+(_data+_header->iBufSize-data_from-buffersize1),_data,len-(_data-data_from+_header->iBufSize));
                    }
                }
                _header->iBegin = new_begin;
            }
            
            return true;
        }
        
        void enqueue(const char *buffer1,unsigned len1,const char *buffer2,unsigned len2) throw(struct_full)
        {
            unsigned long len = len1+len2;
            if(len == 0) return;
            unsigned long iBegin = _header->iBegin;
            if(_header->iEnd == iBegin) {
                if(MarkLen+len+ReserveLen>_header->iBufSize) 
                    throw struct_full("SQueue::enqueue full");
                    } else if(_header->iEnd > iBegin) {
                        assert(iBegin+MarkLen < _header->iEnd);
                        if(_header->iBufSize - _header->iEnd + iBegin < MarkLen+len+ReserveLen)
                            throw struct_full("SQueue::enqueue full");
                            } else {
                                assert(_header->iEnd+ReserveLen <= iBegin);
                                if(iBegin - _header->iEnd < MarkLen+len+ReserveLen)
                                    throw struct_full("SQueue::enqueue full");
                                    }
            
            
            
            if(_header->iEnd+MarkLen > _header->iBufSize) {
                char tmp[16]; SetLen(tmp,len);
                memcpy(_data+_header->iEnd,tmp,_header->iBufSize-_header->iEnd);
                memcpy(_data,tmp+_header->iBufSize-_header->iEnd,_header->iEnd+MarkLen-_header->iBufSize);
                memcpy(_data+_header->iEnd+MarkLen-_header->iBufSize,buffer1,len1);
                memcpy(_data+_header->iEnd+MarkLen-_header->iBufSize+len1,buffer2,len2);
                _header->iEnd = len+_header->iEnd+MarkLen-_header->iBufSize;
                assert(_header->iEnd+ReserveLen <= iBegin);
            } 

            
            else if(_header->iEnd+MarkLen+len > _header->iBufSize){
                SetLen(_data+_header->iEnd,len);
                if(_header->iEnd+MarkLen+len1>_header->iBufSize) { //buffer1±ª∑÷∂Œ
                    memcpy(_data+_header->iEnd+MarkLen,buffer1,_header->iBufSize-_header->iEnd-MarkLen);
                    memcpy(_data,buffer1+_header->iBufSize-_header->iEnd-MarkLen,len1-(_header->iBufSize-_header->iEnd-MarkLen));
                    memcpy(_data+len1-(_header->iBufSize-_header->iEnd-MarkLen),buffer2,len2);
                } else {
                    memcpy(_data+_header->iEnd+MarkLen,buffer1,len1);
                    memcpy(_data+_header->iEnd+MarkLen+len1,buffer2,_header->iBufSize-_header->iEnd-MarkLen-len1);
                    memcpy(_data,buffer2+_header->iBufSize-_header->iEnd-MarkLen-len1,len2-(_header->iBufSize-_header->iEnd-MarkLen-len1));
                }
                _header->iEnd = len-(_header->iBufSize-_header->iEnd-MarkLen);
                assert(_header->iEnd+ReserveLen <= iBegin);
            } else {
                SetLen(_data+_header->iEnd,len);
                memcpy(_data+_header->iEnd+MarkLen,buffer1,len1);
                memcpy(_data+_header->iEnd+MarkLen+len1,buffer2,len2);
                _header->iEnd = (_header->iEnd+MarkLen+len)%_header->iBufSize;
            }	
        }

        bool isEmpty() const {unsigned long iEnd=_header->iEnd;return _header->iBegin == iEnd;}

        bool isFull(unsigned long len) const
        {
            unsigned long iBegin = _header->iBegin;
            if(len==0) return false;
            
            if(_header->iEnd == iBegin) {
                if(len+MarkLen+ReserveLen > _header->iBufSize) return true;
                return false;
            } else if(_header->iEnd > iBegin) {
                assert(iBegin+MarkLen < _header->iEnd);
                return _header->iBufSize - _header->iEnd + iBegin < MarkLen+len+ReserveLen;
            }
            assert(_header->iEnd+ReserveLen <= iBegin);
            return (iBegin - _header->iEnd < MarkLen+len+ReserveLen);
        }
        
    private:
        unsigned long GetLen(char *buf) {unsigned long u; memcpy((void *)&u,buf,MarkLen); return u;}
        void SetLen(char *buf,unsigned long u) {memcpy(buf,(void *)&u,MarkLen);}
        
    private:
        const static unsigned long ReserveLen = 8;
        const static unsigned long MarkLen = sizeof(unsigned long);
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

#endif /* shadow_struct_queue_h */
