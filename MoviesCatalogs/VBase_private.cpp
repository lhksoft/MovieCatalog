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
#include "VBase_private.h"
#include "VBase.h"
#include "VBase_validators.h"
#include "VBase_controls.h"
#include "ImageCtrl.h"
#include "SimpleBox.h"
#include "GroupCtrl.h"

//#include <wx/validate.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include <wx/valgen.h>
#include <wx/checkbox.h>
#include <wx/bmpbuttn.h>
#include <wx/utils.h>

#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"
#include "../lkControls/lkStaticImage.h"
#include "../lkControls/lkHyperlinkCtrl.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#include "XPMs.h"
#include "Backgrounds.h"

#define conf_BASE_PATH			wxT("Base")

const wxString GetBaseSection(void)
{
	return conf_BASE_PATH;
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class BaseFrame
////
wxIMPLEMENT_CLASS(BaseFrame, lkSQL3RecordChildFrame)

BaseFrame::BaseFrame() : lkSQL3RecordChildFrame()
{
	m_nID_Movies = wxWindow::NewControlId(1);
	m_nID_Group = wxWindow::NewControlId(1);

	m_nID_Filter = wxWindow::NewControlId(1);
	m_nID_FilterToggle = wxWindow::NewControlId(1);

	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);
}

wxToolBar* BaseFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordChildFrame::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_Movies(), wxT(""), Get_Movies_Xpm(), wxT("Set/Change Movie"));
		tb->AddTool(GetID_Group(), wxT(""), Get_Group_Xpm(), wxT("Set/Change Storage(s)"));
		tb->AddSeparator();
		tb->AddTool(GetID_Filter(), wxT(""), Get_Filter_Xpm(), wxT("Define a Filter"));
		tb->AddTool(GetID_FilterToggle(), wxT(""), Get_FilterTogle_Xpm(), wxT("Toggle Filter"));
		tb->AddSeparator();
		tb->AddTool(GetID_Search(), wxT(""), Get_Search_Xpm(), wxT("Start a Search"));
		tb->AddTool(GetID_SearchNext(), wxT(""), Get_SearchNext_Xpm(), wxT("Search Next"));
		tb->AddTool(GetID_SearchPrev(), wxT(""), Get_SearchPrev_Xpm(), wxT("Search Back"));
	}
	return tb;
}

