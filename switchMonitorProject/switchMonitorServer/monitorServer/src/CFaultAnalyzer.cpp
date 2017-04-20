#include "CFaultAnalyzer.hpp"
#include <fstream>
#include <windows.h>
#include <iostream>
#include <math.h>
#include <sstream>

#define BASE_LENGTH 80000;
#define DATA_LENGTH 505000;
#define NUM_SAMPLE 505000;   // for time 25s
#define SAMPLE_INTERVAL 0.00005;

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
    double arrdTransformRatio[] = { 3.0, 17.857 };
    if( __TransformRawData( arrdTransformRatio ) == 0 )
    {
        //!< read base data
        int nNumSample = BASE_LENGTH;
        double* base_i = (double*)malloc(nNumSample * sizeof(double));
        double* base_v = (double*)malloc(nNumSample * sizeof(double));
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
        double dSampleInterval = SAMPLE_INTERVAL;
        GetScores_ZD6( base_i, base_v, vBaseCounter, m_parrdI1RealData, m_parrdV1RealData, m_nsizeofRealData, dSampleInterval, m_parrdScore );
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
        cout << "Get score failed!" << endl;
    }
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::__TransformRawData( double *parrdTranRatio )
{
    int nNumChannel = 3;
    switch (m_emTypeofSwitch)
    {
    case S700K:
        nNumChannel = 6;
        break;
    case ZYJ7:
        nNumChannel = 9;
        break;
    case ZD6:
        nNumChannel = 3;
        break;
    }
    int nNumSample = DATA_LENGTH;
    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        stringstream stream;
        stream << nCH;
        string strChNum = stream.str();
        string strDataFilePath = m_strPath + "\\DataCH" + strChNum + ".dat";

        ifstream stmRawDataFileIn( strDataFilePath, ios_base::in );
        if( !stmRawDataFileIn.is_open() )
        {
            cerr << "Fail to open the " << strDataFilePath << " raw data file!" << endl;
            return -1;
        }
        cout << "open file " << strDataFilePath << endl;
        m_pparrdAllRealData[nCH] = (double*)malloc( nNumSample*sizeof(double) );
        double *pdChRealData = m_pparrdAllRealData[nCH];
        int nChDataLength = 0;
        double temp = 0;
        double Ratio = parrdTranRatio[nCH];
        while( stmRawDataFileIn.peek() != EOF )
        {
            stmRawDataFileIn >> temp;
            *(pdChRealData++) = (double)( temp*Ratio/pow(2,17) );
            ++nChDataLength;
            if( nChDataLength > nNumSample )
            {
                cerr << "the total number of V1 raw data is morn than 505000, so it just read 505000 datas!" << endl;
                break;
            }
        }
        m_nSizeofChRealData = nChDataLength--;
        stmRawDataFileIn.close();
    }
    cout << "Transfrom raw data is OK!" << endl;
    return 0;
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
 CFaultAnalyzer::CFaultAnalyzer( string strPath, SWITCH_TYPE TypeofSwitch )
{
    m_strPath = strPath;
    m_emTypeofSwitch = TypeofSwitch;
    m_bisLtoR = true;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::SaveRealData( double *arrdTransformRatio )
{
    if( __TransformRawData( arrdTransformRatio ) == 0 )
    {
        cout << "transformation is ok" << endl;
    }

    int nNumChannel = 3;
    switch (m_emTypeofSwitch)
    {
    case S700K:
        nNumChannel = 6;
        break;
    case ZYJ7:
        nNumChannel = 9;
        break;
    case ZD6:
        nNumChannel = 3;
        break;
    }

    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        stringstream stream;
        stream << nCH;
        string strChNum = stream.str();
        string strDataFilePath = m_strPath + "\\DataCH" + strChNum + ".txt";
        ofstream stmRawDataFileOut( strDataFilePath, ios_base::out );
        if( !stmRawDataFileOut.is_open() )
        {
            cerr << "Fail to open the DataCH" << nCH << ".dat raw data file!" << endl;
            return -1;
        }

        double *parrdChRealData = m_pparrdAllRealData[nCH];
        for (int nNum = 0; nNum < m_nSizeofChRealData; ++nNum)
        {
            stmRawDataFileOut << parrdChRealData[nNum] << "\n";
        }
        stmRawDataFileOut.close();
        cout << "save to file: " << strDataFilePath << endl;
    }
    return 0;
}
