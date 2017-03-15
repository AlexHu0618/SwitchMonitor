#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include "CUdpServer.h"

using namespace std;

int main(int argc,char* argv[])
{
    cout << "Hello world!" << endl;
    static char dataTemp[10000][1470]={0};   //!< the big cache
    int frameCout=0;
    bool isDirAcq = false;
    bool isShowData = false;
    if(argc > 1)
    {
        for(int i=1;i<argc;++i)
        {
            if(strcmp(argv[i],"-dirAcq") == 0)
            {
                isDirAcq = true;
            }
            if(strcmp(argv[i],"-show") == 0)
            {
                isShowData = true;
            }
        }
        if(argc>2?!(isDirAcq && isShowData):(!isDirAcq && !isShowData))
        {
            cout << "Fail to execute the program and exit!" << endl;
            cout << "please try again and input the correct parameter '-dirAcq' or '-show'" << endl;
            exit(1);
        }
    }
//    CUdpServer* UDP=new CUdpServer("10.3.3.116","10.3.3.106",1024);
    CUdpServer* UDP=new CUdpServer("192.168.1.106","192.168.1.154",1024);

    //!< send command to continue asquiring
    char startCmd[4] = {0x03,0x00,0x00,0x03};
    int result = UDP->SendData(startCmd);
    if(result<0)
    {
        cout << "send data false!" << endl;
    }
    cout << "have send command 0X03" << endl;

    //delay 200ms and directly acquire data
    //_sleep(200);
    if(isDirAcq)
    {
        cout << "start to directly acquire 30S" << endl;
        for(int i=0;i<1000;++i)
        {
            result = 0;
        }
        char acqCmd[4] = {0x04,0x00,0x00,0x04};
        result = UDP->SendData(acqCmd);
        if(result<0)
        {
            cout << "send command 0X04 false!" << endl;
        }
        cout << "success to send command 0X04" << endl;
    }

    frameCout = UDP->RecvData(dataTemp);

    //!< send command to stop acquiring
    char stopCmd[4] = {0x02,0x00,0x00,0x02};
    result=UDP->SendData(stopCmd);
    if(result<0)
    {
        cout << "send data false!" << endl;
    }

    //!< change the hex data to int
    static int location[10000][2]={0};
    static int intArray[10000][370]={0};
    int dataNum = (dataTemp[0][4]&0xFF) | (dataTemp[0][5]<<8&0xFF00);
    cout << "to change......" << endl;
    for(int i=0;i<frameCout;i++)
    {
        int byteCouter=0;
        for(int j=8;j<1468;j=j+4)
        {
            intArray[i][byteCouter++] = (dataTemp[i][j]&0xFF) | (dataTemp[i][j+1]<<8&0xFF00) | (dataTemp[i][j+2]<<16&0xFF0000) | (dataTemp[i][j+3]<<24&0xFF000000);
        }
        location[i][0] = dataTemp[i][0] | (dataTemp[i][1]<<8);
        location[i][1] = dataTemp[i][2] | (dataTemp[i][3]<<8);
    }

    //!< show the raw data and the int data
    if(isShowData)
    {
        cout << "to show" << endl;
        for(int i=0;i<frameCout;i++)
        {
            cout << "this is the " << location[i][0] << "--" << location[i][1] << " frame" << endl;
            for(int k=0;k<1500;k++)
            {
                cout << hex << setfill('0') << setw(2) << (unsigned int)(unsigned char)dataTemp[i][k] << " ";
            }
            cout << endl;
            for(int j=0;j<365;j++)
            {
                cout << dec << intArray[i][j] << " ";
            }
            cout << endl << endl;
        }
    }

    //!< save the int data to file
    FILE *f = fopen("AllData.dat","wb");
    if( f==NULL )
    {
        printf("Can not open file AllData.dat.\n");
    }
    else
    {
        printf("has opened the file AllData.dat.\n");
    }
    for(int i=0;i<frameCout;i++)
    {
        fprintf(f,"\n");
        fprintf(f,"this is %d--%d frame\n",location[i][0],location[i][1]);
        for(int j=0;j<1468;j++)
        {
            fprintf(f,"%02x ",dataTemp[i][j]);
        }
        fprintf(f,"\n");
    }
    fclose(f);

    FILE *f00 = fopen("Data00.dat","wb");
    if( f00==NULL )
    {
        printf("Can not open file Data00.dat.\n");
    }
    else
    {
        printf("has opened the file Data00.dat.\n");
    }
    for(int i=0;i<frameCout/4;i++)
    {
        for(int j=0;j<365;j++)
        {
            fprintf(f00,"%d\n",intArray[i][j]);
        }
    }
    fclose(f00);

    FILE *f01 = fopen("Data01.dat","wb");
    if( f01==NULL )
    {
        printf("Can not open file Data01.dat.\n");
    }
    else
    {
        printf("has opened the file Data01.dat.\n");
    }
    for(int i=frameCout/4;i<frameCout/2;i++)
    {
        for(int j=0;j<365;j++)
        {
            fprintf(f01,"%d\n",intArray[i][j]);
        }
    }
    fclose(f01);

    FILE *f02 = fopen("Data02.dat","wb");
    if( f02==NULL )
    {
        printf("Can not open file Data02.dat.\n");
    }
    else
    {
        printf("has opened the file Data02.dat.\n");
    }
    for(int i=frameCout/2;i<(3*frameCout/4);i++)
    {
        for(int j=0;j<365;j++)
        {
            fprintf(f02,"%d\n",intArray[i][j]);
        }
    }
    fclose(f02);

    FILE *f03 = fopen("Data03.dat","wb");
    if( f03==NULL )
    {
        printf("Can not open file Data03.dat.\n");
    }
    else
    {
        printf("has opened the file Data03.dat.\n");
    }
    for(int i=(3*frameCout/4);i<frameCout;i++)
    {
        for(int j=0;j<365;j++)
        {
            fprintf(f03,"%d\n",intArray[i][j]);
        }
    }
    fclose(f03);

    cout << "have saved data to the file" << endl;
    cout << "the number of sample is " << dataNum << endl;

    delete UDP;
    return 0;
}
