/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __LK_CONFIG_TOOLS_H__
#define __LK_CONFIG_TOOLS_H__
#pragma once

#include <wx/string.h>

// These loads/saves these settings from wxConfBase::Get

bool			DoFlush(void);
bool			DeleteKey(const wxString& section, const wxString& key);

wxString		GetConfigString(const wxString& section, const wxString& key);
void			SetConfigString(const wxString& section, const wxString& key, const wxString&);

wxInt64			GetConfigInt(const wxString& section, const wxString& key);
void			SetConfigInt(const wxString& section, const wxString& key, wxInt64, wxInt64 defVal = 0);

bool			GetConfigBool(const wxString& section, const wxString& key, bool defVal);
void			SetConfigBool(const wxString& section, const wxString& key, bool, bool defVal = false);

#endif // !__LK_CONFIG_TOOLS_H__
