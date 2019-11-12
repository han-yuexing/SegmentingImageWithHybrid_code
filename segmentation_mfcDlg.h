
// segmentation_mfcDlg.h : 头文件
//

#pragma once


// Csegmentation_mfcDlg 对话框
class Csegmentation_mfcDlg : public CDialogEx
{
// 构造
public:
	Csegmentation_mfcDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SEGMENTATION_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

//变量
private:
	CString filePath;
	int count_button_click;
	int clustering_mode, edge_mode, edge_count;
	int dim, hs, hr, kmeans_k, dbscan_min, dim_temp, hs_temp, hr_temp;
	double dbscan_eps;
public:
	afx_msg void OnBnClickedButtonSegment();
	afx_msg void OnBnClickedRadioKmeans();
	afx_msg void OnBnClickedRadioDbscan();
	afx_msg void OnBnClickedRadioEdgeYes();
	afx_msg void OnBnClickedRadioEdgeNo();
	afx_msg void OnBnClickedOpenimg();
	afx_msg void OnStnClickedStaticDim();
	afx_msg void OnStnClickedStaticHs();
	afx_msg void OnStnClickedStaticHr();
	afx_msg void OnStnClickedStaticKmeans();
	afx_msg void OnStnClickedStaticDbscan1();
	afx_msg void OnStnClickedStaticDbscan2();
	void showImage(UINT ID, int flag);
	//void showResult(UINT ID);
	afx_msg void OnBnClickedStaticExplanation();
	afx_msg void OnBnClickedButtonShowResult();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
