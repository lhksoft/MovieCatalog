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
#include "CMovies.h"

#include "DlgDate.h"
#include "DCountry.h"

#include "Backgrounds.h"
#include "TMovies.h"

#include "../lkControls/lkHyperlinkCtrl.h"
#include "../lkControls/lkColour.h"

//#include "lkSQL3Valid.h"
//#include <lk/lkSQL3Validators.h>
#include "CMovies_validators.h"
//#include "MoviesCopy.h"

#include "../lkControls/lkConfigTools.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/config.h>
#include <wx/statbox.h>

#define conf_MOVIES_PATH				wxT("Movies")


///////////////////////////////////////////////////////////////////////////////////////////////
// class CMovies
////
CMovies::CMovies() : m_Date()
{
	m_pParentFrame = NULL;
	m_pParentPanel = NULL;

	m_pImage = NULL;
	m_pTitleCtrl = NULL;
	m_pCountry = NULL;
	m_pDateTxt = NULL;
	m_pYearTxt = NULL;
	m_pCategory = NULL;
	m_pSubCat = NULL;
	m_pGenres = NULL;
	m_pJudge = NULL;
	m_pRating = NULL;
	m_pChangeDateBtn = NULL;

	m_bYear = false;
	m_bReqAdd = false;

	m_pMoviesCopy = new MoviesCopy();

	m_ID_DateBtn = wxWindow::NewControlId(1);
	m_ID_CountryBtn = wxWindow::NewControlId(1);
	m_ID_YearChkb = wxWindow::NewControlId(1);
}
CMovies::~CMovies()
{
	if ( m_pMoviesCopy )
		delete m_pMoviesCopy;
}

//static
wxString CMovies::GetConfigKeyString()
{
	wxString s = conf_MOVIES_PATH;
	return s;
}

wxImage CMovies::GetBackground() const
{
	return GetImage_FBG_MOVIES();
}

void CMovies::SetMoviesFocus()
{
	if ( m_pTitleCtrl )
		m_pTitleCtrl->SetFocus();
}

void CMovies::OnSelectDate(wxCommandEvent& event)
{
	DlgDate dlg(m_Date);
	if ( dlg.Create(m_pParentFrame) )
		if ( dlg.ShowModal() == wxID_OK )
		{
			m_Date = dlg.GetDate();

			if ( m_pDateTxt )
			{
				lkSQL3DateValidator* val = NULL;
				wxValidator* v = m_pDateTxt->GetValidator();
				if ( v && v->IsKindOf(wxCLASSINFO(lkSQL3DateValidator)) )
					val = dynamic_cast<lkSQL3DateValidator*>(v);

				// don't validate, just put the date into the control
				val->DateTimeToWindow(true);
			}
		}
}

void CMovies::OnYearCheck(wxCommandEvent& event)
{
	event.Skip();
	if ( m_pDateTxt )
		m_bYear = event.IsChecked();

	m_pDateTxt->Enable(!m_bYear);
	m_pDateTxt->Show(!m_bYear);
	m_pYearTxt->Enable(m_bYear);
	m_pYearTxt->Show(m_bYear);
	m_pChangeDateBtn->Enable(!m_bYear);
	m_pChangeDateBtn->Show(!m_bYear);

	if ( m_bYear )
	{
		MoviesYearValidator* valY = NULL;
		wxValidator* v = m_pYearTxt->GetValidator();
		if ( v && v->IsKindOf(wxCLASSINFO(MoviesYearValidator)) )
			valY = dynamic_cast<MoviesYearValidator*>(v);

		// don't validate, just put the year of the date into the control
		valY->DateTimeToWindow();
	}
	else
	{
		wxValidator* v;
		MoviesYearValidator* valY = NULL;
		v = m_pYearTxt->GetValidator();
		if ( v && v->IsKindOf(wxCLASSINFO(MoviesYearValidator)) )
			valY = dynamic_cast<MoviesYearValidator*>(v);

		lkSQL3DateValidator* valS = NULL;
		v = m_pDateTxt->GetValidator();
		if ( v && v->IsKindOf(wxCLASSINFO(lkSQL3DateValidator)) )
			valS = dynamic_cast<lkSQL3DateValidator*>(v);

		// don't validate, just put the year of the date into the control
		valY->WindowToDateTime();
		valS->DateTimeToWindow(true);
	}

	m_pParentPanel->Layout();
}

