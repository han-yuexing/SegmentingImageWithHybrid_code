
// segmentation_mfcDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "segmentation_mfc.h"
#include "segmentation_mfcDlg.h"
#include "afxdialogex.h"
#include "segment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Csegmentation_mfcDlg 对话框



Csegmentation_mfcDlg::Csegmentation_mfcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Csegmentation_mfcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Csegmentation_mfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//消息映射
BEGIN_MESSAGE_MAP(Csegmentation_mfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEGMENT, &Csegmentation_mfcDlg::OnBnClickedButtonSegment)
	ON_BN_CLICKED(IDC_RADIO_KMEANS, &Csegmentation_mfcDlg::OnBnClickedRadioKmeans)
	ON_BN_CLICKED(IDC_RADIO_DBSCAN, &Csegmentation_mfcDlg::OnBnClickedRadioDbscan)
	ON_BN_CLICKED(IDC_RADIO_EDGE_YES, &Csegmentation_mfcDlg::OnBnClickedRadioEdgeYes)
	ON_BN_CLICKED(IDC_RADIO_EDGE_NO, &Csegmentation_mfcDlg::OnBnClickedRadioEdgeNo)
	ON_BN_CLICKED(IDC_OpenImg, &Csegmentation_mfcDlg::OnBnClickedOpenimg)
	ON_STN_CLICKED(IDC_STATIC_DIM, &Csegmentation_mfcDlg::OnStnClickedStaticDim)
	ON_STN_CLICKED(IDC_STATIC_HS, &Csegmentation_mfcDlg::OnStnClickedStaticHs)
	ON_STN_CLICKED(IDC_STATIC_HR, &Csegmentation_mfcDlg::OnStnClickedStaticHr)
	ON_STN_CLICKED(IDC_STATIC_KMEANS, &Csegmentation_mfcDlg::OnStnClickedStaticKmeans)
	ON_STN_CLICKED(IDC_STATIC_DBSCAN1, &Csegmentation_mfcDlg::OnStnClickedStaticDbscan1)
	ON_STN_CLICKED(IDC_STATIC_DBSCAN2, &Csegmentation_mfcDlg::OnStnClickedStaticDbscan2)
	ON_BN_CLICKED(IDC_STATIC_EXPLANATION, &Csegmentation_mfcDlg::OnBnClickedStaticExplanation)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_RESULT, &Csegmentation_mfcDlg::OnBnClickedButtonShowResult)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// Csegmentation_mfcDlg 消息处理程序

BOOL Csegmentation_mfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	((CButton *)GetDlgItem(IDC_RADIO_KMEANS))->SetCheck(TRUE);//默认选上KMEANS,并隐藏DBSCAN的参数编辑框
	 CEdit *edit1 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_EPS);
	 CEdit *edit2 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_MIN);
	 CEdit *edit3 = (CEdit*)GetDlgItem(IDC_EDIT_DIM);
	 CEdit *edit4 = (CEdit*)GetDlgItem(IDC_EDIT_HS);
	 CEdit *edit5 = (CEdit*)GetDlgItem(IDC_EDIT_HR);
	 CEdit *edit6 = (CEdit*)GetDlgItem(IDC_EDIT_KMEANS);
	 CEdit *edit7 = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
	 CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN1);
	 CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN2);
	 edit1->ShowWindow(FALSE);
	 edit2->ShowWindow(FALSE);
	 //参数的初始化
	 edit1->SetWindowText(_T("0.1"));
	 edit2->SetWindowText(_T("5"));
	 edit3->SetWindowText(_T("3"));
	 edit4->SetWindowText(_T("20"));
	 edit5->SetWindowText(_T("20"));
	 edit6->SetWindowText(_T("2"));
	 edit7->SetWindowTextW(_T("这是一个对金相图像进行分割的程序，点击参数名可查看具体信息"));
	 static1->ShowWindow(FALSE);
	 static2->ShowWindow(FALSE);

	 ((CButton *)GetDlgItem(IDC_RADIO_EDGE_NO))->SetCheck(TRUE);//默认不选边缘检测

	 count_button_click=0; //初始化分割按钮按的次数
	 clustering_mode=1; //初始化聚类方法为kmeans
	 edge_mode=0; //初始化不选择边缘检测
	 edge_count=0;
	 dim_temp = 0;
	 hs_temp = 0;
	 hr_temp = 0;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Csegmentation_mfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Csegmentation_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Csegmentation_mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//string转wstring
std::wstring s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}

