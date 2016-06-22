//
//  netinet.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/3.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef netinet_util_h
#define netinet_util_h

#include <string>
#include <stdio.h>
#include <netinet/in.h>

namespace shadow
{
    struct my_in_addr {
        union {
            struct {
                unsigned char s_b1,
                s_b2,
                s_b3,
                s_b4;
            } S_un_b;
            struct {
                unsigned short s_w1,
                s_w2;
            } S_un_w;
            unsigned long S_addr;
        } S_un;
    };
    
    /** inet_ntoa's thread safe version
     *  \param ip	src ip address
     *  \return		string ip
     */
    std::string inet_ntoa_safe(unsigned int ip);
    
    /** inet_ntoa's thread safe version
     *  \param in	src ip address
     *  \return		string ip
     */
    std::string inet_ntoa_safe(struct in_addr in);
    
    /** inet_ntoa's thread safe version
     *  \param in			src ip address
     *  \param dest			buffer to save string address
     *  \param dest_len		dest buffer len
     *  \return				number of bytes writed
     */
    int inet_ntoa_safe(struct in_addr in, char * dest, int dest_len);
    
    /** inet_ntoa's thread safe version
     *  \param in			src ip address
     *  \param dest			buffer to save string address
     *  \param dest_len		dest buffer len
     *  \return				number of bytes writed
     */
    int inet_ntoa_safe(struct sockaddr_in in, char * dest, int dest_len);
    
    /////////////////////////////////////////////////////////////////////////
    // implement
    inline std::string inet_ntoa_safe(unsigned int ip)
    {
        in_addr addr;
        addr.s_addr = ip;
        return inet_ntoa_safe(addr);
    }
    
    inline std::string inet_ntoa_safe(struct in_addr in)
    {
        char buf[16];
        inet_ntoa_safe(in, buf, 16);
        return buf;
    }
    
    inline int inet_ntoa_safe(struct sockaddr_in in, char * dest, int dest_len)
    {
        return inet_ntoa_safe(in.sin_addr, dest, dest_len);
    }
    
    inline int inet_ntoa_safe(struct in_addr in, char * dest, int dest_len)
    {
        my_in_addr *p = (my_in_addr*) &in;
        return snprintf(dest, dest_len, "%u.%u.%u.%u", p->S_un.S_un_b.s_b1, p->S_un.S_un_b.s_b2, p->S_un.S_un_b.s_b3, p->S_un.S_un_b.s_b4);
    }
    
}

#endif /* netinet_util_h */
