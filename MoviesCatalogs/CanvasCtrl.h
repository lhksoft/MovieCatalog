/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#ifndef __CANVAS_CTRL_H__
#define __CANVAS_CTRL_H__
#pragma once

#include <wx/panel.h>
#include <wx/custombgwin.h>
#include <wx/image.h>
#include "../lkControls/lkStaticText.h"
#include "../lkSQLite3/lkSQL3FieldData.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CanvasCtrl
////
class CanvasCtrl : public wxCustomBackgroundWindow<wxPanel>
{
public:
	CanvasCtrl();
	virtual ~CanvasCtrl();

	bool					Create(wxWindow* parent, const wxSize& size = wxDefaultSize, long style = wxBORDER_SUNKEN, const wxValidator& validator = wxDefaultValidator);
	bool					Create(wxWindow* parent, const wxImage& backGround, const wxSize& size = wxDefaultSize, long style = wxBORDER_SUNKEN, const wxValidator& validator = wxDefaultValidator);

	wxUint64				GetInternalID(void) const;
	virtual void			SetInternalID(wxUint64);

	virtual bool			AcceptsFocus() const wxOVERRIDE;

protected:
	void					OnRightClick(wxMouseEvent& event);
	void					OnBtnCopyTitle(wxCommandEvent& event);

// Attributes
protected:
	lkStaticText*			m_pTitleCtrl;
	bool					m_bValid; // used internally when nInternalID becomes '0'
	wxUint64				m_nInternalID; // internal RowID
	wxWindowID				m_nBtnCopyTitle;

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_ABSTRACT_CLASS(CanvasCtrl);
	wxDECLARE_NO_COPY_CLASS(CanvasCtrl);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// class lkStaticPanelValidator -- for BaseMoviesCanvas-Control, 
////
class lkStaticPanelValidator : public wxValidator
{
public:
	lkStaticPanelValidator();
	lkStaticPanelValidator(lkSQL3FieldData_Int*);
	lkStaticPanelValidator(const lkStaticPanelValidator&);
	virtual ~lkStaticPanelValidator();

	// required overrides
	virtual wxObject*			Clone() const wxOVERRIDE;
	virtual bool				Validate(wxWindow* parent) wxOVERRIDE;
	virtual bool				TransferToWindow() wxOVERRIDE;
	virtual bool				TransferFromWindow() wxOVERRIDE;

protected:
	lkSQL3FieldData_Int*		m_pField;
	CanvasCtrl*					GetControl(void);

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkStaticPanelValidator);
};


#endif // !__CANVAS_CTRL_H__
