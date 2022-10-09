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
#include "CMovies_validators.h"
#include "ImageCtrl.h"

static wxString ValidFailMsg = wxT("Validation should NOT be called when copy of a movies-record is in progress !!");

///////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesDateValidator
wxIMPLEMENT_DYNAMIC_CLASS(MoviesYearValidator, wxTextValidator)

MoviesYearValidator::MoviesYearValidator() : wxTextValidator(wxFILTER_DIGITS)
{
	m_pRS = NULL;
	m_pField = NULL;
	m_pDate = NULL;

	m_bReqAdd = false;
	m_bReqAll = false;

	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;

	SetMinYear(-1);
	SetMaxYear(-1);
	AddOnlyRequired(true);
}
MoviesYearValidator::MoviesYearValidator(lkDateTime* dt, lkSQL3FieldData_Real* fd, lkSQL3RecordSet* rs, MoviesCopy* pMC, MoviesCopy::What mvWhat) : wxTextValidator(wxFILTER_DIGITS)
{
	m_pRS = rs;
	m_pField = fd;
	m_pDate = dt;

	m_bReqAdd = false;
	m_bReqAll = false;

	m_eWhat = mvWhat;
	m_pMoviesCopy = pMC;

	SetMinYear(-1);
	SetMaxYear(-1);
	AddOnlyRequired(true);
}
MoviesYearValidator::MoviesYearValidator(const MoviesYearValidator& other) : wxTextValidator(other)
{
	m_pRS = other.m_pRS;
	m_pField = other.m_pField;
	m_pDate = other.m_pDate;

	m_bReqAdd = other.m_bReqAdd;
	m_bReqAll = other.m_bReqAll;

	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;

	SetMinYear(other.m_nMinYear);
	SetMaxYear(other.m_nMaxYear);
}
MoviesYearValidator::~MoviesYearValidator()
{
}
void MoviesYearValidator::AllwaysRequired(bool b)
{
	m_bReqAll = b;
}
void MoviesYearValidator::AddOnlyRequired(bool b)
{
	m_bReqAdd = b;
}
// <0 will set it to default 1900, 0 = not using minimum when validating
void MoviesYearValidator::SetMinYear(int y)
{
	m_nMinYear = (y < 0) ? 1900 : y;
}
// <0 will set it to default Current Year, 0 = not using maximum when validating
void MoviesYearValidator::SetMaxYear(int y)
{
	m_nMaxYear = (y < 0) ? wxDateTime::GetCurrentYear() : y;
}
//virtual
wxObject* MoviesYearValidator::Clone() const
{
	return new MoviesYearValidator(*this);
}

//virtual
wxString MoviesYearValidator::IsValid(const wxString& str) const
{
	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return wxString();

	wxString err = wxTextValidator::IsValid(str);
	if ( !err.IsEmpty() )
		return err;

	// Now doin our own validation
	if ( (m_pDate == NULL) || (m_pRS == NULL) || (m_pField == NULL) )
		return err;

	unsigned long ul = 0;
	wxString s = str;
	s.Trim(true);
	s.Trim(false);

	if ( str.IsEmpty() || !str.ToULong(&ul) )
		ul = 0;

	if ( ul == 0 )
	{
		if ( m_bReqAll || (m_pField->IsNotNull() && !m_pField->UseDefault()) ) // cannot be null & not using a default value
			err = GetRequiredText();
		else if ( m_bReqAdd && m_pRS->IsAdding() )
			err = GetRequiredText();
	}
	else
	{
		// got something, see if it's valid
		if ( (m_nMinYear > 0) && (ul < (unsigned long)m_nMinYear) )
			err.Printf(wxT("Entered year should be later or the same as %lu."), m_nMinYear);
		else if ( (m_nMaxYear > 0) && (ul > (unsigned long)m_nMaxYear) )
			err.Printf(wxT("Entered year should be the same or before %lu."), m_nMaxYear);
	}

	return err;
}

//virtual
bool MoviesYearValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return wxTextValidator::Validate(parent);
}

