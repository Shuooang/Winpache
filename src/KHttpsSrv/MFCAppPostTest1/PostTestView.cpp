
// PostTestView.cpp: CPostTestView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCAppPostTest1.h"
#endif

#include "PostTestDoc.h"
#include "PostTestView.h"

#include "KwLib64/ThreadPool.h"
#include "KwLib64/Lock.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPostTestView

IMPLEMENT_DYNCREATE(CPostTestView, CFormInvokable)

BEGIN_MESSAGE_MAP(CPostTestView, CFormInvokable)
		// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_Send, &CPostTestView::OnBnClickedSend)
	ON_LBN_SELCHANGE(IDC_ListResponse, &CPostTestView::OnLbnSelchangeListresponse)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_UploadImages, &CPostTestView::OnBnClickedUploadimages)
	ON_BN_CLICKED(IDC_Stop, &CPostTestView::OnBnClickedStop)
END_MESSAGE_MAP()



// CPostTestView 생성/소멸

CPostTestView::CPostTestView() noexcept
	: CFormInvokable(IDD_MFCAPPPOSTTEST1_FORM) //할아버지 객체 것을 바로 부를수 없어서 아버지꺼 부름.
	, _iSend(0)
	, _MaxItems(10)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CPostTestView::~CPostTestView()
{
}

void CPostTestView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Method, _cMethod);
	//DDX_Control(pDX, IDC_URL, _cURL);

	DDX_Control(pDX, IDC_URL, c_URL);
	DDX_Control(pDX, IDC_Request, c_Request);
	DDX_Control(pDX, IDC_Response, c_Response);
	DDX_Control(pDX, IDC_Count, c_Count);
	DDX_Control(pDX, IDC_Delay, c_Delay);
	DDX_Control(pDX, IDC_Thread, c_Thread);
	DDX_Control(pDX, IDC_Parallel, c_Parallel);

	//DDX_Text(pDX, IDC_Method  , _method);
	//DDX_Text(pDX, IDC_URL     , _URL);
	//DDX_Text(pDX, IDC_Request , _request);
	//DDX_Text(pDX, IDC_Response, _response);

	CPostTestDoc* doc = GetDocument();
	if(doc == NULL)
		return;
	KDDX_CBBoxA(_Method);
	//if(pDX->m_bSaveAndValidate) {
	//	if(pDX->m_bSaveAndValidate) {	CString v; 
	//	DDX_CBString(pDX, IDC_Method, v); doc->_Method = CStringA(v);
	//}
	//else {
	//	DDX_CBString(pDX, IDC_Method, CStringW(doc->_Method)); }}
	KDDX_Text(_URL);
	KDDX_TextA(_Request);
	KDDX_TextA(_Response);
	KDDX_Text(_Count);
	KDDX_Text(_Delay);
	KDDX_Text(_Thread);
	KDDX_Check(_noCache);
	KDDX_Check(_Parallel);
	KDDX_Check(_AddSrl);
	KDDX_Check(_AddUuid);

	DDX_Control(pDX, IDC_ListResponse, _cListResponse);
	DDX_Text(pDX, IDC_MaxItems, _MaxItems);
	//DDX_Check(pDX, IDC_bParallel, _bParallel);
	//DDX_Text(pDX, IDC_NumberTest, _NumTest);
	DDV_MinMaxInt(pDX, doc->_Thread, 1, 32);
	DDV_MinMaxInt(pDX, doc->_Delay, 0, 3600000);
	DDV_MinMaxUInt(pDX, doc->_Count, 1, 4000000000);
// 	DDX_Text(pDX, IDC_Thread2, _test);
// 	DDV_MinMaxUInt(pDX, _test, 1, 4000000000);
}

BOOL CPostTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CPostTestView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	GetDlgItem(IDC_Stop)->EnableWindow(0);

	UpdateData(0);
}


// CPostTestView 인쇄

BOOL CPostTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPostTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPostTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CPostTestView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// CPostTestView 진단

