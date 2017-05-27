// AudioClassify.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MFCC.h"
#include <stdio.h>
#include <math.h>
#include "AudioClassify.h"
#include "linear.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAudioClassifyApp

BEGIN_MESSAGE_MAP(CAudioClassifyApp, CWinApp)
	//{{AFX_MSG_MAP(CAudioClassifyApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioClassifyApp construction

CAudioClassifyApp::CAudioClassifyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAudioClassifyApp object
#define FFT_LENGTH_FRAME 512
#define PI 3.1415926536F
#define SILENCE 0.0118        // 静音阈值
#define FEATURE_DIMENSION  3
#define  MIN_FRE  150  //Hz
#define  MAX_FRE  3500 //Hz
#define MaxFramesPerSec 100 
//###########################################################################
int g_iFrameLen = 512;
int g_iFrameShift = 400;
int g_iSampleFrequency = 8000;

//###########################################################################
CAudioClassifyApp theApp;
void FFT(float *xr, float *xi);
void AutoCorrelation(const float *data, float *fResult);
float CalFC(float *fFFT,int fs);
void CalFrameEnZCR(const float *data, int g_iFrameLen, float * fEn, float *fZcr, float *fSquareEn);
void CalFrameFeatures(const float *data, int g_iFrameLen, float *fFea) ; 

//Feature Vector: [En, Zcr, Fc, Harmony_Low,Harmony_High,WeightedHarmonicity]
//20160620 Feature Vector: [LSTABSENR, HZCRR, LSTER]

void CalFrameEnZCR(const float *data, int g_iFrameLen,  float *fEn, float * fZcr, float *fSquareEn)
{
	//计算每帧的过零率和能量绝对值和
	//short *data 帧的第一个数据地址
	//float * result  结果保存地址的起始值
	float   fTempZcrThreshold;
	double  dblABSEn, dbSquareEn;
	int i,data0, data1;
	float fTempZcr ;

	dblABSEn= dbSquareEn = 0;
	fTempZcrThreshold=0;
	for(i=0; i<g_iFrameLen; i++)
	{
		dblABSEn = dblABSEn + abs( data[i]);
		dbSquareEn += data[i] * data[i];
	}

	dblABSEn = (1.0 / g_iFrameLen)*dblABSEn; 
	dbSquareEn = dbSquareEn;

	data1 =0;
	fTempZcr=0;

	//计算过零率
	for(i=0; i < g_iFrameLen; i++)
	{
		if(data[i] > 0)
			data0 = 1;
		else
			data0 = -1;

		if((data0*data1)<0)
			fTempZcr++;
		data1 = data0;
	}


	*fZcr=fTempZcr/g_iFrameLen;
	*fEn=(float)dblABSEn;
	*fSquareEn=(float)dbSquareEn;
}

void CalFrameFeatures(const float *pData, int g_iFrameLen, float *fFea) 
{ 
	//Feature Vector: [LSTABSENR, HZCRR, LSTER]

	//计算一帧的特征

	if (g_iFrameLen < 100 || g_iFrameLen > FFT_LENGTH_FRAME)
		return;
	CalFrameEnZCR(pData, g_iFrameLen, fFea, fFea + 1, fFea + 2);

	//得到fFea[0]:该帧的过零率
	//得到fFea[1]:该帧的绝对值能量均值
	//得到fFea[2]:该帧的平方能量
}

