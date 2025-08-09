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
#include "DGroup_validators.h"
#include "../lkSQLite3/lkDateTime.h"
#include <wx/stattext.h>
#include "../lkControls/lkStaticText.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DGroupStaticImageValidator
////
wxIMPLEMENT_DYNAMIC_CLASS(DGroupStaticImageValidator, lkSQL3ImageValidator)

DGroupStaticImageValidator::DGroupStaticImageValidator() : lkSQL3ImageValidator()
{
	m_pValid = NULL;
}
DGroupStaticImageValidator::DGroupStaticImageValidator(lkSQL3FieldData_Image* pField, bool* pValid) : lkSQL3ImageValidator(pField)
{
	m_pValid = pValid;
}
DGroupStaticImageValidator::DGroupStaticImageValidator(const DGroupStaticImageValidator& other) : lkSQL3ImageValidator(other)
{
	m_pValid = other.m_pValid;
}

//virtual
wxObject* DGroupStaticImageValidator::Clone() const
{
	return new DGroupStaticImageValidator(*this);
}
//virtual
bool DGroupStaticImageValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}
//virtual
bool DGroupStaticImageValidator::TransferFromWindow()
{
	return true;
}
bool DGroupStaticImageValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	lkStaticImage* const control = GetImageCtrl();
	if ( !control )
		return false;

	if ( m_pValid && !(*m_pValid) )
		control->SetEmpty(true);
	else
		return lkSQL3ImageValidator::TransferToWindow();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class DGroupStaticDateValidator
////
wxIMPLEMENT_DYNAMIC_CLASS(DGroupStaticDateValidator, wxValidator)

DGroupStaticDateValidator::DGroupStaticDateValidator() : wxValidator()
{
	m_pField = NULL;
	m_pValid = NULL;
	m_bLong = true;
	m_bDutch = false;
}
DGroupStaticDateValidator::DGroupStaticDateValidator(lkSQL3FieldData_Real* pField, bool* pValid, bool longDate, bool dutchFormat) : wxValidator()
{
	m_pField = pField;
	m_pValid = pValid;
	m_bLong = longDate;
	m_bDutch = dutchFormat;
}
DGroupStaticDateValidator::DGroupStaticDateValidator(const DGroupStaticDateValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
	m_pValid = other.m_pValid;
	m_bLong = other.m_bLong;
	m_bDutch = other.m_bDutch;
}
wxWindow* DGroupStaticDateValidator::GetStaticControl()
{
	if ( wxDynamicCast(m_validatorWindow, wxStaticText) ||
		wxDynamicCast(m_validatorWindow, lkStaticText) )
	{
		return m_validatorWindow;
	}

	wxFAIL_MSG("DGroupStaticDateValidator can only be used with wxStaticText/lkStaticText");

	return NULL;
}

//virtual
wxObject* DGroupStaticDateValidator::Clone() const
{
	return new DGroupStaticDateValidator(*this);
}
bool DGroupStaticDateValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}
//virtual
bool DGroupStaticDateValidator::TransferFromWindow()
{
	return true;
}
//virtual
bool DGroupStaticDateValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	wxWindow* statWnd = GetStaticControl();
	if ( !statWnd ) return true;

	lkDateTime m_Date;
	if ( m_pField && !m_pField->IsNull() )
		m_Date = m_pField->GetValue2();
	else
		m_Date.SetInvalid();

	if ( m_pValid && !(*m_pValid) )
		m_Date.SetInvalid();

	wxString szTxt;
	if ( m_bDutch )
		szTxt = m_Date.FormatFullDate_Dutch(m_bLong);
	else
		szTxt = m_Date.FormatFullDate_English(m_bLong);

	if ( szTxt.IsEmpty() ) // maybe m_Date is InValid
		szTxt = wxT("# unset #");

	if ( statWnd->IsKindOf(wxCLASSINFO(wxStaticText)) )
		(dynamic_cast<wxStaticText*>(statWnd))->SetLabel(szTxt);
	else if ( statWnd->IsKindOf(wxCLASSINFO(lkStaticText)) )
		(dynamic_cast<lkStaticText*>(statWnd))->SetLabel(szTxt);

	return true;
}
