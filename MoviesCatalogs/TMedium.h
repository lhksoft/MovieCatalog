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
#ifndef __T_MEDIUM_H__
#define __T_MEDIUM_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Medium			wxT("tblMedium")
// Required Field-definitions
#define t3Medium_CODE		wxT("Code")			// (vereist) korte Naam
#define t3Medium_NAME		wxT("Name")			// (optioneel) lange Naam
#define t3Medium_IMG		wxT("Image")		// (optioneel) Plaatje
#define t3Medium_LIKES		wxT("Likes")		// (optional) dbText -- used in StorageForm = Leading character
#define t3Medium_NOTLIKES	wxT("NotLikes")		// (optional) dbText -- used in StorageForm = Leading character(s) to exclude

class TMedium : public lkSQL3TableSet
{
public:
	TMedium();
	TMedium(lkSQL3Database* pDB);


private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldCode;
	lkSQL3FieldData_Text*		m_pFldName;
	lkSQL3FieldData_Image*		m_pFldImage;
	lkSQL3FieldData_Text*		m_pFldLikes;
	lkSQL3FieldData_Text*		m_pFldNotLikes;

protected:
	wxString					m_sImageName;

public:
	// Short Name
	wxString					GetCodeValue(void) const;
	void						SetCodeValue(const wxString&); // NULL not allowed

	// Long Name
	wxString					GetNameValue(void) const;
	void						SetNameValue(const wxString&);

	wxImage						GetImage(void) const;
	void						SetImage(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetImageName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

	wxString					GetStorageLikes(void) const;
	void						SetStorageLikes(const wxString&);

	wxString					GetStorageNotLikes(void) const;
	void						SetStorageNotLikes(const wxString&);

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueCode(lkSQL3Database* _DB, const wxString& code, void*); // TRUE if given location-name doesn't exist in table

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TMedium);
};

#endif // !__T_MEDIUM_H__
