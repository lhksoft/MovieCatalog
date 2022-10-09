/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3FieldData.h"
#include "internal_tools.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(lkSQL3FieldSetBase);

// //////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData, wxObject);

lkSQL3FieldData::lkSQL3FieldData() : wxObject(), m_Name(), m_Default()
{
	m_Type = sqlFieldType::sqlNone;
	m_nIndex = -1;
	m_bNotNull = false;
	m_bUseDefault = false;

	m_bDirty = false;
	m_bNull = true;

	m_DDVmin = 0;
	m_DDVmax = 0;
}
lkSQL3FieldData::lkSQL3FieldData(const lkSQL3FieldData& fld) : wxObject(), m_Name(fld.m_Name), m_Default(fld.m_Default)
{
	m_Type = fld.m_Type;
	m_nIndex = fld.m_nIndex;
	m_bNotNull = fld.m_bNotNull;
	m_bUseDefault = fld.m_bUseDefault;

	m_bDirty = fld.m_bDirty;;
	m_bNull = fld.m_bNull;

	m_DDVmin = fld.m_DDVmin;
	m_DDVmax = fld.m_DDVmax;
}
lkSQL3FieldData::lkSQL3FieldData(const wxString& _name, sqlFieldType _type) : wxObject(), m_Name(_name), m_Default()
{
	m_nIndex = -1;
	m_Type = _type;
	m_bNotNull = false;
	m_bUseDefault = false;

	m_bDirty = false;
	m_bNull = true;

	m_DDVmin = 0;
	m_DDVmax = 0;
}

lkSQL3FieldData::~lkSQL3FieldData()
{
}

wxString lkSQL3FieldData::GetName() const
{
	return m_Name;
}
void lkSQL3FieldData::SetName(const wxString& _n)
{
	m_Name = _n;
}
void lkSQL3FieldData::SetIndex(int _idx)
{
	m_nIndex = _idx;
}
int lkSQL3FieldData::GetIndex() const
{
	return m_nIndex;
}
void lkSQL3FieldData::SetSqlType(sqlFieldType _t)
{
	m_Type = _t;
}
void lkSQL3FieldData::SetSqlType(const wxString& _sT)
{
	m_Type = GetSQLiteType(_sT);
}
sqlFieldType lkSQL3FieldData::GetSqlType() const
{
	return m_Type;
}
void lkSQL3FieldData::SetNotNull(bool _b)
{
	m_bNotNull = _b;
}
bool lkSQL3FieldData::IsNotNull() const
{
	return m_bNotNull;
}
void lkSQL3FieldData::SetDefault(bool bUseDefault, const wxString& _def/* = wxEmptyString*/)
{
	m_bUseDefault = bUseDefault;
	m_Default.Empty();
	if ( !m_bUseDefault )
		return;

	if (!_def.IsEmpty())
	{
		m_Default = lkTrim(_def, wxT("\"' "));
	}
}
bool lkSQL3FieldData::UseDefault() const
{
	return m_bUseDefault;
}
wxString lkSQL3FieldData::GetsDefault() const
{
	return m_Default;
}
int lkSQL3FieldData::GetiDefault() const
{
	long i = 0;
	if ( !m_Default.IsEmpty() )
		m_Default.ToLong(&i);

	return (int)i;
}
wxInt64 lkSQL3FieldData::Geti64Default() const
{
	wxLongLong_t i = 0;
	if ( !m_Default.IsEmpty() )
		m_Default.ToLongLong(&i);

	return (wxInt64)i;
}
double lkSQL3FieldData::GetdDefault() const
{
	double d = 0.0;
	if ( !m_Default.IsEmpty() )
		m_Default.ToDouble(&d);

	return d;
}
void lkSQL3FieldData::SetDirty(bool _dirty/* = true*/)
{
	m_bDirty = _dirty;
}
bool lkSQL3FieldData::IsDirty() const
{
	return m_bDirty;
}
void lkSQL3FieldData::SetNull(bool _n)
{
	m_bNull = _n;
}
bool lkSQL3FieldData::IsNull() const
{
	return m_bNull;
}
void lkSQL3FieldData::SetDDVmin(int m)
{
	m_DDVmin = m;
}
void lkSQL3FieldData::SetDDVmax(int m)
{
	m_DDVmax = m;
}
int lkSQL3FieldData::GetDDVmin() const
{
	return m_DDVmin;
}
int lkSQL3FieldData::GetDDVmax() const
{
	return m_DDVmax;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData_Int, lkSQL3FieldData);

