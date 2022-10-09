/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __INTERNAL_TOOLS_H__
#define __INTERNAL_TOOLS_H__
#pragma once

#include <wx/string.h>

// ///////////////
// Maybe one day I should include these into an extra library or so
//
// lkTrim.. created while MFC's CString has them, but wxWidget's wxString don't
//

wxString lkTrimRight(const wxString& str, const wxString& trim);
wxString lkTrimLeft(const wxString& str, const wxString& trim);
wxString lkTrim(const wxString& str, const wxString& trim);

wxString lkMakeAnsi(const wxString& src);

#endif // !__INTERNAL_TOOLS_H__