//显示图像
void Csegmentation_mfcDlg::showImage( UINT ID, int flag){
	CImage image;
	if(flag ==1){ //显示原图
		image.Load(filePath);
	}
	else if(flag ==2){ //显示结果
		image.Load(_T("results\\result_clustering.png"));
	}
    CRect rectControl;
    CRect rectPicture;
    int x = image.GetWidth();
    int y = image.GetHeight();
    CWnd  *pWnd = GetDlgItem(ID);
    pWnd->GetClientRect(rectControl);

    CDC *pDc = GetDlgItem(ID)->GetDC();
    SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);

    rectPicture = CRect(rectControl.TopLeft(), CSize((int)rectControl.Width(), (int)rectControl.Height()));
    ((CStatic*)GetDlgItem(ID))->SetBitmap(NULL);
	
    //绘制图片
    image.Draw(pDc->m_hDC, rectPicture);
    image.Destroy();
    pWnd->ReleaseDC(pDc);
}





//选择图片路径
void Csegmentation_mfcDlg::OnBnClickedOpenimg() 
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL isOpen = TRUE;     //是否打开(否则为保存)  
    CString filter = L"图片 (*.png; *.jpg; *.jpeg; *.tif; *.bmp)|*.png;*.jpg;*.jpeg;*.tif;*.bmp||";   //文件过虑的类型  
    CFileDialog openFileDlg(isOpen, NULL, NULL, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);
	if(openFileDlg.DoModal() == IDOK) {  
        filePath = openFileDlg.GetPathName();  //获取文件路径
		//每读一张新图，还要初始化参数
		count_button_click=0;
		edge_count=0;
		dim_temp = 0;
	    hs_temp = 0;
		hr_temp = 0;
    }  
	else {return;}
    CWnd::SetDlgItemTextW(IDC_FILEPATH, filePath);//显示文件路径
	showImage(IDC_ORIGIN_IMAGE,1);
}



//按下分割按钮,运行分割程序
void Csegmentation_mfcDlg::OnBnClickedButtonSegment()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_STATE); //显示分割状态
	CEdit *edit2 = (CEdit*)GetDlgItem(IDC_EDIT_RESULT);//显示分割分类结果
	LPCTSTR results_class;

	std::string imgPath;//图片路径

	//转换为opencv中符合imread参数要求的文件路径
	imgPath = CT2CA(filePath.GetBuffer(0));
	for(int i=0;i<imgPath.length();i++){
		if(imgPath[i]=='\\'){
			imgPath[i]='/';
	    }
	}
	//imgPath="1.png";

	//获取参数
	CString str_dim, str_hs, str_hr, str_kmeans_k, str_dbscan_eps, str_dbscan_min;
	GetDlgItemText(IDC_EDIT_DIM,str_dim);
	GetDlgItemText(IDC_EDIT_HS,str_hs);
	GetDlgItemText(IDC_EDIT_HR,str_hr);
	GetDlgItemText(IDC_EDIT_KMEANS,str_kmeans_k);
	GetDlgItemText(IDC_EDIT_DBSCAN_EPS,str_dbscan_eps);
	GetDlgItemText(IDC_EDIT_DBSCAN_MIN,str_dbscan_min);

	//_tcstoi  _tcstod   _tstoi...
	dim=_tstoi(str_dim);
	hs=_tstoi(str_hs);
	hr=_tstoi(str_hr);
	kmeans_k=_tstoi(str_kmeans_k);
	dbscan_min=_tstoi(str_dbscan_min);
	dbscan_eps=_tstof(str_dbscan_eps);
	
	//判断参数是否在取值范围内
	if(imgPath.length() == 0){
		MessageBox(_T("图片不能为空"));
		return;
	}
	else if((dim != 3) && (dim != 5) && (dim != 7) && (dim != 9)){
		MessageBox(_T("参数dim不在取值范围内"));
		return;
	}
	else if((clustering_mode==1) &&((hs < 10) || (hs >50) || (hr < 10) || (hr >50))){
		MessageBox(_T("参数hs或hr不在取值范围内"));
		return;
	}
	else if((clustering_mode==2) && ((dbscan_eps < 0) || (dbscan_eps == 0) || (dbscan_eps >0.5) || (dbscan_min < 1) || (dbscan_min >30))){
		MessageBox(_T("参数Eps或minPts不在取值范围内"));
		return;
	}
	else{
		count_button_click++;
		//如果第一部分参数不变的话，可以利用上次的中间结果
		if((count_button_click != 1) && (dim == dim_temp) && (hs == hs_temp) && (hr == hr_temp)){
			//MessageBox(_T("如果第一部分参数不变的话"));
			edit->SetWindowTextW(_T("正在分割，请稍等..."));
			MessageBox(_T("图像分割已开始"));
			segmentImg segmentRun;
			segmentRun.segmentMainSimple(imgPath,edge_mode,edge_count,clustering_mode,kmeans_k,dbscan_eps,dbscan_min);//执行图像分割
			
			edit->SetWindowTextW(_T("分割结束"));

			std::wstring stemp = s2ws(segmentRun.results);
			results_class=stemp.c_str();
			edit2->SetWindowTextW(results_class);

		}
		else{
			edit->SetWindowTextW(_T("正在分割，请稍等..."));
			MessageBox(_T("图像分割已开始"));
			dim_temp = dim;
			hs_temp = hs;
			hr_temp = hr;
            segmentImg segmentRun;
			//segmentRun.showImg(imgPath); //测试
			segmentRun.segmentMain(imgPath,dim,hs,hr,edge_mode,edge_count,clustering_mode,kmeans_k,dbscan_eps,dbscan_min);//执行图像分割
			
			
			edit->SetWindowTextW(_T("分割结束"));

			std::wstring stemp = s2ws(segmentRun.results);
			results_class=stemp.c_str();
			edit2->SetWindowTextW(results_class);

		}
	}
	showImage(IDC_RESULT_IMAGE,2); //显示图片

}