#ifdef _DEBUG
void CPostTestView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPostTestView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPostTestDoc* CPostTestView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPostTestDoc)));
	return (CPostTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CPostTestView 메시지 처리기
#include "KwLib64\HTTPS\HttpClient.h"
static int svi = 0;

void CPostTestView::OnBnClickedSend()
{
	UpdateData();
	GetDlgItem(IDC_Send)->EnableWindow(0);
	GetDlgItem(IDC_Stop)->EnableWindow(1);

//	char* pjs = "{\"key\":\"val1\", \"ikey\":1234}";
//	cl.RequestGetSSL(L"https://localhost/api?key=xxx", &binr, 1024);//OK
//	binr.SetPtr(pjs);
//	cl.RequestGetSSL(L"https://localhost/api?key=xxx", &binr, 1024, "POST");//

	CPostTestDoc* doc = GetDocument();
	if(doc == NULL)
		return;

	if(doc->_URL.GetLength() < 10)
		KwMessageBoxError(L"URL is wrong!");

	_nResponse = 0;
	_bStop = false;
	GetDlgItem(IDC_Elapsed)->SetWindowTextW(L"milli seconds");

	_start = GetTickCount64();

	if (doc->_Method == "POST" && doc->_Request.IsEmpty())
	{
		AfxMessageBox(L"POST data is empty.");
		return;
	}

	if(doc->_Parallel)
		SendAsync();
	else
		SendSync();
}

void CPostTestView::SendSync()
{
	CPostTestDoc* doc = GetDocument();
	if(doc == NULL)
		return;
	// URL에 같으면 자동캐시로 서버에 request를 보내지 않고 response 동일한거를 리턴 하는데, 
// 응답 stream이 NULL이므로 만약 쓰려면 자체 캐시를 써야 한다.
// 여기서는 URL 뒤에 파라미터라도 조금씩 바꿔서 매번 다른 URL을 준다.
	CString surl;
	CHttpClient cl;
	KBinary binr, bin;

	CStringA jsa;//(doc->_Request);
	KwCharToUTF8(doc->_Request, jsa);
	if(doc->_Method == "POST")
		bin.SetPtr((PAS)jsa, jsa.GetLength());
	for(UINT i=0;i<doc->_Count;i++)
	{
		AddUrlParams(surl);
// 		surl = doc->_URL;
// 		if(doc->_AddSrl)
// 		{
// 			WCHAR ch = '&';
// 			if(surl.Find('?') < 0)
// 				ch = '?';
// 			CString sw; sw.Format(L"%csrl=%d", ch, _iSend++); //svi
// 			surl += sw;
// 		}
// 		if(doc->_AddUuid)
// 		{
// 			WCHAR ch = '&';
// 			if(surl.Find('?') < 0)
// 				ch = '?';
// 			CString sw; sw.Format(L"%cuuid=%d", ch, _iSend++); //svi
// 			surl += sw;
// 		}

		hres hr = S_OK;
		std::map<string, string> hdrs;
		if(doc->_noCache)
			hdrs["Cache-Control"] = "no-cache";
		if(doc->_Method == "POST")
		{
			hdrs["Content-Type"] = "application/json";
			hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);
		}
		else
			hr = cl.RequestGetSSL(surl, &binr, &bin, doc->_Method, &hdrs);
		//cl.RequestGetSSL(surl, &binr, &bin, doc->_Method);

		CStringW sWstrR;
		KwUTF8ToWchar(binr.GetPA(), sWstrR);//utf8을 UNICODE로
		CStringA sAstrR(sWstrR);//다시 멀티바이트로 해야 저장 하지.
		doc->_Response.Format("%4u.[%u] %s", i, cl.m_dwRet, sAstrR);
		//CString sidx;sidx.Format(L"%d", i);
		SetDlgItemInt(IDC_Elapsed, (int)i);
		//CStringA resp(binr.GetPA());
		MonitorRequest(sAstrR);
		if(doc->_Delay > 0)
		{
			Sleep(doc->_Delay);
		}
	}
	CString selp;
	LONGLONG l = GetTickCount64() - _start;
	selp.Format(L"%ld msec.(%.2f)", l, (double)(doc->_Count/((double)l/1000.)));
	GetDlgItem(IDC_Elapsed)->SetWindowTextW(selp);
	UpdateData(0);

}

