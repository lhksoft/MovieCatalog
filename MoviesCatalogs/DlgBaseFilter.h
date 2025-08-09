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
#ifndef __DLG_BASE_FILTER_H__
#define __DLG_BASE_FILTER_H__
#pragma once

#include "DlgFilter.h"

class BaseFilterDlg : public DlgFilter
{
public:
	BaseFilterDlg(lkSQL3Database*);
	bool							Create(wxWindow* parent);
	virtual bool					TransferDataFromWindow() wxOVERRIDE;

	wxString						GetQueryFrom(void) const;

	// used in CompactThread
	static void						CorrectConfig(lkSQL3Database* dbTmp);

protected:
	virtual wxString				GetConfigSection(void) const wxOVERRIDE;
	virtual void					AddCanvas(void) wxOVERRIDE;
	virtual void					SafeSettings(void) wxOVERRIDE; // safes what was been set in our ini file -- called in OnButton<OK>
	virtual bool					TestFilter(void) wxOVERRIDE;

// Attributes
protected:
	bool							m_bSortMovies, m_bSortStorages;

// Event Handling
protected:
	void							OnInitDialog(wxInitDialogEvent& event);

private:
	wxDECLARE_EVENT_TABLE();
	wxDECLARE_CLASS(BaseFilterDlg);
	wxDECLARE_NO_COPY_CLASS(BaseFilterDlg);
};

#endif // !__DLG_BASE_FILTER_H__
