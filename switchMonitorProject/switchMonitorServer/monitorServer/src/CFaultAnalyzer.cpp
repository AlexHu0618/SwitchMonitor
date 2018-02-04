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
    double szdVoltageRMS[9] = {0};
    int nResult = __JudgePosL2R( szdVoltageRMS );
    cout << "judge is ok" << endl;
    if( nResult < 0 )
    {
        cerr << "ERROR! Function JudgePosL2R() return error!" << endl;
        return -1;
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

    //!< read the trigger base data
    int nNumSample = MAX_BASE_LENGTH;
    int nBaseDataLen;
    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        stringstream stream;
        stream << nCH;
        string strChNum = stream.str();
        string strFilePath = "BaseData_LR\\DataCH" + strChNum + ".txt";
        if( !m_bisL2R )
        {
            strFilePath = "BaseData_RL\\DataCH" + strChNum + ".txt";
        }
        ifstream stmBaseDataFileIn( strFilePath, ios_base::in );
        if (!stmBaseDataFileIn.is_open())
        {
            cerr << "Fail to open the " << strFilePath << " base data file! Maybe it is not existed" << endl;
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
        m_arrnAllChBaseDataLen[nCH] = (--nBaseDataLen);
        stmBaseDataFileIn.close();
    }

    //!< read the static base data
    string strStaticFilePath = "BaseData_LR\\DataStatic.txt";
    if( !m_bisL2R )
    {
        strStaticFilePath = "BaseData_RL\\DataStatic.txt";
    }
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
//    m_parrdScore = (double *)malloc( 10*sizeof(double) );
    double dSampleInterval = SAMPLE_INTERVAL;
    //!< trigger score
    if( m_emTypeofAcq == Ttrigger )
    {
        switch (m_emTypeofSwitch)
        {
        case ZD6:
            if ( m_bisL2R )   // L2R
            {
                GetScores_ZD6( m_pparrdAllBaseData[2], m_pparrdAllBaseData[0], m_arrnAllChBaseDataLen[0], m_pparrdAllRealData[2], m_pparrdAllRealData[0], m_nAllChRealDataLen, dSampleInterval, m_parrdScore );
            }
            else
            {
                GetScores_ZD6( m_pparrdAllBaseData[2], m_pparrdAllBaseData[1], m_arrnAllChBaseDataLen[1], m_pparrdAllRealData[2], m_pparrdAllRealData[1], m_nAllChRealDataLen, dSampleInterval, m_parrdScore );
            }
            break;
        case S700K:
            GetScores_S700K( m_pparrdAllBaseData[3], m_pparrdAllBaseData[4], m_pparrdAllBaseData[5], m_pparrdAllBaseData[0], m_pparrdAllBaseData[1], m_pparrdAllBaseData[2], m_arrnAllChBaseDataLen[0],
                             m_pparrdAllRealData[3], m_pparrdAllRealData[4], m_pparrdAllRealData[5], m_pparrdAllRealData[0], m_pparrdAllRealData[1], m_pparrdAllRealData[2], m_nAllChRealDataLen, dSampleInterval, m_parrdScore );
            break;
        case ZYJ7:
            GetScores_ZYJ7( m_pparrdAllBaseData[4], m_pparrdAllBaseData[5], m_pparrdAllBaseData[6], m_pparrdAllBaseData[1], m_pparrdAllBaseData[0], m_pparrdAllBaseData[3], m_pparrdAllBaseData[7], m_pparrdAllBaseData[8], nBaseDataLen,
                            m_pparrdAllRealData[4], m_pparrdAllRealData[5], m_pparrdAllRealData[6], m_pparrdAllRealData[1], m_pparrdAllRealData[0], m_pparrdAllRealData[3], m_pparrdAllRealData[7], m_pparrdAllRealData[8], m_nAllChRealDataLen,
                            dSampleInterval, m_parrdScore );
            break;
        }
        //!< copy the last static score to trigger score
        m_parrdScore[0] = m_szdLastScore[0];
        m_parrdScore[2] = m_szdLastScore[2];
    }
    //!< static score, we calculate all data by RMS, but the static voltage is just the DC value
    //!< measured by multimeter, so the RMS value should multiply 0.65
    else
    {
        double static_base_v1 = arrdStaticData[0]*0.65;
        double static_base_v2 = arrdStaticData[1]*0.65;
        double static_base_v3 = arrdStaticData[2]*0.65;
        double static_data_v1 = -szdVoltageRMS[0]*0.65;
        double static_data_v2 = -szdVoltageRMS[1]*0.65;
        double static_data_v3 = -szdVoltageRMS[2]*0.65;

        cout << "the staticVol: baseV1=" << static_base_v1 << "; baseV2=" << static_base_v2 << "; baseV3=" << static_base_v3 << endl;
        cout << "the staticVOl: dataV1=" << static_data_v1 << "; dataV2=" << static_data_v2 << "; dataV3=" << static_data_v3 << endl;

        switch( m_emTypeofSwitch )
        {
        case ZD6:
            GetStaticScores_ZD6( static_base_v1, static_base_v2, static_data_v1, static_data_v2, m_parrdScore );
            break;
        case S700K:
            GetStaticScores_S700K( static_base_v1, static_base_v2, static_base_v3, static_data_v1, static_data_v2, static_data_v3, m_parrdScore );
            break;
        }
        cout << "static score = " << m_parrdScore[0] << "\t" << m_parrdScore[2] << endl;
        //!< copy the last trigger score to static score
        m_parrdScore[1] = m_szdLastScore[1];
        m_parrdScore[3] = m_szdLastScore[3];
        m_parrdScore[4] = m_szdLastScore[4];
        m_parrdScore[5] = m_szdLastScore[5];
        m_parrdScore[6] = m_szdLastScore[6];
        m_parrdScore[7] = m_szdLastScore[7];
    }

    // clean the score if it is default
    __JudgeDefault( );
    if (m_bIsDefault)
    {
        if (m_emTypeofAcq == Tstatic)
        {
            m_parrdScore[0] = 0;
            m_parrdScore[2] = 0;
        }
        else
        {
            m_parrdScore[1] = 0;
            m_parrdScore[3] = 0;
            m_parrdScore[4] = 0;
            m_parrdScore[5] = 0;
            m_parrdScore[6] = 0;
            m_parrdScore[7] = 0;
        }
    }

    for (int i=0; i<8; ++i)
    {
        m_szdLastScore[i] = m_parrdScore[i];
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
    return NULL;
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
double* CFaultAnalyzer::GetScore( double *arrdTransformRatio, string strPath, SWITCH_TYPE emTypeofSwitch )
{
    m_bisL2R = true;
    m_bisTransformed = false;
    m_bIsDefault = false;
    m_strPath = strPath;
    m_emTypeofSwitch = emTypeofSwitch;
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
        m_nAllChRealDataLen = (nChDataLength--);
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
 CFaultAnalyzer::CFaultAnalyzer( CSqlController *pDBCtrler )
{
    m_bisL2R = true;
    m_bisTransformed = false;
    m_emTypeofAcq = Ttrigger;
    m_pDBCtrler = pDBCtrler;
    m_bIsDefault = false;
    m_szFlagofSetDefault = 0b00000000;    // bits: 0 0 0 0 staticL2R staticR2L triggerL2R triggerR2L, 0--normal;1--should set
    m_parrdScore = (double *)malloc( 10*sizeof(double) );
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
        strFilePath = "BaseData_LR";
        if( !m_bisL2R )
        {
            strFilePath = "BaseData_RL";
        }
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
            for (int nNum = 0; nNum < m_nAllChRealDataLen; ++nNum)
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
    //!< save static value or dynamic data, just voltage channel
    if( m_emTypeofAcq == Tstatic )       // save static datas
    {
        int nNumChannel = 2;
        switch (m_emTypeofSwitch)
        {
        case S700K:
            nNumChannel = 3;
            break;
        case ZYJ7:
            nNumChannel = 9;
            break;
        case ZD6:
            nNumChannel = 2;
            break;
        }

        string strDataFilePath = strFilePath + "\\DataStatic" + ".txt";
        ifstream istmRawDataFileOut( strDataFilePath, ios_base::in );
        if( !istmRawDataFileOut.is_open() )
        {
            cerr << "Fail to open the data file!" << endl;
            return -1;
        }
        double arrd[9]={0};
        for( int i=0;i<nNumChannel;++i )
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
        for( int i=0;i<nNumChannel;++i )
        {
            arrd[i] = m_szdVoltageRMS[i];
        }

        for(int i=0;i<nNumChannel;++i)
        {
            ostmRawDataFileOut << arrd[i] << "\n";
        }
        ostmRawDataFileOut.close();
        cout << "save to file: " << strDataFilePath << endl;
    }
    else                          // save trigger datas
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

        for (int nCH = 0; nCH < nNumChannel; ++nCH)
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
            for (int nNum = 0; nNum < m_nAllChRealDataLen; ++nNum)
            {
                stmRawDataFileOut << parrdChRealData[nNum] << "\n";
            }
            stmRawDataFileOut.close();
            cout << "save to file: " << strDataFilePath << endl;
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
    int nResult = -1;
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

    // save default data if it is need
    if (m_bIsDefault)
    {
        nResult = SaveRealData( arrdTransformRatio );
        if (nResult != 0)
        {
            cout << "save real data fail!" << endl;
            return 1;
        }
    }

    // caculate data RMS and save
    vector<double> vecDataOut;
    CPreprocessor* preprocessor = new CPreprocessor();

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
        nResult = preprocessor->doPreprocess( parrdChRealData, &vecDataOut, m_nAllChRealDataLen, 200, "RMS" );
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

    //!< save power for S700K
    if (m_emTypeofSwitch == S700K)
    {
        vector<double> vecPowerData;
        vecPowerData.clear();
        if (__CalculatePower( &vecPowerData) != 0)
        {
            cout << "ERROR in function __CalculatePower(), no save power data!" << endl;
            return -1;
        }
        string strPowerFilePath = m_strPath + "\\Power.txt";
        ofstream stmPowerDataFileOut( strPowerFilePath, ios_base::out );
        if( !stmPowerDataFileOut.is_open() )
        {
            cerr << "Fail to open the file Power.txt!" << endl;
            return -1;
        }
        if( !vecPowerData.empty() )
        {
            vector<double>::iterator it;
            for( it=vecPowerData.begin(); it!=vecPowerData.end(); ++it )
            {
                stmPowerDataFileOut << *it << "\n";
            }
        }
        else
        {
            cout << "vector of power is empty" << endl;
            return -1;
        }

        stmPowerDataFileOut.close();
        cout << "save to file: " << strPowerFilePath << endl;
    }

    delete preprocessor;
    preprocessor = NULL;
    return 0;
}

int CFaultAnalyzer::__JudgePosL2R( double* pszdVoltageRMS )
{
    vector<double> vecDataOut;
    int nNumChannel = 2;
    int nNumBegin = 20000;
    int nNumEnd = 21000;  // intercept 50ms from Num begin to end for judging
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
        break;
    }

    //!< RMS data between nNumBegin to nNumEnd and get the average value
    CPreprocessor* preprocessor = new CPreprocessor();
    double szdVS700K1000[3]={0};
    for (int nCH = 0; nCH < nNumChannel; ++nCH)
    {
        double *parrdChRealData = m_pparrdAllRealData[nCH];
        int nResult = -1;
        double dSum = 0;
        vecDataOut.clear();
        // RMS
        nResult = preprocessor->doPreprocess( parrdChRealData, &vecDataOut, m_nAllChRealDataLen, 200, "RMS" );
        if( nResult != 0 )
        {
            cout << "doPreprocess() error!" << endl;
            delete preprocessor;
            preprocessor = NULL;
            return -1;
        }
        // average
        if( !vecDataOut.empty() )
        {
            for( int nNum=nNumBegin; nNum<nNumEnd; ++nNum )
            {
                dSum += vecDataOut[nNum]*vecDataOut[nNum];
            }
            pszdVoltageRMS[nCH] = sqrt( dSum/(nNumEnd-nNumBegin) );
            cout << "Voltage of CH" << nCH << " is: " << pszdVoltageRMS[nCH] << "V" << endl;

            //!< just for S700K judging whether is L2R in static
            if( m_emTypeofSwitch == S700K )
            {
                double dSum1000=0;
                for( int nNum=1000; nNum<2000; ++nNum )
                {
                    dSum1000 += vecDataOut[nNum]*vecDataOut[nNum];
                }
                szdVS700K1000[nCH] = sqrt( dSum1000/1000 );
            }
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

    //!< judge status(static or trigger) and isL2R
    switch (m_emTypeofSwitch)
    {
    case S700K:
        //!< static, sample 20000~21000
        if( ( pszdVoltageRMS[0] + pszdVoltageRMS[1] + pszdVoltageRMS[2]) < 300 )
        {
            cout << "It is Static!" << endl;
            m_emTypeofAcq = Tstatic;
            if( pszdVoltageRMS[0]>pszdVoltageRMS[1] && pszdVoltageRMS[2]>pszdVoltageRMS[1] )
            {
                cout << "It is L2R, at position L!" << endl;
                m_bisL2R = true;
            }
            else
            {
                cout << "It is R2L, at position R!" << endl;
                m_bisL2R = false;
            }
            m_szdVoltageRMS[0] = pszdVoltageRMS[0];
            m_szdVoltageRMS[1] = pszdVoltageRMS[1];
            m_szdVoltageRMS[2] = pszdVoltageRMS[2];
        }
        //!< trigger, sample 1000~2000
        else
        {
            cout << "It is Trigger!" << endl;
            m_emTypeofAcq = Ttrigger;
            cout << "The value of position 1000 is: " << szdVS700K1000[0] << " " << szdVS700K1000[1] << " " << szdVS700K1000[2] << endl;
            if( szdVS700K1000[0]>szdVS700K1000[1] && szdVS700K1000[2]>szdVS700K1000[1] )
            {
                cout << "It is L2R, at position L!" << endl;
                m_bisL2R = true;
            }
            else
            {
                cout << "It is R2L, at position R!" << endl;
                m_bisL2R = false;
            }
        }
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
            }
            else
            {
                cout << "it is L2R!" << endl;
                m_bisL2R = true;
                m_szdVoltageRMS[0] = -pszdVoltageRMS[0];
                m_szdVoltageRMS[1] = -pszdVoltageRMS[1];
            }
        }
        else
        {//Trig
            cout << "it is Trigger!" << endl;
            m_emTypeofAcq = Ttrigger;
            if( pszdVoltageRMS[0] < pszdVoltageRMS[1] )
            {
                cout << "it is R2L!" << endl;
                m_bisL2R = false;
            }
            else
            {
                cout << "it is L2R!" << endl;
                m_bisL2R = true;
            }
        }
        break;
    }
    return 0;
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
    m_szFlagofSetDefault = 0b00001111;
    cout << "m_szFlagofSetDefault = 0x0" << hex << m_szFlagofSetDefault << endl;
}

int CFaultAnalyzer::__CalculatePower( vector<double>* pvecPowerData )
{
    int nChVol = 2;
    switch( m_emTypeofSwitch )
    {
    case ZD6:
        nChVol = 2;
        break;
    case S700K:
        nChVol = 3;
        break;
    case ZYJ7:
        nChVol = 4;
        break;
    default:
        nChVol = 2;
    }

    double *parrdPower = (double *)malloc( m_nAllChRealDataLen*sizeof(double) );
    double nTem = 0;
    for( int i=0; i<m_nAllChRealDataLen; ++i )
    {
        for( int j=0; j<nChVol; ++j )
        {
            nTem += m_pparrdAllRealData[j][i] * m_pparrdAllRealData[j+nChVol][i];
        }
        parrdPower[i] = nTem;
        nTem = 0;
    }
    CPreprocessor* preprocessor = new CPreprocessor();
    int nResult = preprocessor->doPreprocess( parrdPower, pvecPowerData, m_nAllChRealDataLen, 200, "RMS" );
    if( nResult != 0 )
    {
        cout << "caculate power doPreprocess() error!" << endl;
        return -1;
    }

    delete preprocessor;
    preprocessor = NULL;


    free( parrdPower );
    parrdPower = NULL;

    return 0;
}

void CFaultAnalyzer::__JudgeDefault( void )
{
    m_bIsDefault = false;
    if (m_szFlagofSetDefault != 0b00000000)
    {
        cout << "FlagofSetDefault = 0x0" << hex << m_szFlagofSetDefault << endl;
        int nResult = 0;
        if (m_emTypeofAcq == Tstatic)    // static Acq
        {
            if (m_bisL2R)
            {
                if (m_szFlagofSetDefault&0b00001000)  // staticL2R
                {
                    m_bIsDefault = true;
                    m_szFlagofSetDefault = m_szFlagofSetDefault&0b11110111;
                }
            }
            else
            {
                if (m_szFlagofSetDefault&0b00000100)  // staticR2L
                {
                    m_bIsDefault = true;
                    m_szFlagofSetDefault = m_szFlagofSetDefault&0b11111011;
                }
            }
        }
        else
        {
            if (m_bisL2R)
            {
                if (m_szFlagofSetDefault&0b00000010)  // triggerL2R
                {
                    m_bIsDefault = true;
                    m_szFlagofSetDefault = m_szFlagofSetDefault&0b11111101;
                    cout << "FlagofSetDefault = 0x0" << hex << m_szFlagofSetDefault << endl;
                }
            }
            else
            {
                if (m_szFlagofSetDefault&0b00000001)  // triggerR2L
                {
                    m_bIsDefault = true;
                    m_szFlagofSetDefault = m_szFlagofSetDefault&0b11111110;
                }
            }
        }
    }
    else
    {
        m_bIsDefault = false;
        cout << "m_szFlagofSetDefault=0x00" << endl;
    }
}

int CFaultAnalyzer::Diagnosing( SWITCH_TYPE emTypeofSwitch, wxString strPath )
{
    string strDataDirPath = strPath.ToStdString();

    //double arrdTransformRatioZD6[] = { 300.0, 300.0, 17.857 };    // {v1,v2,i} 3.0=300V, i=17.857A , for before experiment data file "ZD6(old&new)"
    double arrdTransformRatioZD6[] = { 600, 600, 40 };
    double arrdTransformRatioS700K[] = { 600, 600, 600, 40, 40, 40 };  //{v,v,v,i,i,i}
    double arrdTransformRatioZYJ7[] = { 600, 600, 600, 600, 40, 40, 40, 3000, 3000 };   // {v,v,v,v,i,i,i,p,p}
    double *parrdTransformRatio = arrdTransformRatioZD6;
    if( emTypeofSwitch == S700K )
    {
        parrdTransformRatio = arrdTransformRatioS700K;
    }
    if( emTypeofSwitch == ZYJ7 )
    {
        parrdTransformRatio = arrdTransformRatioZYJ7;
    }

    double *parrdScore = NULL;
    parrdScore = this->GetScore( parrdTransformRatio, strDataDirPath, emTypeofSwitch );

    printf("SWITCH fault confidences for provided data:\n\n");
    printf("Actuating fault: %.2f %%\n", parrdScore[0]);
    printf("Engage difficult: %.2f %%\n", parrdScore[1]);
    printf("Indicating fault: %.2f %%\n", parrdScore[2]);
    printf("Jam: %.2f %%\n", parrdScore[3]);
    printf("Motor fault: %.2f %%\n", parrdScore[4]);
    printf("Movement resistance: %.2f %%\n", parrdScore[5]);
    printf("Power fault: %.2f %%\n", parrdScore[6]);
    printf("Unlock difficult: %.2f %%\n", parrdScore[7]);

    cout << "Analyzing is successful!" << endl;

//    //!< change default data, here just save the real data not preprocess data to the default directory
//    if( m_bIsDefault )
//    {
//        m_pAnalyzer->SetBaseData( true );
////        int result = m_pAnalyzer->SaveRealData( parrdTransformRatio );
////        if (result != 0)
////        {
////            cout << "save real data fail!" << endl;
////            return 1;
////        }
//        m_bIsDefault = false;
//    }

    //!< save preprocessing data
    int nResult = this->SaveAfterPreProcessing( parrdTransformRatio );
    if( nResult != 0 )
    {
        cout << "Error, fail to save preprocessing data!" << endl;
        return 1;
    }
    else
    {
        cout << "Preprocessing data was successfully saved!" << endl;
    }

    //!< insert into DB
    cout << strPath << endl;
    wxString wstrTime = strPath.AfterLast('\\');
    wstrTime.Replace("-",":");
    string strTime = string( wstrTime.mb_str() );
    cout << strTime << endl;
    wxString wstrDate = strPath.BeforeLast('\\').AfterLast('\\');
    string strDate = string( wstrDate.mb_str() );
    cout << strDate << endl;
    string strTYPE = (m_emTypeofSwitch == ZD6) ? "ZD6" : (m_emTypeofSwitch == S700K) ? "S700K" : "ZYJ7";
    string strTypeofAcq = "trigger";
    int nIsL2R = 1;
    this->GetInfo( &strTypeofAcq, &nIsL2R );
    wxString wstrIsL2R = wxString::Format(wxT("%i"), nIsL2R );
    string strIsL2R = std::string(wstrIsL2R.mbc_str());
    string arrstrScore[10] = {"0"};
    for( int i=0;i<8;++i )
    {
        arrstrScore[i] = wxString::Format(wxT("%f"), parrdScore[i]);
    }
    wxString strTmp(strDataDirPath);
    strTmp.Replace("\\","\\\\");
    strDataDirPath = strTmp.mb_str();
    string strDBCmd = "INSERT INTO tab4alldata(TYPE, DATE, TIME, PATH, STATUS, ISL2R, ACTUATING, ENGAGE, \
INDICATING, JAM, MOTOR, MOVEMENT, POWERERR, UNLOCKERR) VALUES ('" + strTYPE + "', '" + strDate + "', '"
+ strTime + "', '" + strDataDirPath + "', '" + strTypeofAcq + "', '" + strIsL2R +"', "
+ arrstrScore[0] + ", " + arrstrScore[1] + ", " + arrstrScore[2] + ", " + arrstrScore[3] + ", "
+ arrstrScore[4] + ", " + arrstrScore[5] + ", " + arrstrScore[6] + ", " + arrstrScore[7] + ");";
    cout << strDBCmd << endl;
    nResult = m_pDBCtrler->Insert( strDBCmd );
    if( nResult < 0 )
    {
        cout << "sql update error" << endl;
        return -1;
    }
    else
    {
//        SendMSG2UI( "MSG=UPDATE\r\n", 12);
        cout << "Database updata success!" << endl;
    }

    return 0;
}
