
// segmentation_mfcDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "segmentation_mfc.h"
#include "segmentation_mfcDlg.h"
#include "afxdialogex.h"
#include "segment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Csegmentation_mfcDlg �Ի���



Csegmentation_mfcDlg::Csegmentation_mfcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Csegmentation_mfcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Csegmentation_mfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//��Ϣӳ��
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


// Csegmentation_mfcDlg ��Ϣ�������

BOOL Csegmentation_mfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	((CButton *)GetDlgItem(IDC_RADIO_KMEANS))->SetCheck(TRUE);//Ĭ��ѡ��KMEANS,������DBSCAN�Ĳ����༭��
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
	 //�����ĳ�ʼ��
	 edit1->SetWindowText(_T("0.1"));
	 edit2->SetWindowText(_T("5"));
	 edit3->SetWindowText(_T("3"));
	 edit4->SetWindowText(_T("20"));
	 edit5->SetWindowText(_T("20"));
	 edit6->SetWindowText(_T("2"));
	 edit7->SetWindowTextW(_T("����һ���Խ���ͼ����зָ�ĳ��򣬵���������ɲ鿴������Ϣ"));
	 static1->ShowWindow(FALSE);
	 static2->ShowWindow(FALSE);

	 ((CButton *)GetDlgItem(IDC_RADIO_EDGE_NO))->SetCheck(TRUE);//Ĭ�ϲ�ѡ��Ե���

	 count_button_click=0; //��ʼ���ָť���Ĵ���
	 clustering_mode=1; //��ʼ�����෽��Ϊkmeans
	 edge_mode=0; //��ʼ����ѡ���Ե���
	 edge_count=0;
	 dim_temp = 0;
	 hs_temp = 0;
	 hr_temp = 0;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Csegmentation_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Csegmentation_mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//stringתwstring
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

//��ʾͼ��
void Csegmentation_mfcDlg::showImage( UINT ID, int flag){
	CImage image;
	if(flag ==1){ //��ʾԭͼ
		image.Load(filePath);
	}
	else if(flag ==2){ //��ʾ���
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
	
    //����ͼƬ
    image.Draw(pDc->m_hDC, rectPicture);
    image.Destroy();
    pWnd->ReleaseDC(pDc);
}





//ѡ��ͼƬ·��
void Csegmentation_mfcDlg::OnBnClickedOpenimg() 
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
    CString filter = L"ͼƬ (*.png; *.jpg; *.jpeg; *.tif; *.bmp)|*.png;*.jpg;*.jpeg;*.tif;*.bmp||";   //�ļ����ǵ�����  
    CFileDialog openFileDlg(isOpen, NULL, NULL, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);
	if(openFileDlg.DoModal() == IDOK) {  
        filePath = openFileDlg.GetPathName();  //��ȡ�ļ�·��
		//ÿ��һ����ͼ����Ҫ��ʼ������
		count_button_click=0;
		edge_count=0;
		dim_temp = 0;
	    hs_temp = 0;
		hr_temp = 0;
    }  
	else {return;}
    CWnd::SetDlgItemTextW(IDC_FILEPATH, filePath);//��ʾ�ļ�·��
	showImage(IDC_ORIGIN_IMAGE,1);
}



