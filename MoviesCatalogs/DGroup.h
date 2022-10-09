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
#ifndef __D_GROUP_H__
#define __D_GROUP_H__
#pragma once

#include "lkSQL3RecordDialog.h"
#include "TGroup.h"

class DGroup : public lkSQL3RecordDialog, public TGroup
{
public:
	DGroup(lkSQL3Database*, wxUint64 nBaseID);
	bool							Create(wxWindow* parent);

// Overrides of base-class lkSQL3RecordDialog
public:
	virtual wxToolBar*				OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;
	virtual void					BindButtons(void) wxOVERRIDE;

	virtual bool					TransferDataToWindow() wxOVERRIDE;
	virtual bool					TransferDataFromWindow() wxOVERRIDE;

	virtual wxString				GetDefaultOrder(void) const wxOVERRIDE; // default returns nothing, but you could set here a default filter -- wil be called in GetRecordset()

protected:
	virtual lkSQL3RecordSet*		GetBaseSet(void) wxOVERRIDE;
	// This will be called before actual DoAdd() .. e.g. to set focus to a specific control
	virtual void					DoThingsBeforeAdd(void) wxOVERRIDE;

	void							OnDlgStorages(wxCommandEvent& event);

private:
	wxWindow*						m_pStorageCtrl;
	wxString						m_sHours, m_sMinutes, m_sSeconds;
	wxWindowID						m_nIDStorage;

	wxDECLARE_ABSTRACT_CLASS(DGroup);
	wxDECLARE_NO_COPY_CLASS(DGroup);
};

#endif // !__D_GROUP_H__
