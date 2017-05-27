#include "stdafx.h"
#include "MFCC.h"
#include <limits.h>
void CMFCC::bitrp(float xreal[], float ximag[], int32_t n)
{
	int32_t i, j, a, b, p;
	p = (int32_t)(log(n*1.0)/log(2.0));
	for (i = 0; i < n; i++)
	{
		a = i;
		b = 0;
		for (j = 0; j < p; j++)
		{
			b = (b << 1) + (a & 1);
			a >>= 1;
		}
		if (b > i)
		{
			swap(xreal[i], xreal[b]);
			swap(ximag[i], ximag[b]);
		}
	}
}

void CMFCC::FFT(float xreal[], float ximag[], int32_t n)//xread 表示信号的实部 ximag 表示信号的虚部
{
	float wreal[FFT_LENGTH / 2], wimag[FFT_LENGTH / 2], treal, timag, ureal, uimag, arg;
	int32_t m, k, j, t, index1, index2;

	bitrp(xreal, ximag, n);

	arg = -2 * PI / n;
	treal = cos(arg);
	timag = sin(arg);
	wreal[0] = 1.0;
	wimag[0] = 0.0;
	for (j = 1; j < n / 2; j++)
	{
		wreal[j] = wreal[j - 1] * treal - wimag[j - 1] * timag;
		wimag[j] = wreal[j - 1] * timag + wimag[j - 1] * treal;
	}

	for (m = 2; m <= n; m *= 2)
	{
		for (k = 0; k < n; k += m)
		{
			for (j = 0; j < m / 2; j++)
			{
				index1 = k + j;
				index2 = index1 + m / 2;
				t = n * j / m;
				treal = wreal[t] * xreal[index2] - wimag[t] * ximag[index2];
				timag = wreal[t] * ximag[index2] + wimag[t] * xreal[index2];
				ureal = xreal[index1];
				uimag = ximag[index1];
				xreal[index1] = ureal + treal;
				ximag[index1] = uimag + timag;
				xreal[index2] = ureal - treal;
				ximag[index2] = uimag - timag;
			}
		}
	}
}

int32_t CMFCC::CelFrameMel(int16_t* sample, float* mfcc)
{
	int32_t i, j;
	float *sr, *si, *ss;
	sr = (float*)malloc(sizeof(float)*FFT_LENGTH);
	memset(sr, 0, sizeof(float)*FFT_LENGTH);
	si = (float*)malloc(sizeof(float)*FFT_LENGTH);
	memset(si, 0, sizeof(float)*FFT_LENGTH);

	ss = (float*)malloc(sizeof(float)*FRAME_LENGTH);
	memset(ss, 0, sizeof(float)*FRAME_LENGTH);

	// Pre weight
	ss[0] = sample[0];
	for (i = 1; i < FRAME_LENGTH; i++)
		ss[i] = (float)sample[i] - PRE * (float)sample[i - 1];

	// Hamming window
	for (i = 0; i < FRAME_LENGTH; i++)
		ss[i] *= HAMMING_WINDOW[i];

	// FFT
	for (i = 0; i < FRAME_LENGTH; i++)
		sr[i] = ss[i];
	FFT(sr, si, FFT_LENGTH);

	// Energy of FFT
	for (i = 0; i < FFT_LENGTH / 2 + 1; i++){
		sr[i] = sr[i] * sr[i] + si[i] * si[i];
	}

	// Mel filter bank
	float mel_filter_result[FILTER_NUMBER];
	for (i = 0; i < FILTER_NUMBER; i++){
		mel_filter_result[i] = 0.0f;
		for (j = 0; j < FFT_LENGTH / 2 + 1; j++)
			mel_filter_result[i] += MEL_FILTER_BANK[i][j] * sr[j];
		mel_filter_result[i] = log(mel_filter_result[i]);
	}

	// DCT Coefficient
	for (i = 0; i < MFCC_ORDER; i++){
		mfcc[i] = 0.0f;
		for (j = 0; j < FILTER_NUMBER; j++)
			mfcc[i] += mel_filter_result[j] * DCT_COEF[i][j];
	}

	// Cep Lifter
	for (i = 0; i < MFCC_ORDER; i++)
		mfcc[i] *= CEP_LIFT_WINDOW[i];

	free(sr);
	free(si);
	free(ss);
	return 0;
}

