/** \file CPreprocessor.h
 *
 * \brief the head file include class CPreprocessor, for data pre-processing
 *
 * \author by Alex, jthu4alex@163.com, on 2017/11/14
 *
 */

#ifndef CPreprocessor_H
#define CPreprocessor_H

#include <string>
#include <vector>

using namespace std;

/** \brief 本类主要是负责数据前处理，主要是RMS(Root Meam Square)均方根值
 *
 *
 */
class CPreprocessor
{
private:
    int __RMS( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, int nSubLength );

public:
    CPreprocessor();
    ~CPreprocessor();

    int doPreprocess( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, string strAlgorithm );

};

#endif // CPreprocessor_H