wxWindowID BaseFrame::GetID_Filter() const
{
	return m_nID_Filter;
}
wxWindowID BaseFrame::GetID_FilterToggle() const
{
	return m_nID_FilterToggle;
}
wxWindowID BaseFrame::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID BaseFrame::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID BaseFrame::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}
wxWindowID BaseFrame::GetID_Movies() const
{
	return m_nID_Movies;
}
wxWindowID BaseFrame::GetID_Group() const
{
	return m_nID_Group;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseMoviesCanvas
wxIMPLEMENT_CLASS2(BaseMoviesCanvas, CanvasCtrl, TMovies)

BaseMoviesCanvas::BaseMoviesCanvas(wxWindow* parent, lkSQL3Database* pDB, const wxValidator& validator) : CanvasCtrl(), TMovies(pDB)
{
	(void)BaseMoviesCanvas::Create(parent, validator);
}

//virtual
void BaseMoviesCanvas::SetInternalID(wxUint64 nID)
{
	CanvasCtrl::SetInternalID(nID);

	TMovies* pSet = dynamic_cast<TMovies*>(this);
	if ( !pSet->IsOpen() ) return;

	if ( m_bValid )
	{
		wxString whr;
		whr.Printf(wxT("([ROWID] = %I64u)"), m_nInternalID);
		pSet->SetFilter(whr);
		try
		{
			pSet->Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return;
		}
	}
}

bool BaseMoviesCanvas::Create(wxWindow* pParent, const wxValidator& validator)
{
	long border = wxBORDER_SUNKEN;
#ifdef __WXMSW__
	border = wxBORDER_STATIC;
#endif // __WXMSW__

	if ( !(CanvasCtrl::Create(pParent, GetImage_FBG_MOVIES(), wxSize(709,218), border, validator)) )
		return false;

	TMovies* pSet = dynamic_cast<TMovies*>(this);
	// Open and Initialize TMovies first
	if ( !pSet->IsOpen() )
	{
		pSet->SetOrder(wxT("[ROWID]"));
		try
		{
			pSet->Open();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return false;
		}
	}

	lkColour color(MAKE_RGB(0, 0x40, 0x80)); // 'dark-cyan');
	wxBoxSizer* szMain = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* szTop = new wxBoxSizer(wxVERTICAL);

	/* *******************************************************************************
	 * Poster (StaticImage)
	 * ******************************************************************************* */
	szMain->Add(new lkStaticImage(this, wxSize(140, 188), wxID_ANY, lkStaticCoverValidator(m_pFldCover, &m_bValid)), 0, wxLEFT | wxTOP | wxBOTTOM | wxFIXED_MINSIZE | wxEXPAND, 10);

	/* *******************************************************************************
	 * Main Title
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);

		wxSize size =
#ifdef __WXMSW__
			wxSize(0, 23)
#else
			wxSize(0, 32)
#endif // __WXMSW__
			;
		wxFont mtFont(size, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Comic Sans MS"));
		szHor->Add(m_pTitleCtrl = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
												   border | wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END,
												   lkSQL3StaticTxtValidator(m_pFldTitle, &m_bValid)), 1, wxRIGHT | wxEXPAND | wxALIGN_TOP, 5);
		if ( mtFont.IsOk() ) m_pTitleCtrl->SetFont(mtFont);
		if ( color.IsOk() ) m_pTitleCtrl->SetForegroundColour(color);

		wxBitmapButton* but;
		szHor->Add(but = new wxBitmapButton(this, m_nBtnCopyTitle, Get_Copy24_Xpm()), 0, wxALIGN_CENTER_VERTICAL);
		but->SetToolTip(wxT("Copy Movies label to Clipboard"));

		szTop->Add(szHor, 0, wxLEFT | wxEXPAND, 5);

	}
	/* *******************************************************************************
	 * Sub Title
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Subtitle :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);

		lkStaticText* t;
		szHor->Add(t = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
										border | wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END,
										lkSQL3StaticTxtValidator(m_pFldSubTtl, &m_bValid)), 1, wxEXPAND, 0);
		if ( color.IsOk() ) t->SetForegroundColour(color);
		szTop->Add(szHor, 0, wxTOP | wxLEFT | wxEXPAND, 5);
	}

	/* *******************************************************************************
	 * Alias / Episode
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Aliases :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		lkStaticText* t;
		szHor->Add(t = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
										border | wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END,
										lkSQL3StaticTxtValidator(m_pFldAlias, &m_bValid)), 1, wxRIGHT | wxEXPAND, 5);
		if ( color.IsOk() ) t->SetForegroundColour(color);

		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Episode :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		szHor->Add(t = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 
										border | wxALIGN_RIGHT | wxST_NO_AUTORESIZE,
										lkSQL3StaticTxtValidator(m_pFldEp, &m_bValid)), 0, wxEXPAND, 0);
		if ( color.IsOk() ) t->SetForegroundColour(color);
		szTop->Add(szHor, 0, wxTOP | wxLEFT | wxEXPAND, 5);
	}
	/* *******************************************************************************
	 * Country/Year / URL
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Country / Year :"), wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		lkStaticText* t;
		szHor->Add(t = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
										border | wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_MIDDLE,
										lkStaticCountyYearValidator(m_pFldCountry, m_pFldDate, &m_bValid)), 1, wxRIGHT | wxEXPAND, 5);
		if ( color.IsOk() ) t->SetForegroundColour(color);

		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("URL :"), wxDefaultPosition, wxSize(45, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
		szHor->Add(new lkHyperlinkCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(50, -1), lkHL_ALIGN_LEFT | border | lkHL_FONT_NO_UNDERLINE,
									   lkStaticHyperlinkValidator(m_pFldURL, &m_bValid)), 2, wxEXPAND, 0);
		szTop->Add(szHor, 0, wxTOP | wxLEFT | wxEXPAND, 5);
	}
	/* *******************************************************************************
	 * About (title) / Series Chkbox
	 * ******************************************************************************* */
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("About :"), wxDefaultPosition, wxSize(65, -1), wxALIGN_LEFT), 0, wxBOTTOM, 2);
		szHor->AddStretchSpacer(1);
		wxCheckBox* cB;
		szHor->Add(cB = new wxCheckBox(this, wxID_ANY, wxT("Serie :"), wxDefaultPosition, wxSize(68, -1), wxALIGN_RIGHT | wxCHK_2STATE,
									   lkStaticChkbxValidator(m_pFldSerie, &m_bValid)), 0, wxBOTTOM | wxEXPAND, 2);
		cB->Enable(false);
		szTop->Add(szHor, 0, wxTOP | wxLEFT | wxEXPAND, 5);
	}
	/* *******************************************************************************
	 * About
	 * ******************************************************************************* */
	{
		lkStaticText* t;
		szTop->Add(t = new lkStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 50),
										wxALIGN_LEFT | border | wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END,
										lkSQL3StaticTxtValidator(m_pFldInfo, &m_bValid)), 1, wxLEFT | wxEXPAND, 5);
		if ( color.IsOk() ) t->SetForegroundColour(color);
	}
	szMain->Add(szTop, 2, wxTOP | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	Bind(wxEVT_BUTTON, &BaseMoviesCanvas::OnBtnCopyTitle, this, m_nBtnCopyTitle);

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class BaseCanvas
////
wxIMPLEMENT_CLASS(BaseCanvas, lkSQL3panel)

BaseCanvas::BaseCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_pMovies = NULL;
	m_pAudio = NULL;
	m_pSubs = NULL;
	m_pQuality = NULL;
	m_pOrigine = NULL;
	m_pScreen = NULL;
	m_pGroup = NULL;

	m_bReqAdd = false;
	m_nCurRow = 0;

	Create();
}