void CPostTestView::SendAsync()
{
	/*
	auto lmda = [&]() -> void
	{
		CPostTestDoc* doc = GetDocument();
		if(doc == NULL)
			return;
		for(int i=0;i<doc->_Count;i++)
		{
			SendOneAsync(i);
		}
	};
	QueueFUNCN(lmda, "HTTPS 테스트.");
*/
	CPostTestDoc* doc = GetDocument();
	int cnth = doc->_Delay >= 10 ? 1 : doc->_Thread;
	int nmul = doc->_Delay >= 10 ? 1 : ((int)doc->_Count < cnth ? (int)doc->_Count : cnth);

	for(int m=0;m < nmul;m++)
	{
		// 0, 4, 8, ...
		// 1, 5, 9, ...
		// 2, 6, 10 ...
		// 3, 7, 11 ...
		auto lbd = [&, doc, m, nmul, cnth]() -> void
		{
			for(int i = m; i < (int)doc->_Count; i += cnth)
			{
				SendOneAsync(i);
				//		QueueFUNCN(lmda, "HTTPS 테스트.");
				if(doc->_Delay > 0)
					Sleep(doc->_Delay);

				if(_bStop)
				{
					KwBeginInvoke(this, ([&, doc]()-> void
						{
							SetElapsed();
						}));//?beginInvoke 4
					break;
				}
				//	i = doc->_Count - 1;
			}
		};
		CMainPool::QueueFunc(lbd, "HTTPS 테스트.", __FUNCTION__, __LINE__);
	}

}

void CPostTestView::AddUrlParams(CString& surl)
{
	CPostTestDoc* doc = GetDocument();
	CString url0 = doc->_URL;
	/// surl이 비어 있고, AddSrl도 AddUuid도 없으면 이거만 리턴
	WCHAR ch = '&';
	ULONG srl = 0;
	if(doc->_AddSrl || doc->_AddUuid)
	{
		AUTOLOCK(_csi);
		InterlockedIncrement(&_iSend);//_iSend++;
		srl = _iSend;
	}
	
	if(surl.GetLength() > 0)// 뭘 가져 왔으면
	{
		ch = url0.Find('?') < 0 ? '?' : '&';
		url0 += ch;
		url0 += surl;//가져온거 붙이고
	}
	if(doc->_AddSrl)
	{
		ch = url0.Find('?') < 0 ? '?' : '&';
		CString sw; sw.Format(L"%csrl=%u", ch, srl); //svi
		url0 += sw;
	}
	if(doc->_AddUuid)
	{
		ch = url0.Find('?') < 0 ? '?' : '&';
		CString sw; sw.Format(L"%cuuid=%u", ch, srl); //svi
		url0 += sw;
	}///https://localhost/api?key=xxx&Content-type=application/xlsx&Content-Length=1996851&filename=KgmServer_Prject.xlsx&srl=2&uuid=2
	surl = url0;
}
void CPostTestView::SendOneAsync(int i)
{
	CPostTestDoc* doc = GetDocument();

	CString surl;
	CHttpClient cl;
	KBinary binr, bin;
	CStringA jsa;//(doc->_Request);
	KwCharToUTF8(doc->_Request, jsa);
	if(doc->_Method == "POST")
		bin.SetPtr((PAS)jsa, jsa.GetLength());

	AddUrlParams(surl);

// 	if(doc->_AddSrl)
// 	{
// 		WCHAR ch = '&';
// 		if(doc->_URL.Find('?') < 0)
// 			ch = '?';
// 		surl.Format(L"%s%csrl=%d", doc->_URL, ch, _iSend++); //svi
// 		//surl.Format(L"%s&srl=%d", doc->_URL, _iSend++);
// 	}
// 	else
// 		surl = doc->_URL;

	hres hr = S_OK;
	std::map<string, string> hdrs;
	if(doc->_noCache)
		hdrs["Cache-Control"] = "no-cache";
	///?주의: "no-cache"를 안붙여줘도, "POST"일때는 알아서 붙어 버리므로, srl=# 를 캐시 여부로 쓴다.
	//	hdrs["Content-Length"] = slenA;
	if (doc->_Method == "POST")
	{
		hdrs["Content-Type"] = "application/json";
		hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);
	} else
		hr = cl.RequestGetSSL(surl, &binr, &bin, doc->_Method, &hdrs);

	CStringA sAstrR;//다시 멀티바이트로 해야 저장 하지.
	if(hr != S_OK && binr.Size() == 0)
	{
		sAstrR = "Server Error!";
	}
	else
	{
		CStringW sWstrR;
		KwUTF8ToWchar(binr.GetPA(), sWstrR);//utf8을 UNICODE로
		sAstrR = CStringA(sWstrR);//다시 멀티바이트로 해야 저장 하지.
		doc->_Response.Format("%4d.[HTTP %u] %s", i, cl._status, sAstrR);
	}
	_InterlockedIncrement(&_nResponse);

	KwBeginInvoke(this, ([&, i, doc, sAstrR]()-> void
		{
			UiOutput(i, sAstrR);
		}));//?beginInvoke 4
