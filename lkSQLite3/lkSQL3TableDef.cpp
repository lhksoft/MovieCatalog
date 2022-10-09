/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3TableDef.h"
#include "lkSQL3Exception.h"
#include <wx/debug.h> 

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3TableDef, wxObject);

lkSQL3TableDef::lkSQL3TableDef() : wxObject(), m_Fields(), m_Indexes(), m_Name()
{
	m_Fields.DeleteContents(true);
	m_Indexes.DeleteContents(true);
}
lkSQL3TableDef::lkSQL3TableDef(const lkSQL3TableDef& _d) : wxObject(), m_Fields(), m_Indexes(), m_Name(_d.m_Name)
{
	m_Fields.DeleteContents(true);
	m_Indexes.DeleteContents(true);

	if ( !_d.m_Fields.IsEmpty() )
	{
		lkSQL3FieldDefList::const_iterator Iter;
		for ( Iter = _d.m_Fields.begin(); Iter != _d.m_Fields.end(); Iter++ )
			m_Fields.Append(new lkSQL3FieldDef(**Iter));
	}

	if ( !_d.m_Indexes.IsEmpty() )
	{
		lkSQL3IndexDefList::const_iterator Iter;
		for ( Iter = _d.m_Indexes.begin(); Iter != _d.m_Indexes.end(); Iter++ )
			m_Indexes.Append(new lkSQL3IndexDef(**Iter));
	}

	wxASSERT_MSG(!m_Name.IsEmpty() && !m_Fields.IsEmpty(), wxT("m_Name and m_Fields-list shouldn't be empty, in lkSQL3TableDef C'tor."));
}
lkSQL3TableDef::lkSQL3TableDef(const sqlTblDef_t& _d) : wxObject(), m_Fields(), m_Indexes(), m_Name(_d.sName)
{
	m_Fields.DeleteContents(true);
	m_Indexes.DeleteContents(true);

	int i;
	// add Fields
	if (_d.pFields != NULL)
		for ( i = 0; _d.pFields[i].sName && _d.pFields[i].sName[0]; i++ )
			m_Fields.Append(new lkSQL3FieldDef(_d.pFields[i]));

	// add Indexes
	if ( _d.pIndex != NULL )
		for ( i = 0; _d.pIndex[i].sName && _d.pIndex[i].sName[0]; i++ )
			m_Indexes.Append(new lkSQL3IndexDef(_d.pIndex[i]));

	wxASSERT_MSG(!m_Name.IsEmpty() && !m_Fields.IsEmpty(), wxT("m_Name and m_Fields-list shouldn't be empty, in lkSQL3TableDef C'tor."));
}

lkSQL3TableDef::~lkSQL3TableDef()
{
}

wxString lkSQL3TableDef::GetName() const
{
	return m_Name;
}

void lkSQL3TableDef::SetName(const wxString& _name)
{
	m_Name = _name;
}

const lkSQL3FieldDefList* lkSQL3TableDef::GetFields(void) const
{
	return &m_Fields;
}

wxString lkSQL3TableDef::GetTableDefinition(bool _Temp) const
{
	wxString def;
	if ( m_Name.IsEmpty() || m_Fields.IsEmpty() )
		return def;

	wxString flds;
	lkSQL3FieldDefList::const_iterator Iter;
	for ( Iter = m_Fields.begin(); Iter != m_Fields.end(); Iter++ )
	{
		if ( !flds.IsEmpty() )
			flds += wxT(", ");
		flds += (*Iter)->GetDefinition();
	}

	def.Printf(wxT("CREATE %sTABLE [%s] (%s);"), (_Temp) ? wxT("TEMP ") : wxT(""), m_Name, flds);
	return def;
}

// Operations
bool lkSQL3TableDef::CreateTable(lkSQL3Database& _db, bool _Temp/* = false*/) // Creates the table and Indexes -- false on error or empty table definition
{
	bool bRet = false;
	if ( (!_db.IsOpen()) || (m_Name.IsEmpty()) || (m_Fields.IsEmpty()) )
		return bRet; // nothing to do

	if ( _db.TableExists(m_Name) )
		return false;

	// Create the Table
	try
	{
		wxString s;

		_db.ExecuteSQL(wxT("BEGIN"));

		s = GetTableDefinition(_Temp);
		_db.ExecuteSQL(s);

		// are there any Indexes ?
		if ( !m_Indexes.IsEmpty() )
		{
			lkSQL3IndexDefList::iterator Iter;
			for ( Iter = m_Indexes.begin(); Iter != m_Indexes.end(); Iter++ )
			{
				if ( !(s = (*Iter)->GetDefinition(m_Name)).IsEmpty() )
					_db.ExecuteSQL(s);
			}
		}

		_db.ExecuteSQL(wxT("COMMIT"));
		bRet = true;
	}
	catch ( const lkSQL3Exception& )
	{
		_db.ExecuteSQL(wxT("ROLLBACK"));
		throw; // send the exception to the caller
	}

	return bRet;
}