//当选择kmeans时,隐藏DBSCAN相关选项
void Csegmentation_mfcDlg::OnBnClickedRadioKmeans()
{
	// TODO: 在此添加控件通知处理程序代码
	clustering_mode=1;
	CEdit *edit1 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_EPS);
	CEdit *edit2 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_MIN);
	CEdit *edit3= (CEdit*)GetDlgItem(IDC_EDIT_KMEANS);
	CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN1);
	CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN2);
	CStatic *static3 = (CStatic*)GetDlgItem(IDC_STATIC_KMEANS);
	edit1->ShowWindow(FALSE);
	edit2->ShowWindow(FALSE);
	edit3->ShowWindow(TRUE);
	static1->ShowWindow(FALSE);
	static2->ShowWindow(FALSE);
	static3->ShowWindow(TRUE);
}

//当选择DBSCAN时,隐藏kmeans相关选项
void Csegmentation_mfcDlg::OnBnClickedRadioDbscan()
{
	// TODO: 在此添加控件通知处理程序代码
	clustering_mode=2;
	CEdit *edit1 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_EPS);
	CEdit *edit2 = (CEdit*)GetDlgItem(IDC_EDIT_DBSCAN_MIN);
	CEdit *edit3= (CEdit*)GetDlgItem(IDC_EDIT_KMEANS);
	CStatic *static1 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN1);
	CStatic *static2 = (CStatic*)GetDlgItem(IDC_STATIC_DBSCAN2);
	CStatic *static3 = (CStatic*)GetDlgItem(IDC_STATIC_KMEANS);
	edit1->ShowWindow(TRUE);
	edit2->ShowWindow(TRUE);
	edit3->ShowWindow(FALSE);
	static1->ShowWindow(TRUE);
	static2->ShowWindow(TRUE);
	static3->ShowWindow(FALSE);
}


void Csegmentation_mfcDlg::OnBnClickedRadioEdgeYes()
{
	// TODO: 在此添加控件通知处理程序代码
	edge_mode=1;
}


void Csegmentation_mfcDlg::OnBnClickedRadioEdgeNo()
{
	// TODO: 在此添加控件通知处理程序代码
	edge_mode=0;
}



void Csegmentation_mfcDlg::OnStnClickedStaticDim()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
	edit->SetWindowTextW(_T("该参数是高斯滤波中模板的大小，在本程序中的取值范围是{3,5,7,9}"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticHs()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("该参数是mean shift中的空间带宽大小，在本程序中的取值范围是[10,50]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticHr()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("该参数是mean shift中的颜色带宽大小，在本程序中的取值范围是[10,50]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticKmeans()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("该参数是kmeans中的k值的大小，即分类的类别数目"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticDbscan1()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("该参数是DBSCAN中的Eps值的大小，即核心点领域的半径大小，在本程序中的取值范围是(0,0.5]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticDbscan2()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("该参数是DBSCAN中的minPts值的大小，即核心点邻域内其它点的数目，在本程序中的取值范围是[1,30]"));
}


void Csegmentation_mfcDlg::OnBnClickedStaticExplanation()
{
	// TODO: 在此添加控件通知处理程序代码
}


void Csegmentation_mfcDlg::OnBnClickedButtonShowResult()
{
	// TODO: 在此添加控件通知处理程序代码
	if(filePath.GetLength() == 0 || count_button_click ==0){MessageBox(_T("图片未读取或者还未进行分割!")); return;}
	showImage(IDC_ORIGIN_IMAGE,1); //显示原图
	showImage(IDC_RESULT_IMAGE,2); //显示结果图片
}


void Csegmentation_mfcDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnLButtonDown(nFlags, point);

	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
	edit->SetWindowTextW(_T("这是一个对金相图像进行分割的程序，点击参数名可查看具体信息"));
}
