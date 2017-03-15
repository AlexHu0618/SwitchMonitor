#include "CFaultAnalyzer.hpp"
#include <fstream>
#include <windows.h>
#include <iostream>
#include <math.h>

//!< declear the external function lib
extern "C" _declspec(dllexport) void GetScores_S700K( double *base_i1, double *base_i2, double *base_i3, double *base_v1, double *base_v2, double *base_v3, int base_length, double *data_i1, double *data_i2, double *data_i3, double *data_v1, double *data_v2, double *data_v3, int data_length, double sampleinterval, double* scores);
extern "C" _declspec(dllexport) void GetScores_ZD6( double* base_i1, double* base_v1, int base_length, double* data_i1, double* data_v1, int data_length, double sampleinterval, double* scores);
extern "C" _declspec(dllexport) void GetScores_ZYJ7( double *base_i1, double *base_i2, double *base_i3, double *base_v1, double *base_v2, double *base_v3, double* base_p1, double* base_p2, int base_length, double *data_i1, double *data_i2, double *data_i3, double *data_v1, double *data_v2, double *data_v3, double* data_p1, double* data_p2, int data_length, double sampleinterval, double* scores);


/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::__AnalyzeFault( void )
{
    if( __TransformRawData() == 0 )
    {
        //!< read base data
        double* base_i = (double*)malloc(80000 * sizeof(double));
        double* base_v = (double*)malloc(80000 * sizeof(double));
        ifstream istmIBaseDataFileIn, istmVBaseDataFileIn;
        if( m_bisLtoR )
        {
            istmIBaseDataFileIn.open( "zd6_i1_base_LR.txt", ios_base::in );
            istmVBaseDataFileIn.open( "zd6_v1_base_LR.txt", ios_base::in );
        }
        else
        {
            istmIBaseDataFileIn.open( "zd6_i1_base_RL.txt", ios_base::in );
            istmVBaseDataFileIn.open( "zd6_v1_base_RL.txt", ios_base::in );
        }
        if( !istmIBaseDataFileIn.is_open() || !istmVBaseDataFileIn.is_open() )
        {
            cerr << "the base file is not open" << endl;
            return -1;
        }
        int iBaseCounter = 0;
        while( istmIBaseDataFileIn.peek() != EOF )
        {
            istmIBaseDataFileIn >> base_i[iBaseCounter];
            ++iBaseCounter;
        }
        iBaseCounter--;

        int vBaseCounter = 0;
        while( istmVBaseDataFileIn.peek() != EOF )
        {
            istmVBaseDataFileIn >> base_v[vBaseCounter];
            ++vBaseCounter;
        }
        vBaseCounter--;

        //!< analyzing the fault result
        m_parrdScore = (double*)malloc( 10*sizeof(double) );
        GetScores_ZD6( base_i, base_v, vBaseCounter, m_parrdI1RealData, m_parrdV1RealData, m_nsizeofRealData, m_dsampleInterval, m_parrdScore );
        istmIBaseDataFileIn.close();
        istmVBaseDataFileIn.close();
        free( base_i );
        free( base_v );
        return 0;
    }
    else
    {
        cerr << "TransformRawData fail!" << endl;
        return -1;
    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
double* CFaultAnalyzer::GetRealData(void)
{

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
double* CFaultAnalyzer::GetScore(void)
{
    if( __AnalyzeFault() == 0 )
    {
        return m_parrdScore;
    }
    else
    {
        return NULL;
    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::__TransformRawData( void )
{
    string strV1FileName = "\\Data01.dat";
    string strI1FileName = "\\Data02.dat";
    if( m_strPath.back() == 'L' || m_strPath.back() == 'l' )
    {
        m_bisLtoR = false;
        strV1FileName = "\\Data00.dat";
    }

    //!< read V1 raw data and transform to real data
    string strV1RawDataFile = m_strPath + strV1FileName;
    ifstream istmV1RawDataFileIn( strV1RawDataFile,ios_base::in );
    if( istmV1RawDataFileIn.is_open() )
    {
        m_parrdV1RealData = (double*)malloc( 80000*sizeof(double) );
        int nV1DataLength = 0;
        double temp = 0;
        while( istmV1RawDataFileIn.peek() != EOF )
        {
            istmV1RawDataFileIn >> temp;
            *(m_parrdV1RealData++) = (double)(temp*3/pow(2,17));
            ++nV1DataLength;
            if( nV1DataLength > 80000 )
            {
                cerr << "the total number of V1 raw data is morn than 80000, so it just read 80000 datas!" << endl;
                break;
            }
        }
        m_nsizeofRealData = nV1DataLength--;
        istmV1RawDataFileIn.close();
    }
    else
    {
        cerr << "the V1 raw data is nonexistent!" << endl;
        return -1;
    }

    //!< read I1 raw data and transform to real data
    string strI1RawDataFile = m_strPath + strI1FileName;
    ifstream istmI1RawDataFileIn( strI1RawDataFile,ios_base::in );
    if( istmI1RawDataFileIn.is_open() )
    {
        m_parrdI1RealData = (double*)malloc( 80000*sizeof(double) );
        int nI1DataLength = 0;
        double temp = 0;
        while( istmI1RawDataFileIn.peek() != EOF )
        {
            istmI1RawDataFileIn >> temp;
            *(m_parrdI1RealData++) = (double)(temp*17.857/pow(2,17));
            ++nI1DataLength;
            if( nI1DataLength > 80000 )
            {
                cerr << "the total number of I1 raw data is morn than 80000, so it just read 80000 datas!" << endl;
                break;
            }
        }
        nI1DataLength--;
        istmI1RawDataFileIn.close();
        return 0;
    }
    else
    {
        cerr << "the I1 raw data is nonexistent!" << endl;
        return -1;
    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
 CFaultAnalyzer::~CFaultAnalyzer()
{
    free( m_parrdScore );
    free( m_parrdI1RealData );
    free( m_parrdV1RealData );
    m_parrdScore = NULL;
    m_parrdI1RealData = NULL;
    m_parrdV1RealData = NULL;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
 CFaultAnalyzer::CFaultAnalyzer( string strPath, SwitchType emSwitchType )
{
    m_strPath = strPath;
    m_emSwitchType = emSwitchType;
    m_bisLtoR = true;
    m_dsampleInterval = 0.00005;
}


