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
#ifndef __V_BASE_CONTROLS_H__
#define __V_BASE_CONTROLS_H__
#pragma once

#include "../lkControls/lkChkImgListbox.h"
#include "../lkControls/lkStringObj.h"
#include "../lkSQLite3/lkSQL3Database.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseCountryBox -- using TCountry::t3Country_ISO for marking values
////
class BaseCountryBox : public lkChkImgListbox
{
public:
	BaseCountryBox(lkSQL3Database*, bool bActingAsAudio);
	BaseCountryBox(lkSQL3Database*, bool bActingAsAudio, wxWindow* pParent, wxWindowID nId = wxID_ANY, const wxPoint& nPos = wxDefaultPosition, const wxSize& nSize = wxDefaultSize,
				   long nStyle = 0, const wxValidator& validator = wxDefaultValidator);
	virtual ~BaseCountryBox();

	bool					BuildList(lkExpandFlags flags = lkExpandFlags::EXPAND_Center);

	void					SetCheck(const wxString&); // checks all in given value
	wxString				GetChecked(void) const; // all unique lparam of each checked value will be concatenated into 1 string
	wxString				GetFilter(const wxString& field) const;

protected:
	lkSQL3Database*			m_pDB;
	lkStringObjList			m_TmpList;
	bool					m_bActingAsAudio; // true=AudioList, false=SubsList

private:
	wxDECLARE_CLASS(BaseCountryBox);
	wxDECLARE_NO_COPY_CLASS(BaseCountryBox);
};


#endif // !__V_BASE_CONTROLS_H__
