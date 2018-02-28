/** \file CSqlController.h
 *
 * \brief the head file include class CSqlController, for MySQL database communication
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */

#ifndef CSQLCONTROLLER_H
#define CSQLCONTROLLER_H

#include <string>
using namespace std;
//需要包含mysql的头文件，/usr/include/mysql/
//编译的时候需要加上-L/usr/lib/ -lmysqlclient -I/usr/include/mysql选项

#include <mysql.h> //基本的头文件，一些重要结构体的声明
#include <errmsg.h>//错误信息的存放
#include <mysql_version.h>  //包含当前mysql的版本信息

#ifdef __cplusplus
extern "C"
#endif

/**
 * \brief 本类主要是负责MySQL数据库通信，包含功能为：连接初始化、插入数据
 *
 */
class CSqlController
{
private:
    string m_strIpAddr;
    unsigned int m_nPort;
    string m_strUser;
    string m_strPassWord;
    MYSQL m_mysql;

    int __CheckDB( MYSQL* pMysql, string DBname );
    int __CheckTab( MYSQL* mysql, string TabName );

public:
    CSqlController( string strIpAddr, unsigned int nPort, string strUser, string strPassWord );
    ~CSqlController();

    int Insert( string strSqlCmd );
    int Initial( string DBname, string tabName );
};
#endif // CSQLCONTROLLER_H
