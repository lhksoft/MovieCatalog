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
#include "VJudge.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/config.h>

#include "ImageCtrl.h"
#include "SimpleListbox.h" // Judge's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"

#define conf_JUDGE_PATH		wxT("Judges")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  JudgeCanvas
////
class JudgeCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	JudgeCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetJudgeFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	JudgeListbox*			m_lbJudges;
	wxTextCtrl*				m_pJudgeCtrl;

	wxDECLARE_CLASS(JudgeCanvas);
	wxDECLARE_NO_COPY_CLASS(JudgeCanvas);
};

wxIMPLEMENT_CLASS(JudgeCanvas, lkSQL3panel)

JudgeCanvas::JudgeCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbJudges = NULL;
	m_pJudgeCtrl = NULL;
	Create();
}
bool JudgeCanvas::Create()
{
	// we need this for our Validators
	JudgeDocument* jdgDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(JudgeDocument)) )
			jdgDoc = dynamic_cast<JudgeDocument*>(doc);

		// this forces the document to open the recordset
		if ( jdgDoc )
			(void)jdgDoc->GetRecordset();
	}
	wxASSERT_MSG(jdgDoc != NULL, wxT("wxDocument needs to be of type JudgeDocument"));
	if ( jdgDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_JUDGE()) )
		return false;

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL);
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(jdgDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_JUDGE_PATH);
	}

	{
		wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);

		szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Judge Name :")), 0, wxEXPAND);

		szVer->Add(m_pJudgeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
												 lkSQL3TextValidator(jdgDoc->m_pFldJudge, wxFILTER_NONE, &TJudge::IsUniqueJudge, jdgDoc->GetDB(), NULL)), 0, wxEXPAND);
		szVer->AddSpacer(3);
		szTop->Add(szVer, 1, wxEXPAND);
	}
	szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbJudges = new JudgeListbox(jdgDoc->GetDB(), jdgDoc);
	m_lbJudges->Create(this, ID_JDG_Preview, wxSize(-1, 5 * 18 + 4));
	m_lbJudges->SetSelectionBackground(MAKE_RGB(0xC0, 0xC0, 0xC0));
	szBox->Add(m_lbJudges, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 2, wxALL | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void JudgeCanvas::SetJudgeFocus()
{
	if ( m_pJudgeCtrl )
		m_pJudgeCtrl->SetFocus();
}

//virtual
bool JudgeCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbJudges )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbJudges->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if ( pSet )
				m_lbJudges->ScrollIntoView(m_lbJudges->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class JudgeDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(JudgeDocument, lkSQL3RecordDocument, TJudge)

JudgeDocument::JudgeDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TJudge()
{
	TJudge::SetDatabase(GetDB());
}

//virtual
lkSQL3RecordSet* JudgeDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void JudgeDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_JDG_Preview);
}

//virtual
void JudgeDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	JudgeView* view = (v && v->IsKindOf(wxCLASSINFO(JudgeView))) ? dynamic_cast<JudgeView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	JudgeCanvas* c = (p && p->IsKindOf(wxCLASSINFO(JudgeCanvas))) ? dynamic_cast<JudgeCanvas*>(p) : NULL;

	if ( c )
		c->SetJudgeFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class JudgeView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(JudgeView, lkSQL3RecordView)

wxBEGIN_EVENT_TABLE(JudgeView, lkSQL3RecordView)
wxEND_EVENT_TABLE()

JudgeView::JudgeView() : lkSQL3RecordView()
{
}

//virtual
bool JudgeView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 286)
#else
			wxSize(365, 288)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Judgement"), pos, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 198)
#else
			wxSize(365, 192)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new JudgeCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void JudgeView::OnUpdate(wxView* sender, wxObject* hint)
{
}

//virtual
wxString JudgeView::GetConfigPath() const
{
	return wxT("VJudge");
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class JudgeTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(JudgeTemplate, lkChildDocTemplate)

JudgeTemplate::JudgeTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* JudgeTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new JudgeDocument(parent);
}

//static
wxDocTemplate* JudgeTemplate::CreateTemplate(MainDocManager* man)
{
	return new JudgeTemplate(man, wxCLASSINFO(JudgeDocument), wxCLASSINFO(JudgeView));
}