// Called to transfer data TO the window
bool MoviesYearValidator::DateTimeToWindow()
{
	wxString szTxt;
	if ( m_pDate && m_pDate->IsValid() )
		szTxt = m_pDate->FormatYearOnly();

	wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(GetTextEntry());
	if ( text )
	{
		text->SetValue(szTxt);
		return true;
	}
	//else..
	return false;
}
bool MoviesYearValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pRS != NULL);
	wxASSERT(m_pDate != NULL);

	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Year) )
	{
		if ( m_pDate )
			*m_pDate = m_pMoviesCopy->GetDate();
		return DateTimeToWindow();
	}
	// else..

	if ( (m_pDate != NULL) && (m_pRS != NULL) && (m_pField != NULL) )
	{
		m_pDate->SetInvalid();
		if ( !m_pRS->IsAdding() )
			if ( !m_pField->IsNull() )
				*m_pDate = m_pField->GetValue2();

		return DateTimeToWindow();
	}
	//else..
	return false;
}

bool MoviesYearValidator::WindowToDateTime()
{
	// using year only
	wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(GetTextEntry());
	if ( !text || !m_pDate )
		return false;

	wxString str = text->GetValue();
	str.Trim(true);
	str.Trim(false);

	unsigned long ul = 0;
	if ( !str.IsEmpty() && str.ToULong(&ul) )
	{
		wxASSERT(ul > 0);
		if ( !m_pDate->IsValid() )
			m_pDate->Set(1, wxDateTime::Jan, (int)ul);
		else
			m_pDate->SetYear((int)ul);
	}
	else
		m_pDate->SetInvalid();

	return true;
}
bool MoviesYearValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxASSERT(m_pDate != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	if ( WindowToDateTime() )
	{
		if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Year) )
		{
			double d1 = (m_pDate) ? (double)(*m_pDate) : lkInvalidDateTime;
			m_pMoviesCopy->SetDate(d1);
			return true;
		}
		//else..

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
	//else..
	return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcImgPathValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcImgPathValidator, lkSQL3ImgPathValidator)

mcImgPathValidator::mcImgPathValidator() : lkSQL3ImgPathValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcImgPathValidator::mcImgPathValidator(const mcImgPathValidator& other) : lkSQL3ImgPathValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcImgPathValidator::mcImgPathValidator(lkSQL3FieldData_Text* pField, MoviesCopy* pMC, MoviesCopy::What mvWhat) : lkSQL3ImgPathValidator(pField)
{
	m_eWhat = mvWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcImgPathValidator::Clone() const
{
	return new mcImgPathValidator(*this);
}
//virtual
bool mcImgPathValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3ImgPathValidator::Validate(parent);
}
//virtual
bool mcImgPathValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		lkStaticImage* const control = GetImageCtrl();
		if ( !control )
			return false;

		wxString value = m_pMoviesCopy->GetCover();
		if ( !value.IsEmpty() )
			value = MakeFullPath(value);

		if ( !value.IsEmpty() )
		{
			// try to extract current field and set it into our control
			control->SetImage(value);
		}
		else
			control->SetEmpty(true);

		return true;
	}
	// else..
	return lkSQL3ImgPathValidator::TransferToWindow();
}
//virtual
bool mcImgPathValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		ImgPathCtrl* const control = dynamic_cast<ImgPathCtrl*>(GetImageCtrl());
		if ( !control )
			return false;

		wxString szPath = control->GetImage();
		m_pMoviesCopy->SetCover(szPath);
		return true;
	}
	// else..
	return lkSQL3ImgPathValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcTextValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcTextValidator, lkSQL3TextValidator)

