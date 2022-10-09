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
#ifndef __LK_VIEW_H__
#define __LK_VIEW_H__
#pragma once

#include <wx/docview.h>
#include <wx/app.h>

class lkView : public wxView
{
public:
	lkView();

	// Attributes
protected:
	// Helper for creating ChildFrames
	wxDocParentFrame*				GetParentFrame(void);

	// Set to true will prevent the View/Doc from closing (when a modal or modeless dialog gets opened somewhere) via the wxDocument::OnSaveModified
	bool							m_bCanClose;

	// Implementation
public:
	void							SetCanClose(bool);
	bool							CanClose(void) const;

	// Initial Update : initialize controls with settings at first show (like MFC's CView::OnInitialUpdate)
	virtual void					InitialUpdate(void);

	// this func like MFC's CWnd::UpdateData -- validates and transfers data from controls to/from this recordset -- only makes sense if using wxValidators
	virtual bool					UpdateData(bool bSaveAndValidate = false);
	// if bSaveAndValidate==true, Validate and TransferDataFromWindow will be performed
	// if bSaveAndValidate==false (default) TransferDataToWindow will be performed

	virtual void					Activate(bool activate) wxOVERRIDE;

	// Overrides
public:
	virtual void					OnDraw(wxDC* dc) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(lkView);
	wxDECLARE_NO_COPY_CLASS(lkView);
};


#endif // !__LK_VIEW_H__
