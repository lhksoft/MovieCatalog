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
#include "VCountry.h"
#include "VCountry_private.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Backgrounds.h"

#include "TBase.h"
#include <wx/msgdlg.h>

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/config.h>

//#include <lkChkImgListbox.h>
#include "ImageCtrl.h"
#include "SimpleListbox.h" // Country's Listbox

#include "Defines.h"

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"

#include "XPMs.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CountryFrame
////
class CountryFrame : public lkSQL3RecordChildFrame
{
public:
	CountryFrame();

	virtual wxToolBar* OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

protected:
	wxWindowID				m_nID_Search;
	wxWindowID				m_nID_SearchNext;
	wxWindowID				m_nID_SearchPrev;

public:
	wxWindowID				GetID_Search(void) const;
	wxWindowID				GetID_SearchNext(void) const;
	wxWindowID				GetID_SearchPrev(void) const;

private:
	wxDECLARE_CLASS(CountryFrame);
	wxDECLARE_NO_COPY_CLASS(CountryFrame);
};

wxIMPLEMENT_CLASS(CountryFrame, lkSQL3RecordChildFrame)

CountryFrame::CountryFrame() : lkSQL3RecordChildFrame()
{
	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);
}

wxToolBar* CountryFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordChildFrame::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_Search(), wxT(""), Get_Search_Xpm(), wxT("Start a Search"));
		tb->AddTool(GetID_SearchNext(), wxT(""), Get_SearchNext_Xpm(), wxT("Search Next"));
		tb->AddTool(GetID_SearchPrev(), wxT(""), Get_SearchPrev_Xpm(), wxT("Search Back"));
	}
	return tb;
}

wxWindowID CountryFrame::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID CountryFrame::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID CountryFrame::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  CountryCanvas
////
class CountryCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	CountryCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetIsoFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	CountryListbox*			m_lbCountry;
	wxTextCtrl*				m_pIsoCtrl;
	wxCheckBox*				m_pAudio, *m_pSubs;

protected:
	wxWindowID				m_idChkAudio, m_idChkSubs;

	void					OnCheckBoxClick(wxCommandEvent&);

private:
	wxDECLARE_CLASS(CountryCanvas);
	wxDECLARE_NO_COPY_CLASS(CountryCanvas);
};

wxIMPLEMENT_CLASS(CountryCanvas, lkSQL3panel)

CountryCanvas::CountryCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbCountry = NULL;
	m_pIsoCtrl = NULL;
	m_pAudio = NULL;
	m_pSubs = NULL;

	m_idChkAudio = wxWindow::NewControlId(1);
	m_idChkSubs = wxWindow::NewControlId(1);

	Create();
}
bool CountryCanvas::Create()
{
	// we need this for our Validators
	CountryDocument* ctrDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(CountryDocument)) )
			ctrDoc = dynamic_cast<CountryDocument*>(doc);

		// this forces the document to open the recordset
		if ( ctrDoc )
			(void)ctrDoc->GetRecordset();
	}
	wxASSERT_MSG(ctrDoc != NULL, wxT("wxDocument needs to be of type CountryDocument"));
	if ( ctrDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_COUNTRY()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL), * szVer, * szHor;
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(ctrDoc->m_pFldFlag)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_COUNTRY_PATH);
	}

int width = 95;
#ifdef __WXMSW__
	width = 85;