/*
	if(doc->_Delay > 100)
	{
		KwBeginInvoke(this, ([&, i]()-> void
			{
				CString selp; selp.Format(L"%d", i);
				GetDlgItem(IDC_Elapsed)->SetWindowTextW(selp);
				//UpdateData(0);
			}));//?beginInvoke 4
	}
*/

}

void CPostTestView::UiOutput(int i, PAS sAstrR)
{
	CPostTestDoc* doc = GetDocument();
	if(_nResponse == doc->_Count)//i >= doc->_Count - 1 || _bStop)
		UiForAsync(i, sAstrR);
	else
		SetCount(i);
}
void CPostTestView::SetCount(int i)
{
	CPostTestDoc* doc = GetDocument();
	if(doc->_Delay >= 0)
	{
		CString selp; selp.Format(L"%d", i +1);
		GetDlgItem(IDC_Elapsed)->SetWindowTextW(selp);
	}
}
void CPostTestView::SendOneSyncImage(int i, PWS fname, PWS cntType)
{
	CPostTestDoc* doc = GetDocument();
	if(doc->_Method != "POST")
	{
		AfxMessageBox(L"POST shoud be send mode to upload image!");
		return;
	}

	CString surl, ssrl;
	CHttpClient cl;
	KBinary binr, bin;
	CStringA jsa;//(doc->_Request);
	
	CString fulln(fname);
	int ifname = fulln.ReverseFind('\\');
	CString fileExt = fulln.Mid(ifname + 1);
	//CString fonly = fileExt.Mid(ifname + 1);
	int dot = fileExt.ReverseFind('.');
	if(dot < 0)
	{
		AfxMessageBox(L"Bad file extension.");
		return;
	}
	CString ext = fileExt.Left(dot);

	CFile f(fulln, CFile::modeRead);
	KAtEnd d_f([&]() {	f.Close();   });
	ULONGLONG flen = f.GetLength();
	bin.Alloc(flen);
	f.Read(bin.m_p, (UINT)flen);

// 	if(doc->_AddSrl)
// 		ssrl.Format(L"&srl=%d", _iSend++); 
	CString sCntType(cntType);
	sCntType = KwReplaceStr(sCntType, L"/", L"_");
	//surl.Format(L"?Content-type=image_%s&Content-Length=%d&filename=%s%s", ext, (int)flen, fonly, ext);// , ssrl);
	surl.Format(L"Content-type=%s&Content-Length=%d&filename=%s", cntType, (int)flen, fileExt);// , ssrl);
	AddUrlParams(surl);

	/*
	CKRbVal<CStringW, CStringW> hdrs;
	hdrs.SetAt(L"Cache-Control", L"no-cache");
	hdrs.SetAt(L"Content-type", L"image/jpeg");
	CStringW slen; slen.Format(L"%d", (int)flen);
	hdrs.SetAt(L"Content-Length", slen);
	*/
	CStringA extA(ext);
	
	KStrMap hdrs;
	hdrs["Cache-Control"] = "no-cache";
	CStringA ctA(cntType);// ctA.Format("image/%s", extA);
	hdrs["Content-Type"] = ctA; //"image/jpeg";
	CStringA slenA; slenA.Format("%u", (UINT)flen);
	hdrs["Content-Length"] = slenA;

	hres hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);
	if(hr != S_OK)
		return;
	CStringW sWstrR;
	KwUTF8ToWchar(binr.GetPA(), sWstrR);//utf8을 UNICODE로
	CStringA sAstrR(sWstrR);//다시 멀티바이트로 해야 저장 하지.
	doc->_Response.Format("%4d. %s", i, sAstrR);

	_InterlockedIncrement(&_nResponse);
	KwBeginInvoke(this, ([&, i, sAstrR]()-> void
		{
			UiForAsync(i, sAstrR);
		}));//?beginInvoke 4
}


