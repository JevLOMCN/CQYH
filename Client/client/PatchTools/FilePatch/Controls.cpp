/*====================================================================
	Created Date  :		2005-7-1
	
	Specification :  
====================================================================*/

#include "stdafx.h"
#include "controls.h"

LPTRANSPARENTBLT g_pTransparentBlt = 0;

void CU_SetupRegion(CRgn& rgn, CDC *pDC
				, CBitmap &cBitmap, COLORREF TransColor
				, UINT uTilesX, UINT uOffsetX
				, UINT uTilesY, UINT uOffsetY
				)
{
	SAFE;

	BITMAP bit = {0};
	if (!pDC || !uTilesX || !uTilesY || uOffsetX >= uTilesX || uOffsetY >= uTilesY ||
		!cBitmap.GetBitmap(&bit) || bit.bmWidth <= 0 || bit.bmHeight <= 0 ||
		uTilesX > (UINT)bit.bmWidth || uTilesY > (UINT)bit.bmHeight)
		return;
	CDC memDC;
	if (!memDC.CreateCompatibleDC(pDC))
		return;
	CBitmap* pOldMemBmp = memDC.SelectObject(&cBitmap);
	rgn.CreateRectRgn(0,0,0,0);
	UINT uWidth		= bit.bmWidth / uTilesX;
	UINT uHeight	= bit.bmHeight / uTilesY;

	int iOffsetX = uOffsetX * uWidth;
	int iOffsetY = uOffsetY * uHeight;
	for(UINT y = 0; y < uHeight; y++)
	{
		CRgn rgnTemp;

		int iX = 0;
		UINT iPos = iOffsetX;
		do
		{
			//跳过透明色找到下一个非透明色的点.
			while(iPos < iOffsetX + uWidth
				&& memDC.GetPixel(iPos, iOffsetY + y) == TransColor
				)
			{
				iX++;
				iPos++;
			}

			//记住这个起始点
			int iLeftX = iX;

			//寻找下个透明色的点
			while (iPos < iOffsetX + uWidth  && memDC.GetPixel(iPos, iOffsetY + y) != TransColor)
			{
				++iX;
				++iPos;
			}

			//创建一个包含起点与重点间高为1像素的临时“region”
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y+1);

			//合并到主"region".
			rgn.CombineRgn(&rgn, &rgnTemp, RGN_OR);

			//删除临时"region",否则下次创建时和出错
			rgnTemp.DeleteObject();

		}while(iPos < iOffsetX + uWidth);
	}

	memDC.SelectObject(pOldMemBmp);

	UNSAFE;
}

void CU_TransBlt( HDC hdcDest,      // 目标DC
					 int nXOriginDest,   // 目标X偏移
					 int nYOriginDest,   // 目标Y偏移
					 int nWidthDest,     // 目标宽度
					 int nHeightDest,    // 目标高度
					 HDC hdcSrc,         // 源DC
					 int nXOriginSrc,    // 源X起点
					 int nYOriginSrc,    // 源Y起点
					 int nWidthSrc,      // 源宽度
					 int nHeightSrc,     // 源高度
					 UINT crTransparent  // 透明色,COLORREF类型
					 )
{
	HBITMAP hOldImageBMP, hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest);	// 创建兼容位图
	HBITMAP hOldMaskBMP, hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);			// 创建单色掩码位图
	HDC		hImageDC = CreateCompatibleDC(hdcDest);
	HDC		hMaskDC = CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP)SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP)SelectObject(hMaskDC, hMaskBMP);
	
	// 将源DC中的位图拷贝到临时DC中
	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	else
		StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, 
		hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	
	// 设置透明色
	SetBkColor(hImageDC, crTransparent);

	// 生成透明区域为白色，其它区域为黑色的掩码位图
	BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
	
	// 生成透明区域为黑色，其它区域保持不变的位图
	SetBkColor(hImageDC, RGB(0,0,0));
	SetTextColor(hImageDC, RGB(255,255,255));
	BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);

	// 透明部分保持屏幕不变，其它部分变成黑色
	SetBkColor(hdcDest,RGB(0xff,0xff,0xff));
	SetTextColor(hdcDest,RGB(0,0,0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);
	
	// "或"运算,生成最终效果
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCPAINT);
	
	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}

