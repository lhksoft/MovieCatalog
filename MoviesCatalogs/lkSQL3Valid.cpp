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
#include "lkSQL3Valid.h"
#include "ImageCtrl.h"

#include "../lkControls/lkStaticText.h"
#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkControls/lkConfigTools.h"
#include "TMovies.h" // for conf_MOVIES_PATH, conf_MOVIES_COVERS
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/msgdlg.h>

static const wxChar* _tRequiredText = wxT("This field is required and cannot be empty.");
static const wxChar* _tNotUnique = wxT("Entered value already exists.\nOnly UNIQUE values are allowed.");


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3StaticTxtValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3StaticTxtValidator, wxValidator)

lkSQL3StaticTxtValidator::lkSQL3StaticTxtValidator() : wxValidator()
{
	m_pField = NULL;
	m_pValid = NULL;
}
lkSQL3StaticTxtValidator::lkSQL3StaticTxtValidator(lkSQL3FieldData* pField, bool* pValid) : wxValidator()
{
	m_pField = pField;
	m_pValid = pValid;
}
lkSQL3StaticTxtValidator::lkSQL3StaticTxtValidator(const lkSQL3StaticTxtValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
	m_pValid = other.m_pValid;
}
lkSQL3StaticTxtValidator::~lkSQL3StaticTxtValidator()
{
}

//virtual
wxObject* lkSQL3StaticTxtValidator::Clone() const
{
	return new lkSQL3StaticTxtValidator(*this);
}

wxControl* lkSQL3StaticTxtValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, wxTextCtrl) && (dynamic_cast<wxTextCtrl*>(m_validatorWindow))->HasFlag(wxTE_READONLY) )
	{
		return dynamic_cast<wxTextCtrl*>(m_validatorWindow);
	}
	else if ( wxDynamicCast(m_validatorWindow, wxStaticText) )
	{
		return dynamic_cast<wxStaticText*>(m_validatorWindow);
	}
	else if ( wxDynamicCast(m_validatorWindow, lkStaticText) )
	{
		return dynamic_cast<lkStaticText*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3StaticTxtValidator can only be used with wxStaticText/lkStaticText and a read-only wxTextCtrl");

	return NULL;
}

bool lkSQL3StaticTxtValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

// Called to transfer data TO the window
bool lkSQL3StaticTxtValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	wxControl* ctrl = GetControl();
	if ( !ctrl || !m_pField )
		return false;

	wxString value;
	if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Text)) )
	{
		if ( (m_pValid && *m_pValid) || !m_pValid )
			value = (dynamic_cast<lkSQL3FieldData_Text*>(m_pField))->GetValue2();
	}
	else if ( m_pField->IsKindOf(wxCLASSINFO(lkSQL3FieldData_Int)) )
	{
		if ( (m_pValid && *m_pValid) || !m_pValid )
		{
			wxUint64 val = static_cast<wxUint64>((dynamic_cast<lkSQL3FieldData_Int*>(m_pField))->GetValue2());
			value.Printf(wxT("%llu"), val);
		}
	}
	else
	{
		wxASSERT(false);
		// no other types of Fields supported
		return false;
	}

	if ( wxDynamicCast(ctrl, wxTextCtrl) )
		(dynamic_cast<wxTextCtrl*>(ctrl))->SetValue(value);
	else if ( wxDynamicCast(ctrl, wxStaticText) )
		(dynamic_cast<wxStaticText*>(ctrl))->SetLabel(value);
	else if ( wxDynamicCast(ctrl, lkStaticText) )
		(dynamic_cast<lkStaticText*>(ctrl))->SetLabel(value);

	return true;
}

// Called to transfer data FROM the window
bool lkSQL3StaticTxtValidator::TransferFromWindow()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3ImageValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3ImageValidator, wxValidator)

lkSQL3ImageValidator::lkSQL3ImageValidator() : wxValidator()
{
	m_pField = NULL;
}
lkSQL3ImageValidator::lkSQL3ImageValidator(lkSQL3FieldData_Image* pField) : wxValidator()
{
	m_pField = pField;
}
lkSQL3ImageValidator::lkSQL3ImageValidator(const lkSQL3ImageValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
}