mcTextValidator::mcTextValidator() : lkSQL3TextValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcTextValidator::mcTextValidator(const mcTextValidator& other) : lkSQL3TextValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcTextValidator::mcTextValidator(lkSQL3FieldData* pField, MoviesCopy* pMC, MoviesCopy::What mvWhat, long style, TextFieldIsUnique_Callback pUnique, lkSQL3Database* pDB, void* param)
	: lkSQL3TextValidator(pField, style, pUnique, pDB, param)
{
	m_eWhat = mvWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcTextValidator::Clone() const
{
	return new mcTextValidator(*this);
}
//virtual
bool mcTextValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3TextValidator::Validate(parent);
}
//virtual
bool mcTextValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxTextEntry* const text = GetTextEntry();
		if ( !text )
			return false;

		wxString str;
		switch ( m_eWhat )
		{
			case MoviesCopy::What::Title:
				str = m_pMoviesCopy->GetTitle();
				break;
			case MoviesCopy::What::SubTitle:
				str = m_pMoviesCopy->GetSubTitle();
				break;
			case MoviesCopy::What::Alias:
				str = m_pMoviesCopy->GetAlias();
				break;
			case MoviesCopy::What::Episode:
				str = m_pMoviesCopy->GetEpisode();
				break;
			case MoviesCopy::What::Country:
				str = m_pMoviesCopy->GetCountry();
				break;
			case MoviesCopy::What::About:
				str = m_pMoviesCopy->GetAbout();
				break;

			default:
				text->SetValue(wxEmptyString);
				break;
		}
		text->SetValue(str);
		return true;
	}
	// else..
	return lkSQL3TextValidator::TransferToWindow();
}
//virtual
bool mcTextValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxTextEntry* const text = GetTextEntry();
		if ( !text )
			return false;

		wxString str = text->GetValue();
		str.Trim(true);
		str.Trim(false);

		switch ( m_eWhat )
		{
			case MoviesCopy::What::Title:
				m_pMoviesCopy->SetTitle(str);
				break;
			case MoviesCopy::What::SubTitle:
				m_pMoviesCopy->SetSubTitle(str);
				break;
			case MoviesCopy::What::Alias:
				m_pMoviesCopy->SetAlias(str);
				break;
			case MoviesCopy::What::Episode:
				m_pMoviesCopy->SetEpisode(str);
				break;
			case MoviesCopy::What::Country:
				m_pMoviesCopy->SetCountry(str);
				break;
			case MoviesCopy::What::About:
				m_pMoviesCopy->SetAbout(str);
				break;

			default:
				break;
		}
		return true;
	}
	// else..
	return lkSQL3TextValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcImgComboValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcImgComboValidator, lkSQL3ImgComboValidator)

mcImgComboValidator::mcImgComboValidator() : lkSQL3ImgComboValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcImgComboValidator::mcImgComboValidator(const mcImgComboValidator& other) : lkSQL3ImgComboValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcImgComboValidator::mcImgComboValidator(lkSQL3FieldData_Int* pField, MoviesCopy* pMC, MoviesCopy::What mcWhat, const wxString& valTitle, bool* bReqAdd)
	: lkSQL3ImgComboValidator(pField, valTitle, bReqAdd)
{
	m_eWhat = mcWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcImgComboValidator::Clone() const
{
	return new mcImgComboValidator(*this);
}
//virtual
bool mcImgComboValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3ImgComboValidator::Validate(parent);
}
//virtual
bool mcImgComboValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		lkImgComboBox* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxUint64 n = 0;
		switch ( m_eWhat )
		{
			case MoviesCopy::What::Cat:
				n = m_pMoviesCopy->GetCategory();
				break;
			case MoviesCopy::What::SubCat:
				n = m_pMoviesCopy->GetSubCat();
				break;
			case MoviesCopy::What::Judge :
				n = m_pMoviesCopy->GetJudge();
				break;
			case MoviesCopy::What::Ratio :
				n = m_pMoviesCopy->GetRatio();
				break;

			default:
				n = 0;
				break;
		}
		ctrl->SetSelectedLParam(n);
		return true;
	}
	//else..

	return lkSQL3ImgComboValidator::TransferToWindow();
}

