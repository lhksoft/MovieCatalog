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
#include "QGroup.h"

// static definitions for setting the fields of new table
static sqlTblField_t q3GroupFlds[] = {
	{ q3Group_ID,             sqlFieldType::sqlInt,  true, 0 }, // ROWID
	{ q3Group_Group_BASE,     sqlFieldType::sqlInt,  true, 0 },
	{ q3Group_Storage_MEDIUM, sqlFieldType::sqlInt,  true, 0 },
	{ q3Group_Storage_NAME,  sqlFieldType::sqlText,  true, 0 },
	{ q3Group_Group_PART,	 sqlFieldType::sqlText, false, 0 },
	{ q3Group_Group_TIME,	 sqlFieldType::sqlReal, false, 0 },
	{ q3Group_Location_NAME, sqlFieldType::sqlText,  true, 0 },
	{ q3Group_Storage_INFO,   sqlFieldType::sqlInt,  true, 0 },
	{ 0 }
};

// static definitions
static sqlTblDef_t q3GroupQry = { q3Group, q3GroupFlds, NULL };


///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(QGroup, lkSQL3QuerySet)

QGroup::QGroup() : lkSQL3QuerySet()
{
	Init();
}
QGroup::QGroup(lkSQL3Database* pDB) : lkSQL3QuerySet(pDB)
{
	Init();
}

void QGroup::Init()
{
	m_pFldID = NULL;
	m_pFldBase = NULL;
	m_pFldMedium = NULL;
	m_pFldStorage = NULL;
	m_pFldPart = NULL;
	m_pFldTime = NULL;
	m_pFldLocation = NULL;
	m_pFldInfo = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////

//virtual
bool QGroup::Open()
{
	static const wxString GroupQueryStr = wxT("SELECT [tblGroup].[ROWID], [tblGroup].[BaseID] AS Base, [tblStorage].[MediumID] AS Medium, [tblStorage].[Storage], tblGroup.Part, " \
											  "[tblGroup].[PlayTime], [tblLocation].[Location], [tblStorage].[Info] " \
											  "FROM ([tblGroup] LEFT JOIN [tblStorage] ON [tblGroup].[StorageID] = [tblStorage].[ROWID]) " \
											  "LEFT JOIN [tblLocation] ON [tblStorage].[LocationID] = [tblLocation].[ROWID]");

	if ( IsOpen() ) return true;

	wxString order;
	order.Printf(wxT("[%s]"), q3Group_Storage_NAME);
	SetOrder(order);

	lkInitTable msInit(&q3GroupQry);
	// fill Fields
	msInit.Append(new lkInitField(q3Group_ID, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldID));
	msInit.Append(new lkInitField(q3Group_Group_BASE, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldBase));
	msInit.Append(new lkInitField(q3Group_Storage_MEDIUM, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldMedium));
	msInit.Append(new lkInitField(q3Group_Storage_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldStorage));
	msInit.Append(new lkInitField(q3Group_Group_PART, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldPart));
	msInit.Append(new lkInitField(q3Group_Group_TIME, wxCLASSINFO(lkSQL3FieldData_Real), (lkSQL3FieldData**)&m_pFldTime));
	msInit.Append(new lkInitField(q3Group_Location_NAME, wxCLASSINFO(lkSQL3FieldData_Text), (lkSQL3FieldData**)&m_pFldLocation));
	msInit.Append(new lkInitField(q3Group_Storage_INFO, wxCLASSINFO(lkSQL3FieldData_Int), (lkSQL3FieldData**)&m_pFldInfo));

	bool bRet = lkSQL3QuerySet::Open(&msInit, GroupQueryStr);

	if ( !bRet )
		Throw_InvalidTable(q3Group); // Properly an invalid query

	return bRet;
}

void QGroup::SetBaseFilter(wxUint64 _base)
{
	wxString s;
	s.Printf(wxT("([%s] = %I64u)"), q3Group_Group_BASE, _base);
	SetFilter(s);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
////

wxUint64 QGroup::GetIDValue() const
{
	return m_pFldID->GetValue2();
}
wxUint64 QGroup::GetBaseValue() const
{
	return m_pFldBase->GetValue2();
}
wxUint64 QGroup::GetMediumValue() const
{
	return m_pFldMedium->GetValue2();
}
wxString QGroup::GetStorageValue() const
{
	return m_pFldStorage->GetValue2();
}
wxString QGroup::GetPartValue() const
{
	return m_pFldPart->GetValue2();
}
double QGroup::GetTimeValue() const
{
	return m_pFldTime->GetValue2();
}
wxString QGroup::GetLocationValue() const
{
	return m_pFldLocation->GetValue2();
}
wxUint32 QGroup::GetInfoValue() const
{
	return m_pFldInfo->GetValue2();
}

