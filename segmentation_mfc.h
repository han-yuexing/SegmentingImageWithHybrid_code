
// segmentation_mfc.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Csegmentation_mfcApp:
// �йش����ʵ�֣������ segmentation_mfc.cpp
//

class Csegmentation_mfcApp : public CWinApp
{
public:
	Csegmentation_mfcApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Csegmentation_mfcApp theApp;