void BaseCanvas::SetBaseFocus() // required when entering Add modus, or at Creation
{
}

void BaseCanvas::UpdateMoviesCtrl()
{
	if ( m_pMovies )
		m_pMovies->TransferDataToWindow();
}
void BaseCanvas::UpdateGroupCtrl()
{
	if ( m_pGroup )
		m_pGroup->TransferDataToWindow();
}

//virtual
bool BaseCanvas::Validate()
{
	BaseDocument* basDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(BaseDocument)) )
			basDoc = dynamic_cast<BaseDocument*>(doc);
	}
	m_bReqAdd = (basDoc && basDoc->IsAdding());
	return lkSQL3panel::Validate();
}

//virtual
bool BaseCanvas::TransferDataToWindow()
{
	bool bInitial = false;
	bool bCountry = false, bQuality = false, bOrigine = false, bScreen = false, bMedium = false;
	{
		lkSQL3RecordView* vw = GetView();
		bInitial = (vw && vw->IsInitial());
	}

	BaseDocument* basDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(BaseDocument)) )
			basDoc = dynamic_cast<BaseDocument*>(doc);
	}
	if ( basDoc )
	{
		bCountry = basDoc->IsCountryChanged();
		basDoc->SetCountryChanged(false);

		bQuality = basDoc->IsQualityChanged();
		basDoc->SetQualityChanged(false);

		bOrigine = basDoc->IsOrigineChanged();
		basDoc->SetOrigineChanged(false);

		bScreen = basDoc->IsScreenChanged();
		basDoc->SetScreenChanged(false);

		bMedium = basDoc->IsMediumChanged();
		basDoc->SetMediumChanged(false);

		m_nCurRow = (basDoc->IsAdding()) ? 0 : basDoc->GetActualCurRow();
	}
	else
		m_nCurRow = 0;


	if ( m_pAudio )
		if ( bInitial || bCountry )
			(dynamic_cast<BaseCountryBox*>(m_pAudio))->BuildList();
	if ( m_pSubs )
		if ( bInitial || bCountry )
			(dynamic_cast<BaseCountryBox*>(m_pSubs))->BuildList();

	if ( m_pQuality )
		if ( bInitial || bCountry )
			(dynamic_cast<QualityBox*>(m_pQuality))->BuildList(bCountry);

	if ( m_pOrigine )
		if ( bInitial || bOrigine )
			(dynamic_cast<OrigineBox*>(m_pOrigine))->BuildList(bOrigine);

	if ( m_pScreen )
		if ( bInitial || bScreen )
			(dynamic_cast<ScreenBox*>(m_pScreen))->BuildList(bScreen);

	if ( m_pGroup )
		if ( bInitial || bMedium )
			(dynamic_cast<GroupCtrl*>(m_pGroup))->FillImageList();

	return lkSQL3panel::TransferDataToWindow();
}

