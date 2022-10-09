/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "lkTools.h"

bool CalculateEstimateRect(wxRect* _rc, const wxImage& image, lkResizeOptions drawMode)
{
	if (!_rc || _rc->IsEmpty() || !image.IsOk()) return false;

	wxRect rect(*_rc);
	int nDestX, nDestY, nDestWidth, nDestHeight;
	bool isOk = FALSE;

	switch (drawMode)
	{
		case lkResizeOptions::FIT_CENTERED:
		{
			if ((image.GetWidth() <= rect.GetWidth()) && (image.GetHeight() <= rect.GetHeight()))
			{ // If the image fits, just center it
				isOk = true;
//				nDestX = rect.GetLeft() + (rect.GetWidth() - image.GetWidth()) / 2 + ((rect.GetWidth() - image.GetWidth()) % 2) ? 1 : 0;
				nDestX = rect.GetLeft() + (rect.GetWidth() - image.GetWidth()) / 2;
//				nDestY = rect.GetTop() + (rect.GetHeight() - image.GetHeight()) / 2 + ((rect.GetHeight() - image.GetHeight()) % 2) ? 1 : 0;
				nDestY = rect.GetTop() + (rect.GetHeight() - image.GetHeight()) / 2;
				nDestWidth = image.GetWidth();
				nDestHeight = image.GetHeight();
				break;
			}
			// else, fallthrough
		}
		case lkResizeOptions::FIT_SCALED:
		default:
		{ // scale to fit
			isOk = true;
			if (((float)rect.GetWidth() / (float)image.GetWidth()) <= ((float)rect.GetHeight() / (float)image.GetHeight()))
			{ // Width is constraint
				nDestWidth = rect.GetWidth();
				nDestHeight = (nDestWidth * image.GetHeight()) / image.GetWidth();
				nDestX = rect.GetLeft();
//				nDestY = rect.GetTop() + (rect.GetHeight() - nDestHeight) / 2 + ((rect.GetHeight() - nDestHeight) % 2) ? 1 : 0;
				nDestY = rect.GetTop() + (rect.GetHeight() - nDestHeight) / 2;
			}
			else
			{ // Height is constraint		
				nDestHeight = rect.GetHeight();
				nDestWidth = (nDestHeight * image.GetWidth()) / image.GetHeight();
//				nDestX = rect.GetLeft() + (rect.GetWidth() - nDestWidth) / 2 + ((rect.GetWidth() - nDestWidth) % 2) ? 1 : 0;
				nDestX = rect.GetLeft() + (rect.GetWidth() - nDestWidth) / 2;
				nDestY = rect.GetTop();
			}
			break;
		}
	}

	if (isOk)
	{
		wxRect rect2(wxPoint(nDestX, nDestY), wxSize(nDestWidth, nDestHeight));
		*_rc = rect2;
	}

	return isOk;
}

wxRect CalculateEstimateRect(const wxRect& _rc, const wxImage& image, lkResizeOptions drawMode)
{
	wxRect rc(_rc);
	if (CalculateEstimateRect(&rc, image, drawMode))
		return rc;
	// else
	return wxRect();
}

wxRect Expand(const wxSize& expSize, const wxSize& imgSize, lkExpandFlags expFlags)
{
	int eW = expSize.GetWidth(), iW = imgSize.GetWidth();
	int eH = expSize.GetHeight(), iH = imgSize.GetHeight();
	int L, T, W, H;

	switch ( expFlags )
	{
		case lkExpandFlags::EXPAND_Left:
		{
			L = 0;
			W = iW;
			T = (eH - iH) / 2;
			H = iH;

			T += ((eH - iH) % 2) ? 1 : 0;
			break;
		}
		case lkExpandFlags::EXPAND_Right:
		{
			L = eW - iW;
			W = eW;
			T = (eH - iH) / 2;
			H = iH;

			T += ((eH - iH) % 2) ? 1 : 0;
			break;
		}
		case lkExpandFlags::EXPAND_Center:
		default:
		{
			L = (eW - iW) / 2;
			W = iW;
			T = (eH - iH) / 2;
			H = iH;

			T += ((eH - iH) % 2) ? 1 : 0;
			break;
		}
	}

	wxRect rc = wxRect(L, T, W, H);
	return rc;
}

wxImage Expand(const wxImage& _img, const wxSize& expSize, lkExpandFlags expFlags)
{
	if (!_img.IsOk() || !expSize.IsFullySpecified())
		return wxImage(_img); // nothing done if image not ok

	wxRect rc = Expand(expSize, _img.GetSize(), expFlags);

	wxPoint expPoint = wxPoint(rc.GetLeft(), rc.GetTop());
//	wxSize exSize = wxSize(rc.GetWidth(), rc.GetHeight());

	return _img.Size(expSize, expPoint);
}
