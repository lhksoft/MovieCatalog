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
#include "VScreen.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/config.h>

#include "ImageCtrl.h"
#include "SimpleListbox.h" // Screen's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  ScreenCanvas
////
class ScreenCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	ScreenCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetScreenFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;
	virtual bool			TransferDataFromWindow() wxOVERRIDE;

private:
	ScreenListbox*			m_lbScr;
	wxCheckBox*				m_pChkBx;
	wxTextCtrl*				m_pScrCtrl;

	bool					m_bEnabled; // only true if current record is the default
	bool					m_bChanged; // something got changed

	wxDECLARE_CLASS(ScreenCanvas);
	wxDECLARE_NO_COPY_CLASS(ScreenCanvas);
};

wxIMPLEMENT_CLASS(ScreenCanvas, lkSQL3panel)

ScreenCanvas::ScreenCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbScr = NULL;
	m_pChkBx = NULL;
	m_pScrCtrl = NULL;
	m_bChanged = false;
	m_bEnabled = false;

	Create();
}
bool ScreenCanvas::Create()
{
	// we need this for our Validators
	ScreenDocument* scrDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(ScreenDocument)) )
			scrDoc = dynamic_cast<ScreenDocument*>(doc);

		// this forces the document to open the recordset
		if ( scrDoc )
			(void)scrDoc->GetRecordset();
	}
	wxASSERT_MSG(scrDoc != NULL, wxT("wxDocument needs to be of type ScreenDocument"));
	if ( scrDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_SCREEN()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
	szVer->Add(m_pScrCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT,
										   lkSQL3TextValidator(scrDoc->m_pFldName, wxFILTER_NONE, &TScreen::IsUniqueScreen, scrDoc->GetDB(), NULL)), 0, wxBOTTOM | wxEXPAND, 5);
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->AddSpacer(25);

		szHor->Add(m_pChkBx = new wxCheckBox(this, wxID_ANY, wxT("Set as Default when operating at new media"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE), 0, wxEXPAND);

		szVer->Add(szHor, 0, wxEXPAND | wxBOTTOM, 5);
	}
	szMain->Add(szVer, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbScr = new ScreenListbox(scrDoc->GetDB(), scrDoc);
	m_lbScr->Create(this, ID_SCR_Preview);
	m_lbScr->SetSelectionBackground(MAKE_RGB(0xC0, 0x00, 0x00));
	szBox->Add(m_lbScr, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void ScreenCanvas::SetScreenFocus()
{
	if ( m_pScrCtrl )
		m_pScrCtrl->SetFocus();
}

//virtual
bool ScreenCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		lkSQL3RecordDocument* doc = GetDocument();
		lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
		bool bChanged = (m_bChanged || (doc && doc->HasChanged()));

		if ( pSet && m_pChkBx )
		{
			wxInt64 fromConf = TScreen::GetDefaultScreen(doc->GetDB());
			wxUint64 nRow = pSet->GetActualCurRow();
			m_bEnabled = (static_cast<wxUint64>(fromConf) == nRow);

			if ( m_pChkBx->Is3State() )
				m_pChkBx->Set3StateValue(m_bEnabled ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
			else
				m_pChkBx->SetValue(m_bEnabled);
		}

		// update subs listbox
		if ( m_lbScr )
		{
			lkSQL3RecordView* vw = NULL;
			if ( bChanged || ((vw = GetView()) && vw->IsInitial()) )
				m_lbScr->BuildList();

			if ( pSet )
				m_lbScr->ScrollIntoView(m_lbScr->FindLParam(pSet->GetCurRow()));
		}
		m_bChanged = false;
	}

	return bRet;
}

bool ScreenCanvas::TransferDataFromWindow()
{
	bool bRet = lkSQL3panel::TransferDataFromWindow();

	if ( bRet && m_pChkBx )
	{
		bool n;
		if ( m_pChkBx->Is3State() )
			n = (m_pChkBx->Get3StateValue() == wxCheckBoxState::wxCHK_CHECKED);
		else
			n = m_pChkBx->IsChecked();

		if ( m_bEnabled != n )
		{
			lkSQL3RecordSet* pSet = (GetDocument()) ? GetDocument()->GetRecordset() : NULL;
			if ( pSet )
			{
				wxInt64 x = (n) ? static_cast<wxInt64>(pSet->GetActualCurRow()) : 0;
				TScreen::SetDefaultScreen(x);
				m_bChanged = true;
			}
			m_bEnabled = n;
		}
	}

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenDocument
////

wxIMPLEMENT_ABSTRACT_CLASS2(ScreenDocument, lkSQL3RecordDocument, TScreen)


ScreenDocument::ScreenDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TScreen()
{
	TScreen::SetDatabase(GetDB());
}

//virtual
wxString ScreenDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* ScreenDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void ScreenDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_SCR_Preview);
}

//virtual
void ScreenDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	ScreenView* view = (v && v->IsKindOf(wxCLASSINFO(ScreenView))) ? dynamic_cast<ScreenView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	ScreenCanvas* c = (p && p->IsKindOf(wxCLASSINFO(ScreenCanvas))) ? dynamic_cast<ScreenCanvas*>(p) : NULL;

	if ( c )
		c->SetScreenFocus();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenView
////

wxIMPLEMENT_DYNAMIC_CLASS(ScreenView, lkSQL3RecordView)

ScreenView::ScreenView() : lkSQL3RecordView()
{
}

//virtual
bool ScreenView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 246)
#else
			wxSize(353, 256)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Screen"), pos, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 158)
#else
			wxSize(353, 160)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new ScreenCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void ScreenView::OnUpdate(wxView* sender, wxObject* hint)
{
}

//virtual
wxString ScreenView::GetConfigPath() const
{
	return wxT("VScreen");
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class ScreenTemplate
////

wxIMPLEMENT_CLASS(ScreenTemplate, lkChildDocTemplate)

ScreenTemplate::ScreenTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* ScreenTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new ScreenDocument(parent);
}

//static
wxDocTemplate* ScreenTemplate::CreateTemplate(MainDocManager* man)
{
	return new ScreenTemplate(man, wxCLASSINFO(ScreenDocument), wxCLASSINFO(ScreenView));
}

