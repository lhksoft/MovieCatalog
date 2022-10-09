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
#ifndef __T_BASE_H__
#define __T_BASE_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include <wx/thread.h>

//////////////////////////////////////////////////////////////////////////////
// TBase
//////////////////////////////////////////////////////////////////////////////
// name of Table
#define t3Base				wxT("tblBase")
// Required Field-definitions
#define t3Base_MOVIE		wxT("MoviesID")		// _rowid_ van tblMovies
#define t3Base_QUALITY		wxT("QualityID")	// _rowid_ van tblQuality
#define t3Base_ORIGINE		wxT("OrigineID")	// _rowid_ van tblOrigine
#define t3Base_SCREEN		wxT("ScreenID")		// _rowid_ van tblScreen
#define t3Base_AUDIO		wxT("Audio")
#define t3Base_SUBS			wxT("Subtitles")

class TBase : public lkSQL3TableSet
{
public:
	TBase();
	TBase(lkSQL3Database* pDB);

private:
	void								Init(void);

protected:
	virtual wxString					GetQuery(bool _rowid = false, bool _filter = true, bool _order = true) wxOVERRIDE;

// Filter Attributes
	wxString							m_sFltFrom;

public:
	void								SetFltFrom(const wxString&);

// Operations
public:
	virtual bool						Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool						CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*				m_pFldMovies;
	lkSQL3FieldData_Int*				m_pFldQuality;
	lkSQL3FieldData_Int*				m_pFldOrigine;
	lkSQL3FieldData_Int*				m_pFldScreen;
	lkSQL3FieldData_Text*				m_pFldAudio;
	lkSQL3FieldData_Text*				m_pFldSubs;

public:
	wxUint64							GetMoviesID(void) const;
	void								SetMoviesID(wxUint64);

	wxUint64							GetQualityID(void) const;
	void								SetQualityID(wxUint64);

	wxUint64							GetOrigineID(void) const;
	void								SetOrigineID(wxUint64);

	wxUint64 							GetScreenID(void) const;
	void								SetScreenID(wxUint64);

	wxString							GetAudioValue(void) const;
	void								SetAudioValue(const wxString&);

	wxString							GetSubsValue(void) const;
	void								SetSubsValue(const wxString&);

// Implementation
public:
	virtual wxUint64					FindInit(wxClassInfo* rc, const wxString& search_on, bool bForward) wxOVERRIDE;
	virtual void						CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection) wxOVERRIDE;
	virtual bool						TestFind(const wxString& search_on, bool forward);

	static bool							VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool							CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool							HasCountry(lkSQL3Database*, const wxString& iso, bool bAudio);
	static bool							HasAudio(lkSQL3Database*, const wxString& iso);
	static bool							HasSubtitle(lkSQL3Database*, const wxString& iso);

	static bool							HasMovie(lkSQL3Database*, wxUint64); // TRUE if at least 1 Movie match has been found
	static bool							HasQuality(lkSQL3Database*, wxUint64);
	static bool							HasOrigine(lkSQL3Database*, wxUint64);
	static bool							HasScreen(lkSQL3Database*, wxUint64);

	static bool							Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TBase);
};

#endif // !__T_BASE_H__
