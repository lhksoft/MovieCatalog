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
#include "QStorage.h"

#include "TStorage.h"
#include "TMedium.h"
#include "TLocation.h"

// static definitions for setting the fields of new table
static sqlTblField_t q3StorageFlds[] = {
	{ q3Storage_ID,                sqlFieldType::sqlInt,  true, 0 }, // ROWID
	{ q3Storage_STORAGE_NAME,     sqlFieldType::sqlText,  true, 0 },
	{ q3Storage_LOCATION_NAME,    sqlFieldType::sqlText,  true, 0 },
	{ q3Storage_MEDIUM_IMG,       sqlFieldType::sqlBlob, false, 0 }, // containing image of Medium
	{ q3Storage_STORAGE_CREATION, sqlFieldType::sqlReal, false, 0 },
	{ q3Storage_STORAGE_INFO,      sqlFieldType::sqlInt,  true, 0 },
	{ 0 }
};

// static definitions
static sqlTblDef_t q3StorageQry = { q3Storage, q3StorageFlds, NULL };


///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(QStorage, lkSQL3QuerySet)

QStorage::QStorage() : lkSQL3QuerySet(), m_sImageName()
{
	Init();
}
QStorage::QStorage(lkSQL3Database* pDB) : lkSQL3QuerySet(pDB), m_sImageName()
{
	Init();
}

void QStorage::Init()
{
	m_pFldID = NULL;
	m_pFldStorage = NULL;
	m_pFldLocation = NULL;
	m_pFldImage = NULL;
	m_pFldCreation = NULL;
	m_pFldInfo = NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
bool QStorage::Open()
{
	static const wxString GroupStorageBaseStr = wxT("SELECT [%s].[ROWID], [%s].[%s], [%s].[%s], [%s].[%s], [%s].[%s], [%s].[%s] " \
													"FROM ([%s] LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID]) LEFT JOIN [%s] ON [%s].[%s] = [%s].[ROWID]");
	wxString GroupStorageQuery;
	GroupStorageQuery.Printf(GroupStorageBaseStr, 
							 t3Storage, t3Storage, t3Storage_NAME, t3Location, t3Location_NAME, t3Medium, t3Medium_IMG, t3Storage, t3Storage_CREATION, t3Storage, t3Storage_INFO,
							 t3Storage, t3Location, t3Storage, t3Storage_LOCATION, t3Location, t3Medium, t3Storage, t3Storage_MEDIUM, t3Medium);

	lkInitTable msInit(&q3StorageQry);
	// fill Fields
	msInit.Append(new lkInitField(q3Storage_ID, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldID));
	msInit.Append(new lkInitField(q3Storage_STORAGE_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldStorage));
	msInit.Append(new lkInitField(q3Storage_LOCATION_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldLocation));
	msInit.Append(new lkInitField(q3Storage_MEDIUM_IMG, wxCLASSINFO(lkSQL3FieldData_Image), (lkSQL3FieldData**)&m_pFldImage));
	msInit.Append(new lkInitField(q3Storage_STORAGE_CREATION, wxCLASSINFO(lkSQL3FieldData_Real), (lkSQL3FieldData**)&m_pFldCreation));
	msInit.Append(new lkInitField(q3Storage_STORAGE_INFO, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldInfo));
	bool bRet = lkSQL3QuerySet::Open(&msInit, GroupStorageQuery);

	if ( !bRet )
		Throw_InvalidTable(q3Storage); // Properly an invalid query

	return bRet;
}

void QStorage::SetStorageFilter(wxUint64 _storage)
{
	wxString s;
	s.Printf(wxT("([ROWID] = %I64u)"), _storage);
	SetFilter(s);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
////

wxUint64 QStorage::GetIDValue() const
{
	return m_pFldID->GetValue2();
}
wxString QStorage::GetStorageValue() const
{
	return m_pFldStorage->GetValue2();
}
wxString QStorage::GetLocationValue() const
{
	return m_pFldLocation->GetValue2();
}
wxImage QStorage::GetImageValue() const
{
	wxString _name;
	wxImage img = m_pFldImage->GetAsImage(_name);
	*((wxString*)&m_sImageName) = (img.IsOk()) ? _name : wxT("");

	return img;
}
double QStorage::GetCreationValue() const
{
	return m_pFldCreation->GetValue2();
}
wxUint32 QStorage::GetInfoValue() const
{
	return m_pFldInfo->GetValue2();
}

