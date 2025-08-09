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
#ifndef __T_STORAGE_H__
#define __T_STORAGE_H__
#pragma once

#include "../lkSQLite3/lkSQL3TableSet.h"
#include <wx/thread.h>

// name of Table
#define t3Storage				wxT("tblStorage")
// Required Field-definitions
#define t3Storage_NAME			wxT("Storage")
#define t3Storage_LOCATION		wxT("LocationID")
#define t3Storage_MEDIUM		wxT("MediumID")
#define t3Storage_CREATION		wxT("Creation")		// (optional) dbDbl
#define t3Storage_INFO			wxT("Info")			// (optional) dbInt
// add at : 2022/08/29
#define t3Storage_LINK			wxT("Linked")		// bool, 1:linked in TGroup, 0:NOT linked in TGroup -- will be set in TStorage::Compact

class TStorage : public lkSQL3TableSet
{
public:
	TStorage();
	TStorage(lkSQL3Database* pDB);

private:
	void						Init(void);

// Operations
public:
	virtual bool				Open(void) wxOVERRIDE; // should return false if incorrect table (aka, doesn't exist or 1 or more fields are missing)
	virtual bool				CanDelete(void) wxOVERRIDE;

// Attributes -- needs to be public for our Validators
public:
	lkSQL3FieldData_Text*		m_pFldStorage;
	lkSQL3FieldData_Int*		m_pFldLocation;
	lkSQL3FieldData_Int*		m_pFldMedium;
	lkSQL3FieldData_Real*		m_pFldCreation; // can be NULL
	lkSQL3FieldData_Int*		m_pFldInfo;
	lkSQL3FieldData_Int*		m_pFldLinked; // boolean -- NULL if this is an OLD table

protected:
	wxString					m_sFltFilterPrev, m_sFltOrderPrev;

	bool						TestNewTable(void); // tests whether this is a new table

public:
	wxString					GetStorageValue(void) const;
	void						SetStorageValue(const wxString&); // if empty string, sets it to NULL

	wxUint32					GetLocationValue(void) const;
	void						SetLocationValue(wxUint32);

	wxUint32					GetMediumValue(void) const;
	void						SetMediumValue(wxUint32);

	double						GetCreationValue(void) const;
	void						SetCreationValue(double);

	wxUint32					GetInfoValue(void) const;
	void						SetInfoValue(wxUint32);

	bool						GetLinkekValue(void) const;
	void						SetLinkedValue(bool);

// Implementation
public:
	virtual void				SetFilterActive(bool) wxOVERRIDE;

	virtual void				CreateFindFilter(wxString& Filter, const wxString& Field, const wxString& Search, lkSQL3RecordSet::FIND_DIRECTION findDirection) wxOVERRIDE;
	static wxString				CreateStorageFindFilter(const wxString& Search, FIND_DIRECTION findDirection); // used in tBase::CreateFindFilter

	static bool					VerifyTable(lkSQL3Database*); // verifys that the table exists, and the fields are present with given type
	static bool					CreateTable(lkSQL3Database*); // FALSE on error or already exists

	static bool					IsUniqueStorage(lkSQL3Database* _DB, const wxString& orig, void*);

	static bool					HasLocation(lkSQL3Database*, wxUint32); // TRUE if at least 1 Location match has been found
	static bool					HasMedium(lkSQL3Database*, wxUint32); // TRUE if at least 1 Medium match has been found

	static wxString				GetNextLabel(lkSQL3Database*, wxUint32 nMedium); // returns the next available Storage-Label available for given Medium -- '' on error

	static bool					TestFilter(lkSQL3Database*, const wxString& filter, const wxString& order = wxEmptyString);

	static bool					Compact(wxThread*, lkSQL3Database* dbSrc, lkSQL3Database* dbDest, lkSQL3Database* dbTmp); // wxThread required for sending messages to main-thread
	static bool					SetLinked(lkSQL3Database* dbDest, wxUint64 newStorageID); // called from inside TGroup::Compact; sets Linked-field to true if newStorageID is found

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(TStorage);
};

#endif // !__T_STORAGE_H__
