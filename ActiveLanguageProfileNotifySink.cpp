//
//
// Derived from Microsoft Sample IME by Jeremy '13,7,17
//
//
//#define DEBUG_PRINT

#include "Private.h"
#include "Globals.h"
#include "DIME.h"
#include "CompositionProcessorEngine.h"
#include "UIPresenter.h"


BOOL CDIME::VerifyDIMECLSID(_In_ REFCLSID clsid)
{
	if (IsEqualCLSID(clsid, Global::DIMECLSID))
	{
		return TRUE;
	}
	return FALSE;
}

//+---------------------------------------------------------------------------
//
// ITfActiveLanguageProfileNotifySink::OnActivated
//
// Sink called by the framework when changes activate language profile.
//----------------------------------------------------------------------------

STDAPI CDIME::OnActivated(_In_ REFCLSID clsid, _In_ REFGUID guidProfile, _In_ BOOL isActivated)
{
	guidProfile;
	debugPrint(L"CDIME::OnActivated() isActivated = %d", isActivated);


	if (FALSE == VerifyDIMECLSID(clsid))
	{
		debugPrint(L"not our CLSID return now");
		return S_OK;
	}

	if (isActivated)
	{
		if (_pCompositionProcessorEngine == nullptr) return S_OK;
		Global::imeMode = _pCompositionProcessorEngine->GetImeModeFromGuidProfile(guidProfile);
		CConfig::SetIMEMode(Global::imeMode);
		_LoadConfig(TRUE);//force reversion coversion settings to be reload

		debugPrint(L"activating with imeMode = %d", Global::imeMode);
		_pCompositionProcessorEngine->SetImeMode(guidProfile);

		if (!_AddTextProcessorEngine())  return S_OK;

		ShowAllLanguageBarIcons();

		_lastKeyboardMode = CConfig::GetActivatedKeyboardMode();
		_isChinese = _lastKeyboardMode;
		debugPrint(L"CDIME::OnActivated() Set keyboard mode to last state = %d", _lastKeyboardMode);

		_newlyActivated = TRUE;


		// SetFocus to focused document manager for probing the composition range
		ITfDocumentMgr* pDocuMgr;
		if (SUCCEEDED(_GetThreadMgr()->GetFocus(&pDocuMgr)) && pDocuMgr)
		{
			OnSetFocus(pDocuMgr, NULL);
		}

	}
	else
	{
		debugPrint(L"_isChinese = %d", _isChinese);
		_lastKeyboardMode = _isChinese;
		_DeleteCandidateList(TRUE, nullptr);

		HideAllLanguageBarIcons();
	}

	return S_OK;
}

//+---------------------------------------------------------------------------
//
// _InitActiveLanguageProfileNotifySink
//
// Advise a active language profile notify sink.
//----------------------------------------------------------------------------

BOOL CDIME::_InitActiveLanguageProfileNotifySink()
{
	ITfSource* pSource = nullptr;
	BOOL ret = FALSE;

	if (_pThreadMgr && _pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) != S_OK)
	{
		return ret;
	}

	if (pSource && pSource->AdviseSink(IID_ITfActiveLanguageProfileNotifySink, (ITfActiveLanguageProfileNotifySink *)this, &_activeLanguageProfileNotifySinkCookie) != S_OK)
	{
		_activeLanguageProfileNotifySinkCookie = TF_INVALID_COOKIE;
		goto Exit;
	}

	ret = TRUE;

Exit:
	if (pSource)
		pSource->Release();
	return ret;
}

//+---------------------------------------------------------------------------
//
// _UninitActiveLanguageProfileNotifySink
//
// Unadvise a active language profile notify sink.  Assumes we have advised one already.
//----------------------------------------------------------------------------

void CDIME::_UninitActiveLanguageProfileNotifySink()
{
	ITfSource* pSource = nullptr;

	if (_activeLanguageProfileNotifySinkCookie == TF_INVALID_COOKIE)
	{
		return; // never Advised
	}

	if (_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK && pSource)
	{
		pSource->UnadviseSink(_activeLanguageProfileNotifySinkCookie);
		pSource->Release();
	}

	_activeLanguageProfileNotifySinkCookie = TF_INVALID_COOKIE;
}
