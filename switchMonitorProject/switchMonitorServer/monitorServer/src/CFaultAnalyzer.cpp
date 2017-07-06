#include "CFaultAnalyzer.hpp"
#include "CPreprocessor.h"
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

extern "C" _declspec(dllexport) void GetStaticScores_S700K(double base_v1, double base_v2, double base_v3, double data_v1, double data_v2, double data_v3, double* scores);
extern "C" _declspec(dllexport) void GetStaticScores_ZD6(double base_v1, double base_v2, double data_v1, double data_v2, double* scores);


/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::__AnalyzeFault( double *arrdTransformRatio )
{
    if( !m_bisTransformed )
    {
        if (__TransformRawData( arrdTransformRatio ) != 0)
        {
            cout << "Fail to Transform raw data!" << endl;
            return -1;
        }
    }

    //!< judge Position whether is L2R and type of acquicition is static or trigger
    double szdVoltageRMS[4] = {0};
    int nResult = __JudgePosL2R( szdVoltageRMS );
    cout << "judge is ok" << endl;
    if( nResult < 0 )
    {
        cerr << "ERROR! Function JudgePosL2R() return error!" << endl;
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

    //!< read the trigger base data
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

    //!< read the static base data
    string strStaticFilePath = "BaseData\\DataStatic.txt";
    ifstream stmStaticBaseDataFileIn( strStaticFilePath, ios_base::in );
    if (!stmStaticBaseDataFileIn.is_open())
    {
        cerr << "Fail to open the " << strStaticFilePath << " static base data file!" << endl;
        return -1;
    }
    double arrdStaticData[5] = {0};
    int i=0;
    while (stmStaticBaseDataFileIn.peek() != EOF)
    {
        stmStaticBaseDataFileIn >> arrdStaticData[i++];
    }

    //!< analyzing the fault result
    m_parrdScore = (double *)malloc( 10*sizeof(double) );
    double dSampleInterval = SAMPLE_INTERVAL;
    if( m_emTypeofAcq == Ttrigger )
    {
        switch (m_emTypeofSwitch)
        {
        case ZD6:
            if ( m_bisL2R )   // L2R
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
    }
    else
    {
        double static_base_v1 = 0;
        double static_base_v2 = 0;
        double static_data_v1 = -szdVoltageRMS[0];
        double static_data_v2 = -szdVoltageRMS[1];
        if( m_bisL2R )
        {
            static_base_v1 = arrdStaticData[0];
            static_base_v2 = arrdStaticData[1];
        }
        else
        {
            static_base_v1 = arrdStaticData[2];
            static_base_v2 = arrdStaticData[3];
        }
        cout << "the staticVol: baseV1=" << static_base_v1 << "; baseV2=" << static_base_v2 << endl;
        cout << "the staticVOl: dataV1=" << static_data_v1 << "; dataV2=" << static_data_v2 << endl;
        GetStaticScores_ZD6(static_base_v1, static_base_v2, static_data_v1, static_data_v2, m_parrdScore);
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
    m_bisTransformed = true;
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
    m_bisL2R = true;
    m_bisTransformed = false;
    m_emTypeofAcq = Ttrigger;
    m_bIsDefault = false;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
int CFaultAnalyzer::SaveRealData( double *arrdTransformRatio )
{
    if( !m_bisTransformed )
    {
        if( __TransformRawData( arrdTransformRatio ) != 0 )
        {
            cout << "Fail to Transform raw data!" << endl;
            return -1;
        }
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
    string strFilePath = m_strPath;
    if( m_bIsDefault )
    {
        strFilePath = "BaseData";
        __SaveBaseData( strFilePath );
    }
    else
    {
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
    }
    return 0;
}

int CFaultAnalyzer::__SaveBaseData( string strFilePath )
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

    if( m_emTypeofAcq == Tstatic )
    {
        string strDataFilePath = strFilePath + "\\DataStatic" + ".txt";
        ifstream istmRawDataFileOut( strDataFilePath, ios_base::in );
        if( !istmRawDataFileOut.is_open() )
        {
            cerr << "Fail to open the data file!" << endl;
            return -1;
        }
        double arrd[4]={0};
        for(int i=0;i<4;++i)
        {
            istmRawDataFileOut >> arrd[i];
        }
        istmRawDataFileOut.close();

        ofstream ostmRawDataFileOut( strDataFilePath, ios_base::out );
        if( !ostmRawDataFileOut.is_open() )
        {
            cerr << "Fail to open the data file!" << endl;
            return -1;
        }
        if( m_bisL2R )
        {
            arrd[0] = m_szdVoltageRMS[0];
            arrd[1] = m_szdVoltageRMS[1];
            arrd[2] = 0;
            arrd[3] = 0;
        }
        else
        {
            arrd[0] = 0;
            arrd[1] = 0;
            arrd[2] = m_szdVoltageRMS[0];
            arrd[3] = m_szdVoltageRMS[1];
        }

        for(int i=0;i<4;++i)
        {
            ostmRawDataFileOut << arrd[i] << "\n";
        }
        ostmRawDataFileOut.close();
        cout << "save to file: " << strDataFilePath << endl;
    }
    else
    {
        for (int nCH = 0; nCH < nNumChannel; ++nCH)
        {
            if( m_bisL2R )
            {
                if( nCH != 1 )
                {
                    stringstream stream;
                    stream << nCH;
                    string strChNum = stream.str();
                    string strDataFilePath = strFilePath + "\\DataCH" + strChNum + ".txt";
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
            }
            else
            {
                if( nCH != 0 )
                {
                    stringstream stream;
                    if( nCH == 2 )
                    {
                        stream << 3;
                    }
                    else
                    {
                        stream << nCH;
                    }
                    string strChNum = stream.str();
                    string strDataFilePath = strFilePath + "\\DataCH" + strChNum + ".txt";
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
            }

        }
    }
    return 0;
}

int CFaultAnalyzer::SaveAfterPreProcessing( double *arrdTransformRatio )
{
    if( !m_bisTransformed )
    {
        if( __TransformRawData( arrdTransformRatio ) != 0 )
        {
            cout << "Fail to Transform raw data!" << endl;
            return -1;
        }
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
    vector<double> vecDataOut;
    CPreprocessor* preprocessor = new CPreprocessor();
    int nResult = -1;

    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        stringstream stream;
        stream << nCH;
        string strChNum = stream.str();
        string strDataFilePath = m_strPath + "\\CH" + strChNum + ".txt";
        ofstream stmRawDataFileOut( strDataFilePath, ios_base::out );
        if( !stmRawDataFileOut.is_open() )
        {
            cerr << "Fail to open the file CH" << nCH << ".txt!" << endl;
            return -1;
        }

        double *parrdChRealData = m_pparrdAllRealData[nCH];
        vecDataOut.clear();
        nResult = preprocessor->doPreprocess( parrdChRealData, &vecDataOut, m_nSizeofChRealData, 200, "RMS" );
        if( nResult != 0 )
        {
            cout << "doPreprocess() error!" << endl;
            return -1;
        }
        if( !vecDataOut.empty() )
        {
            vector<double>::iterator it;
            for( it=vecDataOut.begin(); it!=vecDataOut.end(); ++it )
            {
                stmRawDataFileOut << *it << "\n";
            }
        }
        else
        {
            cout << "vector is empty" << endl;
            return -1;
        }

        stmRawDataFileOut.close();
        cout << "save to file: " << strDataFilePath << endl;
    }

    delete preprocessor;
    preprocessor = NULL;
    return 0;
}

int CFaultAnalyzer::__JudgePosL2R( double* pszdVoltageRMS )
{
    vector<double> vecDataOut;
    int nNumChannel = 2;
    int nNumBegin = 0;
    int nNumEnd = 0;
    switch (m_emTypeofSwitch)
    {
    case S700K:
        nNumChannel = 3;   //just voltage
        break;
    case ZYJ7:
        nNumChannel = 4;
        break;
    case ZD6:
        nNumChannel = 2;
        nNumBegin = 20000;
        nNumEnd = 21000;  // time is 50ms from Num begin to end
        break;
    }

    //!< RMS data between nNumBegin to nNumEnd and get the average value
    CPreprocessor* preprocessor = new CPreprocessor();
    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        double *parrdChRealData = m_pparrdAllRealData[nCH];
        int nResult = -1;
        double dSum = 0;
        vecDataOut.clear();
        nResult = preprocessor->doPreprocess( parrdChRealData, &vecDataOut, m_nSizeofChRealData, 200, "RMS" );
        if( nResult != 0 )
        {
            cout << "doPreprocess() error!" << endl;
            delete preprocessor;
            preprocessor = NULL;
            return -1;
        }
        if( !vecDataOut.empty() )
        {
            for( int nNum=nNumBegin; nNum<nNumEnd; ++nNum )
            {
                dSum += vecDataOut[nNum]*vecDataOut[nNum];
            }
            cout << dSum << endl;
            pszdVoltageRMS[nCH] = sqrt( dSum/(nNumEnd-nNumBegin) );
            cout << "Voltage of CH" << nCH << " is: " << pszdVoltageRMS << "V" << endl;
        }
        else
        {
            cout << "vector is empty" << endl;
            delete preprocessor;
            preprocessor = NULL;
            return -1;
        }
    }
    delete preprocessor;
    preprocessor = NULL;

    //!< judge status and isL2R
    switch (m_emTypeofSwitch)
    {
    case S700K:
        break;
    case ZYJ7:
        break;
    case ZD6:
        if( (pszdVoltageRMS[0] + pszdVoltageRMS[1]) < 200 )
        {//static
            cout << "it is Static!" << endl;
            m_emTypeofAcq = Tstatic;
            if( pszdVoltageRMS[0] < pszdVoltageRMS[1] )
            {
                cout << "it is R2L!" << endl;
                m_bisL2R = false;
                m_szdVoltageRMS[0] = pszdVoltageRMS[0];
                m_szdVoltageRMS[1] = pszdVoltageRMS[1];
                return 1;
            }
            cout << "it is L2R!" << endl;
            m_bisL2R = true;
            m_szdVoltageRMS[0] = -pszdVoltageRMS[0];
            m_szdVoltageRMS[1] = -pszdVoltageRMS[1];
            return 0;
        }
        else
        {//Trig
            cout << "it is Trigger!" << endl;
            m_emTypeofAcq = Ttrigger;
            if( pszdVoltageRMS[0] < pszdVoltageRMS[1] )
            {
                cout << "it is R2L!" << endl;
                m_bisL2R = false;
                return 1;
            }
            cout << "it is L2R!" << endl;
            m_bisL2R = true;
            return 0;
        }
        break;
    }
}

void CFaultAnalyzer::GetInfo( string* strTypeofAcq, int* nIsL2R )
{
    if( m_emTypeofAcq == Tstatic )
    {
        *strTypeofAcq = m_bIsDefault?"static_default":"static";
    }
    else
    {
        *strTypeofAcq = m_bIsDefault?"trigger_default":"trigger";
    }
    if( m_bisL2R )
    {
        *nIsL2R = 1;
    }
    else
    {
        *nIsL2R = 0;
    }
}

void CFaultAnalyzer::SetBaseData( bool bIsDefault )
{
    m_bIsDefault = bIsDefault;
}
