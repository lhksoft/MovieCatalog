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
#ifndef __FILTER_CTRLS_H__
#define __FILTER_CTRLS_H__
#pragma once

#include "../lkControls/lkImgComboBox.h"

class SortComboBox : public lkImgComboBox
{
public:
	SortComboBox(wxWindow* parent, const wxValidator& validator = wxDefaultValidator);
	bool		BuildList(void);

	enum
	{
		NoSort = 1,
		Ascending,
		Descending
	};

private:
	wxDECLARE_CLASS(SortComboBox);
	wxDECLARE_NO_COPY_CLASS(SortComboBox);
};

#endif // !__FILTER_CTRLS_H__
