

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Mon Dec 29 00:49:20 2014
 */
/* Compiler settings for CPPToolWindowControl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CPPToolWindowControl_h__
#define __CPPToolWindowControl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IToolWindowControl_FWD_DEFINED__
#define __IToolWindowControl_FWD_DEFINED__
typedef interface IToolWindowControl IToolWindowControl;

#endif 	/* __IToolWindowControl_FWD_DEFINED__ */


#ifndef __ToolWindowControl_FWD_DEFINED__
#define __ToolWindowControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ToolWindowControl ToolWindowControl;
#else
typedef struct ToolWindowControl ToolWindowControl;
#endif /* __cplusplus */

#endif 	/* __ToolWindowControl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IToolWindowControl_INTERFACE_DEFINED__
#define __IToolWindowControl_INTERFACE_DEFINED__

/* interface IToolWindowControl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IToolWindowControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("66FC297E-0CB3-4C2F-A01C-381DDC4E96D2")
    IToolWindowControl : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IToolWindowControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IToolWindowControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IToolWindowControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IToolWindowControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IToolWindowControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IToolWindowControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IToolWindowControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IToolWindowControl * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IToolWindowControlVtbl;

    interface IToolWindowControl
    {
        CONST_VTBL struct IToolWindowControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IToolWindowControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IToolWindowControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IToolWindowControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IToolWindowControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IToolWindowControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IToolWindowControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IToolWindowControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IToolWindowControl_INTERFACE_DEFINED__ */



#ifndef __CPPToolWindowControlLib_LIBRARY_DEFINED__
#define __CPPToolWindowControlLib_LIBRARY_DEFINED__

/* library CPPToolWindowControlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CPPToolWindowControlLib;

EXTERN_C const CLSID CLSID_ToolWindowControl;

#ifdef __cplusplus

class DECLSPEC_UUID("86BA4EFB-0640-4E6F-AE47-E00EB0F69B7B")
ToolWindowControl;
#endif
#endif /* __CPPToolWindowControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