// 다끝났거나 중단 했을때만 불려진다.
void CPostTestView::UiForAsync(int i, const CStringA& resp)
{		
	CPostTestDoc* doc = GetDocument();
	if(doc == NULL)
	return;
	CString selp;
// 	if(i < doc->_Count -1)//doc이 비동기 오류 나네
// 	{
// 	//	SetDlgItemInt(IDC_Elapsed, i);
// 	}
// 	else 
	if(_nResponse == doc->_Count || _bStop) // 모든 response를 다 받으면 화면 갱신을마지막에 한번만 한다.
	{
		//KwBeginInvoke(this, ([&, doc]()-> void {
			SetElapsed();
		//}));//?beginInvoke 4

	}
	else
	{
		selp.Format(L"%d Error.", i);
		GetDlgItem(IDC_Elapsed)->SetWindowTextW(selp);
	}

	MonitorRequest(resp);
}

void CPostTestView::SetElapsed()
{
	GetDlgItem(IDC_Send)->EnableWindow(1);
	GetDlgItem(IDC_Stop)->EnableWindow(0);
	CPostTestDoc* doc = GetDocument();
	CString selp1;
	// &말고 더 들어 가면 람다식을 ()로 한던 더 싸줘야 컴파일 오류 안난다.
	LONGLONG l = GetTickCount64() - _start;//비동기는 시간이 의미 없다. 응답이 갯수 만큼 왔을때 이다.
	//CString ok = binr.GetPA() ? L"OK" : L"Fail";
	double nps = (double)(doc->_Count / ((double)l / 1000.));
	double ms1 = 1. / nps;
	selp1.Format(L"%ldms(%.2f) %.3f sec", l, nps, ms1 );
	GetDlgItem(IDC_Elapsed)->SetWindowTextW(selp1);
	//background  에서도 먹는 UI함수. 내부적으로 SendMessage를 쓰므로
	// KwBeginInvoke 이전에 해도 되지만, 그러면 위에 IDC_Elapsed, i 보다 먼저 찍어서 나중에 찍힌 마지막 번호로 덮혀 쓰인다.
	//UpdateData(0);
}
void CPostTestView::MonitorRequest(CStringA resp)
{
	CStringW txt(resp);
	_cListResponse .InsertString(0, txt);

	int n = _cListResponse.GetCount();
	for(int i = n-1; i >= _MaxItems;i--)//맨끝에서 역으로 max 까지
		_cListResponse.DeleteString(i);
}


void CPostTestView::OnLbnSelchangeListresponse()
{
	int isel = _cListResponse.GetCurSel();
	if(isel >= 0)
	{
		CString resp;
		_cListResponse.GetText(isel, resp);
		CString resp1;
		for(int i=0;i<resp.GetLength();i++)
		{
			TCHAR ch = resp.GetAt(i);
			if(ch == '\n')
				resp1 += L"\r\n";//윈도Edit창은 \r\n일때만 줄바꾼다.
			else
				resp1 += ch;
		}
		GetDlgItem(IDC_Response)->SetWindowTextW(resp1);
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPostTestView::OnSize(UINT nType, int cx, int cy)
{
	CFormInvokable::OnSize(nType, cx, cy);

	int idc[] ={IDC_URL, IDC_Request, IDC_ListResponse, IDC_Response,};

	BOOL b = OnSizeDefault(nType, cx, cy, _countof(idc), idc);

}


#include "CDlgDragImages.h"


void CPostTestView::OnBnClickedUploadimages()
{
	CDlgDragImages dlg;
	if(dlg.DoModal() == IDOK)
	{
		for(int i = 0;i<dlg._arFName.GetCount();i++)
			SendOneSyncImage(i, dlg._arFName[i], dlg._arCntType[i]);
	}

}


void CPostTestView::OnBnClickedStop()
{
	_bStop = true;
}
