//#include "stdafx.h"
#include "conio.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <math.h>

//#pragma comment(lib,"ManagedDll.lib")
extern "C" _declspec(dllexport) void GetScores_S700K(double *base_i1, double *base_i2, double *base_i3, double *base_v1, double *base_v2, double *base_v3, int base_length, double *data_i1, double *data_i2, double *data_i3, double *data_v1, double *data_v2, double *data_v3, int data_length, double sampleinterval, double* scores);
extern "C" _declspec(dllexport) void GetScores_ZD6(double *base_i1, double *base_v1, int base_length, double *data_i1, double *data_v1, int data_length, double sampleinterval, double* scores);
extern "C" _declspec(dllexport) void GetScores_ZYJ7(double *base_i1, double *base_i2, double *base_i3, double *base_v1, double *base_v2, double *base_v3, double* base_p1, double* base_p2, int base_length, double *data_i1, double *data_i2, double *data_i3, double *data_v1, double *data_v2, double *data_v3, double* data_p1, double* data_p2, int data_length, double sampleinterval, double* scores);

int main(int argc,char* argv[])
{
	double* base_i1;
	double* base_v1;
	double* data_i1;
	double* data_v1;

	double sampleinterval = 0.00005;  //0.0000488;

	int base_length = 74825;
	int data_length = 600000;
	char dataFilePath[100] = {0};
	bool isLR = true;
    for(int i=0;i<strlen(argv[1]);++i)
    {
        dataFilePath[i] = argv[1][i];
    }

	int len = strlen(dataFilePath);
	if(dataFilePath[len-1] == 'L')
    {
        isLR = false;
        base_length = 75555;
    }

    base_i1 = (double*)malloc(base_length * sizeof(double));
	base_v1 = (double*)malloc(base_length * sizeof(double));
	data_i1 = (double*)malloc(data_length * sizeof(double));
	data_v1 = (double*)malloc(data_length * sizeof(double));

	/**< build the v1_data file path for the function rbv1() */
	char* v1_dataFile = NULL;
	if(isLR)
    {
        v1_dataFile = "\\Data01.dat";
    }else
    {
        v1_dataFile = "\\Data00.dat";
    }
    for(int i=0;i<strlen(v1_dataFile);++i)
    {
        dataFilePath[len++] = *(v1_dataFile+i);
    }
    dataFilePath[len] = '\0';
    std::fstream rdv1(dataFilePath, std::ios_base::in);
    std::cout << "Voltage data: " << dataFilePath << std::endl;
    int v1DataCounter = 0;
    while(rdv1.peek()!=EOF)
    {
        rdv1 >> data_v1[v1DataCounter];
        data_v1[v1DataCounter] = (double)(data_v1[v1DataCounter]*3/pow(2,17));
        ++v1DataCounter;
    }
    v1DataCounter--;

    /**< build the i1_data file path for the function rbi1() */
    dataFilePath[len-5] = '2';
    std::fstream rdi1(dataFilePath, std::ios_base::in);
    std::cout << "Current data: " << dataFilePath << std::endl;
    int i1DataCounter = 0;
    while(rdi1.peek()!=EOF)
    {
        rdi1 >> data_i1[i1DataCounter];
        data_i1[i1DataCounter] = (double)(data_i1[i1DataCounter]*17.857/pow(2,17));
        ++i1DataCounter;
    }
    i1DataCounter--;

    std::fstream rbi1;
    if(isLR)
    {
        rbi1.open("zd6_i1_base_LR.txt", std::ios_base::in);
    }else
    {
        rbi1.open("zd6_i1_base_RL.txt", std::ios_base::in);
    }
	for (int i = 0; i < base_length; i++)
		rbi1 >> base_i1[i];

    std::fstream rbv1;
    if(isLR)
    {
        rbv1.open("zd6_v1_base_LR.txt", std::ios_base::in);
    }else
    {
        rbv1.open("zd6_v1_base_RL.txt", std::ios_base::in);
    }
	for (int i = 0; i < base_length; i++)
		rbv1 >> base_v1[i];

    double* scores;
	scores = (double*)malloc(10 * sizeof(double));
	GetScores_ZD6(base_i1, base_v1, base_length, data_i1, data_v1, v1DataCounter, sampleinterval, scores);


//	int base_length = 74825;   //70000;
//	int data_length = 77380;  //77380;  //498955;   //82000;
//
//	base_i1 = (double*)malloc(base_length * sizeof(double));
//	base_v1 = (double*)malloc(base_length * sizeof(double));
//
//	data_i1 = (double*)malloc(data_length * sizeof(double));
//	data_v1 = (double*)malloc(data_length * sizeof(double));
//
//	std::fstream rbi1("zd6_i1_base.txt", std::ios_base::in);
//	for (int i = 0; i < base_length; i++)
//		rbi1 >> base_i1[i];
//
//	std::fstream rbv1("zd6_v1_base.txt", std::ios_base::in);
//	for (int i = 0; i < base_length; i++)
//		rbv1 >> base_v1[i];
//
//	std::fstream rdi1("zd6_i1_data.txt", std::ios_base::in);
//	for (int i = 0; i < data_length; i++)
//		rdi1 >> data_i1[i];
//
//	std::fstream rdv1("zd6_v1_data.txt", std::ios_base::in);
//	for (int i = 0; i < data_length; i++)
//		rdv1 >> data_v1[i];
//
//	double* scores;
//	scores = (double*)malloc(10 * sizeof(double));
//	GetScores_ZD6(base_i1, base_v1, base_length, data_i1, data_v1, data_length, sampleinterval, scores);

	printf("ZD6 fault confidences for provided data:\n\n");
	printf("Actuating fault: %.2f %%\n", scores[0]);
	printf("Engage difficult: %.2f %%\n", scores[1]);
	printf("Indicating fault: %.2f %%\n", scores[2]);
	printf("Jam: %.2f %%\n", scores[3]);
	printf("Motor fault: %.2f %%\n", scores[4]);
	printf("Movement resistance: %.2f %%\n", scores[5]);
	printf("Power fault: %.2f %%\n", scores[6]);
	printf("Unlock difficult: %.2f %%\n", scores[7]);

	/*
	printf("Unused: %f\n", scores[8]);
	printf("Unused: %f\n", scores[9]);*/

	free(base_i1);
	free(data_i1);
	free(base_v1);
	free(data_v1);
	free(scores);
	base_i1 = NULL;
	data_i1 = NULL;
	base_v1 = NULL;
	data_v1 = NULL;
	scores = NULL;

	printf("\n");
	system("pause");
	return 0;
}
