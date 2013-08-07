#include <windowsx.h>
#include "Globals.h"
#include "Private.h"
#include "resource.h"
#include "TSFTTS.h"

static struct {
	int id;
	COLORREF color;
} colors[8] = {
	{IDC_COL_BG,  RGB(0xFF,0xFF,0xFF)},
	{IDC_COL_FR,  RGB(0x00,0x00,0x00)},
	{IDC_COL_SE,  RGB(0x00,0x00,0xFF)},
	{IDC_COL_CO,  RGB(0x80,0x80,0x80)},
	{IDC_COL_CA,  RGB(0x00,0x00,0x00)},
	{IDC_COL_SC,  RGB(0x80,0x80,0x80)},
	{IDC_COL_AN,  RGB(0x80,0x80,0x80)},
	{IDC_COL_NO,  RGB(0x00,0x00,0x00)}
};


INT_PTR CALLBACK CTSFTTS::CommonPropertyPageWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	HWND hwnd;
	size_t i;
	WCHAR num[16];
	WCHAR fontname[LF_FACESIZE];
	int fontpoint, fontweight, x, y;
	BOOL fontitalic;
	CHOOSEFONTW cf;
	LOGFONT lf;
	HDC hdc;
	HFONT hFont;
	RECT rect;
	POINT pt;
	LONG w;


	CHOOSECOLORW cc;
	static COLORREF colCust[16];
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_INITDIALOG:
		
		wcsncpy_s(fontname, L"Microsoft JhengHei" , _TRUNCATE);

	

		if(fontpoint < 8 || fontpoint > 72)
		{
			fontpoint = 12;
		}
		if(fontweight < 0 || fontweight > 1000)
		{
			fontweight = FW_NORMAL;
		}
		if(fontitalic != TRUE && fontitalic != FALSE)
		{
			fontitalic = FALSE;
		}

		SetDlgItemTextW(hDlg, IDC_EDIT_FONTNAME, fontname);
		hdc = GetDC(hDlg);
		hFont = CreateFontW(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0,
			fontweight, fontitalic, FALSE, FALSE, SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, fontname);
		SendMessage(GetDlgItem(hDlg, IDC_EDIT_FONTNAME), WM_SETFONT, (WPARAM)hFont, 0);
		ReleaseDC(hDlg, hdc);

		SetDlgItemInt(hDlg, IDC_EDIT_FONTPOINT, fontpoint, FALSE);

		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		if(w < 0 || w > rect.right)
		{
			w = rect.right;
		}
		_snwprintf_s(num, _TRUNCATE, L"%d", w);
		SetDlgItemTextW(hDlg, IDC_EDIT_MAXWIDTH, num);

		ZeroMemory(&colCust, sizeof(colCust));

		for(i=0; i<_countof(colors); i++)
		{
		
		}

		hwnd = GetDlgItem(hDlg, IDC_COMBO_UNTILCANDLIST);
		num[1] = L'\0';
		for(i=0; i<=8; i++)
		{
			num[0] = L'0' + (WCHAR)i;
			SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)num);
		}
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)i, 0);

		CheckDlgButton(hDlg, IDC_CHECKBOX_DISPCANDNO, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_ANNOTATION, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_RADIO_ANNOTATLST, BST_CHECKED);
		if(!IsDlgButtonChecked(hDlg, IDC_RADIO_ANNOTATLST))
		{
			CheckDlgButton(hDlg, IDC_RADIO_ANNOTATALL, BST_CHECKED);
		}
		CheckDlgButton(hDlg, IDC_CHECKBOX_NOMODEMARK,BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_NOOKURICONV, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_DELOKURICNCL, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_BACKINCENTER, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_ADDCANDKTKN, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_SHOWMODEIMM, BST_CHECKED);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_CHOOSEFONT:
			hdc = GetDC(hDlg);

			hFont = (HFONT)SendMessage(GetDlgItem(hDlg, IDC_EDIT_FONTNAME), WM_GETFONT, 0, 0);
			GetObject(hFont, sizeof(LOGFONT), &lf);
			lf.lfHeight = -MulDiv(GetDlgItemInt(hDlg, IDC_EDIT_FONTPOINT, NULL, FALSE), GetDeviceCaps(hdc, LOGPIXELSY), 72);
			lf.lfCharSet = CHINESEBIG5_CHARSET;

			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof(CHOOSEFONTW);
			cf.hwndOwner = hDlg;
			cf.lpLogFont = &lf;
			cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS | CF_SCREENFONTS | CF_SELECTSCRIPT;

			if(ChooseFontW(&cf) == TRUE)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				SetDlgItemText(hDlg, IDC_EDIT_FONTNAME, lf.lfFaceName);
				lf.lfHeight = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72);
				hFont = CreateFontIndirect(&lf);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_FONTNAME), WM_SETFONT, (WPARAM)hFont, 0);
				SetDlgItemInt(hDlg, IDC_EDIT_FONTPOINT, cf.iPointSize / 10, FALSE);
			}

			ReleaseDC(hDlg, hdc);
			return TRUE;

		case IDC_EDIT_MAXWIDTH:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
				PropSheet_Changed(GetParent(hDlg), hDlg);
				return TRUE;
			default:
				break;
			}
			break;

		case IDC_COMBO_UNTILCANDLIST:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				PropSheet_Changed(GetParent(hDlg), hDlg);
				return TRUE;
			default:
				break;
			}
			break;

		case IDC_CHECKBOX_DISPCANDNO:
		case IDC_CHECKBOX_ANNOTATION:
		case IDC_RADIO_ANNOTATALL:
		case IDC_RADIO_ANNOTATLST:
		case IDC_CHECKBOX_NOMODEMARK:
		case IDC_CHECKBOX_NOOKURICONV:
		case IDC_CHECKBOX_DELOKURICNCL:
		case IDC_CHECKBOX_BACKINCENTER:
		case IDC_CHECKBOX_ADDCANDKTKN:
		case IDC_CHECKBOX_SHOWMODEIMM:
			PropSheet_Changed(GetParent(hDlg), hDlg);
			return TRUE;

		default:
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		for(i=0; i<_countof(colors); i++)
		{
			hwnd = GetDlgItem(hDlg, colors[i].id);
			GetWindowRect(hwnd, &rect);
			pt.x = x = GET_X_LPARAM(lParam);
			pt.y = y = GET_Y_LPARAM(lParam);
			ClientToScreen(hDlg, &pt);

			if(rect.left <= pt.x && pt.x <= rect.right &&
				rect.top <= pt.y && pt.y <= rect.bottom)
			{
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hDlg;
				cc.hInstance = NULL;
				cc.rgbResult = colors[i].color;
				cc.lpCustColors = colCust;
				cc.Flags = CC_FULLOPEN | CC_RGBINIT;
				cc.lCustData = NULL;
				cc.lpfnHook = NULL;
				cc.lpTemplateName = NULL;
				if(ChooseColorW(&cc))
				{
					hdc = GetDC(hDlg);
					DrawColor(hwnd, hdc, cc.rgbResult);
					ReleaseDC(hDlg, hdc);
					colors[i].color = cc.rgbResult;
					PropSheet_Changed(GetParent(hDlg), hDlg);
					return TRUE;
				}
				break;
			}
		}
		break;
		
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		for(i=0; i<_countof(colors); i++)
		{
			DrawColor(GetDlgItem(hDlg, colors[i].id), hdc, colors[i].color);
		}
		EndPaint(hDlg, &ps);
		return TRUE;
		break;

	case WM_NOTIFY:
		switch(((LPNMHDR)lParam)->code)
		{
		case PSN_APPLY:
			

			return TRUE;

		default:
			break;
		}
		break;

	default:
		break;
	}
	
	return FALSE;
	
	
}

void DrawColor(HWND hwnd, HDC hdc, COLORREF col)
{
	RECT rect;

	hdc = GetDC(hwnd);
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	SetDCBrushColor(hdc, col);
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	GetClientRect(hwnd, &rect);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	ReleaseDC(hwnd, hdc);
}