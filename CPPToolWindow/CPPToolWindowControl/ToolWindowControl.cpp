//Copyright (c) Microsoft Corporation.  All rights reserved.

// ToolWindowControl.cpp : Implementation of CToolWindowControl
#include "stdafx.h"
#include "ToolWindowControl.h"


// CToolWindowControl

LRESULT CToolWindowControl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	GetWindowRect(&rc);
	::MoveWindow(GetDlgItem(IDC_BUTTON1), 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	return 0;
}

LRESULT CToolWindowControl::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MessageBox(_T("Hello World!"), _T("C++ Tool Window"), MB_OK);
	return 0;
}
