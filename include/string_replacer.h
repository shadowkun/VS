//
//  string_replacer.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef string_replacer_h
#define string_replacer_h

#include <map>

#include <string>



namespace shadow

{
    
    
    
    /** multi string replacer.
     
     */
    
    template < typename Ch, typename Tr = std::char_traits<Ch>, typename A = std::allocator<Ch> >
    
    class basic_string_replacer
    
    {
        
        typedef basic_string_replacer<Ch, Tr, A>	this_type;
        
        typedef std::map<char, this_type *>			ChildsT;
        
        typedef std::basic_string<Ch, Tr, A>		string_type;
        
        ChildsT			_childs;
        
        string_type		_old;
        
        string_type		_new;
        
        
        
        /// 用于存放查找关键字时的位置信息
        
        struct FindResultPosInfo
        
        {
            
            const this_type * msr;	///< 关键字对应的节点
            
            int idx;				///< 关键字出现的位置
            
        };
        
    public:
        
        typedef typename string_type::size_type	size_type;
        
        
        
        basic_string_replacer()
        
        {}
        
        
        
        basic_string_replacer(const basic_string_replacer& r)
        
        : _old(r._old)
        
        , _new(r._new)
        
        {
            
            copy_deap_from(r._childs);
            
        }
        
        
        
        basic_string_replacer& operator=(const basic_string_replacer& r)
        
        {
            
            if(this == &r){
                
                _old = r._old;
                
                _new = r._new;
                
                copy_deap_from(r._childs);
                
            }
            
            return *this;
            
        }
        
        
        
        ~basic_string_replacer()
        
        {
            
            clear();
            
        }
        
        
        
        void clear()
        
        {
            
            for(typename ChildsT::iterator i = _childs.begin(); i != _childs.end(); ++i)
                
                delete i->second;
            
            _childs.clear();
            
            _old.clear();
            
            _new.clear();
            
        }
        
        
        
        /** replace the string
         
         */
        
        string_type replace(const string_type& s) const
        
        {
            
            FindResultPosInfo wi;
            
            bool r = find_words(s, 0, wi);		// 查找第一个替换的位置
            
            if(!r)
                
                return s;
            
            string_type sb;
            
            sb.reserve(s.size());
            
            int writen = 0;
            
            for(; r && wi.idx < (int)s.size(); ){
                
                sb.append(s, writen, wi.idx - writen);		// append 原字符串不需替换部分
                
                sb.append(wi.msr->_new);					// append 新字符串
                
                writen = wi.idx + wi.msr->_old.size();		// 忽略原字符串需要替换部分
                
                r = find_words(s, writen, wi);				// 查找下一个替换位置
                
            }
            
            sb.append(s, writen, s.size() - writen);		// 替换剩下的原字符串
            
            return sb;
            
        }
        
        
        
        /// 判断字符串s中是否存在关键字
        
        bool exist_words(const string_type& s) const
        
        {
            
            FindResultPosInfo fi;
            
            return find_words(s, 0, fi);
            
        }
        
        
        
        /// 获取字符串s中存在的关键字数量
        
        int count_words(const string_type& s) const
        
        {
            
            FindResultPosInfo wi;
            
            bool r = find_words(s, 0, wi);			// 查找第一个替换的位置
            
            if(!r)
                
                return 0;
            
            int writen = 0;
            
            int count = 0;
            
            for(; r && wi.idx < (int)s.size(); ){
                
                ++count;
                
                writen = wi.idx + wi.msr->_old.size();
                
                r = find_words(s, writen, wi);		// 查找下一个替换位置
                
            }
            
            return count;
            
        }
        
        
        
        /**
         
         * 添加一对替换关键字
         
         * \param oldWords	要替换的字符串
         
         * \param newWords	新字符串
         
         * \return			当前对象的引用，用于连写
         
         */
        
        this_type& add(const string_type& oldWords, const string_type& newWords)
        
        {
            
            add(oldWords, 0, newWords);
            
            return *this;
            
        }
        
        
        
        /** 查找第一个出现关键字的位置
         
         *  \param s		要进行查找的字符串
         
         *  \param pos		起始位置
         
         *  \return			一个pair(关键字出现的位置,关键字)，如果字符串中不存在关键字，则pair的第一个值为string::npos
         
         */
        
        std::pair<size_type, string_type> find_first(const string_type& s, size_type pos = 0)
        
        {
            
            FindResultPosInfo wi;
            
            bool r = find_words(s, pos, wi);		// 查找第一个替换的位置
            
            if(!r)
                
                return std::make_pair(string_type::npos, string_type());
            
            return std::make_pair(wi.idx, wi.msr->_old);
            
        }
        
    private:
        
        void copy_deap_from(const ChildsT& from)
        
        {
            
            clear();
            
            for(typename ChildsT::const_iterator i = from.begin(); i != from.end(); ++i)
                
                _childs[i->first] = new this_type(*i->second);
            
        }
        
        
        
        /**
         
         * 从字符串指定位置开始比较当前位置是否有关键字匹配
         
         */
        
        bool compare_words(const string_type& s, int idx, FindResultPosInfo& fi) const
        
        {
            
            // 找到匹配节点，并且该节点没有子节点（满足此条件是为了最长匹配）时，直接返回当前位置
            
            if(!_old.empty() && _childs.size() == 0)
                
            {
                
                fi.msr = this;
                
                fi.idx = idx - _old.size();
                
                return true;
                
            }
            
            
            
            // 如果字符串结束，则返回false
            
            if(idx >= (int)s.size())
                
                return false;
            
            
            
            // 查找匹配的字节点
            
            char c = s[idx];
            
            typename ChildsT::const_iterator it = _childs.find(c);
            
            if(it == _childs.end())
                
                return false;
            
            this_type * ti = it->second;
            
            
            
            // 递归匹配其余的字节点
            
            bool r = ti->compare_words(s, idx + 1, fi);
            
            
            
            // 如果其余的字节点不匹配，则检查当前节点是否需要替换的节点
            
            if(!r && !ti->_old.empty()){
                
                fi.msr = ti;
                
                fi.idx = idx + 1 - ti->_old.size();
                
                return true;
                
            }
            
            
            
            return r;
            
        }
        
        
        
        bool find_words(const string_type& s, int idx, FindResultPosInfo& fi) const
        
        {
            
            // 逐个位置比较是否匹配替换关键字
            
            for(int i = idx; i < (int)s.size(); ++i){
                
                bool r = compare_words(s, i, fi);
                
                if(r){
                    
                    return true;
                    
                }
                
            }
            
            return false;
            
        }
        
        
        
        void add(const string_type& oldWords, int idx, const string_type& newWords)
        
        {
            
            if((int)oldWords.size() == idx){
                
                // 达了树的叶子节点，此时对该节点设置字符串对
                
                _new = newWords;
                
                _old = oldWords;
                
                return;
                
            }
            
            
            
            // 建立一个字节点
            
            char next_ch = oldWords[idx];
            
            this_type *& ti = _childs[next_ch];
            
            if(ti == NULL){
                
                ti = new this_type();
                
            }
            
            
            
            // 递归建立下一个字节点
            
            ti->add(oldWords, idx + 1, newWords);
            
        }
        
    };
    
    
    
    typedef basic_string_replacer<char>		string_replacer;
    
    typedef basic_string_replacer<wchar_t>	wstring_replacer;
    
    
    
}




#endif /* string_replacer_h */