// rect.left	StartX
// rect.top		StartY
// rect.right	Width
// rect.bottom	Heght
void CU_TransparentBlt(CDC *pDC, CBitmap &bmp
					   , CPoint ptStart
					   , CSize szExtend
					   , BOOL bTrans
					   , COLORREF TransColor
					   , UINT uTilesX
					   , UINT uOffsetX
					   , UINT uTilesY
					   , UINT uOffsetY
					   , DWORD bltMode)
{
	SAFE;

	BITMAP bit = {0};
	if (!pDC || !uTilesX || !uTilesY || uOffsetX >= uTilesX || uOffsetY >= uTilesY ||
		!bmp.GetBitmap(&bit) || bit.bmWidth <= 0 || bit.bmHeight <= 0 ||
		uTilesX > (UINT)bit.bmWidth || uTilesY > (UINT)bit.bmHeight || szExtend.cx < 0 || szExtend.cy < 0)
		return;
	CDC memDC;
	if (!memDC.CreateCompatibleDC(pDC))
		return;
	CBitmap* pOldMemBmp = memDC.SelectObject(&bmp);
	UINT uBmpItemWidth	= bit.bmWidth / uTilesX;
	UINT uBmpItemHeight	= bit.bmHeight / uTilesY;

	UINT iW, iH;
	if(szExtend.cx == 0)
		iW = uBmpItemWidth;
	else
		iW = szExtend.cx;
	
	if(szExtend.cy == 0)
		iH = uBmpItemHeight;
	else
		iH = szExtend.cy;

	if(bTrans)
	{
		if(g_pTransparentBlt)
		{
			g_pTransparentBlt(pDC->GetSafeHdc(), ptStart.x, ptStart.y, iW, iH, memDC.GetSafeHdc()
						, uOffsetX * uBmpItemWidth, uOffsetY * uBmpItemHeight
						, uBmpItemWidth, uBmpItemHeight
						, TransColor
						);
		}
		else
		{
			CU_TransBlt(pDC->GetSafeHdc(), ptStart.x, ptStart.y, iW, iH, memDC.GetSafeHdc()
						, uOffsetX * uBmpItemWidth, uOffsetY * uBmpItemHeight
						, uBmpItemWidth, uBmpItemHeight
						, TransColor
						);
		}
	}
	else
	{
		if(iW == uBmpItemWidth && iH == uBmpItemHeight)
		{
			pDC->BitBlt(ptStart.x, ptStart.y, iW, iH
				, &memDC
				, uOffsetX * uBmpItemWidth, uOffsetY * uBmpItemHeight
				, bltMode
				);
		}
		else
		{
			pDC->StretchBlt(ptStart.x, ptStart.y, iW, iH
				, &memDC
				, uOffsetX * uBmpItemWidth, uOffsetY * uBmpItemHeight
				, uBmpItemWidth, uBmpItemHeight
				, bltMode
				);
		}
	}

	memDC.SelectObject(pOldMemBmp);

	UNSAFE;
}


SIZE CU_GetBitmapSize(CBitmap& bmp)
{
	SAFE;

	SIZE sz;
	sz.cx = sz.cy = 0;
	if((HGDIOBJ)bmp)
	{
		BITMAP bmpInfo = {0};
		if (bmp.GetBitmap(&bmpInfo))
		{
			sz.cx = bmpInfo.bmWidth;
			sz.cy = bmpInfo.bmHeight;
		}
	}
	return sz;

	UNSAFE;
}

