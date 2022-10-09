/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3IndexDef.h"
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(lkIdxFieldDefList);

WX_DEFINE_LIST(lkSQL3IndexDefList);

// //////////////////////////////////////////////////////////////////////////////////////////////



// ////////////////////////////////////////////////////////////
// lkIdxFieldDef

wxIMPLEMENT_DYNAMIC_CLASS(lkIdxFieldDef, wxObject);

lkIdxFieldDef::lkIdxFieldDef() : wxObject(), m_fName()
{
	m_Order = sqlSortOrder::sortNone;
}
lkIdxFieldDef::lkIdxFieldDef(const wxString& _name, sqlSortOrder _order) : wxObject(), m_fName(_name)
{
	m_Order = _order;
}
lkIdxFieldDef::lkIdxFieldDef(const lkIdxFieldDef& _def) : wxObject(), m_fName(_def.m_fName)
{
	m_Order = _def.m_Order;
}
lkIdxFieldDef::lkIdxFieldDef(const sqlFldIndex_t& _def) : wxObject(), m_fName(_def.fName)
{
	m_Order = _def.nOrder;
}

wxString lkIdxFieldDef::GetDefinition() const
{
	wxString def;
	def.Printf(wxT("[%s]"), m_fName);

	switch ( m_Order )
	{
		case sqlSortOrder::sortAsc:
		{
			def += wxT(" ASC");
			break;
		}
		case sqlSortOrder::sortDesc:
		{
			def += wxT(" DESC");
			break;
		}
		default:
			break;
	}

	return def;
}


// ////////////////////////////////////////////////////////////
// lkSQL3IndexDef

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3IndexDef, wxObject);

lkSQL3IndexDef::lkSQL3IndexDef() : wxObject(), m_Name(), m_Indexes()
{
	m_bUnique = false;
	m_Indexes.DeleteContents(true);
}
lkSQL3IndexDef::lkSQL3IndexDef(const lkSQL3IndexDef& _d) : wxObject(), m_Name(_d.m_Name), m_Indexes()
{
	m_Indexes.DeleteContents(true);
	m_bUnique = _d.m_bUnique;

	if ( !_d.m_Indexes.IsEmpty() )
	{
		// let's iterate over the list in STL syntax
		lkIdxFieldDefList::const_iterator iter;
		for ( iter = _d.m_Indexes.begin(); iter != _d.m_Indexes.end(); iter++ )
		{
			//lkIdxFieldDef* current = *iter;
			m_Indexes.Append(new lkIdxFieldDef(*(*iter)));
		}
	}
}
lkSQL3IndexDef::lkSQL3IndexDef(const sqlTblIndex_t& _d) : wxObject(), m_Name(_d.sName), m_Indexes()
{
	m_Indexes.DeleteContents(true);
	m_bUnique = _d.bUnique;

	for ( int i = 0; _d.pIdxFields[i].fName != NULL; i++ )
		if ( _d.pIdxFields[i].fName[0] != 0 )
			m_Indexes.Append(new lkIdxFieldDef(_d.pIdxFields[i]));
}

lkSQL3IndexDef::~lkSQL3IndexDef()
{
}

wxString lkSQL3IndexDef::GetDefinition(const wxString& _table) const
{
	wxString def;
	if ( _table.IsEmpty() || m_Indexes.IsEmpty() || m_Name.IsEmpty() )
		return def;

	wxString flds;
	lkIdxFieldDefList::const_iterator iter;
	for ( iter = m_Indexes.begin(); iter != m_Indexes.end(); iter++ )
	{
		const lkIdxFieldDef* current = *iter;

		if ( !flds.IsEmpty() )
			flds += wxT(", ");
		flds += current->GetDefinition();
	}
	if ( flds.IsEmpty() )
		return def;

	def.Printf(wxT("CREATE %sINDEX IF NOT EXISTS [%s] ON [%s] (%s);"), (m_bUnique) ? wxT("UNIQUE ") : wxT(""), m_Name, _table, flds);
	return def;
}
