// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "stdafx.h"
#include "AudioClassify.h"
extern int g_iFrameLen,g_iFrameShift,g_iSampleFrequency;

static void print_usage()
{
	printf("\nUSAGE   :  Test <InputWavFile> <OutPutFeaturefile> <classify?> <Wait after finish? default: 0>\n"
		"The defaults sample rate is 8KHz.\n"
		"---------------------------------------\n\n");
	exit(0);
}
void CalFileFea(char *infile, char *outfile);
int PureMusicJudge(char *infile, char *outfile);
int main(int argc, char* argv[])
{//argv[3] 是程序名字，这里是 "test"
	bool    bWaitAfterFinish = false,bClassify = false;

	if( argc < 4 )
	{
		print_usage();
	}
	bClassify = (atoi(argv[3]) !=0);

	if( argc > 4 )
		bWaitAfterFinish = (atoi(argv[4]) !=0);
	bClassify = true;
	if( bClassify )//分类
		PureMusicJudge(argv[1],argv[2]);
	else//计算特征文件
		CalFileFea(argv[1],argv[2]);

	if( bWaitAfterFinish )
	{
		printf("\n Press any key to return");
		getchar();
	}
	system("pause");
	return 0;
}
void CalFileFea(char *infile, char *outfile)
{
	FILE	*fpWav=NULL,*fpFea=NULL;
	short   iData[8000];
	int     iRead,iDotsPerRead,iFrameNum,iTotalFrames=0;

	int iFs = 8000,iSegNum=0;
	float fSegFea[100];

	fpWav = fopen(infile, "rb");//infile
	fpFea = fopen(outfile, "ab+");//outfile

	if( fpWav == NULL)
	{
		printf("###  Failed to open wave file: %s\n", infile);
		Sleep(3000);
		exit(1);
	}
	else
		printf("\n\n  Wav file: %s", infile);

	if( fpFea == NULL )
	{
		printf("###  Failed to open feature file: %s\n", outfile);
		Sleep(3000);
		exit(1);
	}
	else
		printf("\n  feature file: %s\n", outfile);

	fseek(fpWav,44,SEEK_SET);

	iDotsPerRead = iFs;// one second
	iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);

	//printf("\n Read:%d  Need:%d\n",iRead,iDotsPerRead);
	iFrameNum = 1 + (g_iSampleFrequency - g_iFrameLen) / g_iFrameShift;
	float *fData = new float[g_iSampleFrequency];
	while( iRead == iDotsPerRead)
	{
		for (int i = 0; i < iDotsPerRead; i++)
			fData[i] = 1.0 * iData[i] / 32768.0F;
		CalSegFeatures(fData, iRead, fSegFea);
		iFrameNum = 
			iTotalFrames += iFrameNum;
		if( iFrameNum > 0 && fSegFea[0] > 0 )// not silent frame
			fprintf(fpFea,"%07.5f, %07.5f, %07.5f\n",
			fSegFea[0],fSegFea[1],fSegFea[2]);

		iSegNum++;
		iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);
		if( iSegNum%500 == 0)
			printf("  Processed seg:  %d\n",iSegNum);
	}
	fclose(fpFea);
	fclose(fpWav);
	printf("   =======Finish, Total  %d Segs,  %d Frames\n ",iSegNum,iTotalFrames);
}
int PureMusicJudge(char *infile, char *outfile)
{
	FILE	*fpWav=NULL,*fpResult=NULL;
	short   iData[8000];
	int     iRead,iDotsPerRead,iMusicSegs=0;

	int iFs = 8000,iSegNum=0;

	fpWav = fopen(infile, "rb");//infile
	fpResult = fopen(outfile, "ab+");//outfile
	int a = sizeof(short);
	if( fpWav == NULL)
	{
		printf("###  Failed to open wave file: %s\n", infile);
		Sleep(3000);
		return -10;
	}
	else
		printf("\n\n  Wav file: %s", infile);

	if( fpResult == NULL )
	{
		printf("###  Failed to open result file: %s\n", outfile);
		Sleep(3000);
		return -20;
	}
	else
		printf("\n  classify result file: %s\n", outfile);

	fseek(fpWav,44,SEEK_SET);

	iDotsPerRead = iFs;// one second
	iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);

	//printf("\n Read:%d  Need:%d\n",iRead,iDotsPerRead);

	while( iRead == iDotsPerRead)
	{

		if( IsPureMusicSeg(iData, iRead) )// not silent frame
		{
			fprintf(fpResult,"1\n");
			iMusicSegs++;
		}
		else
			fprintf(fpResult,"0\n");
		iSegNum++;
		iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);
		if( iSegNum%500 == 0)
			printf("  Classifying seg:  %d\n",iSegNum);
	}

	fclose(fpResult);
	fclose(fpWav);
	printf("   =======Finish, Total  %d Segs,  %d Music Segs(%4.2f%%)\n ",iSegNum,iMusicSegs,float(iMusicSegs)/iSegNum);
	return 1;
}