//#define MinZCR      0.15 //音乐的过零率阈值
#define MinZCR      0.1 //音乐的过零率阈值20160620
#define MinFC       0.25 //音乐的FC阈值，MaxSpeechFC
#define MaxH1       0.65
#define MinH2       0.2
#define MaxWH       0.25
#define MinHRatio   0.4  // 低频、高频段的和谐度最低数值
//#define MinZCR      0.15 //音乐的过零率阈值
//#define MinFC       0.25 //音乐的FC阈值，MaxSpeechFC
//#define MaxH1          0.55
//#define MinH2          0.25
//#define MaxWH          0.25
//#define MinHRatio      0.4  // 低频、高频段的和谐度最低数值
//-------------------------------------------------------------------
extern "C" int  PASCAL EXPORT CalSegFeatures(float *pData, int iSampleNum,float *fSegAvgFea)
{
	//Feature Vector: [HZCRR, LSTER, LSTABSENR]
	//return number of frames computated

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int iFrameNum,i,j,iSilenceFrame;
	float features[MaxFramesPerSec][FEATURE_DIMENSION];
	float fFeature[FEATURE_DIMENSION],fSumFea[FEATURE_DIMENSION];	

	//计算iSampleNum包含多少帧iFrameNum;
	if( iSampleNum > g_iFrameLen)
		iFrameNum = 1+(iSampleNum - g_iFrameLen)/g_iFrameShift;
	else
		return iSampleNum;

	iSilenceFrame = 0;
	for(j = 0 ;j < FEATURE_DIMENSION;j++)
	{
		fSumFea[j] = 0;
		fSegAvgFea[j] = 0;
	}	
	for( i = 0;i < iFrameNum;i++)
	{
		CalFrameFeatures(pData + i*g_iFrameShift, g_iFrameLen, fFeature);
		if (fFeature[0] < SILENCE || (fFeature[1] < MinZCR && fFeature[0] < 1.2 * SILENCE))//  silent frame
		{
			iSilenceFrame++;
			features[i][0] = 0;
			features[i][1] = 0;
			features[i][2] = 0;
		}			
		else
		{
			fSumFea[0] += fFeature[0];
			fSumFea[1] += fFeature[1];
			fSumFea[2] += fFeature[2];
			features[i][0] =  fFeature[0];
			features[i][1] =  fFeature[1];
			features[i][2] =  fFeature[2];
		}
	}

	float a,b,c;
	if( iSilenceFrame < iFrameNum / 2 )//至少一半是非静音帧
	{
		float fAvgZcr, fAvgAbsEn, fAvgEn;
		fAvgZcr = fSumFea[1]/(iFrameNum) ;
		fAvgAbsEn = fSumFea[0]/(iFrameNum-iSilenceFrame);
		fAvgEn = fSumFea[2]/(iFrameNum-iSilenceFrame);

		fAvgZcr = 1.5 * fAvgZcr;
		fAvgAbsEn = 0.3 *fAvgAbsEn;
		fAvgEn = 0.5 * fAvgEn;

		for( i=0;i<iFrameNum;i++)
		{
			if(features[i][0] < fAvgAbsEn)
				fSegAvgFea[0] += 1;
			if(features[i][1] > fAvgZcr)
				fSegAvgFea[1] += 1;
			if(features[i][2] < fAvgEn)
				fSegAvgFea[2] += 1;
		}

		fSegAvgFea[0] = fSegAvgFea[0]/iFrameNum;
		fSegAvgFea[1] = fSegAvgFea[1]/iFrameNum;
		fSegAvgFea[2] = fSegAvgFea[2]/iFrameNum;
	}
	else
	{
		for(j=0;j<FEATURE_DIMENSION;j++)
			fSegAvgFea[j] = 0;
	}
	return iSilenceFrame;
}
//-------------------------------------------------------------------

#define MAXLSTER 0.2
#define MAXHZCRR 0.06
extern "C" static void PASCAL EXPORT Normaliaztion(const short data[], int len, float *&fData)
{
	for (int i = 0; i < len; i++)
		fData[i] = 1.0 * data[i] / 32768.0F;
}
extern "C" bool  PASCAL EXPORT IsPureMusicSeg(short *pData, int iSampleNum)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int iSlienceFrameNum, iFrameLen;
	float fFeatures[FEATURE_DIMENSION], fRatio;//Feature Vector: [LSTABSENR,HZCRR, LSTER]
	float fConditionNum = 0;
	bool bIsPureMusic = false;
	float *fData = new float[iSampleNum];

	Normaliaztion(pData, iSampleNum, fData);
	iFrameLen = 1 + (g_iSampleFrequency - g_iFrameLen) / g_iFrameShift;
	iSlienceFrameNum = CalSegFeatures(fData, iSampleNum, fFeatures);

	//all 0 explain that this segment is slilence
	if (iSlienceFrameNum >= iFrameLen / 2)
		bIsPureMusic = false;
	else if (fFeatures[0] == 0 || fFeatures[2] < MAXLSTER || fFeatures[1] < MAXHZCRR)
	{
		bool bResult = JudgeOneSecondBySVM(fData, iSampleNum);
		delete[] fData;
		fData = NULL;
		return bResult;
	}		
	else
		bIsPureMusic = false;
	delete[] fData;
	fData = NULL;
	return bIsPureMusic;
}


