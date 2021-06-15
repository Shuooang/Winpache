// DlgSizeAdjust.cpp: 구현 파일
//

#include "pch.h"
#include "DlgSizeAdjust.h"
#include "afxdialogex.h"

#include "MFCAppServerEx2.h"
#include "resource.h"
#include "ApiSite1.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/ThreadPool.h"

// DlgSizeAdjust 대화 상자

IMPLEMENT_DYNAMIC(DlgSizeAdjust, CDlgInvokable)

DlgSizeAdjust::DlgSizeAdjust(CWnd* pParent /*=nullptr*/)
	: CDlgInvokable(IDD_ImageSizeAdjust, pParent)
	, _folder(_T("C:\\svr\\upload_files-resized\\shop"))
	, _width(1024)
	, _height(1024)
	, _Simulation(TRUE)
	, _ExceptDir(_T("\\docs\\"))
{

}

DlgSizeAdjust::~DlgSizeAdjust()
{
}

void DlgSizeAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SrcImagePath, _folder);
	DDX_Text(pDX, IDC_Width, _width);
	DDX_Text(pDX, IDC_Height, _height);
	DDX_Check(pDX, IDC_Simulation, _Simulation);
	DDX_Text(pDX, IDC_ExceptDir, _ExceptDir);
}


BEGIN_MESSAGE_MAP(DlgSizeAdjust, CDlgInvokable)
	ON_BN_CLICKED(IDOK, &DlgSizeAdjust::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DlgSizeAdjust::OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgSizeAdjust 메시지 처리기


void DlgSizeAdjust::OnBnClickedOk()
{
	UpdateData();
	auto app = (CMFCAppServerEx2*)AfxGetApp();

	KwEnableWindow(this, IDOK, FALSE);
	KwEnableWindow(this, IDCANCEL, FALSE);
	CSize szf(_width, _height);
	auto run = [&]() -> void
	{
		BACKGROUND(2);

		_nImgFileChanged = _nImgFile = _nExceptDir = 0;
		FileAdjust(_folder + '\\', szf);

		KwBeginInvoke(this, ([&]()-> void
			{ //?beginInvoke 4
				KwMessageBox(L"Completed!");
				KwEnableWindow(this, IDOK, TRUE);
				KwEnableWindow(this, IDCANCEL, TRUE);
			}));
	};
	QueueFUNCN(run, "FileAdjust");
}

void DlgSizeAdjust::FileAdjust(LPCTSTR pDir, CSize szf)
{
	BOOL bRecursive = 1;
	CString path1 = pDir;

	if (path1.Right(1) != '\\')
		path1 += '\\';

	WIN32_FIND_DATA wfd;

	HANDLE h = FindFirstFile(path1 + L"*.*", &wfd);
	if (h == INVALID_HANDLE_VALUE)
		return ;
	static PWS arExt[] = { L"jpg", L"jpeg", L"png" };
	CStringArray arSub;
	for (;;)
	{
		CString sFile = wfd.cFileName;
		bool bDoReal = true;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
		{
			if (bRecursive)	{
				if (sFile != L"." && sFile != L"..")	{
					arSub.Add(sFile);
				}
			}
		}
		else	
		{
			CString sFile1 = path1 + sFile;
			CString sExptDir;
			auto idot = sFile.ReverseFind('.');
			CString ext;
			BOOL found = FALSE;
			if (idot >= 0)	{
				ext = sFile.Mid(idot + 1);
				ext.MakeLower();
				for (auto sExt : arExt) {
					if (ext == sExt) {
						found = TRUE;
						break;
					}
				}
			}
				
			if (found)
			{
				if (path1.Find(_ExceptDir) >= 0)
				{
					bDoReal = false;
					_nExceptDir++;
				}
				_nImgFile++;

				CImage img;
				HRESULT res = NULL;
				//HGLOBAL h_buffer = nullptr;
				//IStream* p_stream = KwMemoryToStream((LPVOID)idata, isize, h_buffer);
				try 
				{
					if (bDoReal)
					{
						img.Load(sFile1);
						if (img.IsNull()) {
							throw("Image file load Error.");
						}
						KAtEnd d_img([&]() {
							img.Destroy();
							});
						CRect rc2(CPoint(0, 0), szf);//크기는 배열에 원본 크기가 빠지 므로 -1
						CRect rc3;
						CImage sm_img;
						CSize isz(img.GetWidth(), img.GetHeight());
						if (isz.cx > szf.cx || isz.cy > szf.cy) 
						{
							rc3 = KwCalcInplaceRect(rc2, isz);//원본이 더 클때만
							if (!_Simulation)
							{
								CImage sm_img;
								sm_img.Create(rc3.Width(), rc3.Height(), 32);
								HDC smdc = sm_img.GetDC();
								KAtEnd d_DC([&]() {
									sm_img.ReleaseDC();
									sm_img.Destroy();
									});
								SetStretchBltMode(smdc, COLORONCOLOR);
								img.StretchBlt(smdc, 0, 0, rc3.Width(), rc3.Height(), SRCCOPY);
								CString sOld = sFile1 + _T(".old");

								try {
									KwSetReadOnly(sFile1, false);
									CFile::Rename(sFile1, sOld);
								}catch (...) {	throw("Image file Rename Error.");	}
								sm_img.Save(sFile1);
								try {
									DeleteFile(sOld); 
								}catch (...) { throw("Image Delete Error."); }
							}
							_nImgFileChanged++;
						}
					}
					else {
						//_nImgFileChanged++;
					}
				} catch(PAS serr) {
					TRACE("%s\n",serr);
				}
				CString str; str.Format(L"%s %d(x), %d/%d", sFile1, _nExceptDir,_nImgFileChanged, _nImgFile);
				KwBeginInvoke(this, ([&, str]()-> void { //?beginInvoke 4
					SetDlgItemText(IDC_STATIC1, str);
					}));
			}
		}

		if (FindNextFile(h, &wfd) == FALSE)
			break;
	}
	FindClose(h);

	for (int i = 0; i <= arSub.GetUpperBound(); i++)	{
		CString subD = arSub[i];
		FileAdjust(path1 + subD, szf);
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
}


void DlgSizeAdjust::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}






