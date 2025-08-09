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
#include "lkCanvas.h"

wxIMPLEMENT_CLASS(lkCanvas, wxPanel)

lkCanvas::lkCanvas(wxView* view, wxWindow* parent) : wxCustomBackgroundWindow<wxPanel>()
{
	m_pView = NULL;
	m_Parent = NULL;
	SetParent(view, parent);
	SetView(view);
}
lkCanvas::~lkCanvas()
{
}

void lkCanvas::SetView(wxView* view)
{
	wxASSERT_MSG(!m_pView, "shouldn't be already associated with a view");
	m_pView = view;
}

void lkCanvas::ResetView()
{
	wxASSERT_MSG(m_pView, "should be associated with a view");
	m_pView = NULL;
}

wxWindow* lkCanvas::GetParent()
{
	return m_Parent;
}
void lkCanvas::SetParent(wxView* view, wxWindow* parent)
{
	wxASSERT((parent != NULL) || (view != NULL)); // either must be valid
	m_Parent = (parent) ? parent : view->GetFrame();
}


//virtual
bool lkCanvas::Create(wxWindow* parent, const wxString& sBackImage)
{
	wxImage img(sBackImage);
	return Create(parent, img);
}

//virtual
bool lkCanvas::Create(wxWindow* parent, const wxImage& sBackImage)
{
	if ( !wxCustomBackgroundWindow<wxPanel>::Create(parent) )
		return false;

	if ( sBackImage.IsOk() )
	{
		wxBitmap bmp(sBackImage);
		SetBackgroundBitmap(bmp);
	}

	return true;
}
