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
#ifndef __D_STORAGE_H__
#define __D_STORAGE_H__
#pragma once

#include "lkSQL3RecordDialog.h"
#include "CStorage.h"
#include "TStorage.h"

class DStorage : public lkSQL3RecordDialog, public TStorage, private CStorage
{
public:
	DStorage(lkSQL3Database*);
	bool							Create(wxWindow* parent);

// Overrides of base-class lkSQL3RecordDialog
public:
	virtual wxToolBar*				OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;
	virtual void					BindButtons(void) wxOVERRIDE;
	virtual bool					Validate() wxOVERRIDE;
	virtual bool					TransferDataToWindow() wxOVERRIDE;
	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()

protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;

	// This will be called before actual DoAdd() .. e.g. to set focus to a specific control
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

	virtual bool					DoUpdateRecordFind(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordFindNext(void) wxOVERRIDE;
	virtual bool					DoUpdateRecordFindPrev(void) wxOVERRIDE;


// Overrides of base-class CStorage
protected:
	virtual bool					IsMediumChanged(void) const wxOVERRIDE;
	virtual bool					IsLocationChanged(void) const wxOVERRIDE;

	virtual lkSQL3Database*			GetDBase(void) wxOVERRIDE;
	virtual lkSQL3RecordSet*		GetBaseRecordset(void) wxOVERRIDE;

	virtual lkSQL3FieldData_Text*	GetFldStorage(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldLocation(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldMedium(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Real*	GetFldCreation(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldInfo(void) wxOVERRIDE;

// Attributes
protected:
	wxWindowID						m_nID_NextID;
	wxWindowID						m_nID_Search;
	wxWindowID						m_nID_SearchNext;
	wxWindowID						m_nID_SearchPrev;

public:
	wxWindowID						GetID_NextID(void) const;
	wxWindowID						GetID_Search(void) const;
	wxWindowID						GetID_SearchNext(void) const;
	wxWindowID						GetID_SearchPrev(void) const;

// Event Handling
protected:
	void							OnUpdateNextID(wxUpdateUIEvent& event);
	void							OnNextID(wxCommandEvent& event);
	void							OnFind(wxCommandEvent& event);

private:
	wxDECLARE_ABSTRACT_CLASS(DStorage);
	wxDECLARE_NO_COPY_CLASS(DStorage);
};

#endif // !__D_STORAGE_H__
