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
#ifndef __C_STORAGE_H__
#define __C_STORAGE_H__
#pragma once

#include "DlgFind.h"

#include <wx/image.h>
#include <wx/event.h>

#include "SimpleBox.h"
#include "../lkControls/lkTransTextCtrl.h"
#include "../lkSQLite3/lkDateTime.h"

#include "../lkSQLite3/lkSQL3RecordSet.h"
#include "../lkSQLite3/lkSQL3Field.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgCountryFind
////
class DlgStorageFind : public DlgFind
{
public:
	DlgStorageFind(lkSQL3RecordSet*, lkFindList*);
	bool					Create(wxWindow*, long style = wxDEFAULT_DIALOG_STYLE);

	// returns first rowid if successful, otherwise returns 0
	static wxUint64			FindStorage(wxWindow* parent, lkSQL3RecordSet* pRS);

protected:
	virtual void			DoInit(void) wxOVERRIDE;
	virtual void			DoOK(void) wxOVERRIDE;

private:
	wxDECLARE_ABSTRACT_CLASS(DlgStorageFind);
	wxDECLARE_NO_COPY_CLASS(DlgStorageFind);
};


///////////////////////////////////////////////////////////////
// class CStorage
////
class CStorage
{
public:
	CStorage();
	virtual ~CStorage();

	static wxString					GetConfigKeyString(void);

	wxImage							GetBackground(void) const;
	bool							CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel);

	void							SetStorageFocus(void);

	bool							stTransferDataToWindow(bool bInitial);

protected:
	virtual bool					IsMediumChanged(void) const = 0;
	virtual bool					IsLocationChanged(void) const = 0;

	virtual lkSQL3Database*			GetDBase(void) = 0;
	virtual lkSQL3RecordSet*		GetBaseRecordset(void) = 0;

	virtual lkSQL3FieldData_Text*	GetFldStorage(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldLocation(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldMedium(void) = 0;
	virtual lkSQL3FieldData_Real*	GetFldCreation(void) = 0;
	virtual lkSQL3FieldData_Int*	GetFldInfo(void) = 0;

	void							OnSelectDate(wxCommandEvent& event);

protected:
	bool							m_bReqAdd;

	lkTransTextCtrl*				m_pStorageCtrl; // when adding a new record, this control will get focus
	MediumBox*						m_pMedium;
	LocationBox*					m_pLocation;
	lkTransTextCtrl*				m_pCreaDate;

	wxWindowID						m_ID_DateBtn;
	lkDateTime						m_CreaDate;


private:
	wxWindow*						m_pParentFrame; // either a wxFrame or wxDialog
	wxWindow*						m_pParentPanel; // either a wxPanel or wxDialog

	wxDECLARE_NO_COPY_CLASS(CStorage);
};



#endif // !__C_STORAGE_H__