#endif

	szVer = new wxBoxSizer(wxVERTICAL);
	// First line : ISO and Custom Code
	{
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("ISO Code :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);
		szHor->Add(m_pIsoCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(35, -1), wxTE_LEFT, 
											   lkSQL3TextValidator(ctrDoc->m_pFldIsoDefault, wxFILTER_ALPHA, &TCountry::IsUniqueIso, ctrDoc->GetDB(), NULL)), 0, wxRIGHT | wxEXPAND, 5);
		m_pIsoCtrl->ForceUpper();

		wxTextCtrl* t;
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Custom Code :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);
		szHor->Add(t = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(35, -1), wxTE_LEFT,
									  lkSQL3TextValidator(ctrDoc->m_pFldIsoCustom, wxFILTER_ALPHA, &TCountry::IsUniqueCode, ctrDoc->GetDB(), NULL)), 1, wxEXPAND);
		t->ForceUpper();
		szVer->Add(szHor, 0, wxEXPAND);
	}
	// Second line : Default Country Name
	{
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Default Name :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);
		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
								  lkSQL3TextValidator(ctrDoc->m_pFldCountryDefault, wxFILTER_NONE, &TCountry::IsUniqueCountry, ctrDoc->GetDB(), NULL)), 1, wxEXPAND);
		szVer->Add(szHor, 0, wxTOP | wxEXPAND, 5);
	}
	// Third line : Custom Country Name
	{
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Custom Name :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);
		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
								  lkSQL3TextValidator(ctrDoc->m_pFldCountryCustom, wxFILTER_NONE, &TCountry::IsUniqueCustom, ctrDoc->GetDB(), NULL)), 1, wxEXPAND);
		szVer->Add(szHor, 0, wxTOP | wxEXPAND, 5);
	}
	szTop->Add(szVer, 1, wxEXPAND);
	szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);

	szTop = new wxBoxSizer(wxHORIZONTAL);
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(ctrDoc->m_pFldSpokenFlag)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_COUNTRY_PATH);
	}
	szVer = new wxBoxSizer(wxVERTICAL);
	// Fourth line : Checkboxes
	{
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->AddSpacer(width + 10);

		szHor->Add(m_pAudio = new wxCheckBox(this, m_idChkAudio, wxT("Use as Audio"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxCHK_2STATE,
											 lkSQL3CountryChkbValidator(ctrDoc->m_pFldAudioSubs, lkSQL3CountryChkbValidator::audio)), 1, wxRIGHT | wxEXPAND, 5);

		szHor->Add(m_pSubs = new wxCheckBox(this, m_idChkSubs, wxT("Use as Subtitle"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
											lkSQL3CountryChkbValidator(ctrDoc->m_pFldAudioSubs, lkSQL3CountryChkbValidator::subs)), 1, wxEXPAND);

		szVer->Add(szHor, 0, wxTOP | wxEXPAND, 5);
	}
	// Fifth line : Spoken Name
	{
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Spoken Name :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);

		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, lkSQL3TextValidator(ctrDoc->m_pFldSpoken)), 1, wxEXPAND);
		szVer->Add(szHor, 0, wxTOP | wxEXPAND, 5);
	}
	szTop->Add(szVer, 1, wxEXPAND);
	szMain->Add(szTop, 0, wxLEFT | wxRIGHT | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbCountry = new CountryListbox(ctrDoc->GetDB(), ctrDoc);
	m_lbCountry->Create(this, ID_CTR_Preview, wxSize(-1, 6 * 18 + 4));
	m_lbCountry->SetSelectionBackground(MAKE_RGB(0x00, 0x76, 0xC0));
	szBox->Add(m_lbCountry, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 2, wxALL | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	SetIsoFocus();

	Bind(wxEVT_CHECKBOX, &CountryCanvas::OnCheckBoxClick, this, wxID_ANY);

	return true;
}

void CountryCanvas::SetIsoFocus()
{
	if ( m_pIsoCtrl )
		m_pIsoCtrl->SetFocus();
}

//virtual
bool CountryCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbCountry )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbCountry->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if (pSet)
				m_lbCountry->ScrollIntoView(m_lbCountry->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}