extern "C" bool  PASCAL EXPORT ContainPureMusic(char *srcFile, bool *bIsMusicSeg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(srcFile == NULL || bIsMusicSeg == NULL)
		return false;
	FILE *fpWav;
	bool bContainMusicSeg;
	short iData[8000];
	int	i,iRead,iDotsPerRead;

	int iFs = 8000,iSegNum=0;
	fpWav = fopen(srcFile,"rb");
	if( fpWav == NULL)
	{
		printf("###  Failed to open wave file: %s\n", srcFile);
		return false;
	}
	else
		printf("\n\n  Wav file: %s", srcFile);	

	fseek(fpWav,44,SEEK_SET);
	iDotsPerRead = iFs;// one second
	iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);

	iSegNum = 0;
	while( iRead == iDotsPerRead)
	{
		bIsMusicSeg[iSegNum] = IsPureMusicSeg(iData, iDotsPerRead);
		iSegNum++;
		iRead = fread(iData,sizeof(short),iDotsPerRead,fpWav);
	}
	fclose(fpWav);

	bContainMusicSeg = false;
	i = 0;
	while( !bContainMusicSeg && i < iSegNum - 3)
	{
		if( bIsMusicSeg[i] && bIsMusicSeg[i+1] && bIsMusicSeg[i+2] && bIsMusicSeg[i+3] )
			bContainMusicSeg = true;
		i++;
	}
	return bContainMusicSeg;
}
extern "C" static void  PASCAL EXPORT smooth(int32_t *predict_label, uint32_t segments)
{

	ArrayXi temp_label = ArrayXi(segments);
	memcpy(temp_label.data(), predict_label, sizeof(int)*segments);
	for (int i = SMOOTH_SEGMENT; i < segments - SMOOTH_SEGMENT; i++)
	{
		int num_1 = 0, num_2 = 0;
		num_1 = (temp_label.segment(i - SMOOTH_SEGMENT, 2 * SMOOTH_SEGMENT) == 1).count();
		num_2 = (temp_label.segment(i - SMOOTH_SEGMENT, 2 * SMOOTH_SEGMENT) == 0).count();
		//predict_label[i] = (num_1>1.5*num_2) ? 1 : 0;
		predict_label[i] = (num_1>1.5*num_2) ? 1 : 0;


	}
	for (int i = 1; i < segments - 1; i++)
	{
		if (predict_label[i - 1] == predict_label[i + 1])
			predict_label[i] = predict_label[i - 1];
	}
}
extern "C" static bool PASCAL EXPORT calLastLabel(int32_t * predict_label, uint32_t segments)
{
	if (predict_label == NULL||segments <= 0)
		return false;

	int sum = 0;
	bool bResult = false;
	for (int i = 0; i < segments; i++)
		sum += predict_label[i];
	if (sum > 0)
		bResult = true;

	return bResult;
}

  

extern "C" bool  PASCAL EXPORT JudgeOneSecondBySVM(float *raw_data, size_t length)
{
	int *predict_label;
	CMFCC mfcc;
	MatrixXf features;
	mfcc.CelMfcc(raw_data, length, features);
	//uint32_t nFrames = (length - FRAME_LENGTH) / FRAME_SHIFT + 1;
	//nFrames = nFrames - 8;
	uint32_t segments = features.cols();
	FILE *fp_ml = fopen("model", "r");//训练模型
	predict_label = (int*)malloc(sizeof(int)*segments);
	model_predict(features, fp_ml, predict_label);
	//改标签
	vector<int> a(predict_label,predict_label+segments);
	for (int i = 0; i < segments; i++)
	{
		if (predict_label[i] == 1 || predict_label[i] == 2)
			predict_label[i] = 1;
		else
			predict_label[i] = 0;

		if (features(features.rows() - 1, i) > 0)
			predict_label[i] = 1;
	}
	vector<int> b(predict_label, predict_label + segments);
	//平滑
	//smooth(predict_label, segments);
	return calLastLabel(predict_label, segments);
}

