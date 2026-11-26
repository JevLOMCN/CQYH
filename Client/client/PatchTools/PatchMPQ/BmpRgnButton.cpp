/*====================================================================
Created Date  :		2005-7-3

Specification :  
====================================================================*/

#include "stdafx.h"
#include "BmpRgnButton.h"
#include "Resource.h"
#include "controls.h"

#include <MMSystem.h>
#pragma  comment(lib, "Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void BmpRgnButton::AdjustSize()
{
	SAFE;
	_uBmpItemWidth = 0;
	_uBmpItemHeight = 0;

	if(!_pBmp)
	{
		return;
	}

	BITMAP bit = {0};
	if (!_pBmp->GetBitmap(&bit) || bit.bmWidth < 4 || bit.bmHeight <= 0)
		return;
	_uBmpItemWidth	= bit.bmWidth / 4;
	_uBmpItemHeight	= bit.bmHeight;

	SetWindowPos(NULL, -1, -1, _uBmpItemWidth, _uBmpItemHeight,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

	UNSAFE;
}


void BmpRgnButton::Set4In1Bitmap(UINT uID, COLORREF refTrans/* = RGB(255, 0, 255)*/)
{
	SAFE;

	//Util_Assert(_pBmp == 0);
	if(_pBmp)
	{
		_pBmp->DeleteObject();
		delete _pBmp;
	}

	_pBmp = new CBitmap;
	_pBmp->LoadBitmap(uID);

	_refTrans	= refTrans;

	AdjustSize();

	UNSAFE;
}

void BmpRgnButton::Set4In1Bitmap(CBitmap* p, COLORREF refTrans/* = RGB(255, 0, 255)*/)
{
	SAFE;

	_pBmp		= p;
	_refTrans	= refTrans;

	AdjustSize();

	UNSAFE;
}

/////////////////////////////////////////////////////////////////////////////
// BmpRgnButton

BmpRgnButton::BmpRgnButton()
{
	SAFE;

	_bMouseHover		= FALSE;
	_pBmp				= 0;
	_uBmpItemWidth		= 0;
	_uBmpItemHeight		= 0;
	_refTrans			= RGB(255, 0, 255);
	_refDrawTrans		= RGB(255, 0, 255);
	_bIsVerticalCaption = false;
	_bRgnCreated		= FALSE;
	_bDrawTrans			= FALSE;

	_hCursor			= LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_HAND_DLL));

	UNSAFE;
}

BmpRgnButton::~BmpRgnButton()
{
	if(_pBmp)
	{
		_pBmp->DeleteObject();
		//delete _pBmp;
	}
}


BEGIN_MESSAGE_MAP(BmpRgnButton, CButton)
	//{{AFX_MSG_MAP(BmpRgnButton)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BmpRgnButton message handlers
BOOL BmpRgnButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


BOOL BmpRgnButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	SAFE;

	//HINSTANCE	hInstResource	= AfxGetResourceHandle();
	//AfxSetResourceHandle(::GetModuleHandle(_T("ControlsD.dll")));

	//HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_HAND);
	//SetCursor( hCursor );

	//AfxSetResourceHandle(hInstResource);

	if(_hCursor)
	{
		::SetCursor(_hCursor);
		return TRUE;
	}

	return CButton::OnSetCursor(pWnd, nHitTest, message);

	UNSAFE;
}

LRESULT BmpRgnButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	SAFE;
	
	if(_bMouseHover)
	{
		_bMouseHover = FALSE;
		Invalidate();
	}

	return 0;

	UNSAFE;
}

void BmpRgnButton::OnMouseMove(UINT nFlags, CPoint point)
{
	SAFE;

	if(!_bMouseHover)
	{
		//::sndPlaySound(_strSoundFile.c_str(), SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
	}

	TRACKMOUSEEVENT		csTME = {0};

	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	GetWindowRgn(hRgn);
	BOOL bResult = PtInRegion(hRgn, point.x, point.y);
	DeleteObject(hRgn);

	if(bResult && !_bMouseHover)
	{
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE;
		csTME.hwndTrack = this->GetSafeHwnd();
		::_TrackMouseEvent(&csTME);

		_bMouseHover = TRUE;
		Invalidate();
	}

	CButton::OnMouseMove(nFlags, point);

	UNSAFE;
}

void BmpRgnButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	SAFE;

	if (!_pBmp || !_uBmpItemWidth || !_uBmpItemHeight)
		return;

	CRect rt;
	rt = lpDrawItemStruct->rcItem;		//Get button rect

	CDC dcDest;
	dcDest.Attach(lpDrawItemStruct->hDC);
	
	CFont fnt;
	fnt.CreatePointFont(90, _T("ËÎÌו"), &dcDest);
	CFont* pFntSrc = dcDest.SelectObject(&fnt);

	CDC memDC;
	memDC.CreateCompatibleDC(&dcDest);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dcDest, rt.Width(), rt.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&memBmp);

	CFont* pOldFnt = memDC.SelectObject(pFntSrc);

	CBrush br(RGB(255, 0, 255));
	CBrush* pOldBr = memDC.SelectObject(&br);

	CPen pen(PS_SOLID, 0, RGB(255, 0, 255));
	CPen* pOldPen = memDC.SelectObject(&pen);

	memDC.Rectangle(rt);

	CPoint ptStart(0, 0);
	CSize szWnd(_uBmpItemWidth, _uBmpItemHeight);

	UINT state = lpDrawItemStruct->itemState;	//Get state of the button
	BOOL bPress = state & ODS_SELECTED;
	BOOL bWatch = !bPress && _bMouseHover;

	if(!_bRgnCreated)
	{
		SetupRegion(&memDC, *_pBmp, _refTrans);
		_bRgnCreated = TRUE;
	}

	if(bPress)		// If it is pressed
	{
		CU_TransparentBlt(&memDC, *_pBmp, ptStart, szWnd, _bDrawTrans, _refDrawTrans, 4, 2);
		rt.OffsetRect(CSize(2, 2));
	}
	else if(bWatch)
	{
		CU_TransparentBlt(&memDC, *_pBmp, ptStart, szWnd, _bDrawTrans, _refDrawTrans, 4, 1);
	}
	else
	{
		CU_TransparentBlt(&memDC, *_pBmp, ptStart, szWnd, _bDrawTrans, _refDrawTrans, 4, 0);
	}

	rt.OffsetRect(CSize(2, 2));
//    if(!_strCaption.IsEmpty())
//	{
//		if(IsVerticalCaption())
//		{
//			rt.OffsetRect(CSize(6, 0));
//			CU_DrawVString((LPCTSTR)_strCaption, &memDC, rt.TopLeft());
//		}
//		else
//		{
//			int dcID = memDC.SaveDC();
//			memDC.SetBkMode(TRANSPARENT);
//			memDC.DrawText(_strCaption, rt, DT_SINGLELINE | DT_NOCLIP | DT_VCENTER | DT_CENTER);
//			memDC.RestoreDC(dcID);
//		}
//	}

	//dcDest.BitBlt(0, 0, szWnd.cx, szWnd.cy, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBr);
	memDC.SelectObject(pOldPen);
	memDC.SelectObject(pOldFnt);
	memDC.SelectObject(pOldBmp); // select out for save the draw result in the bitmap

	dcDest.SelectObject(pFntSrc);
	CU_TransparentBlt(&dcDest, memBmp, CPoint(0, 0), szWnd, TRUE, RGB(255, 0, 255));
	dcDest.Detach();

	memBmp.DeleteObject();
	fnt.DeleteObject();
	pen.DeleteObject();

	UNSAFE;
}

void BmpRgnButton::SetupRegion(CDC *pDC, CBitmap &cBitmap, COLORREF TransColor)
{
	SAFE;

	CU_SetupRegion(_RgnWnd, pDC, cBitmap, TransColor, 4, 3);

	SetWindowRgn(_RgnWnd,TRUE);

	UNSAFE;
}
