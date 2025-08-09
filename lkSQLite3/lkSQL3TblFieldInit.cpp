/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3TblFieldInit.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(lkSQL3TblInitBase);

// //////////////////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3TblFieldInit, wxObject);

lkSQL3TblFieldInit::lkSQL3TblFieldInit() : wxObject(), m_fldName()
{
	m_pClass = NULL;
	m_ppField = NULL;
}
lkSQL3TblFieldInit::lkSQL3TblFieldInit(const wxString& fldName, const wxClassInfo* pClass, lkSQL3FieldData** pFldData) : wxObject(), m_fldName(fldName)
{
	m_pClass = pClass;
	m_ppField = pFldData;
}

lkSQL3TblFieldInit::~lkSQL3TblFieldInit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


lkSQL3TblInit::lkSQL3TblInit() : lkSQL3TblInitBase(), m_TableName()
{
	DeleteContents(true);
}
lkSQL3TblInit::lkSQL3TblInit(const wxString& tblName) : lkSQL3TblInitBase(), m_TableName(tblName)
{
	DeleteContents(true);
}
lkSQL3TblInit::~lkSQL3TblInit()
{
}

void lkSQL3TblInit::AddFldInit(const wxString& fldName, const wxClassInfo* pClass, lkSQL3FieldData** ppField)
{
	Append(new lkSQL3TblFieldInit(fldName, pClass, ppField));
}

lkSQL3TblFieldInit* lkSQL3TblInit::FindByName(const wxString& fldName) const
{
	if ( IsEmpty() )
		return NULL;

	lkSQL3TblInit::const_iterator iter;
	for ( iter = begin(); iter != end(); iter++ )
	{
		if ( (*iter)->m_fldName.CmpNoCase(fldName) == 0 )
			return *iter;
	}

	return NULL;
}
