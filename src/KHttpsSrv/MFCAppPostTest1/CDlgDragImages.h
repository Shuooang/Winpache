#pragma once


// CDlgDragImages 대화 상자

class CDlgDragImages : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDragImages)

public:
	CDlgDragImages(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDragImages();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DragImages };
#endif
	CStringArray _arFName;
	CStringArray _arCntType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	virtual BOOL OnInitDialog();
	CListBox _list;
};