void CountryCanvas::OnCheckBoxClick(wxCommandEvent& event)
{
	lkSQL3RecordDocument* doc = GetDocument();
	lkSQL3RecordView* view = GetView();
	if ( !doc || !doc->IsKindOf(wxCLASSINFO(CountryDocument)))
	{
		event.Skip();
		return;
	}
	wxString s = wxT("Unable to deselect this country as %s, while still titles are linked to it in tblBase.");
	wxString ttl = wxT("Attention");
	if ( (event.GetId() == m_idChkAudio) && !event.IsChecked() )
	{
		if ( TBase::HasAudio(doc->GetDB(), (dynamic_cast<CountryDocument*>(doc))->GetIsoValue()) && m_pAudio)
		{
			if ( view ) view->SetCanClose(false);
			wxString ss;
			ss.Printf(s, wxT("Audio"));
			wxMessageBox(ss, ttl, wxOK | wxICON_EXCLAMATION, GetParent());
			m_pAudio->SetValue(true);
			if ( view ) view->SetCanClose(true);
			return;
		}
	}
	else if ( (event.GetId() == m_idChkSubs) && !event.IsChecked() )
	{
		if ( TBase::HasSubtitle(doc->GetDB(), (dynamic_cast<CountryDocument*>(doc))->GetIsoValue()) && m_pSubs)
		{
			if ( view ) view->SetCanClose(false);
			wxString ss;
			ss.Printf(s, wxT("Subtitle"));
			wxMessageBox(ss, ttl, wxOK | wxICON_EXCLAMATION, GetParent());
			m_pSubs->SetValue(true);
			if ( view ) view->SetCanClose(true);
			return;
		}
	}
	event.Skip();
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class CountryDocument
// /////////////////////////////////////////////////////////////////////////////////////////////


wxIMPLEMENT_ABSTRACT_CLASS2(CountryDocument, lkSQL3RecordDocument, TCountry)

CountryDocument::CountryDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TCountry()
{
	TCountry::SetDatabase(GetDB());
}

//virtual
lkSQL3RecordSet* CountryDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
wxString CountryDocument::GetDefaultOrder() const
{
	wxString order;
	order.Printf(rowset_COUNTRY_DEFORDER, t3Country_CUSTOM, t3Country_DEF);
	return order;
}

//virtual
void CountryDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	CountryFrame* frame = NULL;
	{
		lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());
		if ( f && f->IsKindOf(wxCLASSINFO(CountryFrame)) )
			frame = dynamic_cast<CountryFrame*>(f);
	}
	if ( !frame )
		return;

	Bind(wxEVT_UPDATE_UI, &CountryDocument::OnUpdateFind, this, frame->GetID_Search());
	Bind(wxEVT_UPDATE_UI, &CountryDocument::OnUpdateFindNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &CountryDocument::OnUpdateFindPrev, this, frame->GetID_SearchPrev());

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_CTR_Preview);
	Bind(wxEVT_TOOL, &CountryDocument::OnSearch, this, frame->GetID_Search());
	Bind(wxEVT_TOOL, &CountryDocument::OnSearchNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_TOOL, &CountryDocument::OnSearchPrev, this, frame->GetID_SearchPrev());
}

void CountryDocument::OnSearch(wxCommandEvent& WXUNUSED(event))
{
	TCountry* pRS = dynamic_cast<TCountry*>(GetBaseSet());
	wxUint64 _row = DlgCountryFind::FindCountry(GetFrame(), pRS);

	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDocument::UpdateData(false);
	}
}


//virtual
void CountryDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	CountryView* view = (v && v->IsKindOf(wxCLASSINFO(CountryView))) ? dynamic_cast<CountryView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	CountryCanvas* c = (p && p->IsKindOf(wxCLASSINFO(CountryCanvas))) ? dynamic_cast<CountryCanvas*>(p) : NULL;

	if ( c )
		c->SetIsoFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class CountryTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CountryTemplate, lkChildDocTemplate)

CountryTemplate::CountryTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* CountryTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new CountryDocument(parent);
}

//static
wxDocTemplate* CountryTemplate::CreateTemplate(MainDocManager* man)
{
	return new CountryTemplate(man, wxCLASSINFO(CountryDocument), wxCLASSINFO(CountryView));
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class CountryView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(CountryView, lkSQL3RecordView)

wxBEGIN_EVENT_TABLE(CountryView, lkSQL3RecordView)
wxEND_EVENT_TABLE()

CountryView::CountryView() : lkSQL3RecordView()
{
}

//virtual
bool CountryView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		CountryFrame* frame = new CountryFrame();

		wxSize size = 
#ifdef __WXMSW__
			wxSize(385, 461)
#else
			wxSize(404, 483)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Countries"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(369, 373)
#else
			wxSize(404, 387)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new CountryCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void CountryView::OnUpdate(wxView* sender, wxObject* hint)
{
}

/////////////////////////////////////////////////////////////////////////
// Event Processing

