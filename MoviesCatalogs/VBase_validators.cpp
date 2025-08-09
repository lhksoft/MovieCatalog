/* ********************************************************************************
 * MoviesCatalog - a Programm to catalogue a private collection of Movies using SQLite3
 * Copyright (C) 2022 by Laurens Koehoorn (lhksoft)
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ********************************************************************************/
#include "VBase_validators.h"
#include "VBase_private.h"
#include "VBase_controls.h"
#include "GroupCtrl.h"


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkStaticHyperlinkValidator, lkSQL3HyperlinkValidator)

lkStaticHyperlinkValidator::lkStaticHyperlinkValidator() : lkSQL3HyperlinkValidator()
{
	m_pValid = NULL;
}
lkStaticHyperlinkValidator::lkStaticHyperlinkValidator(lkSQL3FieldData_Text* pField, bool* pValid) : lkSQL3HyperlinkValidator(pField)
{
	m_pValid = pValid;
}
lkStaticHyperlinkValidator::lkStaticHyperlinkValidator(const lkStaticHyperlinkValidator& other) : lkSQL3HyperlinkValidator(other)
{
	m_pValid = other.m_pValid;
}
lkStaticHyperlinkValidator::~lkStaticHyperlinkValidator()
{
}

//virtual
wxObject* lkStaticHyperlinkValidator::Clone() const
{
	return new lkStaticHyperlinkValidator(*this);
}

bool lkStaticHyperlinkValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

// Called to transfer data TO the window
bool lkStaticHyperlinkValidator::TransferToWindow()
{
	if ( m_pValid && !(*m_pValid) )
	{
		wxASSERT(m_validatorWindow != NULL);
		wxASSERT(m_pField != NULL);

		lkHyperlinkCtrl* ctrl = GetControl();
		if ( !ctrl || !m_pField )
			return false;

		ctrl->SetVisited(false);
		ctrl->SetEmpty();
	}
	else
		return lkSQL3HyperlinkValidator::TransferToWindow();

	return true;
}

// Called to transfer data FROM the window
bool lkStaticHyperlinkValidator::TransferFromWindow()
{
	return true;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkStaticChkbxValidator, lkSQL3ChkBoxValidator)

lkStaticChkbxValidator::lkStaticChkbxValidator() : lkSQL3ChkBoxValidator()
{
	m_pValid = NULL;
}
lkStaticChkbxValidator::lkStaticChkbxValidator(lkSQL3FieldData_Int* pField, bool* pValid) : lkSQL3ChkBoxValidator(pField)
{
	m_pValid = pValid;
}
lkStaticChkbxValidator::lkStaticChkbxValidator(const lkStaticChkbxValidator& other) : lkSQL3ChkBoxValidator(other)
{
	m_pValid = other.m_pValid;
}
lkStaticChkbxValidator::~lkStaticChkbxValidator()
{
}

//virtual
wxObject* lkStaticChkbxValidator::Clone() const
{
	return new lkStaticChkbxValidator(*this);
}

bool lkStaticChkbxValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

// Called to transfer data TO the window
bool lkStaticChkbxValidator::TransferToWindow()
{
	if ( m_pValid && !(*m_pValid) )
	{
		wxASSERT(m_validatorWindow != NULL);
		wxASSERT(m_sql3Field != NULL);
		wxCheckBox* cb = GetCheckbox();
		if ( !cb || !m_sql3Field )
			return false;

		if ( cb->Is3State() )
			cb->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
		else
			cb->SetValue(false);
	}
	else
		return lkSQL3ChkBoxValidator::TransferToWindow();

	return true;
}

// Called to transfer data FROM the window
bool lkStaticChkbxValidator::TransferFromWindow()
{
	return true;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkStaticCoverValidator, lkSQL3ImgPathValidator)

lkStaticCoverValidator::lkStaticCoverValidator() : lkSQL3ImgPathValidator()
{
	m_pValid = NULL;
}
lkStaticCoverValidator::lkStaticCoverValidator(lkSQL3FieldData_Text* pField, bool* pValid) : lkSQL3ImgPathValidator(pField)
{
	m_pValid = pValid;
}
lkStaticCoverValidator::lkStaticCoverValidator(const lkStaticCoverValidator& other) : lkSQL3ImgPathValidator(other)
{
	m_pValid = other.m_pValid;
}
lkStaticCoverValidator::~lkStaticCoverValidator()
{
}

