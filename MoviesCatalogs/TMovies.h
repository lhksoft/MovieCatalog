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
#ifndef __T_MOVIES_H__
#define __T_MOVIES_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "../lkSQLite3/lkDateTime.h"
#include <wx/thread.h>

//////////////////////////////////////////////////////////////////////////////
// TMovies
//////////////////////////////////////////////////////////////////////////////
// name of Table
#define t3Movies			wxT("tblMovies")
// Required Field-definitions
#define t3Movies_TITLE		wxT("Title")		// (req) - Txt - Main title
#define t3Movies_SUBTITLE	wxT("SubTitle")		// (opt) - Txt - Sub title
#define t3Movies_AKATITLE	wxT("Alias")		// (opt) - Txt - Aka title
#define t3Movies_EPISODE	wxT("Episode")		// (opt:0) - Int - Episode
#define t3Movies_CAT		wxT("Category")		// (opt:0) - Int - 
#define t3Movies_SUBCAT		wxT("SubCategory")	// (opt:0) - Int - 
#define t3Movies_GENRE		wxT("Genre")		// (opt:0) - UInt64 - ** FIX THIS -- should not be 0
#define t3Movies_JUDGE		wxT("Judgement")	// (opt:0) - Int - Quotatie ** FIX THIS -- should not be 0
#define t3Movies_RATE		wxT("Rating")		// (opt:0) - Int - ** FIX THIS -- should not be 0
#define t3Movies_COUNTRY	wxT("Country")		// (opt) - Txt - Country of Issue
#define t3Movies_DATE		wxT("Date")			// (opt) - Dbl - Date of Issue
#define t3Movies_YEAR		wxT("Year")			// (opt:1) - BOOL - 'Date' is Year only -- if '0' then 'Date' is DD-MM-YY
#define t3Movies_SERIE		wxT("Serie")		// (opt:0) - BOOL -
#define t3Movies_URL		wxT("URL")			// (opt) - Txt -
#define t3Movies_COVER		wxT("Cover")		// (opt) - Txt - Path naar Cover ** FIX THIS -- if in path of database, only partial path of subdirs in database-path (using INI)
#define t3Movies_INFO		wxT("Info")			// (opt) - Txt - Diverse

class TMovies : public lkSQL3TableSet
{
public:
	TMovies();
	TMovies(lkSQL3Database* pDB);

private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldTitle;
	lkSQL3FieldData_Text*		m_pFldSubTtl;
	lkSQL3FieldData_Text*		m_pFldAlias;
	lkSQL3FieldData_Int*		m_pFldEp;
	lkSQL3FieldData_Int*		m_pFldCat;
	lkSQL3FieldData_Int*		m_pFldSub;
	lkSQL3FieldData_Int*		m_pFldGenre;
	lkSQL3FieldData_Int*		m_pFldJudge;
	lkSQL3FieldData_Int*		m_pFldRating;
	lkSQL3FieldData_Text*		m_pFldCountry;
	lkSQL3FieldData_Real*		m_pFldDate;
	lkSQL3FieldData_Int*		m_pFldYear;
	lkSQL3FieldData_Int*		m_pFldSerie;
	lkSQL3FieldData_Text*		m_pFldURL;
	lkSQL3FieldData_Text*		m_pFldCover;
	lkSQL3FieldData_Text*		m_pFldInfo;

protected:
	wxString					m_sFltFilterPrev, m_sFltOrderPrev;

public:
	wxString					GetTitleValue(void) const;
	void						SetTitleValue(const wxString&); // cannot be NULL !

	wxString					GetSubTtlValue(void) const;
	void						SetSubTtlValue(const wxString&);

	wxString					GetAliasValue(void) const;
	void						SetAliasValue(const wxString&);

	wxUint32					GetEpisodeValue(void) const;
	void						SetEpisodeValue(wxUint32);

	wxUint32					GetCatValue(void) const;
	void						SetCatValue(wxUint32);

	wxUint32					GetSubValue(void) const;
	void						SetSubValue(wxUint32);

	wxUint64					GetGenreValue(void) const;
	void						SetGenreValue(wxUint64);

	wxUint32					GetJudgeValue(void) const; // ROWID in TJudge
	void						SetJudgeValue(wxUint32);

	wxUint8						GetRateValue(void) const; // FLAG in TRating
	void						SetRateValue(wxUint8);

	wxString					GetCountryValue(void) const; // empty if NULL
	void						SetCountryValue(const wxString&);

	lkDateTime					GetDateValue(void) const;
	void						SetDateValue(const lkDateTime&); // it actually is a double .. if 'lkInvalidDateTime' sets the field to NULL

	bool						GetYearValue(void) const;
	void						SetYearValue(bool);

	bool						GetSerieValue(void) const;
	void						SetSerieValue(bool);

	wxString					GetUrlValue(void) const; // empty if NULL
	void						SetUrlValue(const wxString&);

	wxString					GetCoverValue(void) const; // empty if NULL
	void						SetCoverValue(const wxString&);

	wxString					GetInfoValue(void) const; // empty if NULL
	void						SetInfoValue(const wxString&);


// Implementation
public:
	virtual void				SetFilterActive(bool) wxOVERRIDE;

	virtual void				CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection) wxOVERRIDE;
	static wxString				CreateMoviesFindFilter(const wxString& Search, FIND_DIRECTION findDirection); // used in tBase::CreateFindFilter

	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					TestFilter(lkSQL3Database*, const wxString& filter, const wxString& order);

	static bool					HasCountry(lkSQL3Database*, const wxString& iso, const wxString& code);
	static bool					HasGenre(lkSQL3Database*, wxUint64); // TRUE if at least 1 genre match has been found
	static bool					HasCat(lkSQL3Database*, wxUint32 nCat, wxUint32 nSub); // TRUE if at least 1 cat/sub has been found
	static bool					HasRating(lkSQL3Database*, wxUint8);
	static bool					HasJudge(lkSQL3Database*, wxUint32);

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TMovies);
};

#endif // !__T_MOVIES_H__
