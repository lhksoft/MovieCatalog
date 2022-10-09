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
#ifndef __Q_STORAGE_H__
#define __Q_STORAGE_H__
#pragma once

#include "../lkSQLite3/lkSQL3QuerySet.h"
#include "lkSQL3Image.h"

//////////////////////////////////////////////////////////////////////////////
// QStorage
//////////////////////////////////////////////////////////////////////////////
// name of Table
#define q3Storage					wxT("QEURY")	// Qeury-definition
// Required Field-definitions
#define q3Storage_ID				wxT("rowid")
#define q3Storage_STORAGE_NAME		wxT("Storage")	// t3Storage_NAME
#define q3Storage_LOCATION_NAME		wxT("Location")	// t3Location_Name
#define q3Storage_MEDIUM_IMG		wxT("Image")	// t3Medium_IMG
#define q3Storage_STORAGE_CREATION	wxT("Creation")	// t3Storage_CREATION
#define q3Storage_STORAGE_INFO		wxT("Info")		// t3Storage_INFO

class QStorage : public lkSQL3QuerySet
{
public:
	QStorage();
	QStorage(lkSQL3Database* pDB);

private:
	void								Init(void);

// Operations
public:
	virtual bool						Open(void) wxOVERRIDE;

	void								SetStorageFilter(wxUint64);

// Attributes
public:
	lkSQL3FieldData_Int*				m_pFldID;
	lkSQL3FieldData_Text*				m_pFldStorage;
	lkSQL3FieldData_Text*				m_pFldLocation;
	lkSQL3FieldData_Image*				m_pFldImage;
	lkSQL3FieldData_Real*				m_pFldCreation;
	lkSQL3FieldData_Int*				m_pFldInfo;

protected:
	wxString							m_sImageName;

public:
	wxUint64							GetIDValue(void) const;
	wxString							GetStorageValue(void) const;
	wxString							GetLocationValue(void) const;
	wxImage								GetImageValue(void) const;
	double								GetCreationValue(void) const;
	wxUint32							GetInfoValue(void) const;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(QStorage);
};

#endif // !__Q_STORAGE_H__