//���·ָť,���зָ����
void Csegmentation_mfcDlg::OnBnClickedButtonSegment()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_STATE); //��ʾ�ָ�״̬
	CEdit *edit2 = (CEdit*)GetDlgItem(IDC_EDIT_RESULT);//��ʾ�ָ������
	LPCTSTR results_class;

	std::string imgPath;//ͼƬ·��

	//ת��Ϊopencv�з���imread����Ҫ����ļ�·��
	imgPath = CT2CA(filePath.GetBuffer(0));
	for(int i=0;i<imgPath.length();i++){
		if(imgPath[i]=='\\'){
			imgPath[i]='/';
	    }
	}
	//imgPath="1.png";

	//��ȡ����
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
	
	//�жϲ����Ƿ���ȡֵ��Χ��
	if(imgPath.length() == 0){
		MessageBox(_T("ͼƬ����Ϊ��"));
		return;
	}
	else if((dim != 3) && (dim != 5) && (dim != 7) && (dim != 9)){
		MessageBox(_T("����dim����ȡֵ��Χ��"));
		return;
	}
	else if((clustering_mode==1) &&((hs < 10) || (hs >50) || (hr < 10) || (hr >50))){
		MessageBox(_T("����hs��hr����ȡֵ��Χ��"));
		return;
	}
	else if((clustering_mode==2) && ((dbscan_eps < 0) || (dbscan_eps == 0) || (dbscan_eps >0.5) || (dbscan_min < 1) || (dbscan_min >30))){
		MessageBox(_T("����Eps��minPts����ȡֵ��Χ��"));
		return;
	}
	else{
		count_button_click++;
		//�����һ���ֲ�������Ļ������������ϴε��м���
		if((count_button_click != 1) && (dim == dim_temp) && (hs == hs_temp) && (hr == hr_temp)){
			//MessageBox(_T("�����һ���ֲ�������Ļ�"));
			edit->SetWindowTextW(_T("���ڷָ���Ե�..."));
			MessageBox(_T("ͼ��ָ��ѿ�ʼ"));
			segmentImg segmentRun;
			segmentRun.segmentMainSimple(imgPath,edge_mode,edge_count,clustering_mode,kmeans_k,dbscan_eps,dbscan_min);//ִ��ͼ��ָ�
			
			edit->SetWindowTextW(_T("�ָ����"));

			std::wstring stemp = s2ws(segmentRun.results);
			results_class=stemp.c_str();
			edit2->SetWindowTextW(results_class);

		}
		else{
			edit->SetWindowTextW(_T("���ڷָ���Ե�..."));
			MessageBox(_T("ͼ��ָ��ѿ�ʼ"));
			dim_temp = dim;
			hs_temp = hs;
			hr_temp = hr;
            segmentImg segmentRun;
			//segmentRun.showImg(imgPath); //����
			segmentRun.segmentMain(imgPath,dim,hs,hr,edge_mode,edge_count,clustering_mode,kmeans_k,dbscan_eps,dbscan_min);//ִ��ͼ��ָ�
			
			
			edit->SetWindowTextW(_T("�ָ����"));

			std::wstring stemp = s2ws(segmentRun.results);
			results_class=stemp.c_str();
			edit2->SetWindowTextW(results_class);

		}
	}
	showImage(IDC_RESULT_IMAGE,2); //��ʾͼƬ

}




//��ѡ��kmeansʱ,����DBSCAN���ѡ��
void Csegmentation_mfcDlg::OnBnClickedRadioKmeans()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

//��ѡ��DBSCANʱ,����kmeans���ѡ��
void Csegmentation_mfcDlg::OnBnClickedRadioDbscan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	edge_mode=1;
}


void Csegmentation_mfcDlg::OnBnClickedRadioEdgeNo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	edge_mode=0;
}



void Csegmentation_mfcDlg::OnStnClickedStaticDim()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
	edit->SetWindowTextW(_T("�ò����Ǹ�˹�˲���ģ��Ĵ�С���ڱ������е�ȡֵ��Χ��{3,5,7,9}"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticHs()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("�ò�����mean shift�еĿռ�����С���ڱ������е�ȡֵ��Χ��[10,50]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticHr()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("�ò�����mean shift�е���ɫ�����С���ڱ������е�ȡֵ��Χ��[10,50]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticKmeans()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("�ò�����kmeans�е�kֵ�Ĵ�С��������������Ŀ"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticDbscan1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("�ò�����DBSCAN�е�Epsֵ�Ĵ�С�������ĵ�����İ뾶��С���ڱ������е�ȡֵ��Χ��(0,0.5]"));
}


void Csegmentation_mfcDlg::OnStnClickedStaticDbscan2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
    edit->SetWindowTextW(_T("�ò�����DBSCAN�е�minPtsֵ�Ĵ�С�������ĵ����������������Ŀ���ڱ������е�ȡֵ��Χ��[1,30]"));
}


void Csegmentation_mfcDlg::OnBnClickedStaticExplanation()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void Csegmentation_mfcDlg::OnBnClickedButtonShowResult()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(filePath.GetLength() == 0 || count_button_click ==0){MessageBox(_T("ͼƬδ��ȡ���߻�δ���зָ�!")); return;}
	showImage(IDC_ORIGIN_IMAGE,1); //��ʾԭͼ
	showImage(IDC_RESULT_IMAGE,2); //��ʾ���ͼƬ
}


void Csegmentation_mfcDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnLButtonDown(nFlags, point);

	CEdit *edit = (CEdit*)GetDlgItem(IDC_EDIT_EXPLANATION);
	edit->SetWindowTextW(_T("����һ���Խ���ͼ����зָ�ĳ��򣬵���������ɲ鿴������Ϣ"));
}
