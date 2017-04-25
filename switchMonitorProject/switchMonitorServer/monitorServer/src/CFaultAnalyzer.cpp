#include "CFaultAnalyzer.hpp"
#include <fstream>
#include <windows.h>
#include <iostream>
#include <math.h>
#include <sstream>

#define MAX_BASE_LENGTH 220000;
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
int CFaultAnalyzer::__AnalyzeFault( double *arrdTransformRatio )
{

    if (__TransformRawData( arrdTransformRatio ) != 0)
    {
        cout << "Fail to Transform raw data!" << endl;
        return -1;
    }
    int nNumChannel = 4;
    switch (m_emTypeofSwitch)
    {
    case S700K:
        nNumChannel = 6;
        break;
    case ZYJ7:
        nNumChannel = 9;
        break;
    case ZD6:
        nNumChannel = 4;
        break;
    }
    //!< read the base data
    int nNumSample = MAX_BASE_LENGTH;
    int nBaseDataLen;
    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        stringstream stream;
        stream << nCH;
        string strChNum = stream.str();
        string strFilePath = "BaseData\\DataCH" + strChNum + ".txt";
        ifstream stmBaseDataFileIn( strFilePath, ios_base::in );
        if (!stmBaseDataFileIn.is_open())
        {
            cerr << "Fail to open the " << strFilePath << " base data file!" << endl;
            return -1;
        }

        m_pparrdAllBaseData[nCH] = (double*)malloc( nNumSample*sizeof(double) );
        double *pdChBaseData = m_pparrdAllBaseData[nCH];
        nBaseDataLen = 0;
        while (stmBaseDataFileIn.peek() != EOF)
        {
            stmBaseDataFileIn >> *(pdChBaseData++);
            ++nBaseDataLen;
        }
        m_arrnAllChDataLen[nCH] = (--nBaseDataLen);
        stmBaseDataFileIn.close();
    }

    //!< analyzing the fault result
    m_parrdScore = (double *)malloc( 10*sizeof(double) );
    double dSampleInterval = SAMPLE_INTERVAL;
    char cEndLetter = m_strPath[m_strPath.length()-1];
    switch (m_emTypeofSwitch)
    {
    case ZD6:
        if (cEndLetter == 'R' || cEndLetter == 'r')   // L2R
        {
            GetScores_ZD6( m_pparrdAllBaseData[2], m_pparrdAllBaseData[0], m_arrnAllChDataLen[0], m_pparrdAllRealData[2], m_pparrdAllRealData[0], m_nSizeofChRealData, dSampleInterval, m_parrdScore );
        }
        else
        {
            GetScores_ZD6( m_pparrdAllBaseData[3], m_pparrdAllBaseData[1], m_arrnAllChDataLen[1], m_pparrdAllRealData[2], m_pparrdAllRealData[1], m_nSizeofChRealData, dSampleInterval, m_parrdScore );
        }
        break;
    case S700K:
        GetScores_S700K( m_pparrdAllBaseData[3], m_pparrdAllBaseData[4], m_pparrdAllBaseData[5], m_pparrdAllBaseData[0], m_pparrdAllBaseData[1], m_pparrdAllBaseData[2], m_arrnAllChDataLen[0],
                         m_pparrdAllRealData[3], m_pparrdAllRealData[4], m_pparrdAllRealData[5], m_pparrdAllRealData[0], m_pparrdAllRealData[1], m_pparrdAllRealData[2], m_nSizeofChRealData, dSampleInterval, m_parrdScore );
        break;
    case ZYJ7:
        GetScores_ZYJ7( m_pparrdAllBaseData[4], m_pparrdAllBaseData[5], m_pparrdAllBaseData[6], m_pparrdAllBaseData[1], m_pparrdAllBaseData[0], m_pparrdAllBaseData[3], m_pparrdAllBaseData[7], m_pparrdAllBaseData[8], nBaseDataLen,
                        m_pparrdAllRealData[4], m_pparrdAllRealData[5], m_pparrdAllRealData[6], m_pparrdAllRealData[1], m_pparrdAllRealData[0], m_pparrdAllRealData[3], m_pparrdAllRealData[7], m_pparrdAllRealData[8], m_nSizeofChRealData,
                        dSampleInterval, m_parrdScore );
        break;
    }
    cout << "analyzing finished!" << endl;
    return 0;
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
double* CFaultAnalyzer::GetScore( double *arrdTransformRatio )
{
    if( __AnalyzeFault( arrdTransformRatio ) == 0 )
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
        m_nSizeofChRealData = (nChDataLength--);
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
    m_parrdScore = NULL;
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
        free( m_pparrdAllBaseData[nCH] );
        free( m_pparrdAllRealData[nCH] );
        m_pparrdAllBaseData[nCH] = NULL;
        m_pparrdAllRealData[nCH] = NULL;
    }
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
