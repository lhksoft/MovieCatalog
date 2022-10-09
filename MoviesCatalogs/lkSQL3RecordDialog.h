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
#ifndef __LK_SQL3_RECORD_DIALOG_H__
#define __LK_SQL3_RECORD_DIALOG_H__
#pragma once

#include "../lkControls/lkRecordDlg.h"
#include "../lkSQLite3/lkSQL3RecordSet.h"

class lkSQL3RecordDialog : public lkRecordDlg
{
public:
	lkSQL3RecordDialog(lkSQL3Database* pDB = NULL);

	bool							Create(wxWindow* parent, const wxString& sBackImage, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
										   long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	bool							Create(wxWindow* parent, const wxImage& sBackImage, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
										   long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	virtual void					BindButtons(void) wxOVERRIDE;

	// Initializes by positioning current record at given row
	void							SetCurRecord(wxUint64 nRow);
	wxUint64						GetCurRecord(void) const;

// Attributes
public:
	lkSQL3Database*					GetDB(void);
	void							SetDB(lkSQL3Database*);

	lkSQL3RecordSet*				GetRecordset(void);

protected:
	lkSQL3Database*					m_pDB; // if not set, nothing can be done..
	lkSQL3RecordSet*				m_pSet; // base RecordSet, opened in derived class via GetRecordSet
	wxUint64						m_nCurRow; // will be set when <OK> clicked

	// As derived child-docs also are derived from a TableSet, this should return 'this' in derived classes
	virtual lkSQL3RecordSet*		GetBaseSet(void) = 0;


// Operations
public:
	virtual bool					IsOk(void) const wxOVERRIDE; // true if IsOpen && m_bValidDB==true

	virtual bool					CanAppend(void) wxOVERRIDE;
	virtual bool					CanRemove(void) wxOVERRIDE;

	virtual void					DoEscape(void) wxOVERRIDE;
	virtual bool					DoDelete(void) wxOVERRIDE;
	virtual bool					DoUpdate(void) wxOVERRIDE;
	virtual bool					DoAdd(void) wxOVERRIDE;

	virtual bool					DoMove(MoveIDs nIDMoveCommand) wxOVERRIDE;

protected:
	// Moves to given RowID
	bool							GotoRecord(void);

	virtual void					UpdateRecordStatusbar(void) wxOVERRIDE;

// Event Handling
protected:
	virtual bool					DoUpdateRecordFirst(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordPrev(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordNext(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordLast(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordNew(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordRemove(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordUpdate(void) wxOVERRIDE;

	void							OnInitDialog(wxInitDialogEvent& event);
	void							OnButton(wxCommandEvent& event);

	void							OnFindNext(wxCommandEvent& event);
	void							OnFindPrev(wxCommandEvent& event);


private:
	wxDECLARE_ABSTRACT_CLASS(lkSQL3RecordDialog);
	wxDECLARE_NO_COPY_CLASS(lkSQL3RecordDialog);
};

#endif // !__LK_SQL3_RECORD_DIALOG_H__
