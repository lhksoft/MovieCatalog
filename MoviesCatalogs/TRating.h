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
#ifndef __T_RATING_H__
#define __T_RATING_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Rating			wxT("tblRating")
// Required Field-definitions
#define t3Rating_ID			wxT("Flag")
#define t3Rating_NAME		wxT("Rating")
#define t3Rating_DESCR		wxT("Description")
#define t3Rating_IMAGE		wxT("Image")			// LongBinary

class TRating : public lkSQL3TableSet
{
public:
	TRating();
	TRating(lkSQL3Database* pDB);


private:
	void						Init(void);

	// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;
	virtual bool				CanAdd(void) wxOVERRIDE;

	// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*		m_pFldFlag;
	lkSQL3FieldData_Text*		m_pFldRating;
	lkSQL3FieldData_Text*		m_pFldDescription;
	lkSQL3FieldData_Image*		m_pFldImage;

protected:
	wxString					m_sImageName;

public:
	wxUint8						GetFlagValue(void) const; // cast __int64 to in
	void						SetFlagValue(wxUint8); // null would be allowed, in which case default will be set (0)

	wxString					GetRatingValue(void) const;
	void						SetRatingValue(const wxString&); // NULL not allowed

	wxString					GetDescriptValue(void) const;
	void						SetDescriptValue(const wxString&); // NULL not allowed

	wxImage						GetImage(void) const;
	void						SetImage(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetImageName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueFlag(lkSQL3Database*, const wxString&, void*);
	static bool					IsUniqueRating(lkSQL3Database*, const wxString&, void*);

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TRating);
};

#endif // !__T_RATING_H__
