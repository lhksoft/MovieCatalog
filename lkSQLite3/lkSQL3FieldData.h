/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef __LK_SQL3FIELDDATA_H__
#define __LK_SQL3FIELDDATA_H__
#pragma once

#include "lkSQL3Common.h"
#include "lkSQL3FieldDef.h"

#include <wx/object.h>
#include <wx/list.h>

class lkSQL3FieldData : public wxObject
{
public:
	lkSQL3FieldData();
	lkSQL3FieldData(const lkSQL3FieldData&);
	lkSQL3FieldData(const wxString&, sqlFieldType);
	virtual ~lkSQL3FieldData();

	void			SetName(const wxString&);
	wxString		GetName(void) const;

	void			SetIndex(int);
	int				GetIndex(void) const;

	void			SetSqlType(sqlFieldType);
	void			SetSqlType(const wxString&);
	sqlFieldType	GetSqlType(void) const;

	void			SetNotNull(bool);
	bool			IsNotNull(void) const;

	void			SetDefault(bool bUseDefault, const wxString& _def = wxEmptyString);
	bool			UseDefault(void) const;
	wxString		GetsDefault(void) const;
	int				GetiDefault(void) const;
	wxInt64			Geti64Default(void) const;
	double			GetdDefault(void) const;

	void			SetDirty(bool _dirty = true);
	bool			IsDirty(void) const;

	void			SetNull(bool);
	bool			IsNull(void) const;

	void			SetDDVmin(int);
	int				GetDDVmin(void) const;
	void			SetDDVmax(int);
	int				GetDDVmax(void) const;

	// Members
protected:
	wxString		m_Name;		// could be ALIAS
	// Following should not be set by derived program -- internally retrieved from sqlite
	int				m_nIndex;
	sqlFieldType	m_Type;
	bool			m_bNotNull;
	bool			m_bUseDefault;
	wxString		m_Default;	// based on m_sqlType

	// Following used whith TABLE INSERT & TABLE ALTER (UPDATE)
	bool			m_bDirty;
	bool			m_bNull;

	// Following used with DDV routines
	int				m_DDVmin, m_DDVmax;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3FieldData);
};

class lkSQL3FieldData_Int : public lkSQL3FieldData
{
public:
	lkSQL3FieldData_Int();
	lkSQL3FieldData_Int(const lkSQL3FieldData_Int&);
	lkSQL3FieldData_Int(const wxString&);

	void			SetValue(wxInt64);
	void			SetValueNull(void);
	void			SetValue2(wxInt64);
	void			Setbool(bool);

	wxInt64			GetValue(void) const;
	wxInt64			GetValue2(void) const; // (!IsNull()) ? GetValue() : (UseDefault()) ? Geti64Default() : 0;
	bool			Getbool(void) const;

protected:
	wxInt64			m_Value;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3FieldData_Int);
};

class lkSQL3FieldData_Real : public lkSQL3FieldData
{
public:
	lkSQL3FieldData_Real();
	lkSQL3FieldData_Real(const lkSQL3FieldData_Real&);
	lkSQL3FieldData_Real(const wxString&);

	void			SetValue(double);
	void			SetValueNull(void);
	void			SetValue2(double);

	double			GetValue(void) const;
	double			GetValue2(void) const; // (!IsNull()) ? GetValue() : (UseDefault()) ? GetdDefault() : 0.0;

protected:
	double			m_Value;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3FieldData_Real);
};

class lkSQL3FieldData_Text : public lkSQL3FieldData
{
public:
	lkSQL3FieldData_Text();
	lkSQL3FieldData_Text(const lkSQL3FieldData_Text&);
	lkSQL3FieldData_Text(const wxString&);

	void			SetValue(const wxString&);
	void			SetValueNull(void);
	void			SetValue2(const wxString&);

	wxString		GetValue(void) const;
	wxString		GetValue2(void) const; // (!IsNull()) ? GetValue() : (UseDefault()) ? GetsDefault() : _T("");

protected:
	wxString		m_Value;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3FieldData_Text);
};

class lkSQL3FieldData_Blob : public lkSQL3FieldData
{
public:
	lkSQL3FieldData_Blob();
	lkSQL3FieldData_Blob(const lkSQL3FieldData_Blob&);
	lkSQL3FieldData_Blob(const wxString&);
	virtual ~lkSQL3FieldData_Blob();

	void			SetValue(const void*, const size_t);
	void			SetValue2(const void*, const size_t);

	const void*		GetValue(void) const;
	const size_t	GetSize(void) const;
	const void*		GetValue2(size_t&);

protected:
	void*			m_Blob;
	size_t			m_blobSize;

private:
	wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(lkSQL3FieldData_Blob);
};

// ///////////////////////////////////////////////////////////////////////////////

// this macro declares and partly implements MyList class
WX_DECLARE_LIST(lkSQL3FieldData, lkSQL3FieldSetBase);

class lkSQL3FieldSet : public lkSQL3FieldSetBase
{
public:
	lkSQL3FieldSet();

	// Following returns the index where the new item has been stored in the list

	int							AddField(const wxString&, sqlFieldType);
	int							AddField(lkSQL3FieldDef*);
	int							SetIndex(const wxString&, int);

	lkSQL3FieldData*			GetField(const wxString&);
	lkSQL3FieldData*			GetField(int);
	lkSQL3FieldData*			GetLastField(void);

	void						Reset(void); // initializes all fields to 'Default' value and sets the Dirty-flag to false
};

#endif // !__LK_SQL3FIELDDATA_H__
