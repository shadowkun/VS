//
//  AddressFilter.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef AddressFilter_h
#define AddressFilter_h

#include <netinet/in.h>
#include <string>
#include <vector>
#include <iosfwd>

#include "pthread_util.h"

namespace shadow
{
    /** ip filter item;
     *  wrap an ip filter item, like 192.0.*.*
     */
    class IpFilterItem
    {
        unsigned char	m_addr[4];		///< address data
        int				m_first_star;	///< the first star's position, base on 0
        
        friend bool operator<(const IpFilterItem& o1, const IpFilterItem& o2);
        friend bool operator==(const IpFilterItem& o1, const IpFilterItem& o2);
        
        friend class ipfilter_match_op;
    public:
        /** Constructor
         *  \param in			address data
         *  \param first_star	first star's position
         */
        IpFilterItem(in_addr in, int first_star)
        {
            memcpy(m_addr, &in, sizeof(m_addr));
            m_first_star = first_star;
        }
        
        /** Constructor
         *  \param in			address data
         *  \param first_star	first star's position
         */
        IpFilterItem(in_addr_t in, int first_star)
        {
            memcpy(m_addr, &in, sizeof(m_addr));
            m_first_star = first_star;
        }
        
        /** check whether a ip address match the filter
         *  \param addr		the ip address to check
         *  \return			when match, return true, otherwise return false
         */
        bool Match(in_addr addr) const;
        
        friend std::ostream& operator<<(std::ostream& ost, const IpFilterItem& item);
    };
    
    std::ostream& operator<<(std::ostream& ost, const IpFilterItem& item);
    
    bool operator<(const IpFilterItem& o1, const IpFilterItem& o2);
    
    inline bool operator==(const IpFilterItem& o1, const IpFilterItem& o2)
    {
        if(*reinterpret_cast<const int*>(o1.m_addr) != *reinterpret_cast<const int*>(o2.m_addr))
            return false;
        return o1.m_first_star == o2.m_first_star;
    }
    
    inline bool operator!=(const IpFilterItem& o1, const IpFilterItem& o2)
    {
        return !(o1 == o2);
    }
    
    /** a functor to compare an ip filter and an ip address(less op);
     * to find a match IpFilterItem
     */
    struct ipfilter_match_op
    {
        bool operator()(const IpFilterItem& o1, struct in_addr o2);
        bool operator()(struct in_addr o1, const IpFilterItem& o2);
    };
    
    /** address filter;
     *  to check a ip address if is allow to access server
     */
    class AddressFilter
    {
        std::vector<IpFilterItem>	m_vDeny;		///< deny address list
        std::vector<IpFilterItem>	m_vAllow;		///< allow address list
        
        typedef std::vector<IpFilterItem>::iterator	VIT;
        typedef std::vector<IpFilterItem>::const_iterator	CVIT;
        
        volatile enum EOrder { eAllowDeny, eDenyAllow } m_eOrder;	///< check order
        mutable ReadWriteLocker	m_rwlock;			///< read write lock
    public:
        AddressFilter()
        {}
        
        /** Constructor
         *  \param deny	deny address list, like "192.0.0.* 172.2.1.4"
         *  \param allow	allow address list, same as deny
         *  \param order	check order
         */
        AddressFilter(const std::string& deny, const std::string& allow, const std::string& order = "")
        {
            Parse(deny, allow, order);
        }
        
        bool SetCheckOrder(const std::string& order, std::string& err_msg);
        bool AddDenyItem(const std::string& deny, std::string& err_msg);
        bool DelDenyItem(const std::string& deny, std::string& err_msg);
        bool AddAllowItem(const std::string& allow, std::string& err_msg);
        bool DelAllowItem(const std::string& allow, std::string& err_msg);
        
        bool SetCheckOrder(const std::string& order);
        bool AddDenyItem(const std::string& deny);
        bool DelDenyItem(const std::string& deny);
        bool AddAllowItem(const std::string& allow);
        bool DelAllowItem(const std::string& allow);
        
        /** parse address list
         *  \param deny		deny address list
         *  \param allow		allow address list
         *  \param err_msg		error message
         *  \param order		check order
         *  \return				when success, return true, when an error occurs, return false and err_msg is set
         */
        bool Parse(const std::string& deny, const std::string& allow, const std::string& order, std::string& err_msg);
        
        /** parse address list
         *  \param deny		deny address list
         *  \param allow		allow address list
         *  \param order		check order
         *  \return				when success, return true, when an error occurs, return false
         */
        bool Parse(const std::string& deny, const std::string& allow, const std::string& order)
        {
            std::string err_msg;
            return Parse(deny, allow, order, err_msg);
        }
        
        /// check if an ip address is allow to access server
        bool IsAllow(const sockaddr_in& in) const
        {
            return IsAllow(in.sin_addr);
        }
        
        /// check if an ip address is allow to access server
        bool IsAllow(struct in_addr in) const;
        
        /// check if an ip address is allow to access server
        bool IsAllow(in_addr_t in) const
        {
            in_addr addr = {in};
            return IsAllow(addr);
        }
    private:
        static bool ParseString(const std::string& str, std::vector<IpFilterItem>& vec, std::string& err_msg);
        static EOrder ParseOrder(const std::string& order, std::string& err_msg);
        static bool AddItem(const std::string& item, std::vector<IpFilterItem>& vec, std::string& err_msg);
        static bool DelItem(const std::string& item, std::vector<IpFilterItem>& vec, std::string& err_msg);
        static bool ParseItem(const std::string& item, in_addr_t& in, int& first_star, std::string& err_msg);
        friend std::ostream& operator<<(std::ostream& ost, const AddressFilter& af);
    };
    
    std::ostream& operator<<(std::ostream& ost, const AddressFilter& af);
    
}

#endif /* AddressFilter_h */
