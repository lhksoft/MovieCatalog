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
#ifndef __Q_GROUP_H__
#define __Q_GROUP_H__
#pragma once

#include "../lkSQLite3/lkSQL3QuerySet.h"

//////////////////////////////////////////////////////////////////////////////
// QGroup
//////////////////////////////////////////////////////////////////////////////
// name of Table
#define q3Group					wxT("QEURY")		// Qeury-definition
// Required Field-definitions
#define q3Group_ID				wxT("rowid")
#define q3Group_Group_BASE		wxT("Base")		// t3Group_BASE
#define q3Group_Storage_MEDIUM	wxT("Medium")	// t3Storage_MEDIUM
#define q3Group_Storage_NAME	wxT("Storage")	// t3Storage_NAME
#define q3Group_Group_PART		wxT("Part")		// t3Group_PART
#define q3Group_Group_TIME		wxT("PlayTime")	// t3Group_TIME
#define q3Group_Location_NAME	wxT("Location")	// t3Location_NAME
#define q3Group_Storage_INFO	wxT("Info")		// t3Storage_INFO

class QGroup : public lkSQL3QuerySet
{
public:
	QGroup();
	QGroup(lkSQL3Database* pDB);

private:
	void								Init(void);

// Operations
public:
	virtual bool						Open(void) wxOVERRIDE;

	void								SetBaseFilter(wxUint64);

// Attributes
public:
	lkSQL3FieldData_Int*				m_pFldID; // UINT64
	lkSQL3FieldData_Int*				m_pFldBase; // UINT64
	lkSQL3FieldData_Int*				m_pFldMedium; // UINT64
	lkSQL3FieldData_Text*				m_pFldStorage;
	lkSQL3FieldData_Text*				m_pFldPart;
	lkSQL3FieldData_Real*				m_pFldTime;
	lkSQL3FieldData_Text*				m_pFldLocation;
	lkSQL3FieldData_Int*				m_pFldInfo;

public:
	wxUint64							GetIDValue(void) const;
	wxUint64							GetBaseValue(void) const;
	wxUint64							GetMediumValue(void) const;
	wxString							GetStorageValue(void) const;
	wxString							GetPartValue(void) const;
	double								GetTimeValue(void) const;
	wxString							GetLocationValue(void) const;
	wxUint32							GetInfoValue(void) const;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_COPY(QGroup);
};

#endif // !__Q_GROUP_H__
