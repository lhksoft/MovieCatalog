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
#ifndef __LK_GUI_APP_TRAITS_H__
#define __LK_GUI_APP_TRAITS_H__
#pragma once

#include <wx/apptrait.h>
#include <wx/stdpaths.h>

class lkGUIAppTraits : public wxGUIAppTraits
{
public:
	// create the default configuration object (base class version is
	// implemented in config.cpp and creates wxRegConfig for wxMSW and
	// wxFileConfig for all the other platforms)
	//
	// We will only use wxFileConfig, even so in MSW
	virtual wxConfigBase*			CreateConfig() wxOVERRIDE;

	// wxStandardPaths object is normally the same for wxBase and wxGUI
	virtual wxStandardPaths&		GetStandardPaths() wxOVERRIDE;
};


#endif // !__LK_GUI_APP_TRAITS_H__
