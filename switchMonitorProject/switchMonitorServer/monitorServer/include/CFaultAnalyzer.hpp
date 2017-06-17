#ifndef CFAULTANALYZER_HPP
#define CFAULTANALYZER_HPP

#include <string>

using namespace std;
enum SWITCH_TYPE { ZD6, S700K, ZYJ7 };

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
    CFaultAnalyzer( string strPath, SWITCH_TYPE emTypeofSwitch );
    ~CFaultAnalyzer();

    double* GetScore( double *arrdTransformRatio );
    double* GetRealData( void );
    int SaveRealData( double *arrdTransformRatio );
    int SaveAfterPreProcessing( double *arrdTransformRatio );

private:
    string m_strPath;
    double *m_pparrdAllRealData[12];
    double *m_pparrdAllBaseData[12];
    int m_arrnAllChDataLen[12];
    int m_nSizeofChRealData;
    double* m_parrdV1RealData;
    double* m_parrdI1RealData;
    SWITCH_TYPE m_emTypeofSwitch;
    double* m_parrdScore;
    bool m_bisLtoR;
    int m_nsizeofRealData;
    bool m_bisTransformed;

    int __TransformRawData( double *parrdTranRatio );
    int __AnalyzeFault( double *parrdTranRatio );
};

#endif // CFAULTANALYZER_HPP
