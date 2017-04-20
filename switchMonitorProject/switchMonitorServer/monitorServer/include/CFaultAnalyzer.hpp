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

    double* GetScore( void );
    double* GetRealData( void );
    int SaveRealData( double *arrdTransformRatio );

private:
    string m_strPath;
    double *m_pparrdAllRealData[12];
    int m_nSizeofChRealData;
    double* m_parrdV1RealData;
    double* m_parrdI1RealData;
    SWITCH_TYPE m_emTypeofSwitch;
    double* m_parrdScore;
    bool m_bisLtoR;
    int m_nsizeofRealData;

    int __TransformRawData( double *parrdTranRatio );
    int __AnalyzeFault( void );
};

#endif // CFAULTANALYZER_HPP
