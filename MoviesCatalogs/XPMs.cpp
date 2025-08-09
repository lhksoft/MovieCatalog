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
#include "XPMs.h"
#include "bitmaps/ToolbarExtra.xpms"
#include "bitmaps/Sorting.xpms"
#include "bitmaps/MoviesCatalog.xpm"

const wxIcon Get_MoviesCatalog_Ico()
{
	return wxIcon(MOVIESCATALOG_xpm);
}

const wxBitmap Get_MoviesCopy_Xpm()
{
	return wxBitmap(MOVIES_COPY_XPM);
}

const wxBitmap Get_MoviesPaste_Xpm()
{
	return wxBitmap(MOVIES_PASTE_XPM);
}

const wxBitmap Get_Filter_Xpm()
{
	return wxBitmap(FILTER_XPM);
}

const wxBitmap Get_FilterTogle_Xpm()
{
	return wxBitmap(FILTER_TOGLE_XPM);
}

const wxBitmap Get_Search_Xpm()
{
	return wxBitmap(SEARCH_XPM);
}

const wxBitmap Get_SearchNext_Xpm()
{
	return wxBitmap(SEARCH_NEXT_XPM);
}

const wxBitmap Get_SearchPrev_Xpm()
{
	return wxBitmap(SEARCH_PREV_XPM);
}

const wxBitmap Get_Movies_Xpm()
{
	return wxBitmap(MOVIES_XPM);
}

const wxBitmap Get_Storage_Xpm()
{
	return wxBitmap(STORAGE_XPM);
}

const wxBitmap Get_Group_Xpm()
{
	return wxBitmap(GROUP_XPM);
}

const wxBitmap Get_Copy24_Xpm()
{
	return wxBitmap(COPY_24_XPM);
}

const wxImage Get_NoSort_Xpm()
{
	return wxImage(NOSORT_XPM);
}

const wxImage Get_SortAsc_Xpm()
{
	return wxImage(SORTASC_XPM);
}

const wxImage Get_SortDesc_Xpm()
{
	return wxImage(SORTDESC_XPM);
}
