// CDlgDragImages.cpp: 구현 파일
//

#include "pch.h"
#include "MFCAppPostTest1.h"
#include "CDlgDragImages.h"
#include "afxdialogex.h"
#include "KwLib64/DlgTool.h"

// CDlgDragImages 대화 상자

IMPLEMENT_DYNAMIC(CDlgDragImages, CDialogEx)

CDlgDragImages::CDlgDragImages(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DragImages, pParent)
{

}

CDlgDragImages::~CDlgDragImages()
{
}

void CDlgDragImages::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _list);
}


BEGIN_MESSAGE_MAP(CDlgDragImages, CDialogEx)
	ON_WM_DROPFILES()
//	ON_MESSAGE(WM_DROPFILES, &CDlgDragImages::OnDropFiles)// Message Handler for Drang and Drop
END_MESSAGE_MAP()


// CDlgDragImages 메시지 처리기
void CDlgDragImages::OnDropFiles(HDROP hDropInfo)
//LRESULT CDlgDragImages::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	//HDROP hDropInfo = (HDROP)wParam;
	SetActiveWindow();      // activate us first !
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	CWinApp* pApp = AfxGetApp();
	TCHAR szFileName[1024];
	for(UINT iFile = 0; iFile < nFiles; iFile++)
	{
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		//pApp->OpenDocumentFile(szFileName);
		//AddFile(CString(szFileName));
		CString fname(szFileName);
		int ifname = fname.ReverseFind('\\');
		if(ifname < 0)
		{
			AfxMessageBox(L"Bad file extension.");
			continue;
		}
		int iext = fname.ReverseFind('.');
		CString ext(fname.Mid(iext)); //ex: .jpg
		CStringA extA(ext); //ex: .jpg
		CString cntType(KwContentType(extA));
		// 원래는 "image/jpeg" 인데 URL에 쓰기 때문에 '/' 대신 '_'를 썼다.
		if(cntType.GetLength() == 0)
		{
			cntType.Format(L"application/%s", ext.Mid(1)); // . 빼고
			//KwMessageBox(L"File extension (%s) is not supported.", ext);
			//continue;
		}
		_arFName.Add(fname);
		_arCntType.Add(cntType); // image/jpeg
		_list.AddString(fname); // => CPostTestView::SendOneSyncImage
	}
	::DragFinish(hDropInfo);
}



BOOL CDlgDragImages::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
// http://blog.naver.com/PostView.nhn?blogId=tipsware&logNo=100198054211
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles();

	_list.AddString(L"Drop a files");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
