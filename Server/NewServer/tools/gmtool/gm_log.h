/********************************************************************
	created:	1:3:2014   11:05
	file:		gm_log.h
	author:		Allan Tao
	
	purpose:	
*********************************************************************/


#ifndef _H_GM_LOG_H_
#define _H_GM_LOG_H_
#include "std_type.h"
#include <format>
#include <string_view>
#include <utility>



class CGMLog
{
protected:
    CGMLog();
    ~CGMLog();

    static CGMLog   m_Instance;


public:
    static CGMLog &GetInstance();

    BOOL Init();
    BOOL UnInit();


    BOOL Log(std::string_view text);
    BOOL Log(const char* text) { return text ? Log(std::string_view(text)) : FALSE; }
    template<class... Args> requires(sizeof...(Args)>0)
    BOOL Log(std::format_string<Args...> format,Args&&... args)
    {
        return Log(std::format(format,std::forward<Args>(args)...));
    }





private:







};














#endif //_H_GM_LOG_H_