lkSQL3FieldData_Int::lkSQL3FieldData_Int() : lkSQL3FieldData()
{
	m_Value = 0;
	m_Default = wxT("0");
}
lkSQL3FieldData_Int::lkSQL3FieldData_Int(const lkSQL3FieldData_Int& _i) : lkSQL3FieldData((const lkSQL3FieldData&)_i)
{
	m_Value = _i.m_Value;
}
lkSQL3FieldData_Int::lkSQL3FieldData_Int(const wxString& _name) : lkSQL3FieldData(_name, sqlFieldType::sqlInt)
{
	m_Value = 0;
	m_Default = wxT("0");
}


void lkSQL3FieldData_Int::SetValue(wxInt64 _i)
{
	m_Value = _i;
}
void lkSQL3FieldData_Int::SetValueNull()
{
	if ( IsNull() ) return;

	if ( IsNotNull() && !UseDefault() )
	{
		SetNull(false);
		SetValue(0);
	}
	else
		SetNull(true); // let database decide what to do, use default or realy set to NULL

	SetDirty();
}
void lkSQL3FieldData_Int::SetValue2(wxInt64 _i)
{
	SetNull(false);
	SetValue(_i);
	SetDirty();
}
void lkSQL3FieldData_Int::Setbool(bool _b)
{
	SetValue2((wxInt64)((_b) ? 1 : 0));
}

wxInt64 lkSQL3FieldData_Int::GetValue() const
{
	return m_Value;
}
wxInt64 lkSQL3FieldData_Int::GetValue2() const
{
	return (!IsNull()) ? GetValue() : (UseDefault()) ? Geti64Default() : 0;
}
bool lkSQL3FieldData_Int::Getbool() const
{
	return (GetValue2() != 0) ? true : false;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData_Real, lkSQL3FieldData);

lkSQL3FieldData_Real::lkSQL3FieldData_Real() : lkSQL3FieldData()
{
	m_Value = 0.0;
	m_Default = wxT("0.0");
}
lkSQL3FieldData_Real::lkSQL3FieldData_Real(const lkSQL3FieldData_Real& _i) : lkSQL3FieldData((const lkSQL3FieldData&)_i)
{
	m_Value = _i.m_Value;
}
lkSQL3FieldData_Real::lkSQL3FieldData_Real(const wxString& _name) : lkSQL3FieldData(_name, sqlFieldType::sqlReal)
{
	m_Value = 0.0;
	m_Default = wxT("0.0");
}

void lkSQL3FieldData_Real::SetValue(double _d)
{
	m_Value = _d;
}
void lkSQL3FieldData_Real::SetValueNull()
{
	if ( IsNull() ) return;

	if ( IsNotNull() && !UseDefault() )
	{
		SetNull(false);
		SetValue(0.0);
	}
	else
		SetNull(true); // let database decide what to do, use default or realy set to NULL

	SetDirty();
}
void lkSQL3FieldData_Real::SetValue2(double _d)
{
	SetNull(false);
	SetValue(_d);
	SetDirty();
}

double lkSQL3FieldData_Real::GetValue() const
{
	return m_Value;
}
double lkSQL3FieldData_Real::GetValue2() const
{
	return (!IsNull()) ? GetValue() : (UseDefault()) ? GetdDefault() : 0.0;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData_Text, lkSQL3FieldData);

lkSQL3FieldData_Text::lkSQL3FieldData_Text() : lkSQL3FieldData(), m_Value()
{
}
lkSQL3FieldData_Text::lkSQL3FieldData_Text(const lkSQL3FieldData_Text& _s) : lkSQL3FieldData((const lkSQL3FieldData&)_s), m_Value(_s.m_Value)
{
}
lkSQL3FieldData_Text::lkSQL3FieldData_Text(const wxString& _name) : lkSQL3FieldData(_name, sqlFieldType::sqlText), m_Value()
{
}