bool BaseCanvas::Create(void)
{
	// we need this for our Validators
	BaseDocument* basDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(BaseDocument)) )
			basDoc = dynamic_cast<BaseDocument*>(doc);

		// this forces the document to open the recordset
		if ( basDoc )
			(void)basDoc->GetRecordset();
	}
	wxASSERT_MSG(basDoc != NULL, wxT("wxDocument needs to be of type BaseDocument"));
	if ( basDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_BASE()) )
		return false;

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);

	/* *******************************************************************************
	 * Movies-Preview in a Panel acting as a Control
	 * ******************************************************************************* */
	{
		szMain->Add(m_pMovies = new BaseMoviesCanvas(this, basDoc->GetDB(), lkStaticPanelValidator(basDoc->m_pFldMovies)), 1, wxALL | wxEXPAND, 10);
	}
	/* *******************************************************************************
	 * Controls for Base
	 * ******************************************************************************* */
	{
		wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
		/* *******************************************************************************
		 * Audio and Subtitle Listboxes
		 * ******************************************************************************* */
		{
			wxBoxSizer* szLeft = new wxBoxSizer(wxVERTICAL);
			/* *******************************************************************************
			 * Audio Listbox
			 * ******************************************************************************* */
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				{
					wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
					szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Audio :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxFIXED_MINSIZE | wxALIGN_TOP/* | wxALIGN_CENTER_VERTICAL*/);
					szVer->AddStretchSpacer(1);
					szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 5);
				}
				szHor->Add(m_pAudio = new BaseCountryBox(basDoc->GetDB(), true, this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, lkAudioSubsValidator(basDoc->m_pFldAudio)), 1, wxEXPAND, 0);
				szLeft->Add(szHor, 1, wxBOTTOM | wxRIGHT | wxEXPAND, 5);
			}
			/* *******************************************************************************
			 * Subs Listbox
			 * ******************************************************************************* */
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				{
					wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
					szVer->Add(new wxStaticText(this, wxID_ANY, wxT("SubTitles :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxFIXED_MINSIZE | wxALIGN_TOP/* | wxALIGN_CENTER_VERTICAL*/);
					szVer->AddStretchSpacer(1);
					szHor->Add(szVer, 0, wxRIGHT | wxEXPAND, 5);
				}
				szHor->Add(m_pSubs = new BaseCountryBox(basDoc->GetDB(), false, this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, lkAudioSubsValidator(basDoc->m_pFldSubs)), 1, wxEXPAND, 0);
				szLeft->Add(szHor, 1, wxBOTTOM | wxRIGHT | wxEXPAND, 5);
			}
			szTop->Add(szLeft, 1, wxEXPAND, 0);
		}
		/* *******************************************************************************
		 * Right side of controls
		 * ******************************************************************************* */
		{
			wxBoxSizer* szRight = new wxBoxSizer(wxVERTICAL);
			/* *******************************************************************************
			 * Quality ComboBox
			 * ******************************************************************************* */
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Quality :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
				szHor->Add(m_pQuality = new QualityBox(this, wxDefaultSize, lkSQL3ImgComboValidator(basDoc->m_pFldQuality, wxT("Quality box"), &m_bReqAdd)), 1, wxEXPAND);
				(dynamic_cast<QualityBox*>(m_pQuality))->SetDatabase(basDoc->GetDB());
				szRight->Add(szHor, 0, wxBOTTOM | wxEXPAND, 5);
			}
			/* *******************************************************************************
			 * Origine ComboBox
			 * ******************************************************************************* */
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Origine :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
				szHor->Add(m_pOrigine = new OrigineBox(this, wxDefaultSize, lkSQL3ImgComboValidator(basDoc->m_pFldOrigine, wxT("Origine box"), &m_bReqAdd)), 1, wxEXPAND);
				(dynamic_cast<OrigineBox*>(m_pOrigine))->SetDatabase(basDoc->GetDB());
				szRight->Add(szHor, 0, wxBOTTOM | wxEXPAND, 5);
			}
			/* *******************************************************************************
			 * Screen ComboBox
			 * ******************************************************************************* */
			{
				wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
				szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Display :"), wxDefaultPosition, wxSize(63, -1), wxALIGN_RIGHT), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER_VERTICAL, 5);
				szHor->Add(m_pScreen = new ScreenBox(this, wxDefaultSize, lkSQL3ImgComboValidator(basDoc->m_pFldScreen, wxT("Screen box"), &m_bReqAdd)), 1, wxEXPAND);
				(dynamic_cast<ScreenBox*>(m_pScreen))->SetDatabase(basDoc->GetDB());
				szRight->Add(szHor, 0, wxBOTTOM | wxEXPAND, 5);
			}
			/* *******************************************************************************
			 * Group Control
			 * ******************************************************************************* */
			szRight->Add(m_pGroup = new GroupCtrl(basDoc->GetDB(), this, wxID_ANY, wxDefaultSize, lkGroupValidator(&m_nCurRow)), 1, wxEXPAND, 0);
