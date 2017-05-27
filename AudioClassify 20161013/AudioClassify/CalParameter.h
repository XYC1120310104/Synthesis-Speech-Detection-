// CalParameter.h: interface for the CCalParameter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALPARAMETER_H__FCC5B8A3_ACAD_47E6_8B1A_FA7469F3B57D__INCLUDED_)
#define AFX_CALPARAMETER_H__FCC5B8A3_ACAD_47E6_8B1A_FA7469F3B57D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"CrossType.h"

#include "Const.h"
//#include "AudioSearch.h"

#ifdef  USE_SSE3   //否使用汇编加速指令
float sse3_inner(const float* a, const float* b, unsigned int size);
#endif

class CCalParameter  
{
public:
	CCalParameter();
	virtual ~CCalParameter();
public:
	void CalSegFeature(float (*fFeature)[FEATURE_DIMENSION],const short*data,  int iFrames);
	

//	float   m_fTempFeature[5][FEATURE_DIMENSION];//恢复定义

	void    CalEnZCR(const short *data, float * result);
	void    CalFrameEnZCR(const short *data, int iFrameLen, float * result);
	void    FFT(float *xr, float *xi);
	void    CalMel(const short *data, float *result);
	void    CalAll(const short *data0, float *result);     //计算全部参数
	float   CalSimilarity(float *x, float *y,int iDims);
	void    CNormalizeVector(float *v1,float *v2,int dim); //zgb 2007.09.07

	void    CalDif( float (*data)[FEATURE_DIMENSION], int iFrames); //zgb 2007.12.03

	void    CalIdx1ofSegment( float (*data)[FEATURE_DIMENSION], int iFrames,float *result);
	void	LPC2LPCC(const float * lpc, float * cep); //gby converted from EST 2007.12.14
	void	CalLPC(const short * data, float * lpc); //gby converted from EST 2007.12.14
	float   MaxAutocorrelation(float *data);
	float   MaxAutocorrelation0(const short *data);
	float   CalFC(float *fFFT);
	
public:
	int		m_iBarkNumofFFT[FFT_LENGTH/2+1];		//用于bark域变换
	float   m_fBarkCoef1[FFT_LENGTH/2+1], m_fBarkCoef2[FFT_LENGTH/2+1];	//用于bark域变换
	float	m_fHAMMING_COEF[FRAME_LENGTH];			//汉明窗系数
	float   m_fWeight[FRAME_SPAN];  //均值权系数
	float   m_fMyCos[12][FILTER_NUMBER+1];

#ifdef USE_FFT_RESULT	
	float   m_fFFT[FFT_LENGTH/2+1];//保存FFT变换频谱数据（模数据）
#endif
     //==========    zgb 2011.06.16    ==============================================
    float   CalCosinDist(float *x, float *y,int iDims);
	float   CalEucDist(float *x, float *y,int iDims);
	float   CalWeightedSimilarity(float *x, float *y,int iDims);
    float   CalWeightedCosinDist(float *x, float *y,int iDims);
	void    CalBandEn( short *data, float *result);
	//===========   zgb 2011.07.07 ==================================================
    void    CalIDX0FromMFCC( float (*fMFCC)[MFCC_ORDER],float (*fDetMFCC)[MFCC_ORDER], int iFrames); 

};

#endif // !defined(AFX_CALPARAMETER_H__FCC5B8A3_ACAD_47E6_8B1A_FA7469F3B57D__INCLUDED_)
