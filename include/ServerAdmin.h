//
//  ServerAdmin.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef ServerAdmin_h
#define ServerAdmin_h


#include <vector>
#include <string>
#include <map>
#include <iomanip>

#include "fdstream.h"
#include "pthread_util.h"
#include "meta.h"

namespace shadow
{
    
    class AddressFilter;
    class executor;
    
    /**
     * admin command interface
     */
    struct AdminCommand
    {
        virtual ~AdminCommand()	{}
        
        /** execute the command
         *  \param argc		argc, same with main
         *  \param argv		argv, same with main
         *  \param ios		input output stream to read/write from network
         */
        virtual int Execute(int argc, char ** argv, iofdstream& ios) = 0;
    };
    
    namespace detail
    {
        class CmdInfo;
    }
    
    /**
     *  server admin
     */
    class ServerAdmin
    {
        typedef detail::CmdInfo						CmdInfo;
        typedef std::vector<CmdInfo *>				CmdInfoVecT;
        typedef CmdInfoVecT::size_type				size_type;
        typedef std::map<std::string, size_type>	MapT;
        typedef MapT::iterator						MIT;
        
        CmdInfoVecT				m_cmds;			///< command info
        MapT					m_n2cmd;		///< name or short name to commands
        mutable ReadWriteLocker	m_rwlock;		///< read write lock to lock commands
        std::vector<int>		m_listenfds;	///< listen fd
        std::string				m_prompt;		///< prompt string
        AddressFilter *			m_addr_filter;	///< address filter
        int						m_timeout;		///< timeout
        executor *				m_executor;		///< executor
        
        ServerAdmin(const ServerAdmin&);
        ServerAdmin& operator=(const ServerAdmin&);
    public:
        /** constructor
         *  \param addr_filter		address filter
         *  \param timeout			socket read/write timeout
         *  \param prompt			prompt
         */
        explicit ServerAdmin(AddressFilter * addr_filter = NULL, int timeout = 5000, const std::string& prompt = "");
        ~ServerAdmin();
        
        /// default quit command
        int CloseCommand(int argc, char ** argv, iofdstream& ios);
        
        /// default help command
        int HelpCommand(int argc, char ** argv, std::ostream& os);
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param op		a function object, the operation of command
         *  \param desc		command's help information
         *  \param dummy	useless parameter, you should ignore it; it only use for function overload.
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Op >
        bool AddCommand(const std::string& name, Op op, const std::string& desc, typename disable_if<is_convertible<Op, AdminCommand*> >::type* dummy = 0);
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param cmd		the operation of command
         *  \param desc		command's help information
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        bool AddCommand(const std::string& name, AdminCommand * cmd, const std::string& desc)
        {
            return AddCommandImpl(name, cmd, desc);
        }
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param op		a function object, the operation of command
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Op >
        bool AddCommand(const std::string& name, Op op)
        {
            return AddCommand(name, op, name + " command.");
        }
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param obj		the object to execute member function
         *  \param memfun	member function's pointer
         *  \param desc		command's help information
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Obj, typename Obj2, typename StreamT >
        bool AddCommand(const std::string& name, Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&), const std::string& desc, typename disable_if<is_const_type<Obj> >::type* dummy = 0);
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param obj		the object to execute member function
         *  \param memfun	member function's pointer
         *  \param desc		command's help information
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Obj, typename Obj2, typename StreamT >
        bool AddCommand(const std::string& name, const Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&) const, const std::string& desc);
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param obj		the object to execute member function
         *  \param memfun	member function's pointer
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Obj, typename Obj2, typename StreamT >
        bool AddCommand(const std::string& name, Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&))
        {
            return AddCommand(name, obj, memfun, name + " command.");
        }
        
        /** add a command
         *  \param name		full command name, is name,short_name, like "help,?"
         *  \param obj		the object to execute member function
         *  \param memfun	member function's pointer
         *  \return			if command exist, this command will not add to admin service, and return false, otherwise return true
         */
        template < typename Obj, typename Obj2, typename StreamT >
        bool AddCommand(const std::string& name, const Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&) const)
        {
            return AddCommand(name, obj, memfun, name + " command.");
        }
        
        /// delete all commands
        void Clear();
        
        /** listen an address
         * \param addr		address to bind
         * \param port		port
         * \param backlog	backlog
         */
        void ListenAddress(const char * addr, int port, int backlog = 2);
        
        /// start admin server
        void Start();
        
        /// process client connection
        void ProcessClientFd(int fd);
        
        /// get command count
        size_type GetCommandCount() const	{ return m_cmds.size(); }
    private:
        static std::pair<std::string, std::string> ParseFullname(const std::string& fullname);
        bool AddCommandImpl(const std::string& fullname, AdminCommand * cmd, const std::string& desc);
        bool ExecuteCommand(int argc, char ** argv, iofdstream& ios);
        static void * ThreadProc(void * pPara);
        //static void * CommandProc(void * pPara);
        void Print(std::ostream& ost) const;
        friend std::ostream& operator<<(std::ostream& ost, const ServerAdmin& sa);
    };
    
    std::ostream& operator<<(std::ostream& ost, const ServerAdmin& sa);
    
    //////////////////////////////////////////////////////////////////////////////////
    // implements
    namespace detail
    {
        template < typename Op >
        class AdminCommandStreamImpl : public AdminCommand
        {
            Op m_op;
        public:
            AdminCommandStreamImpl(Op op)
            : m_op(op)
            {}
            
            int Execute(int argc, char ** argv, iofdstream& ios)
            {
                return m_op(argc, argv, ios);
            }
        };
        
        template < typename ObjT, typename ObjT2, typename StreamT >
        class MemFunStreamOp
        {
            typedef typename type_selector<is_const_type<ObjT>::value,
            int (ObjT2::*)(int, char**, StreamT&) const,
            int (ObjT2::*)(int, char**, StreamT&)>::Type	FunT;
            ObjT&	m_obj;
            FunT	m_fun;
        public:
            MemFunStreamOp(ObjT& obj, FunT f)
            : m_obj(obj)
            , m_fun(f)
            {}
            
            int operator()(int argc, char ** argv, iofdstream& ios)
            {
                return (m_obj.*m_fun)(argc, argv, ios);
            }
        };
    }
    
    template < typename Op >
    bool ServerAdmin::AddCommand(const std::string& name, Op op, const std::string& desc, typename disable_if<is_convertible<Op, AdminCommand*> >::type* dummy)
    {
        return AddCommandImpl(name, new detail::AdminCommandStreamImpl<Op>(op), desc);
    }
    
    template < typename Obj, typename Obj2, typename StreamT >
    bool ServerAdmin::AddCommand(const std::string& name, Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&), const std::string& desc, typename disable_if<is_const_type<Obj> >::type* dummy)
    {
        detail::MemFunStreamOp<Obj, Obj2, StreamT> op(obj, memfun);
        return AddCommand(name, op, desc);
    }
    
    template < typename Obj, typename Obj2, typename StreamT >
    bool ServerAdmin::AddCommand(const std::string& name, const Obj& obj, int (Obj2::* memfun)(int, char**, StreamT&) const, const std::string& desc)
    {
        detail::MemFunStreamOp<const Obj, Obj2, StreamT> op(obj, memfun);
        return AddCommand(name, op, desc);
    }
    
}

#endif /* ServerAdmin_h */
