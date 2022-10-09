/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3FIELDDEF_H__
#define __LK_SQL3FIELDDEF_H__
#pragma once

//#include <wx/string.h>
#include "lkSQL3Common.h"
#include <wx/object.h>
#include <wx/list.h>

// ////////////////////////////////////////////////////////////////////////////////
// Using wxChar instead of char or wchar_t is Unicode safe,
// If no UNICODE support wxChar will be a char, 
// with Unicode support wxChar will be wchar_t
//
typedef struct sqlTblField_s
{
	const wxChar*	sName;    // Name of this field
	sqlFieldType	fType;
	bool			bNotNull; // NOT NULL
	bool			bDefault; // true if sDefault to be used
	const wxChar*	sDefault; // if bDefault, the default value to use
	// -- DDV data
	int				minValue; // if used with sqlText min.len if '0' is not used
	int				maxValue; // if used with sqlText max.len if '0' is not used
} sqlTblField_t, * sqlTblField_p;


class lkSQL3FieldDef : public wxObject
{
public:
	lkSQL3FieldDef();
	lkSQL3FieldDef(const sqlTblField_t&);
	lkSQL3FieldDef(const lkSQL3FieldDef&);
	virtual ~lkSQL3FieldDef();

	// Members
protected:
	wxString			m_Name;
	sqlFieldType		m_Type;
	bool				m_bNotNull;
	bool				m_bDefault; // if true, default in sDefault
	wxString			m_Default; // can be NULL, in which case pragramatic default will be used instead ("", "0" or "0.0")

	int					m_DDVmin, m_DDVmax;

	int					m_Index; // not used when creating a table

public:
	wxString			GetName(void) const;
	void				SetName(const wxString&);

	sqlFieldType		GetType(void) const;
	void				SetType(sqlFieldType);

	bool				IsNotNull(void) const;
	void				SetNotNull(bool);

	// ' should be '' !!
	bool				UseDefault(void) const;
	wxString			GetDefault(void) const;
	void				SetDefault(const wxString&);

	int					GetIndex(void) const;
	void				SetIndex(int);

	int					GetDDVmin(void) const;
	int					GetDDVmax(void) const;

	// Operations
public:
	wxString			GetDefinition(void); // Creates SQL-part when creating a table
	lkSQL3FieldDef&		operator = (const sqlTblField_t&);

private:
	wxDECLARE_DYNAMIC_CLASS(lkSQL3FieldDef);
};

// ///////////////////////////////////////////////////////////////////////////////

// This list used in lkSQL3TableDef
WX_DECLARE_LIST(lkSQL3FieldDef, lkSQL3FieldDefList);

#endif // !__LK_SQL3FIELDDEF_H__