//virtual
bool mcImgComboValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		lkImgComboBox* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxUint64 n = 0;
		if ( ctrl->GetCount() > 0 )
			n = ctrl->GetSelectedLParam();

		switch ( m_eWhat )
		{
			case MoviesCopy::What::Cat:
				m_pMoviesCopy->SetCategory(n);
				break;
			case MoviesCopy::What::SubCat:
				m_pMoviesCopy->SetSubCat(n);
				break;
			case MoviesCopy::What::Judge:
				m_pMoviesCopy->SetJudge(n);
				break;
			case MoviesCopy::What::Ratio:
				m_pMoviesCopy->SetRatio(n);
				break;

			default:
				break;
		}
		return true;
	}
	//else..

	return lkSQL3ImgComboValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcGenresValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcGenresValidator, GenresValidator)

mcGenresValidator::mcGenresValidator() : GenresValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcGenresValidator::mcGenresValidator(const mcGenresValidator& other) : GenresValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcGenresValidator::mcGenresValidator(lkSQL3FieldData_Int* pField, MoviesCopy* pMC, MoviesCopy::What mcWhat) : GenresValidator(pField)
{
	m_eWhat = mcWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcGenresValidator::Clone() const
{
	return new mcGenresValidator(*this);
}
//virtual
bool mcGenresValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return GenresValidator::Validate(parent);
}

//virtual
bool mcGenresValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Genres) )
	{
		GenresBox* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxUint64 n = m_pMoviesCopy->GetGenres();
		ctrl->SetCheck(n);
		return true;
	}
	//else..

	return GenresValidator::TransferToWindow();
}

//virtual
bool mcGenresValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);

	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Genres) )
	{
		GenresBox* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxUint64 n = ctrl->GetChecked();
		m_pMoviesCopy->SetGenres(n);
		return true;
	}
	//else..

	return GenresValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcGenresValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcDateValidator, lkSQL3DateValidator)

mcDateValidator::mcDateValidator() : lkSQL3DateValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcDateValidator::mcDateValidator(const mcDateValidator& other) : lkSQL3DateValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcDateValidator::mcDateValidator(lkDateTime* dt, lkSQL3FieldData_Real* pField, lkSQL3RecordSet* rs, MoviesCopy* pMC, MoviesCopy::What mcWhat, bool longDate, bool dutchFormat)
	: lkSQL3DateValidator(dt, pField, rs, longDate, dutchFormat)
{
	m_eWhat = mcWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcDateValidator::Clone() const
{
	return new mcDateValidator(*this);
}
//virtual
bool mcDateValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3DateValidator::Validate(parent);
}

//virtual
bool mcDateValidator::TransferToWindow()
{
	wxASSERT(m_pDate != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Date) )
	{
		if ( m_pDate )
			*m_pDate = m_pMoviesCopy->GetDate();
		return DateTimeToWindow();
	}
	// else..

	return lkSQL3DateValidator::TransferToWindow();
}

//virtual
bool mcDateValidator::TransferFromWindow()
{
	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxASSERT(m_pDate != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::Date) )
	{
		double d1 = (m_pDate) ? (double)(*m_pDate) : lkInvalidDateTime;
		m_pMoviesCopy->SetDate(d1);
		return true;
	}
	//else..

	return lkSQL3DateValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcChkBoxValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcChkBoxValidator, lkSQL3ChkBoxValidator)

mcChkBoxValidator::mcChkBoxValidator() : lkSQL3ChkBoxValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcChkBoxValidator::mcChkBoxValidator(const mcChkBoxValidator& other) : lkSQL3ChkBoxValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcChkBoxValidator::mcChkBoxValidator(lkSQL3FieldData_Int* pField, MoviesCopy* pMC, MoviesCopy::What mcWhat) : lkSQL3ChkBoxValidator(pField)
{
	m_eWhat = mcWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcChkBoxValidator::Clone() const
{
	return new mcChkBoxValidator(*this);
}
//virtual
bool mcChkBoxValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3ChkBoxValidator::Validate(parent);
}

