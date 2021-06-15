#include "pch.h"
#include "DockTool.h"


void DockTool::ShowHide(CMDIFrameWndEx* frm, CBasePane& win, BOOL bShow)
{
	win.ShowPane(bShow, TRUE, TRUE);
	frm->RecalcLayout(FALSE);
}

/// 이건 BOOL 값이 따로 없고, IsVisible 로 하고 저장은 윈도셋팅에 저장 되므로 BOOL 값으로 Setting 과는 상관 없다. 
void DockTool::ShowHide(CMDIFrameWndEx* frm, CBasePane& win, CCmdUI* pCmdUI)//?petme dock showhide
{
	if(win.GetSafeHwnd() == NULL)
		return;

	if(pCmdUI)
	{
		pCmdUI->SetCheck(win.IsVisible());
	}
	else
	{
		win.ShowPane(!win.IsVisible(), TRUE, TRUE);
		frm->RecalcLayout(FALSE);
	}
}