//virtual
wxObject* lkSQL3ImageValidator::Clone() const
{
	return new lkSQL3ImageValidator(*this);
}

lkStaticImage* lkSQL3ImageValidator::GetImageCtrl()
{
	if ( wxDynamicCast(m_validatorWindow, lkStaticImage) )
	{
		return (lkStaticImage*)m_validatorWindow;
	}

	wxFAIL_MSG("lkSQL3TextValidator can only be used with lkStaticImage");

	return NULL;
}

bool lkSQL3ImageValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

bool lkSQL3ImageValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	lkStaticImage* const control = GetImageCtrl();
	if ( !control || !m_pField )
		return false;

	wxString _name;
	control->SetImage(m_pField->GetAsImage(_name));

	if ( control->IsKindOf(wxCLASSINFO(ImageCtrl)) )
		(dynamic_cast<ImageCtrl*>(control))->SetImageName(_name);

	return true;
}

bool lkSQL3ImageValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkStaticImage* const control = GetImageCtrl();
	if ( !control || !m_pField )
		return false;

	wxString _name;
	if ( control->IsKindOf(wxCLASSINFO(ImageCtrl)) )
		_name = (dynamic_cast<ImageCtrl*>(control))->GetImageName();

	m_pField->SetFromImage(control->GetImage(), _name);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3CountryChkbValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3CountryChkbValidator, wxValidator)

lkSQL3CountryChkbValidator::lkSQL3CountryChkbValidator() : wxValidator()
{
	m_pField = NULL;
	m_nVal = 0;
}
lkSQL3CountryChkbValidator::lkSQL3CountryChkbValidator(lkSQL3FieldData_Int* pField, AudioSubs val) : wxValidator()
{
	m_pField = pField;
	m_nVal = val;
}
lkSQL3CountryChkbValidator::lkSQL3CountryChkbValidator(const lkSQL3CountryChkbValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
	m_nVal = other.m_nVal;
}

//virtual
wxObject* lkSQL3CountryChkbValidator::Clone() const
{
	return new lkSQL3CountryChkbValidator(*this);
}

wxCheckBox* lkSQL3CountryChkbValidator::GetCheckbox()
{
	if ( wxDynamicCast(m_validatorWindow, wxCheckBox) )
		return dynamic_cast<wxCheckBox*>(m_validatorWindow);

	wxFAIL_MSG(wxT("lkSQL3CountryChkbValidator can only be used with a wxCheckBox"));

	return NULL;
}

//virtual
bool lkSQL3CountryChkbValidator::Validate(wxWindow* WXUNUSED(parent))
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	return true;
}

//virtual
bool lkSQL3CountryChkbValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);
	wxCheckBox* cb = GetCheckbox();
	if ( !cb || !m_pField )
		return false;

	// current value from the db
	int n = static_cast<int>(m_pField->GetValue2());

	if ( cb->Is3State() )
		cb->Set3StateValue((n & m_nVal) ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
	else
		cb->SetValue(n & m_nVal);

	return true;
}