//virtual
wxObject* lkStaticCoverValidator::Clone() const
{
	return new lkStaticCoverValidator(*this);
}

bool lkStaticCoverValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

// Called to transfer data TO the window
bool lkStaticCoverValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	lkStaticImage* const control = GetStatImageCtrl();
	if ( !control || !m_pField )
		return false;

	if ( m_pValid && !(*m_pValid) )
		control->SetEmpty(true);
	else
	{
		wxString value = m_pField->GetValue2();
		if ( !value.IsEmpty() )
			value = MakeFullPath(value);

		if ( !value.IsEmpty() )
		{
			// try to extract current field and set it into our control
			control->SetImage(value);
		}
		else
			control->SetEmpty(true);
	}
	return true;
}

// Called to transfer data FROM the window
bool lkStaticCoverValidator::TransferFromWindow()
{
	return true;
}

lkStaticImage* lkStaticCoverValidator::GetStatImageCtrl()
{
	if ( wxDynamicCast(m_validatorWindow, lkStaticImage) )
	{
		return dynamic_cast<lkStaticImage*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3ImgPathValidator can only be used with lkStaticImage");

	return NULL;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkStaticCountyYearValidator, lkSQL3StaticTxtValidator)

lkStaticCountyYearValidator::lkStaticCountyYearValidator() : lkSQL3StaticTxtValidator()
{
	m_pFldDate = NULL;
}
lkStaticCountyYearValidator::lkStaticCountyYearValidator(lkSQL3FieldData* pField, lkSQL3FieldData_Real* pDTfield, bool* pValid) : lkSQL3StaticTxtValidator(pField, pValid)
{
	m_pFldDate = pDTfield;
}
lkStaticCountyYearValidator::lkStaticCountyYearValidator(const lkStaticCountyYearValidator& other) : lkSQL3StaticTxtValidator(other)
{
	m_pFldDate = other.m_pFldDate;
}
lkStaticCountyYearValidator::~lkStaticCountyYearValidator()
{
}

//virtual
wxObject* lkStaticCountyYearValidator::Clone() const
{
	return new lkStaticCountyYearValidator(*this);
}

// Called to transfer data TO the window
bool lkStaticCountyYearValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pFldDate != NULL);

	wxControl* ctrl = GetControl();
	if ( !ctrl || !m_pField || !m_pFldDate )
		return false;

	lkSQL3FieldData_Text* pFldText = NULL;
	if ( wxDynamicCast(m_pField, lkSQL3FieldData_Text) )
		pFldText = dynamic_cast<lkSQL3FieldData_Text*>(m_pField);

	wxString sCountryYear;
	if ( pFldText && m_pFldDate )
		if ( m_pValid && *m_pValid )
		{
			sCountryYear = pFldText->GetValue2();

			if ( !(m_pFldDate->IsNull()) )
			{
				lkDateTime dt = m_pFldDate->GetValue2();
				if ( dt.IsValid() )
				{
					wxString s;
					int y = dt.GetYear();
					if ( y >= 1900 )
						s.Printf(wxT("(%d)"), y);

					if ( !s.IsEmpty() )
					{
						if ( !sCountryYear.IsEmpty() )
							sCountryYear += wxT(" ");
						sCountryYear += s;
					}
				}
			}
		}

	if ( wxDynamicCast(ctrl, wxTextCtrl) )
		(dynamic_cast<wxTextCtrl*>(ctrl))->SetValue(sCountryYear);
	else if ( wxDynamicCast(ctrl, wxStaticText) )
		(dynamic_cast<wxStaticText*>(ctrl))->SetLabel(sCountryYear);
	else if ( wxDynamicCast(ctrl, lkStaticText) )
		(dynamic_cast<lkStaticText*>(ctrl))->SetLabel(sCountryYear);

	return true;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkAudioSubsValidator, wxValidator)