void CMovies::OnPickCountry(wxCommandEvent& event)
{
	wxString ctr = (m_pCountry) ? m_pCountry->GetValue() : wxT("");
	DCountry dlg(ctr, GetDBase());
	if ( dlg.Create(m_pParentFrame) )
	{
		if (dlg.ShowModal() == wxID_OK)
			if ( m_pCountry )
				m_pCountry->SetValue(dlg.GetCountryString());
	}
}


bool CMovies::mvTransferDataToWindow(bool bInitial)
{
	if ( m_pCategory )
		if ( bInitial || IsCategoryChanged() )
			m_pCategory->BuildCatList(!bInitial);

	if ( m_pJudge )
		if ( bInitial || IsJudgesChanged() )
			m_pJudge->BuildList(!bInitial);

	if ( m_pRating )
		if ( bInitial || IsRatingChanged() )
			m_pRating->BuildList(!bInitial);

	if ( m_pGenres )
		if ( bInitial || IsGenresChanged() )
			m_pGenres->BuildList();

	m_bYear = GetYearValue();

	m_pDateTxt->Enable(!m_bYear);
	m_pDateTxt->Show(!m_bYear);
	m_pYearTxt->Enable(m_bYear);
	m_pYearTxt->Show(m_bYear);
	m_pChangeDateBtn->Enable(!m_bYear);
	m_pChangeDateBtn->Show(!m_bYear);

	m_pParentPanel->Layout();

	return true;
}

bool CMovies::MoviesCopyInProgress() const
{
	return (m_pMoviesCopy) ? m_pMoviesCopy->IsInUse() : false;
}
void CMovies::MoviesCopyDisable()
{
	if ( m_pMoviesCopy )
		m_pMoviesCopy->SetInUse(false);
}
void CMovies::MoviesCopyEnable()
{
	if ( m_pMoviesCopy )
		m_pMoviesCopy->SetInUse(true);
}

