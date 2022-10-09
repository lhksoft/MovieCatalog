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
#ifndef  __DLG_MOVIES_FILTER_H__
#define __DLG_MOVIES_FILTER_H__
#pragma once

#include "DlgFilter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesFilterDlg
////
class MoviesFilterDlg : public DlgFilter
{
public:
	MoviesFilterDlg(lkSQL3Database*);
	bool							Create(wxWindow* parent);

	// used in CompactThread
	static void						CorrectConfig(lkSQL3Database* dbTmp);

protected:
	virtual wxString				GetConfigSection(void) const wxOVERRIDE;
	virtual void					AddCanvas(void) wxOVERRIDE;
	virtual bool					TestFilter(void) wxOVERRIDE;

private:
	wxDECLARE_CLASS(MoviesFilterDlg);
	wxDECLARE_NO_COPY_CLASS(MoviesFilterDlg);
};



#endif // ! __DLG_MOVIES_FILTER_H__