//virtual
bool lkSQL3CountryChkbValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	wxCheckBox* cb = GetCheckbox();
	if ( !cb || !m_pField )
		return false;

	int p = static_cast<int>(m_pField->GetValue2());
	int n;
	if ( cb->Is3State() )
		n = (cb->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED) ? m_nVal : 0;
	else
		n = (cb->IsChecked()) ? m_nVal : 0;

	// only if previous value (when initialising the controll) was different than now, then tell sql3-db about the new value
	if ( n == 0 )
	{
		if ( p & m_nVal )
		{
			p &= ~m_nVal;
			m_pField->SetValue2(static_cast<wxInt64>(p));
		}
	}
	else
	{
		if ( !(p & m_nVal) )
		{
			p |= m_nVal;
			m_pField->SetValue2(static_cast<wxInt64>(p));
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3ImgComboValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3ImgComboValidator, wxValidator)

lkSQL3ImgComboValidator::lkSQL3ImgComboValidator() : wxValidator(), m_sValTitle()
{
	m_pField = NULL;
	m_bReqAdd = NULL;
}
lkSQL3ImgComboValidator::lkSQL3ImgComboValidator(lkSQL3FieldData_Int* pField, const wxString& valTitle, bool* bReqAdd) : wxValidator(), m_sValTitle(valTitle)
{
	m_pField = pField;
	m_bReqAdd = bReqAdd;
}
lkSQL3ImgComboValidator::lkSQL3ImgComboValidator(const lkSQL3ImgComboValidator& other) : wxValidator(other), m_sValTitle()
{
	m_pField = other.m_pField;
	m_bReqAdd = other.m_bReqAdd;
	m_sValTitle = other.m_sValTitle;
}

//virtual
wxObject* lkSQL3ImgComboValidator::Clone() const
{
	return new lkSQL3ImgComboValidator(*this);
}

lkImgComboBox* lkSQL3ImgComboValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, lkImgComboBox) )
		return dynamic_cast<lkImgComboBox*>(m_validatorWindow);

	wxFAIL_MSG(wxT("lkSQL3ImgComboValidator can only be used with a lkImgComboBox"));

	return NULL;
}

//virtual
bool lkSQL3ImgComboValidator::Validate(wxWindow* parent)
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	wxUint64 curSel = 0; // not using if ==0
	if ( ctrl->GetCount() > 0 )
		curSel = ctrl->GetSelectedLParam();

	wxString msg;
	if ( curSel == 0 )
		if ( (m_pField->IsNotNull() && !m_pField->UseDefault()) || (m_bReqAdd && *m_bReqAdd) )
			msg = wxT("No selection set yet, allthough this field is required.");

	if ( !msg.IsEmpty() )
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(msg, (m_sValTitle.IsEmpty()) ? wxT("Validation conflict") : m_sValTitle, wxOK | wxICON_EXCLAMATION, parent);

		return false;
	}

	return true;
}

//virtual
bool lkSQL3ImgComboValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	// current value from the db
	wxUint64 n = static_cast<wxUint64>(m_pField->GetValue2());

	ctrl->SetSelectedLParam(n);

	return true;
}

//virtual
bool lkSQL3ImgComboValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkImgComboBox* ctrl = GetControl();
	if ( !ctrl )
		return false;

	wxUint64 n = 0;
	if ( ctrl->GetCount() > 0 )
		n = ctrl->GetSelectedLParam();

	// only if previous value (when initialising the controll) was different than now, then tell sql3-db about the new value
	if ( n == 0 )
	{
		if ( !m_pField->IsNull() )
			m_pField->SetValueNull();
	}
	else
	{
		// current value from the db for Validation
		wxUint64 p = static_cast<wxUint64>(m_pField->GetValue2());

		if ( p != n )
			m_pField->SetValue2(static_cast<wxInt64>(n));
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lkSQL3ImgPathValidator

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3ImgPathValidator, wxValidator)

lkSQL3ImgPathValidator::lkSQL3ImgPathValidator() : wxValidator(), m_sCovers()
{
	m_pField = NULL;
	GetCoversPath();
}
lkSQL3ImgPathValidator::lkSQL3ImgPathValidator(lkSQL3FieldData_Text* pFld) : wxValidator(), m_sCovers()
{
	m_pField = pFld;
	GetCoversPath();
}
lkSQL3ImgPathValidator::lkSQL3ImgPathValidator(const lkSQL3ImgPathValidator& other) : wxValidator(other), m_sCovers(other.m_sCovers)
{
	m_pField = other.m_pField;
}

