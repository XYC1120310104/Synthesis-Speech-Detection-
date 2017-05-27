

#ifndef __MY_MFCC_H__
#define __MY_MFCC_H__
#include "Eigen\Dense"
#include "Const.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "WavFile.h"
#include <iostream>


using namespace Eigen;
using namespace std;




class CMFCC
{
private:
	Matrix<float, FILTER_NUMBER, FFT_LENGTH / 2 + 1, RowMajor> m_MelBank;
	Matrix<float, MFCC_ORDER, FILTER_NUMBER, RowMajor> m_dctCoef;
	VectorXf m_HammingWin;
	VectorXf m_CepLiftWin;
private:
	int CelFrameMel(int16_t* sample, float* mfcc);
	inline void swap(float &a, float &b)
	{
		float t;
		t = a;
		a = b;
		b = t;
	}
	void bitrp(float xreal[], float ximag[], int n);
	void FFT(float xreal[], float ximag[], int n);
public:
	int CelMfcc(float* data, uint32_t length, MatrixXf& features);
	int CelMfcc(char* fn, MatrixXf& features);
	void ZCR(float *data, uint32_t length, uint32_t mfcc_num, RowVectorXf &zcr);
	void AMP(MatrixXf frame_data, MatrixXf &amp);
	RowVectorXf sign(RowVectorXf A);
	CMFCC();
	~CMFCC();
};

#endif
