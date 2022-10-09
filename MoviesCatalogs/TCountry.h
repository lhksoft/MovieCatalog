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
#ifndef __T_COUNTRY_H__
#define __T_COUNTRY_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Country			wxT("tblCountry")
// Required Field-definitions
#define t3Country_ISO		wxT("ISO")			// (vereist) min/max 2chars
//#define t3Country_ISO636	_T("ISO636")		// ISO-639 code, used as DVD-Language -- see 
#define t3Country_CODE		wxT("Code")			// can be NULL, unique if got a value
#define t3Country_DEF		wxT("Country")		// (vereist) unique
#define t3Country_CUSTOM	wxT("Custom")		// can be NULL, unique if got a value
#define t3Country_AUDIOSUBS	wxT("AudioSubs")		// either '1' when used as Audio in Base, or '2' if used as Subs in Base, or '3' when used both or '0' if not (default)
#define t3Country_SPOKEN	wxT("Speaking")		// Spoken Name, like 'dutch' / 'french' / 'english' / etc..
#define t3Country_FLAG		wxT("Flag")			// LongBinary
#define t3Country_SFLAG		wxT("SpeakingFlag")	// LongBinary

class TCountry : public lkSQL3TableSet
{
public:
	TCountry();
	TCountry(lkSQL3Database* pDB);

private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldIsoDefault;
//	lkSQL3FieldData_Text*		m_pFldIso636;
	lkSQL3FieldData_Text*		m_pFldIsoCustom;
	lkSQL3FieldData_Text*		m_pFldCountryDefault;
	lkSQL3FieldData_Text*		m_pFldCountryCustom;
	lkSQL3FieldData_Int*		m_pFldAudioSubs; // UINT8
	lkSQL3FieldData_Text*		m_pFldSpoken;
	lkSQL3FieldData_Image*		m_pFldFlag; // ours, it sets some sort of marking in the db, to identify this is our wxImage
	lkSQL3FieldData_Image*		m_pFldSpokenFlag; // ours, it sets some sort of marking in the db, to identify this is our wxImage

	static sqlTblDef_t			t3CountryTbl;

protected:
	wxString					m_sFlagName, m_sSpokenFlagName;

public:
	wxString					GetIsoValue(void) const;
	void						SetIsoValue(const wxString&); // NULL not allowed

//	wxString					GetIso636Value(void) const;
//	void						SetIso636Value(const wxString&); // NULL allowed

	wxString					GetCustomIsoValue(void) const;
	void						SetCustomIsoValue(const wxString&); // if entering "", sets the value to IsNull

	wxString					GetDefaultCountryValue(void) const;
	void						SetDefaultCountryValue(const wxString&); // NULL not allowed

	wxString					GetCustomCountryValue(void) const;
	void						SetCustomCountryValue(const wxString&); // if entering "", sets te value to IsNull

	wxUint8						GetAudioSubsValue(void) const;
	void						SetAudioSubsValue(wxUint8);

	wxString					GetSpokenValue(void) const;
	void						SetSpokenValue(const wxString&); // if entering "", sets te value to IsNull

	wxImage						GetFlag(void) const;
	void						SetFlag(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetFlagName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

	wxImage						GetSpokenFlag(void) const;
	void						SetSpokenFlag(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetSpokenFlagName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

// Implementation
public:
	virtual void				CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection) wxOVERRIDE;

	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueIso(lkSQL3Database* _DB, const wxString& iso, void*); // TRUE if given iso doesn't exist in table
//	static bool					IsUniqueIso636(lkSQL3Database* _DB, const wxString& iso636, void*); // TRUE if given iso doesn't exist in table
	static bool					IsUniqueCode(lkSQL3Database* _DB, const wxString& code, void*); // TRUE if given code doesn't exist in table
	static bool					IsUniqueCountry(lkSQL3Database* _DB, const wxString& country, void*); // TRUE if given country doesn't exist in table
	static bool					IsUniqueCustom(lkSQL3Database* _DB, const wxString& custom, void*); // TRUE if given custom doesn't exist in table

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS(TCountry);
	wxDECLARE_NO_COPY_CLASS(TCountry);
};

#endif // !__T_COUNTRY_H__
