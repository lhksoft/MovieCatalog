/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Verify.h"
#include <wx/listimpl.cpp>

WX_DEFINE_LIST(lkSQL3VerifyListBase);

// //////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3Verify, wxObject);

lkSQL3Verify::lkSQL3Verify() : wxObject(), m_sName(), m_sDefault()
{
	m_nType = sqlFieldType::sqlNone;
	m_nIdx = -1;
	m_bCanbeNull = true;
	m_bUsingDefault = false;
}

lkSQL3Verify::~lkSQL3Verify()
{
}

// --

lkSQL3VerifyList::lkSQL3VerifyList() : lkSQL3VerifyListBase()
{
	DeleteContents(true);
}
lkSQL3VerifyList::~lkSQL3VerifyList()
{
}

lkSQL3Verify* lkSQL3VerifyList::GetField(const wxString& fName)
{
	if ( fName.IsEmpty() )
		return NULL;

	if ( !IsEmpty() )
	{
		lkSQL3VerifyList::iterator Iter;
		for ( Iter = begin(); Iter != end(); Iter++ )
			if ( (*Iter)->m_sName.CmpNoCase(fName) == 0 )
				return *Iter;
	}

	return NULL; // not found
}
