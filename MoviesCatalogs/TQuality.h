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
#ifndef __T_QUALITY_H__
#define __T_QUALITY_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Quality			wxT("tblQuality")
// Required Field-definitions
#define t3Quality_NAME		wxT("Lable")
#define t3Quality_IMG		wxT("Image")


class TQuality : public lkSQL3TableSet
{
public:
	TQuality();
	TQuality(lkSQL3Database* pDB);


private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldQuality;
	lkSQL3FieldData_Image*		m_pFldImage;

protected:
	wxString					m_sImageName;

public:
	wxString					GetQualityValue(void) const;
	void						SetQualityValue(const wxString&); // NULL not allowed

	wxImage						GetImage(void) const;
	void						SetImage(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetImageName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueQuality(lkSQL3Database* _DB, const wxString& orig, void*); // TRUE if given location-name doesn't exist in table

	static void					SetDefaultQuality(wxUint64);
	static wxUint64				GetDefaultQuality(lkSQL3Database*); // if valid pDB, a check will be done whether the default exists as a valid rowid

	static void					CorrectConfig(lkSQL3Database* dbTmp);
	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TQuality);
};

#endif // !__T_QUALITY_H__
