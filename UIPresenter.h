//
//
// Derived from Microsoft Sample IME by Jeremy '13,7,17
//
//
#ifndef TSFTTSUIPRESENTER_H
#define TSFTTSUIPRESENTER_H


#pragma once

#include "KeyHandlerEditSession.h"
#include "CandidateWindow.h"
#include "NotifyWindow.h"
#include "TfTextLayoutSink.h"
#include "TSFTTS.h"
#include "BaseStructure.h"

class CReadingLine;
class CCompositionProcessorEngine;

//+---------------------------------------------------------------------------
//
// UIPresenter
//
// ITfCandidateListUIElement / ITfIntegratableCandidateListUIElement is used for 
// UILess mode support
// ITfCandidateListUIElementBehavior sends the Selection behavior message to 
// 3rd party IME.
//----------------------------------------------------------------------------

class UIPresenter : 
	public CTfTextLayoutSink,
    public ITfCandidateListUIElementBehavior,
    public ITfIntegratableCandidateListUIElement
{
public:
    UIPresenter(_In_ CTSFTTS *pTextService, CCompositionProcessorEngine *pCompositionProcessorEngine);
    virtual ~UIPresenter();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, _Outptr_ void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // ITfUIElement
    STDMETHODIMP GetDescription(BSTR *pbstr);
    STDMETHODIMP GetGUID(GUID *pguid);
    STDMETHODIMP Show(BOOL showCandidateWindow);
    STDMETHODIMP IsShown(BOOL *pIsShow);

    // ITfCandidateListUIElement (UI-less)
    STDMETHODIMP GetUpdatedFlags(DWORD *pdwFlags);
    STDMETHODIMP GetDocumentMgr(ITfDocumentMgr **ppdim);
    STDMETHODIMP GetCount(UINT *pCandidateCount);
    STDMETHODIMP GetSelection(UINT *pSelectedCandidateIndex);
    STDMETHODIMP GetString(UINT uIndex, BSTR *pbstr);
    STDMETHODIMP GetPageIndex(UINT *pIndex, UINT uSize, UINT *puPageCnt);
    STDMETHODIMP SetPageIndex(UINT *pIndex, UINT uPageCnt);
    STDMETHODIMP GetCurrentPage(UINT *puPage);

    // ITfCandidateListUIElementBehavior methods 
	STDMETHODIMP SetSelection(UINT nIndex); 
    STDMETHODIMP Finalize(void);
    STDMETHODIMP Abort(void);

    // ITfIntegratableCandidateListUIElement (UI-less)
    STDMETHODIMP SetIntegrationStyle(GUID guidIntegrationStyle);
    STDMETHODIMP GetSelectionStyle(_Out_ TfIntegratableCandidateListSelectionStyle *ptfSelectionStyle);
    STDMETHODIMP OnKeyDown(_In_ WPARAM wParam, _In_ LPARAM lParam, _Out_ BOOL *pIsEaten);
    STDMETHODIMP ShowCandidateNumbers(_Out_ BOOL *pIsShow); 
    STDMETHODIMP FinalizeExactCompositionString();

    virtual HRESULT _StartCandidateList(TfClientId tfClientId, _In_ ITfDocumentMgr *pDocumentMgr, _In_ ITfContext *pContextDocument, TfEditCookie ec, _In_ ITfRange *pRangeComposition, UINT wndWidth);
    void _EndCandidateList();

    void _SetCandidateText(_In_ CTSFTTSArray<CCandidateListItem> *pCandidateList, BOOL isAddFindKeyCode);
    void _ClearCandidateList();
    VOID _SetCandidateTextColor(COLORREF crColor, COLORREF crBkColor);
    VOID _SetCandidateFillColor(HBRUSH hBrush);

    DWORD_PTR _GetSelectedCandidateString(_Outptr_result_maybenull_ const WCHAR **ppwchCandidateString);
    BOOL _SetCandidateSelectionInPage(int nPos) { return _pCandidateWnd->_SetSelectionInPage(nPos); }

    BOOL _MoveCandidateSelection(_In_ int offSet);
	BOOL _SetCandidateSelection(_In_ int selectedIndex, _In_opt_ BOOL isNotify = TRUE);
    BOOL _MoveCandidatePage(_In_ int offSet);

    void _MoveCandidateWindowToTextExt();

    // CTfTextLayoutSink
    virtual VOID _LayoutChangeNotification(_In_ RECT *lpRect);
    virtual VOID _LayoutDestroyNotification();

    // Event for ITfThreadFocusSink
    virtual HRESULT OnSetThreadFocus();
    virtual HRESULT OnKillThreadFocus();

    void RemoveSpecificCandidateFromList(_In_ LCID Locale, _Inout_ CTSFTTSArray<CCandidateListItem> &candidateList, _In_ CStringRange &srgCandidateString);
    void AdviseUIChangedByArrowKey(_In_ KEYSTROKE_FUNCTION arrowKey);

	void GetCandLocation(_Out_ POINT *lpPoint);

	HRESULT MakeNotifyWindow(_In_ ITfContext *pContextDocument);
	void SetNotifyText(_In_ CStringRange *pNotifyText);
	void ShowNotify(_In_ BOOL showMode, _In_opt_ int timeToHide = -1);
	void ClearNotify();
	void ShowNotifyText(_In_ ITfContext *pContextDocument, _In_ CStringRange *pNotifyText);


private:
    virtual HRESULT CALLBACK _CandidateChangeNotification(_In_ enum CANDWND_ACTION action);
	virtual HRESULT CALLBACK _NotifyChangeNotification();

    static HRESULT _CandWndCallback(_In_ void *pv, _In_ enum CANDWND_ACTION action);
	static HRESULT _NotifyWndCallback(_In_ void *pv);

    friend COLORREF _AdjustTextColor(_In_ COLORREF crColor, _In_ COLORREF crBkColor);

    HRESULT _UpdateUIElement();
	

    HRESULT ToShowCandidateWindow();

    HRESULT ToHideCandidateWindow();

    HRESULT BeginUIElement();
    HRESULT EndUIElement();

    HRESULT MakeCandidateWindow(_In_ ITfContext *pContextDocument, _In_ UINT wndWidth);
	
    void DisposeCandidateWindow();
	void DisposeNotifyWindow();

    void AddCandidateToTSFTTSUI(_In_ CTSFTTSArray<CCandidateListItem> *pCandidateList, BOOL isAddFindKeyCode);

    void SetPageIndexWithScrollInfo(_In_ CTSFTTSArray<CCandidateListItem> *pCandidateList);

protected:
    CCandidateWindow *_pCandidateWnd;
	CNotifyWindow *_pNotifyWnd;
    BOOL _isShowMode;


private:
	CCompositionProcessorEngine *_pCompositionProcessorEngine;  // to retrieve user settings
    HWND _parentWndHandle;
    CCandidateRange* _pIndexRange;
    KEYSTROKE_CATEGORY _Category;
    DWORD _updatedFlags;
    DWORD _uiElementId;
    CTSFTTS* _pTextService;
    LONG _refCount;
	POINT _candLocation;
};


//////////////////////////////////////////////////////////////////////
//
// CTSFTTS candidate key handler methods
//
//////////////////////////////////////////////////////////////////////

const int MOVEUP_ONE = -1;
const int MOVEDOWN_ONE = 1;
const int MOVETO_TOP = 0;
const int MOVETO_BOTTOM = -1;
#endif