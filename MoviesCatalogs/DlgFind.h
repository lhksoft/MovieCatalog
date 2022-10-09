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
#ifndef __DLG_FIND_H__
#define __DLG_FIND_H__
#pragma once

#include <wx/dialog.h>
#include <wx/list.h>

#include "../lkControls/lkImgComboBox.h"
#include "../lkSQLite3/lkSQL3RecordSet.h"

// standard config-keys
#define conf_FIND_DIRECTION		wxT("FindDirection")
#define conf_FIND_FORWARD		wxT("FindForward")
#define conf_FIND_FIELD			wxT("FindField")

class lkFindParam : public wxObject
{
public:
	lkFindParam();
	lkFindParam(const wxString& field, const wxString& readable);
	virtual ~lkFindParam();

	wxString		GetField(void) const;
	wxString		GetReadable(void) const;

protected:
	wxString		m_sField;
	wxString		m_sReadable;

private:
	wxDECLARE_CLASS(lkFindParam);
	wxDECLARE_NO_COPY_CLASS(lkFindParam);
};

WX_DECLARE_LIST(lkFindParam, lkFindList);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgFind
// Not using a background, just a colored backgroundcolour
////
class DlgFind : public wxDialog
{
public:
	DlgFind(lkSQL3RecordSet*, lkFindList*);
	virtual ~DlgFind();

	bool					Create(wxWindow*, const wxString& szTitle = wxEmptyString, long style = wxDEFAULT_DIALOG_STYLE);

protected:
	lkFindList*				m_pParams; // required in DoInit & DoOk

	wxUint64				m_nField;
	wxUint64				m_nSrcOn; // will hold one of the FIND_DIRECTION enums
	wxString				m_sSearch;
	bool					m_bForward;

protected:
	void					OnInitDialog(wxInitDialogEvent& event);
	void					OnRightClick(wxMouseEvent& event);
	void					OnButton(wxCommandEvent& event);

	virtual void			DoInit(void); // get's called in OnInitDialog before TransferToWindow -- for retrieving defaults and such in derived classes
	virtual void			DoOK(void); // get's called when wxOK clicked -- for storing defaults and such in derived classes

	bool					TestFind(void); // displays a msgbox in case nothing was found

public:
	wxString				GetFindString(void) const;
	bool					IsForward(void) const;

private:
	// for internal usage
	lkSQL3RecordSet*		m_pRS;

	lkImgComboBox*			m_pFieldCombo;
	lkImgComboBox*			m_pSearchCombo;
	wxWindow*				m_pTextbox; // will get focus on initial view
	bool					m_bBackward;

	void					CreateCanvas(void);

private:
	wxDECLARE_EVENT_TABLE();

	wxDECLARE_ABSTRACT_CLASS(DlgFind);
	wxDECLARE_NO_COPY_CLASS(DlgFind);
};

#endif // !__DLG_FIND_H__
