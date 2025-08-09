/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2025 by Laurens Koehoorn (lhksoft)
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
#ifndef __T_MOVIES_PRIVATE_H__
#define __T_MOVIES_PRIVATE_H__
#pragma once

#include <wx/string.h>

 // corrects pathseperators
wxString tmCorrectPath(const wxString&);

// Makes a full path using given Prefix and Partial
wxString tmMakeFullPath(const wxString& sPrefix, const wxString& partial);

// Extracts 'eWhat' from given full-path
wxString tmExtractFirstPath(const wxString& full, const wxString& eWhat);

// Returns the refferenced char that matches the range of given path
wxUniChar tmFirstPathInRange(const wxString& range, const wxString& path);

// Prefixes given filename with [first char of filename in uppercase]/filename
wxString tmPrefixCover(const wxString& cFilename);

// true if both files are the same -- better test for existance of both files before calling this func
bool tmCompareFiles(const wxString& sSrc, const wxString& sDst);

// Copies src into dst (full paths with filename+ext!), also copying filetimes (like robocopy)
bool tmCopyFile(const wxString& sSrc, const wxString& sDst);

// makes prefix for log in Threads
wxString tmMakeLog(const wxUint64 rowID, const wxString& movieTitle);

// reset dirs-timevalues with any copied files
void tmResetDirTimes(const wxString& sNewPath);

// Will create the new destination localy, 1-timer, and we'll don't need it when it's finally done
wxString tmMoveFirst(const wxString& sOldPath, const wxString& sOldPrefix);

// does do a minor Resort of Old- to New-Coverspath
wxString tmCoverResort(const wxString& sOldPath, const wxString& sOldPrefix);

#endif // !__T_MOVIES_PRIVATE_H__

