#pragma once
#include <map>
#include "KwLib64/MfcEx.h"

enum
{
	ID_PANE_OUTPUTWND = 149,
	ID_PANE_CAPTION_BAR = 221,


	ID_PANE_DockEx1 = 4444,
};

/// �ٸ� â�� �Լ� ȣ���� ��
/// iOp �� ��� ���� ����
enum ERefresh
{
	eShowDock1 = 1 << 0,
	eNoBlinkBmp2 = 1 << 1, // �������� ��¦ �ŷ��� ���� �ϴ°� ���δ�.
	eReqNoErrBox4 = 1 << 2, // ���� �ڽ��� �ȶ��.
	ePostAsync8 = 1<< 3, //PostMessage -> _KwBeginInvoke �� �޽��� ť�� �־ �񵿱�� ó��. �Ǵ� ��׶��忡�� ���׶���� �۾� ť
	eNoCheckMsgBox16 = 1 << 4, // 16 ���� Ȯ�� �ϴ� �޽����ڽ� �ȶ��.
	eNoWorkLog32 = 1 << 5, // �۾� �α׸� ���� �Ѵ�.
	eSyncByAction64 = 1 << 6, // ����� �׼����� 1�� ����ȭ ��û ���� ǥ��
	eRunInBG128 = 1 << 7,
	eNotInBG256 = 1 << 8, // ��׶��� �۾� �۾� ����
	eUpdateUI512 = 1 << 9,
};


class DockClientBase :
	public CDockFormChild
{
	DECLARE_DYNCREATE(DockClientBase)

public:
	DockClientBase(UINT nID = 0, CPaneForm* ppn = NULL);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~DockClientBase();

public:
	void StartBlink(int cnt = 10);

	
	///[ getListCtrl() != nullptr �ΰ�� ���̴� �͵�
	ShJObj _sjo;
	CString _filter;
	KMulMap<wstring, ShJObj> _sort;
	string _sortKey;
	int _sortDirection{ 1 }; /// or -1
	/// CListCtrl �� SetData�� �� ������ �� ���� �Ǵ� ��
	CStringArray _data;
	void MakeSort(ShJArr stable);
	virtual CListCtrl* getListCtrl() { return nullptr; }
	///]




	/// CMFCPropertyGridCtrl _cProperty �� �ִ� ��� ��� �ȴ�.
	KProperty* _prData{ nullptr };

	virtual void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) {}//ERefresh
	virtual void InsertDataToList(bool bHasData = true) {}
	virtual void ClearList(int iOp = 0);
	virtual _STitleWidthField* getListFieldInfo(int* pnCols = nullptr) { return nullptr; }
	virtual void InitPropList(ShJObj sjo = nullptr) {}
	void InsertDataToListImpl(CStringA dataKey, CStringA enumTable);
	void SetListSort(CListCtrl& cList, LPNMLISTVIEW pNMLV, _STitleWidthField* arlist, PAS table = "table");
	void SetListSort(CListCtrl& cList, LPNMLISTVIEW pNMLV, _STitleWidthField* arlist, ShJArr sha);
	void SetListItem(CListCtrl& cList, int itm, int subitm, _STitleWidthField* sar, ShJObj sjo);
	

	ShJObj FindByID(PWS fObjID, PAS keyObj, PAS table = "table");


	KStdMapPtr<wstring, KStdMap<wstring, wstring>> _mapEnum;
	void SetEnum(JArr& jar);

	void DoList(ShJObj sjout);

	BOOL DefaultSync(int iOp);
	CStringA GetPrGridFullKey(CMFCPropertyGridCtrl* _prpt);
	void OnDockFocus() override
	{
	}

// 	template<typename TFNC>
// 	BOOL PostThis(PAS eStep, TFNC lambda)
// 	{
// 		if(tchsame(tchbehind(eStep, '_').Pas(), "post"))
// 		{// �ڰ� _post �̸� �񵿱� �޽��� ť�� �־� ���� ȣ�� �Ѵ�.
// 			_KwBeginInvoke(this, [&, sjoin]()
// 				{
// 					NewOrder(tchprecede(eStep, '_').Pas(), sjoin, iOp);
// 				});
// 			return;
// 		}
// 		return TRUE;
// 	}

	/// TFIns �� 1,2,3.. �÷����� �ҷ� ����, 0��°�� �ȺҸ���.
	template<typename TFIns>
	void InsertAllItemT(CListCtrl& cList, int nCols, TFIns lambda, PWS hd = L"")
	{
		if(!_ldInsertItem)
			_ldInsertItem = make_shared<function<void(int, int, ShJObj)>>(lambda);
		InsertAllItem(cList, nCols, hd);
	}

	shared_ptr<function<void(int, int, ShJObj)>> _ldInsertItem;
	
	void InsertAllItem(CListCtrl& cList, int nCols, PWS hd);

	void SetListRow(CListCtrl& cList, int i, int nCols, ShJObj sjo, PWS hd);
	
protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	virtual BOOL Filter(ShJObj sjoRow);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


class SettingDock
{
public:
	SettingDock(CRuntimeClass* prc, LPCWSTR title, UINT nID, DWORD dwStyle, int idiHicon)
	{
		_pRC = prc;
		//_dock = dock;//CDockablePane* dock, 
		_title = title;
		_rc = CRect(CPoint(0,0), CSize(200,200));
		_nID = nID;
		_dwStyle |= dwStyle; //����: or�� ���� �Ѵ�. �ٸ���� ���� �ش�.
		_idiHicon = idiHicon;
	}
	CRuntimeClass* _pRC{ nullptr };
	CObject* _dock{ nullptr };//CDockablePane
	LPCWSTR _title{ nullptr };
	//CWnd* pParent, 
	CRect _rc{ 0,0,0,0 };
	BOOL _bHasGripper{ TRUE };
	UINT _nID{ 0 };
	DWORD _dwStyle{ WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI };
	DWORD _dwTabbedStyle{ AFX_CBRS_REGULAR_TABS };
	DWORD _dwControlBarStyle{ AFX_DEFAULT_DOCKING_PANE_STYLE };
	int _idiHicon{ -1 };
	//CBRS_LEFT | 
};

class DockEx1Pane;
class KDockPane
{
public:
	KDockPane();
	~KDockPane();
	void InitDocks(CFrameWnd* mframe);

	CMDIFrameWndEx* _mframe{ nullptr };
	//DockEx1Pane* _DockEx1{ nullptr };
	BOOL CreateDockingWin(SettingDock& sdck);
	BOOL CreateDockingForm(SettingDock& sdck);
	KStdMap<UINT, CObject*> m_docks;// KStdMapPtr �� auto free���� ����.

	//template<typename TDockPane>
	//void InitDockPane(TDockPane*& pane, UINT ID)
	//{
	//	pane = new TDockPane(ID);
	//}

	CDockablePane* GetDockPane(UINT nID);
	CDockFormChild* GetDockView(UINT nID);
};
