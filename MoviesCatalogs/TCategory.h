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
#ifndef __T_CATEGORY_H__
#define __T_CATEGORY_H__
#pragma once

//#include <wx/image.h>
#include "../lkSQLite3/lkSQL3TableSet.h"
#include "lkSQL3Image.h"
#include <wx/thread.h>

// name of Table
#define t3Category			wxT("tblCategory")
// Required Field-definitions
#define t3Category_catID	wxT("catID")
#define t3Category_subID	wxT("subID")
#define t3Category_NAME		wxT("Name")
#define t3Category_IMG		wxT("Image")			// LongBinary
//#define t3Category_COUNT	wxT("MoviesCount")	// amount of linked movies
//#define t3Category_SORT		wxT("Sort")			// (if subID==0) : if TRUE sorts the subCat's by Name, if False sorts the subCat's by ID -- default '0'
// (only when filtering) if subcat-SORT==-1, then sorts the Movies by Episode, if ==0 then sorts the Movies by Title


class TCategory : public lkSQL3TableSet
{
public:
	TCategory();
	TCategory(lkSQL3Database* pDB);

private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Int*		m_pFldCatID;
	lkSQL3FieldData_Int*		m_pFldSubID;
	lkSQL3FieldData_Text*		m_pFldName;
//	lkSQL3FieldData_Int*		m_pFldSort;
//	lkSQL3FieldData_Int*		m_pFldCount; // UINT
	lkSQL3FieldData_Image*		m_pFldImage; // ours, it sets some sort of marking in the db, to identify this is our wxImage

	static sqlTblDef_t			t3CategoryTbl;

protected:
	wxString					m_sImageName;

public:
	wxUint32					GetCatValue(void) const;
	void						SetCatValue(wxUint32);

	wxUint32					GetSubValue(void) const;
	void						SetSubValue(wxUint32);

	wxString					GetNameValue(void);
	void						SetNameValue(const wxString&);

//	bool						GetSortValue(void) const;
//	void						SetSortValue(bool);

//	wxUint32					GetCountValue(void) const;
//	void						SetCountValue(wxUint32);

	wxImage						GetImage(void) const;
	void						SetImage(const wxImage&, const wxString& _name = wxEmptyString); // if invalid image, sets the image to NULL
	wxString					GetImageName(void) const; // returns the name (if any) retrieved from last GetImage() / SetImage(..)

// Implementation
public:
	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static wxUint32				GetFreeCatID(lkSQL3Database*); // when adding, get a free Cattegory ID
	static wxUint32				GetFreeSubID(lkSQL3Database*, wxUint32 nCat); // when adding get a free Sub ID

	static bool					IsUniqueName(lkSQL3Database* _DB, const wxString& name, void* catID); // this one used when validating -- catId of type wxUint32*
	static bool					HasImages(lkSQL3Database* _DB, wxUint32 catID, wxUint32 subID = 0);

//	static void					CorrectRegistry(lkSQL3Database* dbTmp);

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread

private:
	wxDECLARE_DYNAMIC_CLASS(TCategory);
	wxDECLARE_NO_COPY_CLASS(TCategory);
};

#endif // !__T_CATEGORY_H__