void CU_Blt(CDC* pdcDest,
			CBitmap& srcBmp,
			int destX,
			int destY,
			int destCX,
			int destCY,
			int srcX,
			int srcY,
			DWORD bltMode)
{
	SAFE;

	CDC dc;
	dc.CreateCompatibleDC(pdcDest);
	CBitmap* bmpOld = dc.SelectObject(&srcBmp);

	pdcDest->BitBlt(destX,destY, destCX, destCY
		, &dc
		, srcX
		, srcY
		, bltMode
		);

	dc.SelectObject(bmpOld);

	UNSAFE;
}


void CU_DrawIconCaption(CDC *pDC
						, UINT txtAlignFmt
						, CPoint ptStart, CSize szExtend
						, HICON hIcon, UINT uIconSize
						, LPCTSTR lpText
						, LPCTSTR lpFontFamily, UINT uFontHeight, COLORREF refFontColor, EFontStyle e
						, UINT uSpaceIconText
						, LPCTSTR lpText2
						, LPCTSTR lpFontFamily2, UINT uFontHeight2, COLORREF refFontColor2, EFontStyle e2
						, UINT uSpaceTextText2)
{
	SAFE;

	int iPosX = ptStart.x;

	if (hIcon != NULL)
	{
		::DrawIconEx(pDC->GetSafeHdc(),
			ptStart.x, ptStart.y + (szExtend.cy - uIconSize + 1) / 2,
			hIcon, uIconSize, uIconSize, NULL, (HBRUSH)NULL, DI_NORMAL);

		iPosX += uIconSize + uSpaceIconText + 35;
	}

	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight			= -MulDiv(uFontHeight, pDC->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfItalic			= (BYTE)(e & eFontStyleItalic);
	lf.lfUnderline		= (BYTE)(e & eFontStyleUnderline);
	lf.lfStrikeOut		= (BYTE)(e & eFontStyleStrikeOut);
	lf.lfWeight			= (e & eFontStyleBold) ? FW_BOLD : FW_DONTCARE;
	_tcscpy_s(lf.lfFaceName, lpFontFamily);
	VERIFY(font.CreateFontIndirect(&lf));

	CFont* pOldFont = pDC->SelectObject(&font);

	COLORREF refOldText = pDC->SetTextColor(refFontColor);
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	CRect rectText(iPosX, ptStart.y, ptStart.x + szExtend.cx, ptStart.y + szExtend.cy);
	pDC->DrawText(lpText, rectText, txtAlignFmt);

	CSize size = pDC->GetTextExtent( lpText, int(_tcslen(lpText)) );

	if( lpText2 != 0 )
	{
		iPosX += size.cx + uSpaceTextText2;

		if( lpFontFamily != lpFontFamily2
			|| uFontHeight != uFontHeight2
			|| e != e2
			)
		{
			CFont font2;
			LOGFONT lf2;
			memset(&lf2, 0, sizeof(LOGFONT));
			lf2.lfHeight		= -MulDiv(uFontHeight2, pDC->GetDeviceCaps(LOGPIXELSY), 72);
			lf2.lfItalic		= (BYTE)(e2 & eFontStyleItalic);
			lf2.lfUnderline		= (BYTE)(e2 & eFontStyleUnderline);
			lf2.lfStrikeOut		= (BYTE)(e2 & eFontStyleStrikeOut);
			lf2.lfWeight		= (e2 & eFontStyleBold) ? FW_BOLD : FW_DONTCARE;
			_tcscpy_s(lf2.lfFaceName, lpFontFamily2);
			VERIFY(font2.CreateFontIndirect(&lf2));

			pDC->SelectObject(&font2);
		}

		pDC->SetTextColor(refFontColor2);
		CRect rectText2(iPosX, ptStart.y, ptStart.x + szExtend.cx, ptStart.y + szExtend.cy);
		pDC->DrawText(lpText2, rectText2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(refOldText);
	pDC->SetBkMode(iOldBkMode);

	UNSAFE;
}