//////特征值由以下组成：MFCC:39维  短时能量及其一阶二阶差分3维  ZCR RMS LEnFR 1维 ， LSTER 1维 HZCRR 1维 共47维
int32_t CMFCC::CelMfcc(float* d, uint32_t length, MatrixXf& features)//读数组  d：wav数据点的数组 length:长度
{
	uint32_t nSamples = length;
	float* data = (float*)malloc(sizeof(float)*nSamples);
	memcpy(data, d, sizeof(float)*nSamples);
	uint32_t nFrames = (nSamples - FRAME_LENGTH) / FRAME_SHIFT + 1;
	float* ss = (float*)malloc(sizeof(float)*nSamples);
	VectorXf frameReal = VectorXf(FRAME_LENGTH);
	VectorXf frameImag = VectorXf(FRAME_LENGTH);
	MatrixXf frames = MatrixXf(FFT_LENGTH / 2 + 1, nFrames);
	float *sr, *si;
	sr = (float*)malloc(sizeof(float)*FFT_LENGTH);
	memset(sr, 0, sizeof(float)*FFT_LENGTH);
	si = (float*)malloc(sizeof(float)*FFT_LENGTH);
	memset(si, 0, sizeof(float)*FFT_LENGTH);//置0
	MatrixXf cc = MatrixXf(MFCC_ORDER, nFrames);
	// Pre Weight 预加重滤波器
	ss[0] = data[0];

	VectorXf da = VectorXf(length);
	memcpy(da.data(), data, nSamples*sizeof(float));

	for (uint32_t i = 1; i < nSamples; i++)
		ss[i] = (float)data[i] - PRE * (float)data[i - 1];

	MatrixXf Frame_data = MatrixXf(FRAME_LENGTH, nFrames);//存储所有帧 
	for (int i = 0; i < nFrames; i++)
		memcpy(Frame_data.col(i).data(), ss + i*FRAME_SHIFT, sizeof(float)*FRAME_LENGTH);
	//cout << Frame_data << endl;
	for (int32_t i = 0; i < nFrames; i++){

		memcpy(sr, ss + (i*FRAME_SHIFT), sizeof(float)*FRAME_LENGTH);
		for (int32_t j = 0; j < FRAME_LENGTH; j++)
			sr[j] = sr[j] * HAMMING_WINDOW[j];
		memset(si, 0, sizeof(float)*FFT_LENGTH);
		FFT(sr, si, FFT_LENGTH);
		memcpy(frameReal.data(), sr, sizeof(float)*FRAME_LENGTH);
		memcpy(frameImag.data(), si, sizeof(float)*FRAME_LENGTH);
		frameReal = frameReal.cwiseProduct(frameReal) + frameImag.cwiseProduct(frameImag);//两个矩阵对应位置相乘 相当于matlab的.* 这里是求平方

		frames.col(i) = frameReal.segment(0, FFT_LENGTH / 2 + 1);
		for (int32_t j = 0; j < 129; j++)
		{
			if (frames(j, i) < FLT_MIN)
				frames(j, i) = FLT_MIN;
		}
	}
	cc = m_dctCoef * ((m_MelBank * frames).array().log().matrix());
	for (int32_t i = 0; i < nFrames; i++)
		cc.col(i) = cc.col(i).cwiseProduct(m_CepLiftWin);
	MatrixXf dcc = MatrixXf(cc.rows(), cc.cols());
	MatrixXf ddcc = MatrixXf(cc.rows(), cc.cols());
	RowVectorXf zcr = RowVectorXf(nFrames);
	MatrixXf amp = MatrixXf(3, nFrames);
	for (int32_t i = 2; i < nFrames - 2; i++)
		dcc.col(i) = (cc.col(i + 2) * 2.0 + cc.col(i + 1) - cc.col(i - 1) - cc.col(i - 2)*2.0) / 3.0;

	for (int32_t i = 4; i < nFrames - 4; i++)
		ddcc.col(i) = (dcc.col(i + 2) * 2.0 + dcc.col(i + 1) - dcc.col(i - 1) - dcc.col(i - 2)*2.0) / 3.0;
	//计算过零率和短时能量
	ZCR(ss, nSamples, nFrames, zcr);
	AMP(Frame_data, amp);
	MatrixXf frame_features = MatrixXf(FEATURE_DIM - 3, nFrames - 8);
	frame_features.block(0, 0, MFCC_ORDER, nFrames - 8) << cc.block(0, 4, MFCC_ORDER, nFrames - 8); // << 表示等号 4:因为有一阶差分和二阶差分组成，因此要去掉差分=0的
	frame_features.block(MFCC_ORDER, 0, MFCC_ORDER, nFrames - 8) << dcc.block(0, 4, MFCC_ORDER, nFrames - 8);
	frame_features.block(2 * MFCC_ORDER, 0, MFCC_ORDER, nFrames - 8) << ddcc.block(0, 4, MFCC_ORDER, nFrames - 8);
	frame_features.block(3 * MFCC_ORDER, 0, 3, nFrames - 8) << amp.block(0, 4, 3, nFrames - 8);
	frame_features.row(3 * MFCC_ORDER + 3) << zcr.segment(4, nFrames - 8);
	frame_features.row(3 * MFCC_ORDER + 4) << Frame_data.cwiseAbs2().colwise().sum().cwiseSqrt().segment(4, nFrames - 8);
	//以上是44维度,mfcc+amp+amp1+amp2+zcr+rms
	//归一化
	for (int i = 0; i < frame_features.cols(); i++)
	{
		VectorXf con_f = VectorXf::Constant(frame_features.rows(), 1 / (frame_features.col(i).maxCoeff() - frame_features.col(i).minCoeff()));
		VectorXf min_f = VectorXf::Constant(frame_features.rows(), frame_features.col(i).minCoeff());
		frame_features.col(i) = (frame_features.col(i) - min_f).cwiseProduct(con_f);
	}

	nFrames = nFrames - 8;
	uint32_t seg_n = (nFrames - MODEL_SEGMENT) / SEGMENT_SHIFT + 1;//段的个数
	features = MatrixXf(FEATURE_DIM, seg_n);//features段特征
	RowVectorXf tempFREn = Frame_data.cwiseAbs().colwise().sum() / FRAME_LENGTH;
	for (int i = 0; i < seg_n; i++)
	{
		uint32_t index = 0;

		features.block(0, i, FEATURE_DIM - 3, 1) = frame_features.block(0, i * SEGMENT_SHIFT, FEATURE_DIM - 3, MODEL_SEGMENT).rowwise().mean();

		//高过零率HZCRR
		RowVectorXf avzcr = RowVectorXf::Constant(MODEL_SEGMENT, 1.5*zcr.segment(4 + i*SEGMENT_SHIFT, MODEL_SEGMENT).sum() / MODEL_SEGMENT);
		RowVectorXf HZCRR_sign = sign(zcr.segment(4 + i*SEGMENT_SHIFT, MODEL_SEGMENT) - avzcr) + RowVectorXf::Constant(avzcr.size(), 1);
		features(3 * MFCC_ORDER + 5, i) = (float)HZCRR_sign.sum() / (2 * MODEL_SEGMENT);

		//计算avSTE和LSTER
		float temp_1 = features(0, i);
		RowVectorXf avSTE_1 = RowVectorXf::Constant(MODEL_SEGMENT, temp_1);
		RowVectorXf temp_LSTER_1 = avSTE_1 - amp.block(0, 4 + i*SEGMENT_SHIFT, 1, MODEL_SEGMENT);
		features(3 * MFCC_ORDER + 6, i) = (float)1 / (2 * MODEL_SEGMENT)*((sign(temp_LSTER_1)).sum() + 1);
		//计算LENFR
		RowVectorXf tempFR = tempFREn.segment(4 + i*SEGMENT_SHIFT, MODEL_SEGMENT);
		float avgEn = tempFR.mean();
		for (int j = 0; j < MODEL_SEGMENT; j++)
			if (tempFR(j) - avgEn*0.3 < 0)
				index++;
		features(3 * MFCC_ORDER + 7, i) = (float)index / seg_n;// LEnFR = sum(tempFREn < avgEn * 0.3)/FramePerRead;
	}
	//以上一共计算47维度44+HZCRR+LSTER(mfcc的)+LENFR
	if (sr != NULL)
	{
		free(sr);
		sr = NULL;
	}
	if (si != NULL)
	{
		free(si);
		si = NULL;
	}
	if (data != NULL)
	{
		free(data);
		data = NULL;

	}
	if (ss != NULL)
	{
		free(ss);
		ss = NULL;
	}
	return 0;
}


