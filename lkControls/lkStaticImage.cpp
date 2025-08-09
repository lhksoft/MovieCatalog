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
#include "lkStaticImage.h"
#include "lkColour.h"

#include <wx/msgdlg.h>
#include <wx/dcclient.h>

wxIMPLEMENT_DYNAMIC_CLASS(lkStaticImage, wxControl);

wxBEGIN_EVENT_TABLE(lkStaticImage, wxControl)
	EVT_PAINT(lkStaticImage::OnPaint)
wxEND_EVENT_TABLE()

lkStaticImage::lkStaticImage() : wxControl(), m_Image(),
	m_Pen(lkCol_Black, 1, wxPENSTYLE_SHORT_DASH)
{
    SetDrawMode();
}

lkStaticImage::lkStaticImage(wxWindow* parent, const wxSize& size, wxWindowID id, const wxValidator& validator) : wxControl(), m_Image(),
	m_Pen(lkCol_Black, 1, wxPENSTYLE_SHORT_DASH)
{
    SetDrawMode();
	Create(parent, size, id, validator);
}

lkStaticImage::~lkStaticImage()
{
    if (m_Image.IsOk())
        m_Image.Destroy();
}

bool lkStaticImage::Create(wxWindow* pParent, const wxSize& sSize, wxWindowID id, const wxValidator& validator)
{
	long border = wxBORDER_SUNKEN;
#ifdef __WXMSW__
	border = wxBORDER_STATIC;
#endif // __WXMSW__

	if ( !wxControl::Create(pParent, id, wxDefaultPosition, sSize,
		border | wxFULL_REPAINT_ON_RESIZE | wxTRANSPARENT/* | wxTRANSPARENT_WINDOW*/, validator) )
		return false;

	// do some other stuff
	return true;
}

//virtual
bool lkStaticImage::HasTransparentBackground()
{
	return true;
}

// Implementation
//////////////////////////////

void lkStaticImage::SetDrawMode(lkResizeOptions opt)
{
    m_drawMode = opt;
}

//virtual
void lkStaticImage::SetEmpty(bool bRefresh/*=true*/)
{
    if (m_Image.IsOk())
        m_Image.Destroy();

    wxASSERT( !m_Image.IsOk() );

	if ( bRefresh )
	{
		ClearBackground();
		Refresh(true);
	}
}
bool lkStaticImage::IsEmpty() const
{
	return !m_Image.IsOk();
}

// replaces current image with a File
//virtual
void lkStaticImage::SetImage(const wxString& strPath)
{
	SetEmpty(false);

	wxImage img(strPath, wxBITMAP_TYPE_ANY);
	if (img.IsOk())
		m_Image = img;

	ClearBackground();
	Refresh(true);
}

// replaces current image from an Image
//virtual
void lkStaticImage::SetImage(const wxImage& iImage)
{
    SetEmpty(false);

    if (iImage.IsOk())
        m_Image = iImage;

	ClearBackground();
	Refresh(true);
}

wxImage lkStaticImage::GetImage() const
{
	return m_Image;
}

// Events
//////////////////////////////

void lkStaticImage::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	if (m_Image.IsOk())
	{
		wxRect rcClient(GetClientRect());
		wxRect rcImg = CalculateEstimateRect(rcClient, m_Image, m_drawMode);
		if (!rcImg.IsEmpty())
		{
			// in wxWidgets, only wxBitmap can be drawn - not wxImage
			wxBitmap bmp;
			if ((rcImg != rcClient) || (rcImg.GetHeight() != m_Image.GetHeight()) || (rcImg.GetWidth() != m_Image.GetWidth()))
			{ // Resacle if nessecary
				bmp = wxBitmap(m_Image.Scale(rcImg.GetWidth(), rcImg.GetHeight()));
			}
			else
				bmp = m_Image;

			wxRect rcDraw = Expand(wxSize(rcClient.GetWidth(), rcClient.GetHeight()), bmp.GetSize(), lkExpandFlags::EXPAND_Center);
			dc.DrawBitmap(bmp, rcDraw.GetLeft(), rcDraw.GetTop(), true);
		}
	}
	else
	{
		dc.SetPen(m_Pen);
		wxRect rc = GetClientRect();
		dc.DrawLine(rc.GetLeftTop(), rc.GetBottomRight());
		dc.DrawLine(rc.GetRightTop(), rc.GetBottomLeft());
	}
}