//			szRight->AddSpacer(1);

			szTop->Add(szRight, 1, wxEXPAND, 0);
		}

		szMain->Add(szTop, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);
	}

	SetSizer(szMain);
	Layout();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgBaseFind
////
wxIMPLEMENT_ABSTRACT_CLASS(DlgBaseFind, DlgFind)

DlgBaseFind::DlgBaseFind(lkSQL3RecordSet* pRS, lkFindList* param) : DlgFind(pRS, param)
{
}

bool DlgBaseFind::Create(wxWindow* parent, long style)
{
	return DlgFind::Create(parent, wxT("Find a Base"), style);
}

//static
wxUint64 DlgBaseFind::FindBase(wxWindow* parent, lkSQL3RecordSet* pRS)
{
	if ( !parent || !pRS || !pRS->IsOpen() )
		return 0;

	wxUint64 row = 0;

	lkFindList paramList;
	paramList.DeleteContents(true);
	paramList.Append(new lkFindParam(wxT("MOVIES"), wxT("Movie Title")));
	paramList.Append(new lkFindParam(wxT("STORAGES"), wxT("Storage Label")));

	DlgBaseFind dlg(pRS, &paramList);
	if ( dlg.Create(parent) && (dlg.ShowModal() == wxID_OK) )
	{
		wxString szFind = dlg.GetFindString();
		row = pRS->FindInit(wxCLASSINFO(TBase), szFind, dlg.IsForward());
	}

	return row;
}

//virtual
void DlgBaseFind::DoInit()
{
	wxInt64 ll = GetConfigInt(GetBaseSection(), conf_FIND_DIRECTION);
	if ( (ll > (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndINVALID) && (ll <= (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndLAST) )
		m_nSrcOn = (wxUint64)ll;

	m_bForward = GetConfigBool(GetBaseSection(), conf_FIND_FORWARD, true);

	if ( m_pParams && !m_pParams->IsEmpty() )
	{
		ll = GetConfigInt(GetBaseSection(), conf_FIND_FIELD);
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
void DlgBaseFind::DoOK()
{
	wxInt64 ll = (wxInt64)m_nSrcOn;
	SetConfigInt(GetBaseSection(), conf_FIND_DIRECTION, ll, (wxInt64)lkSQL3RecordSet::FIND_DIRECTION::fndStart);

	SetConfigBool(GetBaseSection(), conf_FIND_FORWARD, m_bForward, true);

	if ( m_pParams && !m_pParams->IsEmpty() && (m_nField != 0) )
	{
		ll = 0;
		for ( lkFindList::iterator iter = m_pParams->begin(); iter != m_pParams->end(); ++iter )
		{
			lkFindParam* current = *iter;
			ll++;
			if ( m_nField == (wxUint64)current )
			{
				SetConfigInt(GetBaseSection(), conf_FIND_FIELD, ll, 1);
				break;
			}
		}
	}
}

