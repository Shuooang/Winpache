#include "pch.h"
#include "DockTool.h"


void DockTool::ShowHide(CMDIFrameWndEx* frm, CBasePane& win, BOOL bShow)
{
	win.ShowPane(bShow, TRUE, TRUE);
	frm->RecalcLayout(FALSE);
}

/// �̰� BOOL ���� ���� ����, IsVisible �� �ϰ� ������ �������ÿ� ���� �ǹǷ� BOOL ������ Setting ���� ��� ����. 
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