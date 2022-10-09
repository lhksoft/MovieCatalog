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
#ifndef __LK_CANVAS_H__
#define __LK_CANVAS_H__
#pragma once

#include <wx/panel.h>
#include <wx/custombgwin.h>
#include <wx/image.h>
#include <wx/docview.h>

class lkCanvas : public wxCustomBackgroundWindow<wxPanel>
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	lkCanvas(wxView* view, wxWindow* parent = NULL);

	virtual ~lkCanvas();

	bool					Create(wxWindow* parent, const wxString& sBackImage);
	bool					Create(wxWindow* parent, const wxImage& sBackImage);

	void					SetView(wxView* view);
	void					ResetView(void);

	wxWindow*				GetParent(void);
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	void					SetParent(wxView* view, wxWindow* parent = NULL);

protected:
	wxView*					m_pView;
	wxWindow*				m_Parent;

private:
	wxDECLARE_CLASS(lkCanvas);
	wxDECLARE_NO_COPY_CLASS(lkCanvas);
};

#endif // !__LK_CANVAS_H__
