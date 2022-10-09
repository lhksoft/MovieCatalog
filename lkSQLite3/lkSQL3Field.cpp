/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Field.h"
#include "lkSQL3Exception.h"
#include <sqlite3.h>


wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3Field, wxObject);

lkSQL3Field::lkSQL3Field() : wxObject(), m_StmtPtr(), m_Uni()
{
	// This constructor is not allowed, we will ASSERT if you do so when constructing m_StmtPtr()
}
lkSQL3Field::lkSQL3Field(const lkSQL3Field& _f) : wxObject(), m_StmtPtr(_f.m_StmtPtr), m_Uni()
{
	m_Index = _f.m_Index;
}
lkSQL3Field::lkSQL3Field(lkSQL3Statement::Ptr& _ptr, int _index) : wxObject(), m_StmtPtr(_ptr), m_Uni()
{
	m_Index = _index;
}

lkSQL3Field::~lkSQL3Field()
{
	// the finalization will be done by the destructor of the last shared pointer
}

wxString lkSQL3Field::GetName() const // nothrow
{
	*((wxString*)&m_Uni) = wxString::FromUTF8(sqlite3_column_name(m_StmtPtr, m_Index));
	return m_Uni;
}
wxString lkSQL3Field::GetOriginName() const // nothrow
{
	*((wxString*)&m_Uni) = wxString::FromUTF8(sqlite3_column_origin_name(m_StmtPtr, m_Index));
	return m_Uni;
}

int lkSQL3Field::GetType() const
{
	return sqlite3_column_type(m_StmtPtr, m_Index);
}
bool lkSQL3Field::IsInt() const
{
	return (GetType() == SQLITE_INTEGER);
}
bool lkSQL3Field::IsFloat() const
{
	return (GetType() == SQLITE_FLOAT);
}
bool lkSQL3Field::IsText() const
{
	return (GetType() == SQLITE_TEXT);
}
bool lkSQL3Field::IsBlob() const
{
	return (GetType() == SQLITE_BLOB);
}
bool lkSQL3Field::IsNull() const
{
	return (GetType() == SQLITE_NULL);
}

// Return the integer value of the Field
int lkSQL3Field::GetInt() const
{
	return sqlite3_column_int(m_StmtPtr, m_Index);
}
// Return the 64bits integer value of the Field
wxInt64 lkSQL3Field::GetI64() const
{
	return sqlite3_column_int64(m_StmtPtr, m_Index);
}
// Return the double (64bits float) value of the Field
double lkSQL3Field::GetReal() const
{
	return sqlite3_column_double(m_StmtPtr, m_Index);
}
// Return a pointer to the text value (NULL terminated string) of the Field
wxString lkSQL3Field::GetText(const wxString& _default/* = _T("")*/) const
{
	const char* pText = reinterpret_cast<const char*>(sqlite3_column_text(m_StmtPtr, m_Index));
	*((wxString*)&m_Uni) = wxString::FromUTF8(pText);
	return (m_Uni.IsEmpty()) ? _default : m_Uni;
}
// Return a pointer to the binary blob value of the Field
const void* lkSQL3Field::GetBlob() const
{
	return sqlite3_column_blob(m_StmtPtr, m_Index);
}

// size of Text (without '\0') or Blob
size_t lkSQL3Field::GetSize() const
{
	return (size_t)sqlite3_column_bytes(m_StmtPtr, m_Index);
}


