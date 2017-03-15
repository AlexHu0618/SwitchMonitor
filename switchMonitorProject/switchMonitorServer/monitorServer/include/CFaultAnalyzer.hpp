#ifndef CFAULTANALYZER_HPP
#define CFAULTANALYZER_HPP

#include <string>

using namespace std;
enum SwitchType { ZD6, S700K, ZYJ7 };

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
    CFaultAnalyzer( string strPath, SwitchType emSwitchType );
    ~CFaultAnalyzer();

    double* GetScore( void );
    double* GetRealData( void );

private:
    string m_strPath;
    double* m_parrdV1RealData;
    double* m_parrdI1RealData;
    SwitchType m_emSwitchType;
    double* m_parrdScore;
    bool m_bisLtoR;
    int m_nsizeofRealData;
    double m_dsampleInterval;

    int __TransformRawData( void );
    int __AnalyzeFault( void );
};

#endif // CFAULTANALYZER_HPP