void lkSQL3ImgPathValidator::GetCoversPath()
{
	wxStandardPathsBase& stdp = wxStandardPaths::Get();
	wxString base = stdp.GetAppDocumentsDir();
	if (base.Right(1) != wxFileName::GetPathSeparator())
		base += wxFileName::GetPathSeparator();

	m_sCovers = GetConfigString(conf_MOVIES_PATH, conf_MOVIES_COVERS);
	if (!m_sCovers.IsEmpty())
	{
		if ((m_sCovers.Find(wxT("/")) == wxNOT_FOUND) && (m_sCovers.Find(wxT("\\")) == wxNOT_FOUND))
		{ // assuming not a full path, just a subdir inside App-Data-dir
			m_sCovers = (base + m_sCovers);
		}
		// else, it's a full-path
	}
	else
		m_sCovers = base; // old-style

	if (!m_sCovers.IsEmpty())
	{
		if (wxFileName::GetPathSeparator() == '\\')
		{
			if (m_sCovers.Find('/') != wxNOT_FOUND)
				m_sCovers.Replace(wxT("/"), wxT("\\"), true);
		}
		else if (wxFileName::GetPathSeparator() == '/')
		{
			if (m_sCovers.Find('\\') != wxNOT_FOUND)
				m_sCovers.Replace(wxT("\\"), wxT("/"), true);
		}
		if (!wxDirExists(m_sCovers))
			m_sCovers = wxT("");
		else
		{
			if (m_sCovers.Right(1) != wxFileName::GetPathSeparator())
				m_sCovers += wxFileName::GetPathSeparator();
		}
	}
}

//virtual
wxObject* lkSQL3ImgPathValidator::Clone() const
{
	return new lkSQL3ImgPathValidator(*this);
}

lkStaticImage* lkSQL3ImgPathValidator::GetImageCtrl()
{
	if ( wxDynamicCast(m_validatorWindow, ImgPathCtrl) )
	{
		return dynamic_cast<ImgPathCtrl*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3ImgPathValidator can only be used with ImgPathCtrl");

	return NULL;
}

wxString lkSQL3ImgPathValidator::MakeFullPath(const wxString& partial)
{
	wxString value = partial;
	if ( !value.IsEmpty() )
	{
		if ( wxFileName::GetPathSeparator() == '\\' )
		{
			if ( value.Find('/') != wxNOT_FOUND )
				value.Replace(wxT("/"), wxT("\\"), true);
		}
		else if ( wxFileName::GetPathSeparator() == '/' )
		{
			if ( value.Find('\\') != wxNOT_FOUND )
				value.Replace(wxT("\\"), wxT("/"), true);
		}

		if (!wxFileExists(value))
		{ // no full path
			if (!m_sCovers.IsEmpty())
			{
				wxString sCovers = m_sCovers;
				sCovers += value;
				if (wxFileExists(sCovers))
					value = sCovers;
				else
					value = wxT("");
			}
			else
				value = wxT("");
/*
			else
			{ // old-style
				if (!m_sBasePath.IsEmpty())
				{
					wxString s = m_sBasePath;
					if ((value[0] != wxFileName::GetPathSeparator()) && (s[s.Len() - 1] != wxFileName::GetPathSeparator()))
						s += wxFileName::GetPathSeparator();
					s += value;

					if (wxFileExists(s))
						value = s;
					else
						value = wxT("");
				}
				else
					value = wxT("");
			}
*/
		}
	}

	return value;
}

bool lkSQL3ImgPathValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

bool lkSQL3ImgPathValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	lkStaticImage* const control = GetImageCtrl();
	if ( !control || !m_pField )
		return false;

	wxString value = m_pField->GetValue2();
	if ( !value.IsEmpty() )
		value = MakeFullPath(value);

	if ( !value.IsEmpty() )
		control->SetImage(value);
	else
		control->SetEmpty(true);

	return true;
}

