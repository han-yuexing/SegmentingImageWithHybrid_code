SegmentingImageWithHybrid_code

An algorithm based on hybrid algorithm, which combines the Gaussian filter, the mean shift method, the FloodFill, the improved FDoG, and the clustering to segment metallographic images.

The work has been published. If you use the software for your research, please cite the reference as follows:

Yuexing Han*, Chuanbin Lai, Bing Wang, Hui Gu. Segmenting Images with Complex Textures by Using Hybrid Algorithm [J]. Journal of Electronic Imaging, 28(1):13-30,2019

E-mail address of Han Yuexing: hanyuexing@gmail.com

Abstract Application of computer images processing technology to analyze materials microstructural images, particularly metallographic images, has received increasing attention. The metallographic images contain the mesoscopic information on structural relation and components of materials. Quantitative analysis of these images can help to correlate the materials structures to their performance and properties at various levels. There are two challengeable issues necessary to be resolved, i.e., automatic segmentation and classification of different microscopic structures in metallographic images. Since the metallographic images often contain complex textures, the segmentation of them is usually inaccurate with present methods. We propose a hybrid algorithm, which combines the Gaussian filter, the mean shift method, the FloodFill, the improved FDoG, and the clustering to resolve the issues. The experiment results and the comparative results show that our method is effective to segment and classify the microstructural elements in metallographic images with complex textures.




================================================================================
    MICROSOFT ������� : segmentation_mfc ��Ŀ����
===============================================================================

Ӧ�ó�������Ϊ�������˴� segmentation_mfc Ӧ�ó��򡣴�Ӧ�ó��򲻽���ʾ Microsoft ������Ļ���ʹ�÷�����������Ϊ����дӦ�ó������㡣

���ļ���Ҫ������� segmentation_mfc Ӧ�ó����ÿ���ļ������ݡ�

segmentation_mfc.vcxproj
    ����ʹ��Ӧ�ó��������ɵ� VC++ ��Ŀ������Ŀ�ļ������а������ɸ��ļ��� Visual C++ �İ汾��Ϣ���Լ��й�ʹ��Ӧ�ó�����ѡ���ƽ̨�����ú���Ŀ���ܵ���Ϣ��

segmentation_mfc.vcxproj.filters
    ����ʹ�á�Ӧ�ó����򵼡����ɵ� VC++ ��Ŀɸѡ���ļ����������й���Ŀ�ļ���ɸѡ��֮��Ĺ�����Ϣ���� IDE �У�ͨ�����ֹ��������ض��ڵ����Է�����ʽ��ʾ����������չ�����ļ������磬��.cpp���ļ��롰Դ�ļ���ɸѡ��������

segmentation_mfc.h
    ����Ӧ�ó������ͷ�ļ���
    ���а���������Ŀ�ض��ı�ͷ������ Resource.h���������� Csegmentation_mfcApp Ӧ�ó����ࡣ

segmentation_mfc.cpp
    ���ǰ���Ӧ�ó����� Csegmentation_mfcApp ����Ӧ�ó���Դ�ļ���

segmentation_mfc.rc
    ���ǳ���ʹ�õ����� Microsoft Windows ��Դ���б������� RES ��Ŀ¼�д洢��ͼ�ꡢλͼ�͹�ꡣ���ļ�����ֱ���� Microsoft Visual C++ �н��б༭����Ŀ��Դ������ 2052 �С�

res\segmentation_mfc.ico
    ��������Ӧ�ó���ͼ���ͼ���ļ�����ͼ�����������Դ�ļ� segmentation_mfc.rc �С�

res\segmentation_mfc.rc2
    ���ļ��������� Microsoft Visual C++ �н��б༭����Դ����Ӧ�ý���������Դ�༭���༭��������Դ���ڴ��ļ��С�


/////////////////////////////////////////////////////////////////////////////

Ӧ�ó����򵼴���һ���Ի����ࣺ

segmentation_mfcDlg.h��segmentation_mfcDlg.cpp - �Ի���
    ��Щ�ļ����� Csegmentation_mfcDlg �ࡣ���ඨ��Ӧ�ó�������Ի������Ϊ���Ի���ģ������� segmentation_mfc.rc �У����ļ������� Microsoft Visual C++ �б༭��

/////////////////////////////////////////////////////////////////////////////

�������ܣ�

ActiveX �ؼ�
    ��Ӧ�ó��������ʹ�� ActiveX �ؼ���֧�֡�

��ӡ�ʹ�ӡԤ��֧��
    Ӧ�ó�����ͨ���� MFC ����� CView ���еĳ�Ա�������ɴ��룬�������ӡ����ӡ���úʹ�ӡԤ�����

/////////////////////////////////////////////////////////////////////////////

������׼�ļ�:

StdAfx.h, StdAfx.cpp
    ��Щ�ļ�����������Ϊ segmentation_mfc.pch ��Ԥ����ͷ (PCH) �ļ�����Ϊ StdAfx.obj ��Ԥ���������ļ���

Resource.h
    ���Ǳ�׼ͷ�ļ��������ڶ����µ���Դ ID��Microsoft Visual C++ ����ȡ�����´��ļ���

segmentation_mfc.manifest
	Windows XP ʹ��Ӧ�ó����嵥�ļ��������ض��汾�Ĳ��г��򼯵�Ӧ�ó�����������س���ʹ����Щ��Ϣ���ӳ��򼯻����м�����Ӧ�ĳ��򼯣��������䲻��Ӧ�ó�����ʡ�Ӧ�ó����嵥���ܻ�������ڣ�����Ϊ��Ӧ�ó����ִ���ļ���װ��ͬһ�ļ����е��ⲿ .manifest �ļ��������·ַ���������������Դ����ʽ�����ڿ�ִ���ļ��С�
/////////////////////////////////////////////////////////////////////////////

����ע��:

Ӧ�ó�����ʹ�á�TODO:����ָʾӦ��ӻ��Զ����Դ���벿�֡�

���Ӧ�ó���ʹ�ù��� DLL �е� MFC��������Ҫ���·ַ� MFC DLL�����Ӧ�ó�����ʹ�õ����������ϵͳ���������ò�ͬ������Ҫ���·ַ���Ӧ�ı��ػ���Դ mfc110XXX.DLL��
�й���������ĸ�����Ϣ����μ� MSDN �ĵ����й����·ַ� Visual C++ Ӧ�ó���Ĳ��֡�

/////////////////////////////////////////////////////////////////////////////