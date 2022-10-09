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
#ifndef __XPMS_H__
#define __XPMS_H__
#pragma once

#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/icon.h>

const wxIcon Get_MoviesCatalog_Ico(void);

const wxBitmap Get_MoviesCopy_Xpm(void);
const wxBitmap Get_MoviesPaste_Xpm(void);

const wxBitmap Get_Filter_Xpm(void);
const wxBitmap Get_FilterTogle_Xpm(void);

const wxBitmap Get_Search_Xpm(void);
const wxBitmap Get_SearchNext_Xpm(void);
const wxBitmap Get_SearchPrev_Xpm(void);

const wxBitmap Get_Movies_Xpm(void);
const wxBitmap Get_Storage_Xpm(void);

const wxBitmap Get_Group_Xpm(void);
const wxBitmap Get_Copy24_Xpm(void);

const wxImage Get_NoSort_Xpm(void);
const wxImage Get_SortAsc_Xpm(void);
const wxImage Get_SortDesc_Xpm(void);

#endif // !__XPMS_H__
