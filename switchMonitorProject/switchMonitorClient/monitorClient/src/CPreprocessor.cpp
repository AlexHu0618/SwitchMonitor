/** \file CPreprocessor.cpp
 *
 * \brief the c++ file for caculating RMS
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */

#include "CPreprocessor.h"
#include <iostream>
#include <malloc.h>
#include <math.h>

CPreprocessor::CPreprocessor()
{

}

CPreprocessor::~CPreprocessor()
{

}

int CPreprocessor::__RMS( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, int nSubLength )
{
    double dSum = 0;
    double dRmsVal = 0;
    for( int nNum = nSubLength; nNum < nLength-nSubLength; ++nNum )   //nSubLength is the caculating interval
    {
        dSum = 0;
        for( int i = nNum-nSubLength; i < nNum+nSubLength; ++i )
        {
            dSum += (parrdDataIn[i] * parrdDataIn[i]);
        }
        dRmsVal = sqrt( dSum/(nSubLength<<1) );
        (*pvecdDataOut).push_back( dRmsVal );
    }
    return 0;
}

/** \brief 本函数主要是计算RMS
 *
 * \param parrdDataIn, 源数据缓冲区指针; pvecdDataOut, 计算后的RMS数据缓冲区指针; nLength, 源数据长度; strAlgorithm, 算法名称
 *
 * \return 成功返回0; 不成功返回-1
 *
 */
int CPreprocessor::doPreprocess( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, string strAlgorithm )
{
    (*pvecdDataOut).clear();
    int nResult = 0;
    nResult = __RMS( parrdDataIn, pvecdDataOut, nLength, 3 );
    if( nResult == 0 )
    {
        return 0;
    }
    else
    {
        cerr << "doPreprocess() error";
        return -1;
    }
}
