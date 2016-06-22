//
//  CmdLineParser.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef CmdLineParser_h
#define CmdLineParser_h


#include <string>
#include <vector>

namespace shadow
{
    
    /** command line parser
     *  parser the argc and argv value from command line
     */
    class CmdLineParser
    {
        std::vector<char>		argvbuf;	///< buffer to save argv's value
        std::vector<char *>		argv;		///< argv
    public:
        CmdLineParser()	{}
        explicit CmdLineParser(const std::string& cmdline);
        
        template < typename FI >
        CmdLineParser(FI first, FI last)
        {
            parse(first, last);
        }
        
        template < typename FI >
        bool parse(FI first, FI last)
        {
            argvbuf.assign(first, last);
            argvbuf.push_back('\0');
            return parse();
        }
        
        /** parse a command line
         *  \param cmdline		command line string
         *  \return				when success, return true, otherwise false
         */
        bool parse(const std::string& cmdline)
        {
            return parse(cmdline.begin(), cmdline.end());
        }
        
        /** get argc
         *  \return		argc
         */
        int get_argc() const 			{ return argv.size(); }
        
        /** get argv
         *  \return		argv
         */
        char ** get_argv()				{ return &argv[0]; }
    private:
        bool parse();
    };
    
}



#endif /* CmdLineParser_h */
