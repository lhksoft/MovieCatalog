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
#ifndef __DLG_SUBCAT_H__
#define __DLG_SUBCAT_H__
#pragma once

#include "../lkControls/lkDialog.h"
#include "TCategory.h"

class SubCatDlg : public lkDialog
{
public:
	SubCatDlg();
	SubCatDlg(lkSQL3Database*, wxUint32 nCat, wxUint32 nSub);
	virtual ~SubCatDlg();

	// Create NOT called from C'tor !!
	bool								Create(wxWindow* pParent);
	void								SetConfigSection(const wxString&);

// Attributes
protected:
	lkSQL3Database*						m_pDB;
	TCategory*							m_pSet; // base RecordSet
	wxUint32							m_nCat, m_nSub; // having m_nSub==0 will put this dialog in ADD modus

	wxString							m_ConfigSection;

protected:
	wxDECLARE_EVENT_TABLE();

	void								OnOK(wxCommandEvent& event);
	void								OnCancel(wxCommandEvent& event);

private:
	void								Init();

	wxDECLARE_DYNAMIC_CLASS(SubCatDlg);
	wxDECLARE_NO_COPY_CLASS(SubCatDlg);
};

#endif // !__DLG_SUBCAT_H__