bool CMovies::CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel) // doc needs to be MoviesDocument
{
	if ( !(parentFrame && (parentFrame->IsKindOf(wxCLASSINFO(wxFrame)) || parentFrame->IsKindOf(wxCLASSINFO(wxDialog)))) )
		return false;
	if ( !(parentPanel && (parentPanel->IsKindOf(wxCLASSINFO(wxPanel)) || parentPanel->IsKindOf(wxCLASSINFO(wxDialog)))) )
		return false;

	m_pParentFrame = parentFrame;
	m_pParentPanel = parentPanel;

	lkColour color(MAKE_RGB(0, 0x40, 0x80)); // 'dark-cyan');

	wxBoxSizer* szMainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* szMain = new wxBoxSizer(wxHORIZONTAL), * szVer;
	/* *******************************************************************************
	 * Poster (StaticImage)
	 * ******************************************************************************* */
	{
		szMain->Add(m_pImage = new ImgPathCtrl(m_pParentPanel, wxSize(250, 355), wxID_ANY, 
											   mcImgPathValidator(GetFldCover(), m_pMoviesCopy, MoviesCopy::What::ImgPath)), 0, wxALL | wxFIXED_MINSIZE | wxEXPAND, 10);
		m_pImage->SetConfigSection(CMovies::GetConfigKeyString());
	}

	szVer = new wxBoxSizer(wxVERTICAL);
	/* *******************************************************************************
	 * Main Title
	 * ******************************************************************************* */
	{
		wxSize size =
#ifdef __WXMSW__
			wxSize(0, 23)
#else
			wxSize(0, 32)
#endif // __WXMSW__
			;
		wxFont* mtFont = new wxFont(size, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Comic Sans MS"));
		if ( !mtFont->IsOk() )
		{
			delete mtFont;
			mtFont = NULL;
		}
		// required but not unique, and not using a default
		szVer->Add(m_pTitleCtrl = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(570, -1), wxBORDER_SUNKEN, 
													  mcTextValidator(GetFldTitle(), m_pMoviesCopy, MoviesCopy::What::Title)), 0, wxEXPAND | wxALIGN_TOP, 0);
		if ( mtFont ) m_pTitleCtrl->SetFont(*mtFont);
		if ( color.IsOk() ) m_pTitleCtrl->SetForegroundColour(color);
		if ( mtFont ) delete mtFont;
	}

	/* *******************************************************************************
	 * Sub Title
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);

		wxSize size =
#ifdef __WXMSW__
		wxSize(63, -1)
#else
		wxSize(80,-1)
#endif
			;
		szHor->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Subtitle :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

		// not required, and not using a default
		lkTransTextCtrl* t;
		szHor->Add(t = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN, 
										   mcTextValidator(GetFldSubTtl(), m_pMoviesCopy, MoviesCopy::What::SubTitle)), 1, wxEXPAND, 0);
		if ( color.IsOk() ) t->SetForegroundColour(color);
		szVer->Add(szHor, 0, wxEXPAND | wxTOP, 5);
	}

	/* *******************************************************************************
	 * Alias / Episode
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		wxSize size =
#ifdef __WXMSW__
		wxSize(63, -1)
#else
		wxSize(80,-1)
#endif
			;
		szHor->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Aliases :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		// not required, and not using a default
		lkTransTextCtrl* t;
		szHor->Add(t = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(340, -1), wxBORDER_SUNKEN,
										   mcTextValidator(GetFldAlias(), m_pMoviesCopy, MoviesCopy::What::Alias)), 1, wxRIGHT | wxEXPAND, 5);
		if ( color.IsOk() ) t->SetForegroundColour(color);

		size =
#ifdef __WXMSW__
			wxSize(63, -1)
#else
			wxSize(80,-1)
#endif
			;
		szHor->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Episode :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		// not required, and not using a default
		szHor->Add(t = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxBORDER_SUNKEN | wxALIGN_RIGHT,
										   mcTextValidator(GetFldEpisode(), m_pMoviesCopy, MoviesCopy::What::Episode, wxFILTER_DIGITS)), 0, wxEXPAND, 0);
		if ( color.IsOk() ) t->SetForegroundColour(color);
		szVer->Add(szHor, 0, wxEXPAND | wxTOP, 5);
	}

	/* *******************************************************************************
	 * Category & SubCategory
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		wxSize size =
#ifdef __WXMSW__
		wxSize(63, -1)
#else
		wxSize(80,-1)
#endif
			;
		szHor->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Categories :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

		// not required but advisable to use it
		szHor->Add(m_pCategory = new CategoryBox(m_pParentPanel, wxID_ANY, wxSize(115, 22), wxCB_READONLY,
												 mcImgComboValidator(GetFldCat(), m_pMoviesCopy, MoviesCopy::What::Cat, wxT("Category box"), NULL)), 1, wxRIGHT | wxEXPAND, 5);
		m_pCategory->SetDatabase(GetDBase());

		size =
#ifdef __WXMSW__
		wxSize(90, -1)
#else
		wxSize(100,-1)
#endif
			;
		szHor->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("SubCategories :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

		// not required
		szHor->Add(m_pSubCat = new CategoryBox(m_pParentPanel, wxID_ANY, wxSize(115, 22), wxCB_READONLY, 
											   mcImgComboValidator(GetFldSubCat(), m_pMoviesCopy, MoviesCopy::What::SubCat, wxT("SubCategory box"), NULL)), 1, wxEXPAND, 0);
		m_pSubCat->SetDatabase(GetDBase());
		m_pCategory->SetSubCatlist(m_pSubCat);

		szVer->Add(szHor, 0, wxEXPAND | wxTOP, 5);
	}

	/* *******************************************************************************
	 * Genres & Issue
	 * ******************************************************************************* */
	{
		wxSize size;
		int defHeight = -1;
#ifndef __WXMSW__
	defHeight = 24;
#endif
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		{
			wxBoxSizer* szV = new wxBoxSizer(wxVERTICAL);
			szV->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Genres :")), 0, wxFIXED_MINSIZE, 0);

			size =
#ifdef __WXMSW__
				wxSize(115, 5 * 18 + 4)
#else
				wxDefaultSize
#endif
				;
			szV->Add(m_pGenres = new GenresBox(GetDBase(), m_pParentPanel, wxID_ANY, wxDefaultPosition, size, wxLB_SORT, mcGenresValidator(GetFldGenre(), m_pMoviesCopy, MoviesCopy::What::Genres)), 1, wxEXPAND, 0);
			szHor->Add(szV, 1, wxRIGHT | wxTOP | wxBOTTOM | wxEXPAND, 5);
		}
		wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxVERTICAL, m_pParentPanel, wxT(" Issue "));
		{
			wxWindow* boxParent = szBox->GetStaticBox();
			wxBoxSizer* szH = new wxBoxSizer(wxHORIZONTAL);
			size = 
#ifdef __WXMSW__
				wxSize(50, -1)
#else
				wxSize(60,-1)
#endif
				;
			szH->Add(new wxStaticText(boxParent, wxID_ANY, wxT("Country :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

			szH->Add(m_pCountry = new lkTransTextCtrl(boxParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, defHeight), wxBORDER_SUNKEN, mcTextValidator(GetFldCountry(), m_pMoviesCopy, MoviesCopy::What::Country)), 1, wxRIGHT | wxEXPAND, 5);
			szH->Add(new wxButton(boxParent, m_ID_CountryBtn, wxT("Pick.."), wxDefaultPosition, wxSize(-1, defHeight)), 0);

			szBox->Add(szH, 0, wxALL | wxEXPAND, 5);

			szH = new wxBoxSizer(wxHORIZONTAL);
			{
				wxBoxSizer* szV = new wxBoxSizer(wxVERTICAL);
				szV->AddStretchSpacer(1);
				{
					wxBoxSizer* szH2 = new wxBoxSizer(wxHORIZONTAL);
					size =
#ifdef __WXMSW__
						wxSize(50, -1)
#else
						wxSize(60,-1)
#endif
						;
					szH2->Add(new wxStaticText(boxParent, wxID_ANY, wxT("Date :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

					// used as full date, the date will be selected through a seperate CalendarCtrl, if used as year-only one should modify it inside the control
					// as date, the ctrl should be READONLY, as year the ctrl needs to be editable
					//
					// appearantly, wxMSW doesn't allow to alter the ReadOnly style during runtime, so like this would be the best solution (i think)
					szH2->Add(m_pDateTxt = new lkTransTextCtrl(boxParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, defHeight), wxBORDER_SUNKEN | wxTE_CENTER | wxTE_READONLY,
															   mcDateValidator(&m_Date, GetFldDate(), GetBaseRecordset(), m_pMoviesCopy, MoviesCopy::What::Date, false, true)), 1, wxRIGHT | wxEXPAND, 5);
					szH2->Add(m_pYearTxt = new lkTransTextCtrl(boxParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, defHeight), wxBORDER_SUNKEN | wxTE_CENTER,
															   MoviesYearValidator(&m_Date, GetFldDate(), GetBaseRecordset(), m_pMoviesCopy, MoviesCopy::What::Year)), 1, wxRIGHT | wxEXPAND, 5);
					m_pYearTxt->Enable(false);
					m_pYearTxt->Show(false);

					szH2->Add(m_pChangeDateBtn = new wxButton(boxParent, m_ID_DateBtn, wxT("Change"), wxDefaultPosition, wxSize(50, defHeight)), 0, wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);
					szV->Add(szH2, 0, wxEXPAND);
				}
				szH->Add(szV, 1, wxEXPAND);
				szV = new wxBoxSizer(wxVERTICAL);
				{
					szV->Add(new wxStaticText(boxParent, wxID_ANY, wxT("Year Only")), 0, wxALIGN_CENTER_HORIZONTAL, 0);
					szV->Add(new wxCheckBox(boxParent, m_ID_YearChkb, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
											mcChkBoxValidator(GetFldYear(), m_pMoviesCopy, MoviesCopy::What::YrBox)), 0, wxALIGN_CENTER);
				}
				szH->Add(szV, 0, wxALIGN_CENTER);
			}
			szBox->Add(szH, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);

			szH = new wxBoxSizer(wxHORIZONTAL);
			size =
#ifdef __WXMSW__
				wxSize(50, -1)
#else
				wxSize(60,-1)
#endif
				;
			szH->Add(new wxStaticText(boxParent, wxID_ANY, wxT("Serie :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
			szH->Add(new wxCheckBox(boxParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
									mcChkBoxValidator(GetFldSerie(), m_pMoviesCopy, MoviesCopy::What::Serie)), 0, wxEXPAND, 0);
			szH->AddStretchSpacer(1);
			szBox->Add(szH, 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 5);
		}
		szHor->Add(szBox, 1, wxTOP | wxBOTTOM | wxLEFT | wxEXPAND, 5);
		szVer->Add(szHor, 0, wxEXPAND, 0);
	}

	/* *******************************************************************************
	 * Online Information (HyperlinkCtrl)
	 * ******************************************************************************* */
	{
		wxSize size;
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer* szV = new wxBoxSizer(wxVERTICAL);
		{
			szV->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("OnLine Information (URL) :"), wxDefaultPosition, wxDefaultSize /*wxSize(150, -1)*/, wxALIGN_LEFT), 0, wxFIXED_MINSIZE, 0);

			szV->Add(new lkHyperlinkCtrl(m_pParentPanel, wxID_ANY, wxDefaultPosition, wxSize(280, -1), lkHL_ALIGN_LEFT | wxBORDER_SUNKEN | lkHL_CONTEXTMENU | lkHL_FONT_NO_UNDERLINE,
										 mcHyperlinkValidator(GetFldURL(), m_pMoviesCopy, MoviesCopy::What::URL)), 1, wxEXPAND);
#ifndef __WXMSW__
			szV->AddStretchSpacer(1);
#endif
			szV->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("About :"), wxDefaultPosition, wxDefaultSize/*wxSize(50, -1)*/, wxALIGN_LEFT), 0, wxTOP, 5);
		}
		szHor->Add(szV);

		/* *******************************************************************************
		 * Judgement / Ratio (SimpleBox)
		 * ******************************************************************************* */
		szV = new wxBoxSizer(wxVERTICAL);
		{
			wxBoxSizer* szH = new wxBoxSizer(wxHORIZONTAL);
			{
				size =
#ifdef __WXMSW__
					wxSize(75, -1)
#else
					wxSize(85,-1)
#endif
					;
				szH->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Judgement :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
				szH->Add(m_pJudge = new JudgeBox(m_pParentPanel, GetDBase(), wxDefaultPosition, wxSize(175, 22), wxCB_READONLY, 
												 mcImgComboValidator(GetFldJudge(), m_pMoviesCopy, MoviesCopy::What::Judge, wxT("Judgement box"), &m_bReqAdd)), 1, wxEXPAND, 0);
			}
			szV->Add(szH, 0, wxTOP | wxALIGN_TOP | wxEXPAND, 5);

			szH = new wxBoxSizer(wxHORIZONTAL);
			{
				size =
#ifdef __WXMSW__
					wxSize(75, -1)
#else
					wxSize(85,-1)
#endif
					;
				szH->Add(new wxStaticText(m_pParentPanel, wxID_ANY, wxT("Rating :"), wxDefaultPosition, size, wxALIGN_RIGHT), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
				szH->Add(m_pRating = new RatingBox(m_pParentPanel, GetDBase(), wxDefaultPosition, wxSize(175, 22), wxCB_READONLY, 
												   mcImgComboValidator(GetFldRating(), m_pMoviesCopy, MoviesCopy::What::Ratio, wxT("Rating box"), &m_bReqAdd)), 1, wxEXPAND, 0);
			}
			szV->Add(szH, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
		}
		szHor->Add(szV, 1, wxLEFT | wxEXPAND, 5);
		szVer->Add(szHor, 0, wxEXPAND | wxALIGN_TOP, 0);
	}

	/* *******************************************************************************
	 * About box
	 * ******************************************************************************* */
	{
		lkTransTextCtrl* t;
		szVer->Add(t = new lkTransTextCtrl(m_pParentPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN | wxTE_MULTILINE,
										   mcTextValidator(GetFldInfo(), m_pMoviesCopy, MoviesCopy::What::About)), 1, wxEXPAND, 0);
		if ( color.IsOk() ) t->SetForegroundColour(color);
	}

	szMain->Add(szVer, 1, wxTOP | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
	szMainSizer->Add(szMain, 1, wxEXPAND);
	m_pParentPanel->SetSizer(szMainSizer);
	m_pParentPanel->Layout();

	return true;
}


///////////////////////////////////////////////////////////////
// class DlgMoviesFind
////
wxIMPLEMENT_ABSTRACT_CLASS(DlgMoviesFind, DlgFind)

DlgMoviesFind::DlgMoviesFind(lkSQL3RecordSet* pRS, lkFindList* param) : DlgFind(pRS, param)
{
}

bool DlgMoviesFind::Create(wxWindow* parent, long style)
{
	return DlgFind::Create(parent, wxT("Find a Movie"), style);
}

//static
wxUint64 DlgMoviesFind::FindMovie(wxWindow* parent, lkSQL3RecordSet* pRS)
{
	if ( !parent || !pRS || !pRS->IsOpen() )
		return 0;

	wxUint64 row = 0;

	lkFindList paramList;
	paramList.DeleteContents(true);
	paramList.Append(new lkFindParam(t3Movies_TITLE, wxT("Movie Title")));

//	TMovies* RS = dynamic_cast<TMovies*>(pRS);
	DlgMoviesFind dlg(pRS, &paramList);
	if ( dlg.Create(parent) && (dlg.ShowModal() == wxID_OK) )
	{
		wxString szFind = dlg.GetFindString();
		row = pRS->FindInit(wxCLASSINFO(TMovies), szFind, dlg.IsForward());
	}

	return row;
}

//virtual
void DlgMoviesFind::DoInit()
{
	wxInt64 ll = GetConfigInt(CMovies::GetConfigKeyString(), conf_FIND_DIRECTION);
	if ( (ll > (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndINVALID) && (ll <= (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndLAST) )
		m_nSrcOn = (wxUint64)ll;

	m_bForward = GetConfigBool(CMovies::GetConfigKeyString(), conf_FIND_FORWARD, true);

	if ( m_pParams && !m_pParams->IsEmpty() )
	{
		ll = GetConfigInt(CMovies::GetConfigKeyString(), conf_FIND_FIELD);
		if ( ll <= 0 )
			ll = 1;

		wxInt64 i = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			if ( ++i == ll )
			{
				m_nField = (wxUint64)current;
				break;
			}
		}
	}
}

//virtual
void DlgMoviesFind::DoOK()
{
	wxInt64 ll = (wxInt64)m_nSrcOn;
	SetConfigInt(CMovies::GetConfigKeyString(), conf_FIND_DIRECTION, ll, (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart);

	SetConfigBool(CMovies::GetConfigKeyString(), conf_FIND_FORWARD, m_bForward, true);

	if ( m_pParams && !m_pParams->IsEmpty() && (m_nField != 0) )
	{
		ll = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			ll++;
			if ( m_nField == (wxUint64)current )
			{
				SetConfigInt(CMovies::GetConfigKeyString(), conf_FIND_FIELD, ll, 1);
				break;
			}
		}
	}
}

