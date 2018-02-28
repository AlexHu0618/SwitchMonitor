/** \file CSqlController.cpp
 *
 * \brief the c++ file for MySQL database communication
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */
#include "CSqlController.h"
#include <stdio.h>
#include <iostream>

using namespace std;

/** \brief ���캯��
 *
 * \param strIPAddr, ���ݿ�IP��ַ; nPort, �˿ںţ� strUser, �û���; strPassWord, ��¼����
 *
 * \return ��
 *
 */
CSqlController::CSqlController( string strIpAddr, unsigned int nPort, string strUser, string strPassWord )
{
    m_strIpAddr = strIpAddr;
    m_nPort = nPort;
    m_strUser = strUser;
    m_strPassWord = strPassWord;
}

CSqlController::~CSqlController()
{

}

/** \brief ��������Ҫ�ǳ�ʼ�����ݿ����ӣ��������ݿ����ӣ����ָ�������ݿ��Ƿ���ڣ������ھͱ������ָ���ı��Ƿ���ڣ��������򱨴�
 *
 * \param strDBname, ���ݿ���; strTabName, ����
 *
 * \return �ɹ�����0�����ɹ�����-1
 *
 */
int CSqlController::Initial( string strDBname, string strTabName )
{
    if( !mysql_init(&m_mysql) )
    {
        perror( "mysql_init:" );
        return -1;
    }

    if( !mysql_real_connect( &m_mysql, m_strIpAddr.c_str(), m_strUser.c_str(), m_strPassWord.c_str(), NULL, m_nPort, NULL, 0 ) )
    {
        perror( "mysql_real_connect" );
        return -1;
    }

    int err = __CheckDB( &m_mysql, strDBname );
    if( err != 1 )
    {
        cout << "create db is err!" << endl;;
        mysql_close( &m_mysql );
        return -1;
    }
    //select which db
    if( mysql_select_db( &m_mysql, strDBname.c_str() ) ) //return 0 is success ,!0 is err
    {
        perror("mysql_select_db:");
        mysql_close(&m_mysql);
        return -1;
    }

    err = __CheckTab( &m_mysql, strTabName );
    if( err != 1 )
    {
        cout << "create tab is err!" << endl;
        mysql_close(&m_mysql);
        return -1;
    }

    return 0;
}

int CSqlController::__CheckDB( MYSQL* pMysql, string strDBname )
{
    MYSQL_ROW row = NULL;
    MYSQL_RES *pResult = NULL;

    pResult = mysql_list_dbs( pMysql, NULL );
    if( pResult )
    {
        while( (row = mysql_fetch_row(pResult)) != NULL )
        {
            cout << "db is " << row[0] << endl;
            if( strcmp( row[0], strDBname.c_str() ) == 0 )
            {
                cout << "find db " << strDBname;
                break;
            }
        }
        //mysql_list_dbs������ڴ棬��Ҫʹ��mysql_free_result�ͷ�
        mysql_free_result( pResult );
    }
    if( !row )  //û��������ݿ⣬����
    {
//        char buf[128]={0};
//        strcpy(buf,"CREATE DATABASE ");
//        strcat(buf,db_name);
//
//        if(mysql_query(mysql,buf))
//        {
//            fprintf(stderr,"Query failed (%s)\n",mysql_error(mysql));
//            return 0;
//        }
        cout << "no database: " << strDBname << endl;
        return 0;
    }
    return 1;
}

int CSqlController::__CheckTab( MYSQL* pMysql, string strTabName )
{
    if( strTabName.empty() )
    {
        return 0;
    }
    MYSQL_ROW row=NULL;
    MYSQL_RES *pResult = NULL;
    pResult = mysql_list_tables( pMysql, NULL );
    if( pResult )
    {
        while( (row = mysql_fetch_row(pResult)) != NULL )
        {
            cout << "tables is " << row[0] << endl;
            if( strcmp( row[0], strTabName.c_str() ) == 0 )
            {
                cout << "find the table " << strTabName << endl;
                break;
            }
        }
        mysql_free_result( pResult );
    }
    if( !row ) //create table
    {
//        char buf[128]={0};
//        char qbuf[128]={0};
//        snprintf(buf,sizeof(buf),"%s (name VARCHAR(20),sex char(1),score int(3));",TABLE_NAME);
//        strcpy(qbuf,"CREATE TABLE ");
//        strcat(qbuf,buf);

//        if(mysql_query(mysql,qbuf)){
//            fprintf(stderr,"Query failed (%s)\n",mysql_error(mysql));
//            exit(1);
//        }
        cout << "no table " << strTabName << endl;
        return 0;
    }

    return 1;
}

/** \brief ��������Ҫ�ǲ������ݵ�MySQL���ݿ�
 *
 * \param strSqlCmd, ���ݿ����ָ�
 *
 * \return �ɹ�����0�����ɹ�����-1
 *
 */
int CSqlController::Insert( string strSqlCmd )
{
    if( mysql_query( &m_mysql, strSqlCmd.c_str() ) != 0 )  //ִ�в������
    {
        perror( "mysql_query() error");
        return -1;
    }
    else
    {
        cout << "insert data success" << endl;
        return 0;
    }
}