//virtual
bool mcChkBoxValidator::TransferToWindow()
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxCheckBox* cb = GetCheckbox();
		if ( !cb )
			return false;
		bool b = false;

		switch ( m_eWhat )
		{
			case MoviesCopy::What::YrBox:
				b = m_pMoviesCopy->GetYrBox();
				break;
			case MoviesCopy::What::Serie:
				b = m_pMoviesCopy->GetSerie();
				break;

			default:
				b = false;
				break;
		}

		if ( cb->Is3State() )
			cb->Set3StateValue(b ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
		else
			cb->SetValue(b);
		return true;
	}
	//else..
	return lkSQL3ChkBoxValidator::TransferToWindow();
}

//virtual
bool mcChkBoxValidator::TransferFromWindow()
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		// If window is disabled, simply return
		if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
			return true;

		wxCheckBox* cb = GetCheckbox();
		if ( !cb )
			return false;

		bool n;
		if ( cb->Is3State() )
			n = (cb->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED);
		else
			n = cb->IsChecked();

		switch ( m_eWhat )
		{
			case MoviesCopy::What::YrBox:
				m_pMoviesCopy->SetYrBox(n);
				break;
			case MoviesCopy::What::Serie:
				m_pMoviesCopy->SetSerie(n);
				break;

			default:
				break;
		}
		return true;
	}
	//else..
	return lkSQL3ChkBoxValidator::TransferFromWindow();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mcHyperlinkValidator

wxIMPLEMENT_DYNAMIC_CLASS(mcHyperlinkValidator, lkSQL3HyperlinkValidator)

mcHyperlinkValidator::mcHyperlinkValidator() : lkSQL3HyperlinkValidator()
{
	m_eWhat = MoviesCopy::What::Invalid;
	m_pMoviesCopy = NULL;
}
mcHyperlinkValidator::mcHyperlinkValidator(const mcHyperlinkValidator& other) : lkSQL3HyperlinkValidator(other)
{
	m_eWhat = other.m_eWhat;
	m_pMoviesCopy = other.m_pMoviesCopy;
}
mcHyperlinkValidator::mcHyperlinkValidator(lkSQL3FieldData_Text* pField, MoviesCopy* pMC, MoviesCopy::What mcWhat) : lkSQL3HyperlinkValidator(pField)
{
	m_eWhat = mcWhat;
	m_pMoviesCopy = pMC;
}

//virtual
wxObject* mcHyperlinkValidator::Clone() const
{
	return new mcHyperlinkValidator(*this);
}
//virtual
bool mcHyperlinkValidator::Validate(wxWindow* parent)
{
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() )
	{
		wxFAIL_MSG(ValidFailMsg);
		return true;
	}

	return lkSQL3HyperlinkValidator::Validate(parent);
}
//virtual
bool mcHyperlinkValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::URL) )
	{
		lkHyperlinkCtrl* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxString url = m_pMoviesCopy->GetURL();
		if ( url.IsEmpty() )
			ctrl->SetEmpty();
		else
		{
			wxString szUrl, szLabel;
			ExtractDbHyperlink(url, szLabel, szUrl);
			if ( szLabel.IsEmpty() )
				szLabel = szUrl;

			ctrl->SetURL(szUrl);
			ctrl->SetLabel(szLabel);
		}
		return true;
	}
	//else..
	return lkSQL3HyperlinkValidator::TransferToWindow();
}
//virtual
bool mcHyperlinkValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	if ( m_pMoviesCopy && m_pMoviesCopy->IsInUse() && (m_eWhat == MoviesCopy::What::URL) )
	{
		lkHyperlinkCtrl* ctrl = GetControl();
		if ( !ctrl )
			return false;

		wxString szUrl = ctrl->GetURL();
		wxString szLabel = ctrl->GetLabel();

		wxString szHyper;

		if ( !szUrl.IsEmpty() )
		{
			if ( !szLabel.IsEmpty() && szLabel.IsSameAs(szUrl, false) )
				szLabel = wxT("");

			MakeDbHyperlink(szHyper, szLabel, szUrl);
		}
		m_pMoviesCopy->SetURL(szHyper);
		return true;
	}
	//else..
	return lkSQL3HyperlinkValidator::TransferFromWindow();
}
