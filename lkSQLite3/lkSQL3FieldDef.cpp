/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3FieldDef.h"
#include <wx/wx.h>
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(lkSQL3FieldDefList);

// //////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldDef, wxObject);

lkSQL3FieldDef::lkSQL3FieldDef() : wxObject(), m_Name(), m_Default()
{
	m_Index = -1;
	m_Type = sqlFieldType::sqlNone;
	m_bNotNull = false;
	m_bDefault = false;
	m_DDVmin = 0;
	m_DDVmax = 0;
}
lkSQL3FieldDef::lkSQL3FieldDef(const sqlTblField_t& _d) : wxObject(), m_Name(), m_Default()
{
	m_Index = -1;
	m_Type = sqlFieldType::sqlNone;
	m_bNotNull = false;
	m_bDefault = false;
	m_DDVmin = 0;
	m_DDVmax = 0;

	if ( (_d.sName != NULL) && (_d.sName[0]) )
	{
		m_Name = _d.sName;
		m_Type = _d.fType;
		m_bNotNull = _d.bNotNull;
		m_bDefault = _d.bDefault;
		m_DDVmin = _d.minValue;
		m_DDVmax = _d.maxValue;

		if ( m_bDefault )
		{
			if ( (_d.sDefault != NULL) && (!wxIsEmpty(_d.sDefault)) )
				m_Default = _d.sDefault;
			else
			{
				switch ( m_Type )
				{
					case sqlFieldType::sqlInt:
					{
						m_Default = wxT("0");
						break;
					}
					case sqlFieldType::sqlReal:
					{
						m_Default = wxT("0.0");
						break;
					}
					default:
						break;
				}
			}
		}
	}
}
lkSQL3FieldDef::lkSQL3FieldDef(const lkSQL3FieldDef& _d) : wxObject(), m_Name(_d.m_Name), m_Default(_d.m_Default)
{
	m_Index = -1;
	m_Type = _d.m_Type;
	m_bNotNull = _d.m_bNotNull;
	m_bDefault = _d.m_bDefault;
	m_DDVmin = _d.m_DDVmin;
	m_DDVmax = _d.m_DDVmax;
}

lkSQL3FieldDef::~lkSQL3FieldDef()
{
}

wxString lkSQL3FieldDef::GetName() const
{
	return m_Name;
}
void lkSQL3FieldDef::SetName(const wxString& _s)
{
	wxASSERT(!_s.IsEmpty());
	if ( !_s.IsEmpty() )
		m_Name = _s;
}
sqlFieldType lkSQL3FieldDef::GetType() const
{
	return m_Type;
}
void lkSQL3FieldDef::SetType(sqlFieldType _t)
{
	wxASSERT(_t != sqlFieldType::sqlNone);
	m_Type = _t;
}
bool lkSQL3FieldDef::IsNotNull() const
{
	return m_bNotNull;
}
void lkSQL3FieldDef::SetNotNull(bool _b)
{
	m_bNotNull = _b;
}
bool lkSQL3FieldDef::UseDefault() const
{
	return m_bDefault;
}
wxString lkSQL3FieldDef::GetDefault() const
{
	return m_Default;
}
void lkSQL3FieldDef::SetDefault(const wxString& _d)
{
	if (_d.IsEmpty())
	{
		switch ( m_Type )
		{
			case sqlFieldType::sqlInt:
			{
				m_Default = wxT("0");
				m_bDefault = true;
				break;
			}
			case sqlFieldType::sqlReal:
			{
				m_Default = wxT("0.0");
				m_bDefault = true;
				break;
			}
			default:
			{
				m_Default.Empty();
				m_bDefault = false;
				break;
			}
		}
	}
	else
	{
		m_Default = _d;
		m_bDefault = true;
	}
}
int lkSQL3FieldDef::GetIndex() const
{
	return m_Index;
}
void lkSQL3FieldDef::SetIndex(int _i)
{
	m_Index = _i;
}
int lkSQL3FieldDef::GetDDVmin() const
{
	return m_DDVmin;
}
int lkSQL3FieldDef::GetDDVmax() const
{
	return m_DDVmax;
}

wxString lkSQL3FieldDef::GetDefinition()
{
	wxString def;

	def.Printf(wxT("[%s] %s"), GetName(), GetSQLiteTypeStr(m_Type));

	if ( UseDefault() )
	{
		wxString s;
		s.Printf(wxT(" DEFAULT '%s'"), GetDefault());
		def += s;
	}
	if ( IsNotNull() )
	{
		def += _T(" NOT NULL");
		if ( UseDefault() )
			def += _T(" ON CONFLICT REPLACE");
	}

	return def;
}

lkSQL3FieldDef& lkSQL3FieldDef::operator=(const sqlTblField_t& fld)
{
	m_Index = -1;
	m_Type = sqlFieldType::sqlNone;
	m_bNotNull = false;
	m_bDefault = false;
	m_DDVmin = 0;
	m_DDVmax = 0;

	if ( !wxIsEmpty(fld.sName) )
	{
		m_Name = fld.sName;
		m_Type = fld.fType;
		m_bNotNull = fld.bNotNull;
		m_bDefault = fld.bDefault;
		m_DDVmin = fld.minValue;
		m_DDVmax = fld.maxValue;

		if ( m_bDefault )
		{
			if ( !wxIsEmpty(fld.sDefault) )
				m_Default = fld.sDefault;
			else
			{
				switch ( m_Type )
				{
					case sqlFieldType::sqlInt:
						m_Default = wxT("0");
						break;

					case sqlFieldType::sqlReal:
						m_Default = wxT("0.0");
						break;

					default:
						break;
				}
			}
		}
	}

	return *this;
}
