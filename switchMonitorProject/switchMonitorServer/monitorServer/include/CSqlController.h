#ifndef CSQLCONTROLLER_H
#define CSQLCONTROLLER_H

#include <string>
using namespace std;
//��Ҫ����mysql��ͷ�ļ���/usr/include/mysql/
//�����ʱ����Ҫ����-L/usr/lib/ -lmysqlclient -I/usr/include/mysqlѡ��

#include <mysql.h> //������ͷ�ļ���һЩ��Ҫ�ṹ�������
#include <errmsg.h>//������Ϣ�Ĵ��
#include <mysql_version.h>  //������ǰmysql�İ汾��Ϣ

#ifdef __cplusplus
extern "C"
#endif

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
    int Update( string strSqlCmd );
    int Initial( string DBname, string tabName );
};
#endif // CSQLCONTROLLER_H