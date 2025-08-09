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
#include "lkImageList.h"

#include <wx/bitmap.h>
#include <wx/filefn.h> 
#include "wx/settings.h"

wxIMPLEMENT_DYNAMIC_CLASS(lkImageList, wxImageList)

lkImageList::lkImageList() : wxImageList(), m_ForceSize()
{
	m_ExpandAlign = lkExpandFlags::EXPAND_Center;
	m_bFitScaled = true;
	m_bUseMask = true;
}
lkImageList::lkImageList(int width, int height, lkExpandFlags ExpandAlign, bool bFitScaled) : wxImageList(), m_ForceSize(width, height)
{
	m_ExpandAlign = ExpandAlign;
	m_bFitScaled = bFitScaled;
	m_bUseMask = true;

	(void)lkImageList::Create(width, height, m_bUseMask, 1);
}
lkImageList::~lkImageList()
{
}

bool lkImageList::Create(int width, int height, bool mask, int initialCount)
{
	m_bUseMask = mask;
	m_ForceSize = wxSize(width, height);

	return wxImageList::Create(width, height, mask, initialCount);
}

wxBitmap lkImageList::Prepare(const wxImage& _img)
{ // prepares given image for addition/replacement
	wxImage img(_img); // required, we have no direct access to a const object
	if ( !img.IsOk() )
		return wxBitmap(); // needs to be a valid image

	wxImage tmpImg;
	// check that the size of given image does mach
	wxSize size = img.GetSize();
	if ( (size.GetWidth() != m_ForceSize.GetWidth()) || (size.GetHeight() != m_ForceSize.GetHeight()) )
	{
		wxRect rect(wxPoint(0, 0), m_ForceSize);

		if ( (!m_bFitScaled && (m_ForceSize.GetWidth() == m_ForceSize.GetHeight())) ||
			(!CalculateEstimateRect(&rect, img, lkResizeOptions::FIT_SCALED)) )
		{
			wxImage iScaled(img.Scale(m_ForceSize.GetWidth(), m_ForceSize.GetHeight()));
			tmpImg = iScaled;
		}
		else
		{ // expanding, so we display it correctly - i hope
			wxImage iScaled(img.Scale(rect.GetWidth(), rect.GetHeight()));
			wxImage iExpanded(Expand(iScaled, m_ForceSize, m_ExpandAlign));
			tmpImg = iExpanded;
		}
	}
	else
		tmpImg = img; // just copy
// -----------
	wxBitmap bmp = wxBitmap(tmpImg);
	if ( m_bUseMask )
	{
		if ( tmpImg.HasMask() )
		{
			if ( tmpImg.HasAlpha() )
			{
				// We need to remove alpha channel for compatibility with
				// native-based wxMSW wxImageList where stored images are not allowed
				// to have both mask and alpha channel.
				tmpImg.ClearAlpha();
				bmp = wxBitmap(tmpImg);
			}
		}
		else
		{
			if ( tmpImg.HasAlpha() )
			{
				// Convert alpha channel to mask.
				tmpImg.ConvertAlphaToMask();
				bmp = wxBitmap(tmpImg);
			}
			else
			{
				// Like for wxMSW, use the light grey from standard colour map as transparent colour.
				wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
				bmp = wxBitmap(tmpImg);
				bmp.SetMask(new wxMask(bmp, col));
			}
		}
	}
	else
	{
		if ( tmpImg.HasMask() )
		{
			if ( !tmpImg.HasAlpha() )
			{
				// Convert a mask to alpha values.
				tmpImg.InitAlpha();
			}
			bmp = wxBitmap(tmpImg);
			// TODO: It would be better to blend a mask with existing alpha values.
			bmp.SetMask(NULL);
		}
	}

	return bmp;
}

int lkImageList::Add(const wxString& path)
{ // Adds an image based on wxImage from a File
	if ( !path.IsEmpty() && wxFileExists(path) )
		return Add(wxImage(path));
	//else..
	return wxNOT_FOUND;
}

int lkImageList::Add(const wxImage& image)
{ // Adds given lkImage as new image to the list, will get the transparancy colour dynamicly (if any), resizes image if required
	if ( image.IsOk() )
	{
		wxBitmap bmp = Prepare(image);
		if ( bmp.IsOk() )
			return wxImageList::Add(bmp);
		//else.. fallthrough
	}

	return wxNOT_FOUND;
}

bool lkImageList::Replace(int index, const wxString& path)
{
	if ( !path.IsEmpty() && wxFileExists(path) )
		return Replace(index, wxImage(path));
	//else..
	return false;
}

bool lkImageList::Replace(int index, const wxImage& image)
{
	if ( image.IsOk() )
	{
		wxBitmap bmp = Prepare(image);
		if ( bmp.IsOk() )
			return wxImageList::Replace(index, bmp);
		//else.. fallthrough
	}

	return false;
}
