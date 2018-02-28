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
    for( int nNum = nSubLength; nNum < nLength-nSubLength; ++nNum )     // [-nSubLength, nSubLength]
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

int CPreprocessor::doPreprocess( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, int nSubLength, string strAlgorithm )
{
    (*pvecdDataOut).clear();
    int nResult = 0;
    nResult = __RMS( parrdDataIn, pvecdDataOut, nLength, nSubLength );
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
