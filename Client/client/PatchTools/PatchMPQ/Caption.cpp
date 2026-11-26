// *************************************************************************************
// 
// 
// 	All Rights Reserved 
// 
// 	Create Date		:		2006-10-11
// *************************************************************************************

#include "stdafx.h"
#include "Caption.h"
#include "Controls.h"
#include "MemDC.h"

#ifdef _DEBUG

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

Caption::Caption()
: _iIconSize(0)
, _pBgImg(0)
{
	_rcOnBgImg.SetRectEmpty();
	SAFE;

	m_hIcon				= NULL;
	SetCaptionFont(9, _T("ËÎÌו"), RGB(0,0,0), eFontStyleNormal);

	_textAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;


	UNSAFE;
}

Caption::~Caption()
{
}

BEGIN_MESSAGE_MAP(Caption, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Caption message handlers

void Caption::SetCaptionFont(int iHeight, LPCTSTR lpszFamilyName, COLORREF clr, EFontStyle e)
{
	_iFontHeight	= iHeight;
	_strFontFamily	= lpszFamilyName;
	m_clrFont		= clr;
	_eFontStyle		= e;
}

void Caption::SetCaptionText(LPCTSTR lpszString, HICON hIcon, int iIconSize)
{
	SAFE;
	
	if(_tcslen(lpszString) > 0)
		SetWindowText(lpszString);

	m_hIcon		= hIcon;
	_iIconSize	= iIconSize;
	Invalidate();

	UNSAFE;
}

void Caption::OnPaint()
{
	SAFE;
	
	CPaintDC dcTarget(this); // device context for painting

	//MemDC dc(&dcTarget);
	CRect rcClient;
	GetClientRect(&rcClient);

	CDC& dc = _memDC;
	dc.CreateCompatibleDC(&dcTarget);

	dc.SetBkMode(TRANSPARENT);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dcTarget, rcClient.Width(), rcClient.Height());
	
	CBitmap* pOldBmp = dc.SelectObject(&bmp);

	CString strText;
	GetWindowText(strText);

	CBrush br(RGB(255, 0, 0));

	dc.SelectObject(&br);
	dc.Rectangle(&rcClient);

	if(_pBgImg)
	{
		CU_Blt(&dc, *_pBgImg, 0, 0, rcClient.Width(), rcClient.Height(), _rcOnBgImg.left, _rcOnBgImg.top);
	}

	CU_DrawIconCaption(&dc, _textAlign, rcClient.TopLeft(), rcClient.Size(), m_hIcon, _iIconSize
					, strText, _strFontFamily, _iFontHeight, m_clrFont, _eFontStyle, 4);

	dcTarget.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pOldBmp);
	bmp.DeleteObject();
	dc.DeleteDC();

	UNSAFE;
}

void Caption::SetTextColor(COLORREF clr)
{
	m_clrFont = clr;
}
BOOL Caption::OnEraseBkgnd(CDC* pDC)
{
	//CRect rc;
	//GetClientRect(&rc);
	//CBrush br(RGB(255, 0 ,0));
	//pDC->SelectObject(&br);
	//pDC->Rectangle(&rc);
	//
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}