bool lkSQL3ImgPathValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	ImgPathCtrl* const control = dynamic_cast<ImgPathCtrl*>(GetImageCtrl());
	if ( !control || !m_pField )
		return false;

	wxString value = control->GetImage();
	if ( value.IsEmpty() )
	{
		wxString orig = m_pField->GetValue2();
		if (!orig.IsEmpty())
		{
			orig = MakeFullPath(orig);
			if (orig.IsEmpty())
				return true; // leave in the DB as is, maybe no acces to the path but then don't remove the entry
			/*
			if ( wxFileName::GetPathSeparator() == '\\' )
			{
				if ( orig.Find('/') != wxNOT_FOUND )
					orig.Replace(wxT("/"), wxT("\\"), true);
			}
			else if ( wxFileName::GetPathSeparator() == '/' )
			{
				if ( orig.Find('\\') != wxNOT_FOUND )
					orig.Replace(wxT("\\"), wxT("/"), true);
			}

			if ( !wxFileExists(orig) )
			{
				if ( !m_sBasePath.IsEmpty() )
				{
					wxString s = m_sBasePath;
					if ( s[s.Len() - 1] != wxFileName::GetPathSeparator() )
						s += wxFileName::GetPathSeparator();
					s += orig;
					if ( wxFileExists(s) )
						orig = s;
				}
				if ( !wxFileExists(orig) )
					return true; // leave in the DB as is, maybe no acces to the path but then don't remove the entry
			}
*/
			m_pField->SetValueNull();
		}
	}
	else
	{
		// got data
		if ( wxFileExists(value) )
		{
			wxString orig = MakeFullPath(m_pField->GetValue2());
			if ( orig.IsEmpty() || !value.IsSameAs(orig, false) )
			{
				if ( !m_sCovers.IsEmpty() )
				{
					wxString s = m_sCovers;
					wxString szLeft = value.Left(s.Len());
					if ( szLeft.IsSameAs(s, false) )
					{
						// extraxt basepath from path
						size_t len = s.Len();
						s = value.Mid(len);
						value = s;
					}
				}

				m_pField->SetValue2(value);
			}
		}
	}

	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

wxIMPLEMENT_DYNAMIC_CLASS(lkSQL3HyperlinkValidator, wxValidator)

lkSQL3HyperlinkValidator::lkSQL3HyperlinkValidator() : wxValidator()
{
	m_pField = NULL;
}
lkSQL3HyperlinkValidator::lkSQL3HyperlinkValidator(lkSQL3FieldData_Text* pField) : wxValidator()
{
	m_pField = pField;
}
lkSQL3HyperlinkValidator::lkSQL3HyperlinkValidator(const lkSQL3HyperlinkValidator& other) : wxValidator(other)
{
	m_pField = other.m_pField;
}
lkSQL3HyperlinkValidator::~lkSQL3HyperlinkValidator()
{
}

//virtual
wxObject* lkSQL3HyperlinkValidator::Clone() const
{
	return new lkSQL3HyperlinkValidator(*this);
}

lkHyperlinkCtrl* lkSQL3HyperlinkValidator::GetControl()
{
	if ( wxDynamicCast(m_validatorWindow, lkHyperlinkCtrl) )
	{
		return dynamic_cast<lkHyperlinkCtrl*>(m_validatorWindow);
	}

	wxFAIL_MSG("lkSQL3HyperlinkValidator can only be used with lkHyperlinkCtrl");

	return NULL;
}

bool lkSQL3HyperlinkValidator::Validate(wxWindow* WXUNUSED(parent))
{
	return true;
}

// Called to transfer data TO the window
bool lkSQL3HyperlinkValidator::TransferToWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	lkHyperlinkCtrl* ctrl = GetControl();
	if ( !ctrl || !m_pField )
		return false;

	ctrl->SetVisited(false);
	if ( m_pField->IsNull() )
		ctrl->SetEmpty();
	else
	{
		wxString szUrl, szLabel;
		ExtractDbHyperlink(m_pField->GetValue2(), szLabel, szUrl);
		if ( szLabel.IsEmpty() )
			szLabel = szUrl;

		ctrl->SetURL(szUrl);
		ctrl->SetLabel(szLabel);
	}

	return true;
}

// Called to transfer data FROM the window
bool lkSQL3HyperlinkValidator::TransferFromWindow()
{
	wxASSERT(m_validatorWindow != NULL);
	wxASSERT(m_pField != NULL);

	// If window is disabled, simply return
	if ( !m_validatorWindow || !m_validatorWindow->IsEnabled() )
		return true;

	lkHyperlinkCtrl* ctrl = GetControl();
	if ( !ctrl || !m_pField )
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

	if ( szHyper.IsEmpty() )
	{
		if ( !m_pField->IsNull() )
			m_pField->SetValueNull();
	}
	else
	{
		wxString szOrig = m_pField->GetValue2();
		if ( szOrig.IsEmpty() || !szOrig.IsSameAs(szHyper, false) )
			m_pField->SetValue2(szHyper);
	}

	return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

