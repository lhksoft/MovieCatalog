/***********************************************************
 * lkSQLite3 - a custom API of SQLite3, based at C(DAO)Recordset by MFC (Microsoft)
 * Copyright (c) 2015-'16-2022 by Laurens Koehoorn (lhksoft)
 * The GNU Lesser General Public License, version 2.1 (LGPL-2.1),
 * for more info see also : <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 ***********************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include "lkSQL3Validators.h"
#include <wx/object.h>
#include <wx/hyperlink.h>
#include <wx/msgdlg.h>

static const wxChar* _tRequiredText = wxT("This field is required and cannot be empty.");
static const wxChar* _tNotUnique = wxT("Entered value already exists.\nOnly UNIQUE values are allowed.");

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

const wxString GetRequiredText()
{
	return _tRequiredText;
}
const wxString GetNotUniqueText()
{
	return _tNotUnique;
}

bool ExtractDbHyperlink(const wxString& lpszHyperlink, wxString& sDisplay, wxString& sURL)
{
	sDisplay = wxT("");
	sURL = wxT("");
	wxUniChar cross = '#';
	wxString link = wxT("://");

	if ( lpszHyperlink.IsEmpty() ) return false;

	wxString s = lpszHyperlink;
	size_t pos1 = s.find(cross, 0);
	size_t pos2 = ((pos1 != wxString::npos) && (pos1 >= 0)) ? s.find(cross, pos1 + 1) : wxString::npos;

	if ( (pos1 != wxString::npos) && (pos2 != wxString::npos) )
	{ // that's how we like it
		sDisplay = s.Left(pos1); // excluding '#' mark
		sURL = s.Mid(pos1 + 1);
	}
	else if ( (pos1 != wxString::npos) && (pos2 == wxString::npos) )
	{
		sDisplay = s.Left(pos1); // excluding '#' mark
		sURL = s.Mid(pos1 + 1);

		if ( (sURL.Find(link) == wxString::npos) && (sDisplay.Find(link) != wxString::npos) )
		{
			sURL = s;
			sDisplay = wxT("");
		}
	}
	else // if ((pos1 == -1) && (pos2 == -1))
	{
		// is it a hyperlink or just plain text?
		if ( s.Find(link) != wxString::npos )
			sURL = s;
		else
		{
			sDisplay = s;
			return false; // invalid hyperlink data
		}
	}

	if ( !sURL.IsEmpty() && ((pos2 = sURL.rfind(cross)) != wxString::npos) )
	{
		if ( pos2 == sURL.Len() - 1 )
			sURL = sURL.Left(pos2);
	}

	return (!sURL.IsEmpty());
}
bool MakeDbHyperlink(wxString& sHyperlink, const wxString& szDisplay, const wxString& szURL)
{
	sHyperlink = wxT("");
	if ( szURL.IsEmpty() ) return false;

	wxString sDisplay = (!szDisplay.IsEmpty() && !szDisplay.IsSameAs(szURL, false)) ? szDisplay : wxT("");
	sHyperlink.Printf(wxT("%s#%s"), sDisplay, szURL);

	int pos1 = sHyperlink.Find('#'); // first occurance, which always should be there
	if ( sHyperlink.Find('#', pos1 + 1) == wxString::npos )
		sHyperlink += wxT("#");

	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3DateValidator, wxValidator)

lkSQL3DateValidator::lkSQL3DateValidator() : wxValidator()
{
	m_pRS = NULL;
	m_pField = NULL;
	m_pDate = NULL;

	m_bDutch = false;
	m_bLongDate = true;

	m_bReqAdd = false;
	m_bReqAll = false;

	SetMinYear(-1);
	SetMaxYear(-1);
}
lkSQL3DateValidator::lkSQL3DateValidator(lkDateTime* dt, lkSQL3FieldData_Real* fd, lkSQL3RecordSet* rs, bool longDate, bool dutchFormat) : wxValidator()
{
	m_pRS = rs;
	m_pField = fd;
	m_pDate = dt;

	m_bDutch = dutchFormat;
	m_bLongDate = longDate;

	m_bReqAdd = false;
	m_bReqAll = false;

	SetMinYear(-1);
	SetMaxYear(-1);
}
lkSQL3DateValidator::lkSQL3DateValidator(const lkSQL3DateValidator& other) : wxValidator(other)
{
	m_pRS = other.m_pRS;
	m_pField = other.m_pField;
	m_pDate = other.m_pDate;

	m_bDutch = other.m_bDutch;
	m_bLongDate = other.m_bLongDate;

	m_bReqAdd = other.m_bReqAdd;
	m_bReqAll = other.m_bReqAll;

	SetMinYear(other.m_nMinYear);
	SetMaxYear(other.m_nMaxYear);
}
lkSQL3DateValidator::~lkSQL3DateValidator()
{
}


void lkSQL3DateValidator::AllwaysRequired(bool b)
{
	m_bReqAll = b;
}
void lkSQL3DateValidator::AddOnlyRequired(bool b)
{
	m_bReqAdd = b;
}


// <0 will set it to default 1900, 0 = not using minimum when validating
void lkSQL3DateValidator::SetMinYear(int y)
{
	m_nMinYear = (y < 0) ? 1900 : y;
}

// <0 will set it to default Current Year, 0 = not using maximum when validating
void lkSQL3DateValidator::SetMaxYear(int y)
{
	m_nMaxYear = (y < 0) ? wxDateTime::GetCurrentYear() : y;
}

//virtual
wxObject* lkSQL3DateValidator::Clone() const
{
	return new lkSQL3DateValidator(*this);
}

wxTextCtrl* lkSQL3DateValidator::GetTextEntry()
{
	if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
	{
		return dynamic_cast<wxTextCtrl*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3DateValidator can only be used with wxTextCtrl");

	return NULL;
}

//virtual
wxString lkSQL3DateValidator::IsValid() const
{
	wxString msg;

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return msg;

	if ( (m_pDate == NULL) || (m_pRS == NULL) || (m_pField == NULL) )
		return msg;

	unsigned long ul = 0;

	if ( m_pDate->IsValid() )
		ul = m_pDate->GetYear();

	if ( ul == 0 )
	{
		if ( m_bReqAll || (m_pField->IsNotNull() && !m_pField->UseDefault()) ) // cannot be null & not using a default value
			msg = _tRequiredText;
		else if ( m_bReqAdd && m_pRS->IsAdding() )
			msg = _tRequiredText;
	}
	else
	{
		// got something, see if it's valid
		if ( (m_nMinYear > 0) && (ul < (unsigned long)m_nMinYear) )
			msg.Printf(wxT("Entered year should be later or the same as %lu."), m_nMinYear);
		else if ( (m_nMaxYear > 0) && (ul > (unsigned long)m_nMaxYear) )
			msg.Printf(wxT("Entered year should be the same or before %lu."), m_nMaxYear);
	}

	return msg;
}

bool lkSQL3DateValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pDate != NULL);
	wxASSERT(m_pRS != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxString msg = IsValid();

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

// Called to transfer data TO the window
bool lkSQL3DateValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pRS != NULL);
	wxASSERT(m_pDate != NULL);

	if ( m_pDate != NULL )
		m_pDate->SetInvalid();

	if ( m_pRS && m_pField && m_pDate )
	{
		if ( !m_pRS->IsAdding() )
			if ( !m_pField->IsNull() )
				*m_pDate = m_pField->GetValue2();
	}

	return DateTimeToWindow();
}

// Called to transfer data FROM the window
bool lkSQL3DateValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pDate != NULL);

	if ( !m_pDate || !m_pField )
		return false;

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	if ( !m_pDate->IsValid() )
	{
		if ( !m_pField->IsNull() )
			m_pField->SetValueNull();
	}
	else
	{
		double d1 = (double)(*m_pDate);
		if ( m_pField->IsNull() || (m_pField->GetValue2() != d1) )
			m_pField->SetValue2(d1);
	}
	return true;
}

// Called to transfer data TO the window
bool lkSQL3DateValidator::DateTimeToWindow(bool bAllowEmpty)
{
	wxTextCtrl* text = GetTextEntry();
	if ( !text )
		return true;

	wxString szTxt;
	if ( m_pDate )
	{
		if ( m_bDutch )
			szTxt = m_pDate->FormatFullDate_Dutch(m_bLongDate);
		else
			szTxt = m_pDate->FormatFullDate_English(m_bLongDate);

		if ( szTxt.IsEmpty() && !bAllowEmpty ) // maybe m_pDate is InValid
			szTxt = wxT("# unset #");
	}
	if ( szTxt.IsEmpty() && !bAllowEmpty )
		szTxt = wxT("# error #");

	text->SetValue(szTxt);
	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3TextValidator, wxTextValidator)

lkSQL3TextValidator::lkSQL3TextValidator() : wxTextValidator()
{
	m_pField = NULL;
	m_pUniqueTextCallback = NULL;
	m_pDB = NULL;
	m_pParam = NULL;
}
lkSQL3TextValidator::lkSQL3TextValidator(lkSQL3FieldData* pField, long style, TextFieldIsUnique_Callback pUnique, lkSQL3Database* pDB, void* param) : wxTextValidator(style)
{
	m_pField = pField;
	m_pUniqueTextCallback = pUnique;
	m_pDB = pDB;
	m_pParam = param;
}

lkSQL3TextValidator::lkSQL3TextValidator(const lkSQL3TextValidator& other) : wxTextValidator(other)
{
	m_pField = other.m_pField;
	m_pUniqueTextCallback = other.m_pUniqueTextCallback;
	m_pDB = other.m_pDB;
	m_pParam = other.m_pParam;
}

//virtual
wxObject* lkSQL3TextValidator::Clone() const
{
	return new lkSQL3TextValidator(*this);
}

wxTextEntry* lkSQL3TextValidator::GetTextEntry()
{
	if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
	{
		return (wxTextCtrl*)m_validatorWindow;
	}

	wxFAIL_MSG("lkSQL3TextValidator can only be used with wxTextCtrl");

	return NULL;
}

// Called when the value in the window must be validated.
// This function can pop up an error message.
bool lkSQL3TextValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxTextEntry* const text = GetTextEntry();
	if ( !text )
		return false;

	wxString str = text->GetValue();
	str.Trim(true);
	str.Trim(false);
	wxString msg;

	if ( str.IsEmpty() )
	{
		if ( m_pField->IsNotNull() && !m_pField->UseDefault() ) // cannot be null & not using a default value
			msg = _tRequiredText;
	}
	else // if ( !str.IsEmpty() )
	{
		if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
		{
			size_t len = str.Len();
			if ( (m_pField->GetDDVmin() > 0) && (len < (size_t)(m_pField->GetDDVmin())) )
				msg.Printf(wxT("This field requires to have at least %d chars."), m_pField->GetDDVmin());
			else if ( (m_pField->GetDDVmax() > 0) && (len > (size_t)(m_pField->GetDDVmax())) )
			{
				text->SetMaxLength(static_cast<unsigned long>(m_pField->GetDDVmax()));
				msg.Printf(wxT("This field cannot exceed %d chars."), m_pField->GetDDVmax());
			}
			else
			{
				if ( (msg = wxTextValidator::IsValid(str)).IsEmpty() )
				{
					// last check
					wxString orig = (dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->GetValue2();
					if ( orig.IsEmpty() || !orig.IsSameAs(str, false) )
					{ // value not same as original, check whether it's unique
						if ( (m_pUniqueTextCallback && m_pDB) && (!m_pUniqueTextCallback(m_pDB, str, m_pParam)) )
							msg = _tNotUnique;
					}
				}
			}
		}
		else if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
		{
			wxUint64 val;
			if ( str.ToULongLong(&val) )
			{
				if ( (m_pField->GetDDVmin() > 0) && (val < m_pField->GetDDVmin()) )
					msg.Printf(wxT("This field must be greated than %d."), m_pField->GetDDVmin());
				else if ( (m_pField->GetDDVmax() > 0) && (val > m_pField->GetDDVmax()) )
					msg.Printf(wxT("This field must be smaller than %d."), m_pField->GetDDVmax());
				else
				{
					if ( (msg = wxTextValidator::IsValid(str)).IsEmpty() )
					{
						// last check
						wxUint64 orig = static_cast<wxUint64>((dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->GetValue2());
						if ( orig != val )
						{ // value not same as original, check whether it's unique
							if ( (m_pUniqueTextCallback && m_pDB) && (!m_pUniqueTextCallback(m_pDB, str, m_pParam)) )
								msg = _tNotUnique;
						}
					}
				}

			}
		}
		else
		{
			wxASSERT(false);
			// currently no other fields supported
			return false;
		}
	}

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

// Called to transfer data TO the window
bool lkSQL3TextValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	wxTextEntry* const text = GetTextEntry();
	if ( !text || !m_pField )
		return false;

	if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
		text->SetValue((dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->GetValue2());
	else if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
	{
		wxUint64 val = static_cast<wxUint64>((dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->GetValue2());
		wxString s; s.Printf(wxT("%llu"), val);
		text->SetValue(s);
	}
	else
	{
		wxASSERT(false);
		// no other types of Fields supported
		return false;
	}

	return true;
}

// Called to transfer data FROM the window
bool lkSQL3TextValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxTextEntry* const text = GetTextEntry();
	if ( !text || !m_pField )
		return false;

	wxString str = text->GetValue();
	str.Trim(true);
	str.Trim(false);

	// little validation in here -- only set data to db if it really has changed, leave it otherwise

	if ( str.IsEmpty() ) // empty
	{
		if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) ||
			m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
		{
			if ( !m_pField->IsNull() )
			{
				m_pField->SetNull(true); // if not can be null, the db will auto use the default
				m_pField->SetDirty(true);
			}
		}
		else
		{
			wxASSERT(false);
			// No other types of Fields supported
			return false;
		}
	}
	else
	{ // got data
		if ( !m_pField->IsNull() )
		{
			if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
			{
				wxString orig = (dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->GetValue2();
				// only set into DB if the entered value is different than what it was
				if ( orig.IsEmpty() || !orig.IsSameAs(str) )
					(dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->SetValue2(str);
			}
			else if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
			{
				wxUint64 val;
				if ( str.ToULongLong(&val) )
				{
					wxUint64 orig = static_cast<wxUint64>((dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->GetValue2());
					if ( val != orig )
						(dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->SetValue2(val);
				}
			}
			else
			{
				wxASSERT(false);
				// No other types of Fields supported
				return false;
			}
		}
		else
		{
			if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
				(dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->SetValue2(str);
			else if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
			{
				wxUint64 val;
				if ( str.ToULongLong(&val) )
					(dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->SetValue2(val);
			}
			else
			{
				wxASSERT(false);
				// No other types of Fields supported
				return false;
			}
		}
	}

	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3ChkBoxValidator, wxValidator);

lkSQL3ChkBoxValidator::lkSQL3ChkBoxValidator() : wxValidator()
{
	m_sql3Field = NULL;
}
lkSQL3ChkBoxValidator::lkSQL3ChkBoxValidator(lkSQL3FieldData_Int* pField) : wxValidator()
{
	m_sql3Field = pField;
}
lkSQL3ChkBoxValidator::lkSQL3ChkBoxValidator(const lkSQL3ChkBoxValidator& v) : wxValidator(v)
{
	m_sql3Field = v.m_sql3Field;
}
lkSQL3ChkBoxValidator::~lkSQL3ChkBoxValidator()
{
}

wxCheckBox* lkSQL3ChkBoxValidator::GetCheckbox()
{
	if ( wxDynamicCast(m_validatorWindow, wxCheckBox) )
		return dynamic_cast<wxCheckBox*>(m_validatorWindow);

	wxFAIL_MSG(wxT("lkSQL3ChkBoxValidator can only be used with a wxCheckBox"));

	return NULL;
}

// -- Overrides

//virtual
wxObject* lkSQL3ChkBoxValidator::Clone() const
{
	return new lkSQL3ChkBoxValidator(*this);
}

//virtual
bool lkSQL3ChkBoxValidator::Validate(wxWindow* WXUNUSED(parent))
{
    wxASSERT(m_validatorWindow != NULL);
    wxASSERT(m_sql3Field != NULL);

    return true;
}

//virtual
bool lkSQL3ChkBoxValidator::TransferToWindow()
{
    wxASSERT(m_validatorWindow != NULL);
    wxASSERT(m_sql3Field != NULL);
    wxCheckBox* cb = GetCheckbox();
    if ( !cb || !m_sql3Field )
        return false;

    // current value from the db
    bool b = m_sql3Field->Getbool();

    if ( cb->Is3State() )
        cb->Set3StateValue(b ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
    else
        cb->SetValue(b);

    return true;
}

//virtual
bool lkSQL3ChkBoxValidator::TransferFromWindow()
{
    wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_sql3Field != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxCheckBox* cb = GetCheckbox();
	if ( !cb || !m_sql3Field )
		return false;

	bool p = m_sql3Field->Getbool();
	bool n;
    if ( cb->Is3State() )
        n = (cb->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED);
    else
        n = cb->IsChecked();

	// only if previous value (when initialising the controll) was different than now, then tell sql3-db about the new value
	if ( n != p )
		m_sql3Field->Setbool(n);
	
	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3DateTimeSpanValidator, wxValidator)

lkSQL3DateTimeSpanValidator::lkSQL3DateTimeSpanValidator() : wxValidator()
{
	m_pRS = NULL;
	m_pField = NULL;
	m_pDateSpan = NULL;

	m_bReqAdd = false;
	m_bReqAll = false;
}
lkSQL3DateTimeSpanValidator::lkSQL3DateTimeSpanValidator(lkDateTimeSpan* dt, lkSQL3FieldData_Real* fd, lkSQL3RecordSet* rs, bool bReqAdd, bool bReqAll) : wxValidator()
{
	m_pRS = rs;
	m_pField = fd;
	m_pDateSpan = dt;

	m_bReqAdd = bReqAdd;
	m_bReqAll = bReqAll;
}
lkSQL3DateTimeSpanValidator::lkSQL3DateTimeSpanValidator(const lkSQL3DateTimeSpanValidator& other) : wxValidator(other)
{
	m_pRS = other.m_pRS;
	m_pField = other.m_pField;
	m_pDateSpan = other.m_pDateSpan;

	m_bReqAdd = other.m_bReqAdd;
	m_bReqAll = other.m_bReqAll;
}
lkSQL3DateTimeSpanValidator::~lkSQL3DateTimeSpanValidator()
{
}


void lkSQL3DateTimeSpanValidator::AllwaysRequired(bool b)
{
	m_bReqAll = b;
}
void lkSQL3DateTimeSpanValidator::AddOnlyRequired(bool b)
{
	m_bReqAdd = b;
}


//virtual
wxObject* lkSQL3DateTimeSpanValidator::Clone() const
{
	return new lkSQL3DateTimeSpanValidator(*this);
}

wxTextCtrl* lkSQL3DateTimeSpanValidator::GetTextEntry()
{
	if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) )
	{
		return dynamic_cast<wxTextCtrl*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3DateTimeSpanValidator can only be used with wxTextCtrl");

	return NULL;
}

//virtual
wxString lkSQL3DateTimeSpanValidator::IsValid() const
{
	wxString msg;

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return msg;

	if ( (m_pDateSpan == NULL) || (m_pRS == NULL) || (m_pField == NULL) )
		return msg;

	if (!(m_pDateSpan->IsValid()))
	{
		if ( m_bReqAll || (m_pField->IsNotNull() && !m_pField->UseDefault()) ) // cannot be null & not using a default value
			msg = _tRequiredText;
		else if ( m_bReqAdd && m_pRS->IsAdding() )
			msg = _tRequiredText;
	}

	return msg;
}

bool lkSQL3DateTimeSpanValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pDateSpan != NULL);
	wxASSERT(m_pRS != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxString msg = IsValid();

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

// Called to transfer data TO the window
bool lkSQL3DateTimeSpanValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pRS != NULL);
	wxASSERT(m_pDateSpan != NULL);

	if ( m_pDateSpan != NULL )
		m_pDateSpan->SetNull();

	if ( (m_pRS != NULL) && (m_pField != NULL) )
	{
		if ( !m_pRS->IsAdding() )
			if ( !m_pField->IsNull() )
				*m_pDateSpan = m_pField->GetValue2();
	}

	return DateTimeToWindow();
}

// Called to transfer data FROM the window
bool lkSQL3DateTimeSpanValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pDateSpan != NULL);

	if ( !m_pDateSpan || !m_pField )
		return false;

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	if ( !m_pDateSpan->IsValid() )
	{
		if ( !m_pField->IsNull() )
			m_pField->SetValueNull();
	}
	else
	{
		double d1 = (double)(*m_pDateSpan);
		if ( m_pField->IsNull() || (m_pField->GetValue2() != d1) )
			m_pField->SetValue2(d1);
	}
	return true;
}

// Called to transfer data TO the window
bool lkSQL3DateTimeSpanValidator::DateTimeToWindow(bool bAllowEmpty)
{
	wxTextCtrl* text = GetTextEntry();
	if ( !text )
		return true;

	wxString szTxt;
	if ( m_pDateSpan )
	{
		if ( m_pDateSpan->IsValid() )
			szTxt = m_pDateSpan->Format();

		if ( szTxt.IsEmpty() && !bAllowEmpty )
			szTxt = wxT("# unset #");
	}
	if ( szTxt.IsEmpty() && !bAllowEmpty )
		szTxt = wxT("# error #");

	text->SetValue(szTxt);
	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //
