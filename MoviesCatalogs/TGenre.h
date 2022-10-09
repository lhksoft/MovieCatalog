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
#ifndef __T_GENRE_H__
#define __T_GENRE_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Genre				wxT("tblGenres")
// Required Field-definitions
#define t3Genre_FLAG   		wxT("Flag")
#define t3Genre_GENRE		wxT("Genre")
#define t3Genre_IMG			wxT("Image")


class TGenre : public lkSQL3TableSet
{
public:
	TGenre();
	TGenre(lkSQL3Database* pDB);


private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;
	virtual bool				CanAdd(void) wxOVERRIDE;

protected:
	virtual bool				AddData(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*		m_pFldFlag; // wxUint64
	lkSQL3FieldData_Text*		m_pFldGenre;
	lkSQL3FieldData_Image*		m_pFldImage;

protected:
	wxString					m_sImageName;

public:
	wxUint64					GetFlagValue(void) const; // cast __int64 to UINT64
	void						SetFlagValue(wxUint64); // NULL not allowed

	wxString					GetGenreValue(void) const;
	void						SetGenreValue(const wxString&); // NULL not allowed

	wxImage						GetImage(void) const;
	void						SetImage(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetImageName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static wxUint64				FindFreeID64(lkSQL3Database*);
	static bool					IsUniqueGenre(lkSQL3Database* _DB, const wxString& genre, void*); // TRUE if given genre-name doesn't exist in table

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TGenre);
};


#endif // !__T_GENRE_H__
