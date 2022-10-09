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
#ifndef __DEFINES_H__
#define __DEFINES_H__
#pragma once

#include <wx/defs.h>

// IDs for the controls and the menu commands


///////////////////////////////////////////////////////////////
// Child-Views Navigation
#define ID_MENU_Views_Base			(wxID_HIGHEST + 151)
#define ID_MENU_Views_Category		(wxID_HIGHEST + 152)
#define ID_MENU_Views_Country		(wxID_HIGHEST + 153)
#define ID_MENU_Views_Genres		(wxID_HIGHEST + 154)
#define ID_MENU_Views_Judge			(wxID_HIGHEST + 155)
#define ID_MENU_Views_Location		(wxID_HIGHEST + 156)
#define ID_MENU_Views_Media			(wxID_HIGHEST + 157)
#define ID_MENU_Views_Movies		(wxID_HIGHEST + 158)
#define ID_MENU_Views_Origine		(wxID_HIGHEST + 159)
#define ID_MENU_Views_Quality		(wxID_HIGHEST + 160)
#define ID_MENU_Views_Rating		(wxID_HIGHEST + 161)
#define ID_MENU_Views_Screen		(wxID_HIGHEST + 162)
#define ID_MENU_Views_Storage		(wxID_HIGHEST + 163)

#define ID_CAT_SubsList				(wxID_HIGHEST + 201)
#define ID_CAT_SUBS_Add				(wxID_HIGHEST + 202)
#define ID_CAT_SUBS_Edit			(wxID_HIGHEST + 203)
#define ID_CAT_SUBS_Del				(wxID_HIGHEST + 204)

#define ID_CTR_Preview				(wxID_HIGHEST + 211)
#define ID_GNR_Preview				(wxID_HIGHEST + 212)
#define ID_JDG_Preview				(wxID_HIGHEST + 213)
#define ID_LOC_Preview				(wxID_HIGHEST + 214)
#define ID_MDM_Preview				(wxID_HIGHEST + 215)
#define ID_ORG_Preview				(wxID_HIGHEST + 216)
#define ID_QLT_Preview				(wxID_HIGHEST + 217)
#define ID_RTG_Preview				(wxID_HIGHEST + 218)
#define ID_SCR_Preview				(wxID_HIGHEST + 219)

#define ID_MAIN_Compact				(wxID_HIGHEST + 250)
#define ID_MAIN_About				(wxID_HIGHEST + 251)

#endif // !__DEFINES_H__
