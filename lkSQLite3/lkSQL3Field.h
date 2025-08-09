/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3FIELD_H__
#define __LK_SQL3FIELD_H__
#pragma once

#include "lkSQL3Statement.h"
#include <wx/object.h>

class lkSQL3Field : public wxObject
{
public:
	lkSQL3Field(); // base constructor not allowed
	lkSQL3Field(lkSQL3Statement::Ptr&, int _index);
	lkSQL3Field(const lkSQL3Field&);
	virtual ~lkSQL3Field();

	// Members
public:
	/**
	* Return a pointer to the named assigned to this result column (potentially aliased)
	* see GetOriginName() to get original column name (not aliased)
	*/
	wxString				GetName(void) const; // nothrow

	// Return a pointer to the table column name that is the origin of this result column
	wxString				GetOriginName(void) const; // nothrow

	/**
	 * @brief Return the type of the value of the column
	 *
	 * Return either SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL.
	 * (warning) After a type conversion (by a call to a getXxx on a Column of a Yyy type),
	 *           the value returned by sqlite3_column_type() is undefined.
	 */
	int						GetType(void) const;

	bool					IsInt(void) const;
	bool					IsFloat(void) const;
	bool					IsText(void) const;
	bool					IsBlob(void) const;
	bool					IsNull(void) const;

	int						GetInt(void) const; // Return the integer value of the Field
	wxInt64					GetI64(void) const; // Return the 64bits integer value of the Field
	double					GetReal(void) const; // Return the double (64bits float) value of the Field
	wxString				GetText(const wxString& _default = wxEmptyString) const; // Return the text value of the Field -- converts UTF8 to UNICODE
	const void*				GetBlob(void) const; // Return a pointer to the binary blob value of the Field

	size_t					GetSize(void) const; // size of Text (without '\0') or Blob

	inline operator int() const
	{
		return GetInt();
	}
	inline operator wxInt64() const
	{
		return GetI64();
	}
	inline operator wxUint64() const
	{
		return static_cast<wxUint64>(GetI64());
	}
	inline operator double() const
	{
		return GetReal();
	}
	inline operator wxString() const
	{
		return GetText();
	}
	inline operator const void*() const
	{
		return GetBlob();
	}


	// Data Members
private:
	lkSQL3Statement::Ptr	m_StmtPtr;	//!< Shared Pointer to the prepared SQLite Statement Object
	int						m_Index;	//!< Index of the column in the row of result
	wxString				m_Uni;

	wxDECLARE_DYNAMIC_CLASS(lkSQL3Field);
};

#endif // !__LK_SQL3FIELD_H__