lkAudioSubsValidator::lkAudioSubsValidator() : wxValidator()
{
	m_pField = NULL;
	m_pString = NULL;
}
lkAudioSubsValidator::lkAudioSubsValidator(lkSQL3FieldData_Text* pField) : wxValidator()
{
	m_pField = pField;
	m_pString = NULL;
}
lkAudioSubsValidator::lkAudioSubsValidator(wxString* pString) : wxValidator()
{
	m_pField = NULL;
	m_pString = pString;
}
lkAudioSubsValidator::lkAudioSubsValidator(const lkAudioSubsValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
	m_pString = other.m_pString;
}
lkAudioSubsValidator::~lkAudioSubsValidator()
{
}

//virtual
wxObject* lkAudioSubsValidator::Clone() const
{
	return new lkAudioSubsValidator(*this);
}

wxWindow* lkAudioSubsValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, BaseCountryBox) )
		return m_validatorWindow;

	wxFAIL_MSG(wxT("lkAudioSubsValidator can only be used with a BaseCountryBox"));
	return NULL;
}

//virtual
bool lkAudioSubsValidator::Validate(wxWindow* parent)
{
	// is Optional field, so don't bother
	return true;
}

//virtual
bool lkAudioSubsValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField || m_pString);

	BaseCountryBox* ctrl = NULL;
	{
		wxWindow* w = GetControl();
		if ( w )
			ctrl = dynamic_cast<BaseCountryBox*>(w);
	}
	if ( !ctrl )
		return false;
	
	wxString current;
	if ( m_pField ) // current value from the db
		current = m_pField->GetValue2();
	else if ( m_pString )
		current = *m_pString;

	ctrl->SetCheck(current);

	return true;
}

//virtual
bool lkAudioSubsValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField || m_pString);

	BaseCountryBox* ctrl = NULL;
	{
		wxWindow* w = GetControl();
		if ( w )
			ctrl = dynamic_cast<BaseCountryBox*>(w);
	}
	if ( !ctrl )
		return false;

	wxString current = ctrl->GetChecked();

	// only if previous value (when initialising the controll) was different than now, then tell sql3-db about the new value
	if ( current.IsEmpty() )
	{
		if ( m_pField )
		{
			if ( !m_pField->IsNull() )
				m_pField->SetValueNull();
		}
		else if ( m_pString )
			m_pString->Empty();
	}
	else
	{
		if ( m_pField )
		{
			// current value from the db for Validation
			wxString orig = m_pField->GetValue2();

			if ( !current.IsSameAs(orig) )
				m_pField->SetValue2(current);
		}
		else if ( m_pString )
			*m_pString = current;
	}

	return true;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkGroupValidator, wxValidator)

lkGroupValidator::lkGroupValidator() : wxValidator()
{
	m_pCurRow = NULL;
}
lkGroupValidator::lkGroupValidator(wxUint64* pRow) : wxValidator()
{
	m_pCurRow = pRow;
}
lkGroupValidator::lkGroupValidator(const lkGroupValidator& other) : wxValidator(other)
{
	m_pCurRow = other.m_pCurRow;
}
lkGroupValidator::~lkGroupValidator()
{
}

//virtual
wxObject* lkGroupValidator::Clone() const
{
	return new lkGroupValidator(*this);
}

wxWindow* lkGroupValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, GroupCtrl) )
		return m_validatorWindow;

	wxFAIL_MSG(wxT("lkGroupValidator can only be used with a GroupCtrl"));
	return NULL;
}

//virtual
bool lkGroupValidator::Validate(wxWindow* parent)
{
	// is Optional field, so don't bother
	return true;
}

//virtual
bool lkGroupValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pCurRow != NULL);

	GroupCtrl* ctrl = NULL;
	{
		wxWindow* w = GetControl();
		if ( w )
			ctrl = dynamic_cast<GroupCtrl*>(w);
	}
	if ( !ctrl )
		return false;

	ctrl->SetData(*m_pCurRow);

	return true;
}

//virtual
bool lkGroupValidator::TransferFromWindow()
{
	return true;
}