void lkSQL3FieldData_Text::SetValue(const wxString& _s)
{
	m_Value = _s;
}
void lkSQL3FieldData_Text::SetValueNull()
{
	if ( IsNull() ) return;

	if ( IsNotNull() )
	{
		SetNull(true); // let database decide what to do, use default or realy set to NULL
		SetDirty();
	}
}
void lkSQL3FieldData_Text::SetValue2(const wxString& _s)
{
	SetNull((_s.IsEmpty()) ? true : false);
	if (!_s.IsEmpty())
		SetValue(_s);
	SetDirty();
}

wxString lkSQL3FieldData_Text::GetValue() const
{
	return m_Value;
}
wxString lkSQL3FieldData_Text::GetValue2() const
{
	wxString result = (!IsNull()) ? GetValue() : (UseDefault()) ? GetsDefault() : wxString(wxEmptyString);
	return result;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3FieldData_Blob, lkSQL3FieldData);

lkSQL3FieldData_Blob::lkSQL3FieldData_Blob() : lkSQL3FieldData()
{
	m_Blob = NULL;
	m_blobSize = 0;
}
lkSQL3FieldData_Blob::lkSQL3FieldData_Blob(const lkSQL3FieldData_Blob& _b) : lkSQL3FieldData((const lkSQL3FieldData&)_b)
{
	if ( (_b.m_Blob != NULL) && (_b.m_blobSize > 0) )
	{
		m_Blob = new wxUint8[_b.m_blobSize];
		m_blobSize = _b.m_blobSize;
		memcpy(m_Blob, _b.m_Blob, sizeof(wxUint8) * m_blobSize);

		SetNull(false);
	}
	else
	{
		m_Blob = NULL;
		m_blobSize = 0;

		SetNull(true);
	}
}
lkSQL3FieldData_Blob::lkSQL3FieldData_Blob(const wxString& _name) : lkSQL3FieldData(_name, sqlFieldType::sqlBlob)
{
	m_Blob = NULL;
	m_blobSize = 0;
}
lkSQL3FieldData_Blob::~lkSQL3FieldData_Blob()
{
	if ( m_Blob != NULL )
		delete[] (wxUint8*)m_Blob;
}
void lkSQL3FieldData_Blob::SetValue(const void* _b, const size_t _t)
{
	if ( m_Blob != NULL )
	{
		delete[] (wxUint8*)m_Blob;
		m_Blob = NULL;
		m_blobSize = 0;
		SetNull(true);
	}

	if ( (_b != NULL) && (_t > 0) )
	{
		m_Blob = new wxUint8[_t];
		m_blobSize = _t;
		memcpy(m_Blob, _b, sizeof(wxUint8) * m_blobSize);

		SetNull(false);
	}
}
void lkSQL3FieldData_Blob::SetValue2(const void* _b, const size_t _t)
{
//	bool bDirty = (((m_Blob == NULL) && (_b != NULL) && (_t > 0)) || ((m_Blob != NULL) && ((_b == NULL) || (_t == 0))));

	if ( m_Blob != NULL )
	{
		delete[] (wxUint8*)m_Blob;
		m_Blob = NULL;
		m_blobSize = 0;
	}

	if ( (_b != NULL) && (_t > 0) )
	{
		m_Blob = new wxUint8[_t];
		m_blobSize = _t;
		memcpy(m_Blob, _b, sizeof(wxUint8) * m_blobSize);

		SetNull(false);
	}
	else
		SetNull(true);

	SetDirty();
}

const void* lkSQL3FieldData_Blob::GetValue() const
{
	return (IsNull()) ? NULL : m_Blob;
}
const size_t lkSQL3FieldData_Blob::GetSize() const
{
	return (IsNull()) ? 0 : m_blobSize;
}
const void* lkSQL3FieldData_Blob::GetValue2(size_t& t)
{
	const void* _result = GetValue();
	t = (_result == NULL) ? 0 : GetSize();
	return _result;
}

// ///////////////////////////////////////////////////////////////////////////////


lkSQL3FieldSet::lkSQL3FieldSet() : lkSQL3FieldSetBase()
{
	DeleteContents(true);
}
int lkSQL3FieldSet::AddField(const wxString& _colName, sqlFieldType _Type)
{
	if ( (_colName.IsEmpty()) || (_Type == sqlFieldType::sqlNone) )
		return wxNOT_FOUND;

	lkSQL3FieldData* _fldData = NULL;

	switch ( _Type )
	{
		case sqlFieldType::sqlInt:
			_fldData = new lkSQL3FieldData_Int(_colName);
			break;
		case sqlFieldType::sqlReal:
			_fldData = new lkSQL3FieldData_Real(_colName);
			break;
		case sqlFieldType::sqlText:
			_fldData = new lkSQL3FieldData_Text(_colName);
			break;
		case sqlFieldType::sqlBlob:
			_fldData = new lkSQL3FieldData_Blob(_colName);
			break;
		default:
		{
			wxASSERT(0);
			return wxNOT_FOUND;
//			break;
		}
	}

	Append(_fldData);
	return IndexOf(_fldData);
}

int lkSQL3FieldSet::AddField(lkSQL3FieldDef* fldDef)
{
	if ( !fldDef ) return wxNOT_FOUND;

	int pos = AddField(fldDef->GetName(), fldDef->GetType());
	if ( pos == wxNOT_FOUND ) return pos;

	lkSQL3FieldSet::compatibility_iterator node = Item(pos);
	if ( node )
	{
		lkSQL3FieldData* pFld = node->GetData();
		pFld->SetDDVmin(fldDef->GetDDVmin());
		pFld->SetDDVmax(fldDef->GetDDVmax());
	}

	return pos;
}
int lkSQL3FieldSet::SetIndex(const wxString& _colName, int _idx)
{
	if ( (_colName.IsEmpty()) || (_idx < 0) )
		return wxNOT_FOUND;

	lkSQL3FieldSet::iterator iter;
	for ( iter = begin(); iter != end(); iter++ )
	{
		lkSQL3FieldData* current = *iter;
		if ( current->GetName().CmpNoCase(_colName) == 0 )
		{
			current->SetIndex(_idx);
			return IndexOf(current);
		}
	}

	return wxNOT_FOUND; // not found
}
lkSQL3FieldData* lkSQL3FieldSet::GetField(const wxString& _colName)
{
	if ( _colName.IsEmpty() )
		return NULL;

	lkSQL3FieldSet::iterator iter;
	for ( iter = begin(); iter != end(); iter++ )
	{
		lkSQL3FieldData* current = *iter;
		if ( current->GetName().CmpNoCase(_colName) == 0 )
			return current;
	}

	return NULL; // not found
}
lkSQL3FieldData* lkSQL3FieldSet::GetField(int _idx)
{
	lkSQL3FieldSet::compatibility_iterator node = Item(_idx);
	if ( node )
		return node->GetData();

	return NULL; // not found
}
lkSQL3FieldData* lkSQL3FieldSet::GetLastField()
{
	lkSQL3FieldSet::iterator iter = end();
	return *iter;
}

void lkSQL3FieldSet::Reset()
{
	lkSQL3FieldSet::iterator iter;
	for ( iter = begin(); iter != end(); iter++ )
	{
		lkSQL3FieldData* current = *iter;

		current->SetDirty(false);
		if ( current->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
			((lkSQL3FieldData_Int*)current)->SetValue(((lkSQL3FieldData_Int*)current)->Geti64Default());
		else if ( current->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Real)) )
			((lkSQL3FieldData_Real*)current)->SetValue(((lkSQL3FieldData_Real*)current)->GetdDefault());
		else if ( current->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
			((lkSQL3FieldData_Text*)current)->SetValue(((lkSQL3FieldData_Text*)current)->GetsDefault());
		else if ( current->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Blob)) )
			((lkSQL3FieldData_Blob*)current)->SetValue(NULL, 0);

		current->SetNull(true);
	}
}

