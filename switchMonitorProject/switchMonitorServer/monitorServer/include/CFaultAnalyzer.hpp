#ifndef CFAULTANALYZER_HPP
#define CFAULTANALYZER_HPP

#include <string>

using namespace std;
enum SWITCH_TYPE { ZD6, S700K, ZYJ7 };
enum ACQUICITION_TYPE { Tstatic, Ttrigger };

//! \brief
//!
//! \param
//! \param
//! \return
//!
//!
class CFaultAnalyzer
{
public:
    CFaultAnalyzer( void );
    ~CFaultAnalyzer();

    double* GetScore( double *arrdTransformRatio, string strPath, SWITCH_TYPE emTypeofSwitch );
    double* GetRealData( void );
    int SaveRealData( double *arrdTransformRatio );
    int SaveAfterPreProcessing( double *arrdTransformRatio );
    void GetInfo( string* strTypeofAcq, int* nIsL2R );
    void SetBaseData( bool bIsDefault );

private:
    string m_strPath;
    double *m_pparrdAllRealData[12];
    double *m_pparrdAllBaseData[12];
    int m_arrnAllChBaseDataLen[12];
    int m_nAllChRealDataLen;
    double* m_parrdV1RealData;
    double* m_parrdI1RealData;
    SWITCH_TYPE m_emTypeofSwitch;
    ACQUICITION_TYPE m_emTypeofAcq;
    double* m_parrdScore;
    bool m_bisL2R;
    int m_nsizeofRealData;
    bool m_bisTransformed;
    double m_szdVoltageRMS[4] = {0};
    bool m_bIsDefault;
    int m_szFlagofSetDefault;
    double m_szdLastScore[8] = {0};

    int __TransformRawData( double *parrdTranRatio );
    int __AnalyzeFault( double *parrdTranRatio );
    int __JudgePosL2R( double* pdszVoltageRMS );
    int __SaveBaseData( string strFilePath );
    void __CalculatePower( void );
    void __JudgeDefault( void );
};

#endif // CFAULTANALYZER_HPP
