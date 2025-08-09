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
#include "lkView.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS(lkView, wxView)

lkView::lkView() : wxView()
{
	m_bCanClose = true;
}

void lkView::SetCanClose(bool b)
{
	m_bCanClose = b;
}
bool lkView::CanClose() const
{
	return m_bCanClose;
}

////////////////////////////////////////////////////////////////
// lkView Attributes

wxDocParentFrame* lkView::GetParentFrame()
{
	// Create the frame
	wxWindow* parent = wxTheApp->GetTopWindow();
	if ( parent && parent->IsKindOf(wxCLASSINFO(wxDocParentFrame)) )
		return static_cast<wxDocParentFrame*>(parent);

	return NULL;
}

////////////////////////////////////////////////////////////////
// lkView Implementation

//virtual
void lkView::InitialUpdate()
{
}

//virtual
bool lkView::UpdateData(bool bSaveAndValidate)
{
	return true;
}

//virtual
void lkView::Activate(bool activate)
{
	if ( activate )
	{
		wxWindow* w = GetFrame();
		if ( w && w->IsKindOf(wxCLASSINFO(wxFrame)) )
		{
			wxFrame* f = dynamic_cast<wxFrame*>(w);
			if ( f->IsMaximized() )
				f->Maximize(false);
			if ( f->IsIconized() )
				f->Restore();
		}
	}
	wxView::Activate(activate);
}

////////////////////////////////////////////////////////////////
// lkView Overrides

//virtual
void lkView::OnDraw(wxDC* dc)
{
	// nothing to do here, all our Frames contain a Canvas and the Canvas will draw itself
}

