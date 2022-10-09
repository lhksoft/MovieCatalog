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
#include "lkImageBox.h"
#include "lkClientData.h"
#include "lkColour.h"

// For wxODCB_XXX flags
#include <wx/odcombo.h>
#include <wx/combobox.h>
#include <wx/dc.h>

#include <wx/settings.h>

/* **********************************************************************************************************
 * class lkImageBox
 * ********************************************************************************************************** */

lkImageBox::lkImageBox() : m_nImageSize(), m_colSelBack(), m_colSelFore(), m_defcSelBack(), m_defcSelFore()
{
	lkColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT).GetRGB();
	col.SetLighter(30);
	m_defcSelBack = col;
	m_defcSelFore = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);

	m_colSelBack = m_defcSelBack;

	Init();
}
lkImageBox::~lkImageBox()
{
	KillInitList();
}

void lkImageBox::SetSelectedBackColour(WXCOLORREF b)
{
	m_colSelBack = wxColour(b);
}
void lkImageBox::SetSelectedForeColour(WXCOLORREF f)
{
	m_colSelFore = wxColour(f);
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void lkImageBox::Init()
{
	// f'tor StartOver also calls Init() so don't set things in here which should not be re-initialised (like m_colSelBack)
	m_nImageSize = wxSize(lk_ICONSIZE, lk_ICONSIZE);
	m_initList = NULL;
	m_nImageCount = 0;

	m_nBestHeight = 0;
	m_fontHeight = 0;
	m_imgAreaWidth = 0;

	m_bFitScaled = true;
	m_ExpandAlign = (m_bFitScaled) ? lkExpandFlags::EXPAND_Center : lkExpandFlags::EXPAND_Left;
}

void lkImageBox::KillInitList()
{
	if (m_initList)
	{
		delete m_initList;
		m_initList = NULL;
	}
}


//virtual
void lkImageBox::StartOver()
{
	KillInitList();
	Init();

	wxWindow* ctrl = GetControl();
	if ( ctrl && ctrl->GetHandle() )
	{
		if ( ctrl->IsKindOf(wxCLASSINFO(wxVListBox)) )
			(dynamic_cast<wxVListBox*>(ctrl))->SetSelection(-1);
		else if ( ctrl->IsKindOf(wxCLASSINFO(wxOwnerDrawnComboBox)) )
			(dynamic_cast<wxOwnerDrawnComboBox*>(ctrl))->SetSelection(-1);
		else if ( ctrl->IsKindOf(wxCLASSINFO(wxComboBox)) )
			(dynamic_cast<wxComboBox*>(ctrl))->SetSelection(-1);
	}
}

//virtual
void lkImageBox::UpdateInternals()
{
	wxWindow* ctrl = GetControl();
	m_fontHeight = ctrl->GetCharHeight(); // +GetControl()->FromDIP(0);

	m_nBestHeight = ctrl->FromDIP(lkCY_BORDER) * 2 + wxMax(m_fontHeight, m_nImageSize.GetHeight());
}

void lkImageBox::SetForcedHeight(int cy)
{
	m_nImageSize.SetHeight(cy);
}
void lkImageBox::SetForcedWidth(int cx)
{
	m_nImageSize.SetWidth(cx);
}

void lkImageBox::SetImageExpand(lkExpandFlags expAlign)
{
	m_ExpandAlign = expAlign;
}
void lkImageBox::SetImgScaled(bool b)
{
	m_bFitScaled = b;
}

// ----------------------------------------------------------------------------
// Item manipulation
// ----------------------------------------------------------------------------

wxBitmap lkImageBox::GetItemBitmap(int n) const
{
	if ( m_nImageCount == 0 )
		return wxBitmap();
/*
	const wxItemContainer* itemContainer = const_cast<lkImageBox*>(this)->GetItemContainer();
	wxASSERT(itemContainer && itemContainer->HasClientObjectData());

	wxBitmap* pBmp = NULL;
	if (itemContainer)
	{
		const lkImgClientData* imgClientData = dynamic_cast<lkImgClientData*>(itemContainer->GetClientObject(n));
		pBmp = imgClientData->GetBitmap();
	}

	return (pBmp) ? *pBmp : wxBitmap();
*/
	const lkImgClientData* imgClientData = dynamic_cast<lkImgClientData*>(GetItemObject(n));
	wxBitmap* pBmp = imgClientData->GetBitmap();
	return (pBmp) ? *pBmp : wxBitmap();
}
wxSize lkImageBox::GetBitmapSize() const
{
	return m_nImageSize;
}

wxCoord lkImageBox::GetImageHeight() const
{
	wxASSERT(m_nImageSize.GetHeight() != 0);
	wxCoord c = 0;
	if (m_nImageCount > 0)
		c = m_nImageSize.GetHeight();

	return c;
}
wxCoord lkImageBox::GetImageWidth() const
{
	wxASSERT(m_nImageSize.GetWidth() != 0);
	wxCoord c = 0;
	if (m_nImageCount > 0)
		c = m_nImageSize.GetWidth();

	return c;
}

int lkImageBox::GetAreaWidth() const
{
	return m_imgAreaWidth;
}

int lkImageBox::GetBestHeight() const
{
	return m_nBestHeight;
}


// ----------------------------------------------------------------------------
// wxItemContainer methods
// ----------------------------------------------------------------------------

wxBitmap lkImageBox::Prepare(const wxImage& _img)
{ // prepares given image for addition/replacement
	wxImage img(_img); // required, we have no direct access to a const object
	if (!img.IsOk())
		return wxBitmap(); // needs to be a valid image

	wxImage tmpImg;
	// check that the size of given image does mach
	wxSize size = img.GetSize();
	if ((size.GetWidth() != m_nImageSize.GetWidth()) || (size.GetHeight() != m_nImageSize.GetHeight()))
	{
		wxRect rect(wxPoint(0, 0), m_nImageSize);

		if ((!m_bFitScaled && (m_nImageSize.GetWidth() == m_nImageSize.GetHeight())) ||
			(!CalculateEstimateRect(&rect, img, lkResizeOptions::FIT_SCALED)))
		{
			wxImage iScaled(img.Scale(m_nImageSize.GetWidth(), m_nImageSize.GetHeight()));
			tmpImg = iScaled;
		}
		else
		{ // expanding, so we display it correctly - i hope
			wxImage iScaled(img.Scale(rect.GetWidth(), rect.GetHeight()));
			wxImage iExpanded(Expand(iScaled, m_nImageSize, m_ExpandAlign));
			tmpImg = iExpanded;
		}
	}
	else
		tmpImg = img; // just copy

	return wxBitmap(tmpImg);
}


// ----------------------------------------------------------------------------
// Preparation and Calculations
// ----------------------------------------------------------------------------

bool lkImageBox::InitializeFromList(lkBoxInitList* obList, bool calcWidth, lkExpandFlags expAlign)
{
	StartOver();
	wxASSERT(m_initList == NULL);

	if (!obList || obList->IsEmpty())
		return false;

	// setting (new) desired alignement
	SetImageExpand(expAlign);

	lkBoxInitList* initList = new lkBoxInitList();
	initList->DeleteContents(true);

	lkBoxInit* pElement;
	wxSize sz(m_nImageSize), sy, defSize; // sz : default width/height: ICON_SIZE(=16)
	wxRect rect;
	wxImage* img;
	// let's iterate over the list in STL syntax
	for (lkBoxInitList::iterator iter = obList->begin(); iter != obList->end(); ++iter)
	{
		pElement = *iter;
		// first, take a copy of this object
		initList->Append(new lkBoxInit(*pElement));

		if ((img = pElement->GetImage()) != NULL)
		{	// validation for isOk already performed
			m_nImageCount++;

			if (calcWidth)
			{	// calculate best width to fit all images
				sy = wxDefaultSize; // w = h = -1

				// whatever size it is, HEIGHT may not exceed pre-defined height (useally 16 : ICON_SIZE)
				if ( expAlign == lkExpandFlags::EXPAND_Center )
					defSize = wxSize(m_nImageSize.GetWidth(), m_nImageSize.GetHeight()); // force width/height not to exceed 16px
				else
					defSize = wxSize(img->GetWidth(), m_nImageSize.GetHeight()); // force height not to exceed 16px
				rect = wxRect(wxPoint(0, 0), defSize);

				if (CalculateEstimateRect(&rect, *img, lkResizeOptions::FIT_SCALED))
				{
					sy.SetWidth(rect.GetWidth());
					sy.SetHeight(rect.GetHeight());
				}

				if (sy.IsFullySpecified())
				{ // if not, then the image properly is invalid
					// determining height/width
					sz.SetWidth(wxMax(sz.GetWidth(), sy.GetWidth()));
					sz.SetHeight(wxMax(sz.GetHeight(), sy.GetHeight()));

					if (sz.GetHeight() > m_nImageSize.GetHeight()) // m_nImageSize.Height should be lk_ICONSIZE
					{ // height always is ICON_SIZE, hacking height/width so we'll won't loose too much of the aspect-ratio (i hope)
						int cy = m_nImageSize.GetHeight() - sz.GetHeight();
						sz.SetWidth(sz.GetWidth() + cy);
						sz.SetHeight(sz.GetHeight() + cy);
					}
					wxASSERT(sz.GetHeight() <= lk_ICONSIZE);
					// setting new desired width
					SetForcedWidth(sz.GetWidth());
					SetForcedHeight(sz.GetHeight());
				}
			} // if (calcWidth)
		} // if (img != NULL)
	}

	m_initList = initList;
	initList = NULL;
	return true;
}

bool lkImageBox::Populate()
{
	if (!m_initList || m_initList->IsEmpty())
	{
		wxASSERT_MSG(false, wxT("First call 'lkImageBox::InitializeFromList' to initilise the data"));
		return false;
	}

	lkBoxInit* pElement;
	wxImage* img;
	wxBitmap bmp;
	// let's iterate over the list in STL syntax
	for (lkBoxInitList::iterator iter = m_initList->begin(); iter != m_initList->end(); ++iter)
	{
		pElement = *iter;

		if ( (m_nImageCount > 0) && ((img = pElement->GetImage()) != NULL) )
			bmp = Prepare(*img);
		else
			bmp = wxBitmap();

		AddItem(pElement, &bmp);
	}

	KillInitList();
	DetermineIndent();
	UpdateInternals();

	// Adjust control size to vertically fit the bitmap
	wxWindow* ctrl = GetControl();
	ctrl->InvalidateBestSize();
	wxSize newSz = ctrl->GetBestSize();
	wxSize sz = ctrl->GetSize();
	if ( newSz.y > sz.y )
		ctrl->SetSize(sz.x, newSz.y);

	ctrl->Refresh();

	return true;
}

// virtual
int lkImageBox::DetermineIndent()
{
	// Recalculate amount of empty space needed in front of text in control itself.
	int indent = m_imgAreaWidth = 0;

	if ( m_nImageCount > 0 )
		indent += m_nImageSize.x + GetControl()->FromDIP(lkCX_BORDER);
		
	m_imgAreaWidth = indent;

	return indent;
}

// ----------------------------------------------------------------------------
// Item drawing and measuring
// ----------------------------------------------------------------------------

void lkImageBox::DrawBackground(wxDC& dc, const wxRect& rect, int WXUNUSED(item), int flags) const
{
#ifdef __WXDEBUG__
	{
		const wxWindow* ctrl = const_cast<lkImageBox*>(this)->GetControl();
		if ( ctrl && ctrl->IsKindOf(wxCLASSINFO(wxVListBox)) )
		{
			wxASSERT(false);
			// not mend for wxVListBox derived classes ! use wxVListBox::SetSelectionBackground for set different colour for selected item
			return;
		}
	}
#endif // __WXDEBUG__

	wxBrush brush;

	if ( flags & wxODCB_PAINTING_SELECTED )
	{
		wxColour fore;
		if ( !m_colSelFore.IsOk() )
		{
			if ( m_colSelBack == m_defcSelBack )
				fore = m_defcSelFore;
			else
			{
				WXCOLORREF f = lkColour::GetViewableForeColor(m_colSelBack.GetRGB());
				fore = wxColor(f);
			}
		}
		else
			fore = m_colSelFore;

		brush = wxBrush(m_colSelBack);
		const wxWindow* ctrl = const_cast<lkImageBox*>(this)->GetControl();
		if ( !ctrl->IsEnabled() )
			fore = fore.MakeDisabled();
		dc.SetTextForeground(fore);
	}
	else
	{
		const wxWindow* ctrl = const_cast<lkImageBox*>(this)->GetControl();
		wxColour col = ctrl->GetBackgroundColour();
		if ( !col.IsOk() )
			col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
		if ( !ctrl->IsEnabled() )
			col = col.MakeDisabled();
		brush = col;
//		brush = *wxTRANSPARENT_BRUSH;
		col = ctrl->GetForegroundColour();
		if ( !col.IsOk() )
			col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
		if ( !ctrl->IsEnabled() )
			col = col.MakeDisabled();
		dc.SetTextForeground(col);
	}

	dc.SetBrush(brush);
	dc.SetPen(*wxTRANSPARENT_PEN);
	const int vSizeDec = 0;  // Vertical size reduction of selection rectangle edges
	dc.DrawRectangle(rect.x,
					 rect.y + vSizeDec,
					 rect.width,
					 rect.height - (vSizeDec * 2));
}

void lkImageBox::DrawItem(wxDC& dc, const wxRect& rect, int item, const wxString& text, int flags) const
{
	const wxWindow* win = const_cast<lkImageBox*>(this)->GetControl();
	const int imgSpacingLeft = win->FromDIP(lkCX_BORDER);
	const int imgSpacingTop = win->FromDIP(lkCY_BORDER);
	const int x = (rect.x == 0) ? imgSpacingLeft : 0;

	if (m_nImageCount > 0)
	{
		const wxBitmap& bmp = GetItemBitmap(item);
		if (bmp.IsOk())
		{
			wxCoord w = bmp.GetWidth();
			wxCoord h = bmp.GetHeight();

			wxCoord cx = rect.x + x + (m_nImageSize.x - w) / 2;
			wxCoord cy = rect.y + (rect.height - h) / 2 + imgSpacingTop / 2;

			// Draw the image
			dc.DrawBitmap(bmp, cx, cy, true);
			//dc.DrawBitmap(bmp, rect.x + imgSpacingLeft, rect.y + imgSpacingTop, true);
		}
	}

	if ( !text.IsEmpty() )
	{
		wxRect rc = rect;
		rc.x += x + m_imgAreaWidth;
		wxCoord clW = rc.GetWidth() - rc.x;

		wxString label = wxControl::Ellipsize(text, dc, wxELLIPSIZE_END, clW);

		dc.DrawText(label, rect.x + x + m_imgAreaWidth, rect.y + imgSpacingTop + (rect.height - dc.GetCharHeight()) / 2);
	}
}

wxCoord lkImageBox::MeasureItem(size_t WXUNUSED(item)) const
{
	if (m_nBestHeight > 0)
		return m_nBestHeight;

	const wxWindow* win = const_cast<lkImageBox*>(this)->GetControl();
	if (win)
	{
		if (m_nImageSize.y >= 0)
		{
			int yBorder = win->FromDIP(lkCY_BORDER);
			return (((m_nImageSize.y > m_fontHeight) ? m_nImageSize.y : m_fontHeight) + yBorder * 2);
		}
		return win->FromDIP(lkDEFAULT_ITEM_HEIGHT);
	}
	// else
	return 0;
}
