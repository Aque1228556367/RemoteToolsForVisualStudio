//Copyright (c) Microsoft Corporation.  All rights reserved.

// Connect.cpp : Implementation of CConnect
#include "stdafx.h"
#include "AddIn.h"
#include "Connect.h"

extern CAddInModule _AtlModule;

// CConnect
STDMETHODIMP CConnect::OnConnection(IDispatch *pApplication, ext_ConnectMode ConnectMode, IDispatch *pAddInInst, SAFEARRAY ** /*custom*/ )
{
	CComPtr<EnvDTE::Windows> pWindows;
	CComPtr<IDispatch> pProgrammableObject;
	CComPtr<EnvDTE::Window> pWindow;
	HRESULT hr = S_OK;

	pApplication->QueryInterface(__uuidof(DTE2), (LPVOID*)&m_pDTE);
	pAddInInst->QueryInterface(__uuidof(AddIn), (LPVOID*)&m_pAddInInstance);

	m_pDTE->get_Windows(&pWindows);
	pWindows->CreateToolWindow(m_pAddInInstance, CComBSTR("CPPToolWindowControl.ToolWindowControl"), CComBSTR("C++ Tool Window"), CComBSTR("{B8DCB086-5D26-4f18-821D-FA1BA25512B5}"), &pProgrammableObject, &pWindow);

	HBITMAP hToolWindowBitmap = LoadBitmap(_AtlModule.GetResourceInstance(), (LPCSTR)IDB_TOOLWINDOWBITMAP);

	CComVariant varBitmap;
	varBitmap.vt = VT_INT;
	varBitmap.intVal = (INT)hToolWindowBitmap;
	pWindow->SetTabPicture(varBitmap);

	pWindow->put_Visible(VARIANT_TRUE);
	
	return hr;
}

STDMETHODIMP CConnect::OnDisconnection(ext_DisconnectMode /*RemoveMode*/, SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnAddInsUpdate (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnStartupComplete (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnBeginShutdown (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

