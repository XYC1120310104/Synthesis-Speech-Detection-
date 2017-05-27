// AudioClassify.h : main header file for the AUDIOCLASSIFY DLL
//

#if !defined(AFX_AUDIOCLASSIFY_H__4112B65E_C692_4414_ADA3_7723A29B9CBC__INCLUDED_)
#define AFX_AUDIOCLASSIFY_H__4112B65E_C692_4414_ADA3_7723A29B9CBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
#include "MFCC.h"
#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CAudioClassifyApp
// See AudioClassify.cpp for the implementation of this class
//

class CAudioClassifyApp : public CWinApp
{
public:
	CAudioClassifyApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioClassifyApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAudioClassifyApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

/*To calculate the features of a audio segment
return number of frames computated*/
extern "C" int  PASCAL EXPORT CalSegFeatures(float *pData, int iSampleNum, float *fSegAvgFea);

/*To judge whether the audio data contains pure music segment of minimum length of 1 second*/
extern "C" bool  PASCAL EXPORT IsPureMusicSeg(short *pData, int iSampleNum);

/*To judge whether the file contains pure music segment of minimum length of 3 second*/
extern "C" bool  PASCAL EXPORT ContainPureMusic(char *srcFile, bool *bIsMusicSeg);

extern "C" bool  PASCAL EXPORT JudgeOneSecondBySVM(float *raw_data, size_t length);

#endif // !defined(AFX_AUDIOCLASSIFY_H__4112B65E_C692_4414_ADA3_7723A29B9CBC__INCLUDED_)
