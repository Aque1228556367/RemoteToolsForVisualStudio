//Copyright (c) Microsoft Corporation.  All rights reserved.

// Connect.h : Declaration of the CConnect

#pragma once
#include "resource.h"       // main symbols

using namespace AddInDesignerObjects;
using namespace EnvDTE;
using namespace EnvDTE80;

/// <summary>The object for implementing an Add-in.</summary>
/// <seealso class='IDTExtensibility2' />
class ATL_NO_VTABLE CConnect : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CConnect, &CLSID_Connect>,
	
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects, 1, 0>
{
public:
	/// <summary>Implements the constructor for the Add-in object. Place your initialization code within this method.</summary>
	CConnect()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)
DECLARE_NOT_AGGREGATABLE(CConnect)


BEGIN_COM_MAP(CConnect)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IDTExtensibility2)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:
//IDTExtensibility2 implementation:

	/// <summary>Implements the OnConnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being loaded.</summary>
	/// <param term='application'>Root object of the host application.</param>
	/// <param term='connectMode'>Describes how the Add-in is being loaded.</param>
	/// <param term='addInInst'>Object representing this Add-in.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnConnection)(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch *AddInInst, SAFEARRAY **custom);

	/// <summary>Implements the OnDisconnection method of the IDTExtensibility2 interface. Receives notification that the Add-in is being unloaded.</summary>
	/// <param term='disconnectMode'>Describes how the Add-in is being unloaded.</param>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY **custom );

	/// <summary>Implements the OnAddInsUpdate method of the IDTExtensibility2 interface. Receives notification when the collection of Add-ins has changed.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />	
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY **custom );

	/// <summary>Implements the OnStartupComplete method of the IDTExtensibility2 interface. Receives notification that the host application has completed loading.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnStartupComplete)(SAFEARRAY **custom );

	/// <summary>Implements the OnBeginShutdown method of the IDTExtensibility2 interface. Receives notification that the host application is being unloaded.</summary>
	/// <param term='custom'>Array of parameters that are host application specific.</param>
	/// <seealso class='IDTExtensibility2' />
	STDMETHOD(OnBeginShutdown)(SAFEARRAY **custom );
	


private:
	CComPtr<DTE2> m_pDTE;
	CComPtr<AddIn> m_pAddInInstance;
};

OBJECT_ENTRY_AUTO(__uuidof(Connect), CConnect)
