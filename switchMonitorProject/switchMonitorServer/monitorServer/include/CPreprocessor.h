#ifndef CPreprocessor_H
#define CPreprocessor_H

#include <string>
#include <vector>

using namespace std;

class CPreprocessor
{
private:
    int __RMS( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, int nSubLength );

public:
    CPreprocessor();
    ~CPreprocessor();

    int doPreprocess( double* parrdDataIn, vector<double>* pvecdDataOut, int nLength, int nSubLength, string strAlgorithm );

};

#endif // CPreprocessor_H