void CMFCC::ZCR(float *wav_data, uint32_t length, uint32_t nFrames, RowVectorXf &zcr)
{
	int32_t i = 0, j = 0;
	VectorXf tmp1 = VectorXf(FRAME_LENGTH);
	VectorXf tmp2 = VectorXf(FRAME_LENGTH);
	for (i = 0; i < nFrames; i++)
	{
		memcpy(tmp1.data(), wav_data + (i*FRAME_SHIFT), sizeof(float)*FRAME_LENGTH);
		memcpy(tmp2.data(), wav_data + (i*FRAME_SHIFT) + 1, sizeof(float)*FRAME_LENGTH);
		VectorXf opt = (((tmp1.array() * tmp2.array()).array() < 0) * ((tmp1 - tmp2).array() > 0.02)).cast<float>();
		zcr[i] = opt.sum();
		if (i == nFrames - 2 && ((length - 1 - FRAME_LENGTH) / FRAME_SHIFT) != nFrames)
		{
			zcr[i + 1] = zcr[i];
			break;
		}
	}

}

void CMFCC::AMP(MatrixXf frame_data, MatrixXf &amp)//frame_data  MatrixXf(FRAME_LENGTH, nFrames)
{
	uint32_t nFrames = frame_data.cols();
	amp.row(0) = frame_data.cwiseAbs2().colwise().sum();
	for (int32_t i = 2; i < nFrames - 2; i++)
		amp(1, i) = (amp(0, i + 2) * 2.0 + amp(0, i + 1) - amp(0, i - 1) - amp(0, i - 2)*2.0) / 3.0;

	for (int32_t i = 4; i < nFrames - 4; i++)
		amp(2, i) = (amp(1, i + 2) * 2.0 + amp(1, i + 1) - amp(1, i - 1) - amp(1, i - 2)*2.0) / 3.0;
}


CMFCC::CMFCC()
{
	memcpy(m_MelBank.data(), MEL_FILTER_BANK, sizeof(float)* FILTER_NUMBER * (FFT_LENGTH / 2 + 1));

	memcpy(m_dctCoef.data(), DCT_COEF, sizeof(float)*MFCC_ORDER*FILTER_NUMBER);

	m_HammingWin = VectorXf(FFT_LENGTH);
	memcpy(m_HammingWin.data(), HAMMING_WINDOW, sizeof(float)* FFT_LENGTH);

	m_CepLiftWin = VectorXf(MFCC_ORDER);
	memcpy(m_CepLiftWin.data(), CEP_LIFT_WINDOW, sizeof(float)*MFCC_ORDER);
}

CMFCC::~CMFCC()
{
}


RowVectorXf CMFCC::sign(RowVectorXf A)
{
	const RowVectorXf pos = RowVectorXf::Ones(A.cols());
	const RowVectorXf neg = RowVectorXf::Ones(A.cols()) * -1;
	RowVectorXf LSTER_sign = (A.array() >= 0).select(pos, neg);
	return LSTER_sign;
}