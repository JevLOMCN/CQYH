// TransparentCaption.cpp : implementation file
//

#include "stdafx.h"
#include "FixClient.h"
#include "TransparentCaption.h"
#include "MemDC.h"
#include "Controls.h"

// TransparentCaption

IMPLEMENT_DYNAMIC(TransparentCaption, CStatic)

TransparentCaption::TransparentCaption()
{
	_rcOnBgImg.SetRectEmpty();
	_textAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	_font.CreatePointFont(9, _T("ËÎÌו"));
	_txtColor = RGB(0, 0, 0);
	_pBgImg = 0;
}

TransparentCaption::~TransparentCaption()
{
}


BEGIN_MESSAGE_MAP(TransparentCaption, CStatic)
	ON_WM_PAINT()
	//ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// TransparentCaption message handlers



void TransparentCaption::OnPaint()
{
	CPaintDC dcTarget(this); // device context for painting
	//// TODO: Add your message handler code here
	//// Do not call CStatic::OnPaint() for painting messages
	//CFont* pFont = dc.SelectObject(&_font);
	//CString str;
	//this->GetWindowText(str);
	//dc.SetBkMode(TRANSPARENT);
	//CRect rc;
	//this->GetClientRect(&rc);
	//dc.SetTextColor(_txtColor);
	//
	//dc.DrawText(str, &rc, _textAlign);
	//dc.SelectObject(pFont);
	//MemDC dc(&dcTarget);
	
	CPaintDC& dc = dcTarget;

	CFont* pFont = dc.SelectObject(&_font);
	CString str;
	this->GetWindowText(str);
	dc.SetBkMode(TRANSPARENT);
	CRect rc;
	this->GetClientRect(&rc);
	dc.SetTextColor(_txtColor);	
	//CU_Blt(&dc, *_pBgImg, rc.left, rc.top, rc.Width(), rc.Height(), _rcOnBgImg.left, _rcOnBgImg.top);
	//CBrush br(RGB(255, 0, 255));
	//CBrush* pBr = dc.SelectObject(&br);
	
	//dc.Rectangle(&rc);

	dc.DrawText(str, &rc, _textAlign);
	dc.SelectObject(pFont);
	//dc.SelectObject(pBr);

	//br.DeleteObject();
}

BOOL TransparentCaption::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return TRUE;
	//CDC& dc = *pDC;
	CRect rc;
	this->GetClientRect(&rc);

	if(_pBgImg)
	{
		CU_Blt(pDC, *_pBgImg, rc.left, rc.top, rc.Width(), rc.Height(), _rcOnBgImg.left, _rcOnBgImg.top);
	}

	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

void TransparentCaption::OnDestroy()
{
	CStatic::OnDestroy();

	_font.DeleteObject();
}
