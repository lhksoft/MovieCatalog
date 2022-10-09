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
#include "VOrigine.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/config.h>

#include "SimpleListbox.h" // Origine's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  OrigineCanvas
////
class OrigineCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	OrigineCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetOrigFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;
	virtual bool			TransferDataFromWindow() wxOVERRIDE;

private:
	OrigineListbox*			m_lbOrig;
	wxCheckBox*				m_pChkBx;
	wxTextCtrl*				m_pOrigCtrl;

	bool					m_bEnabled; // only true i current record is the default
	bool					m_bChanged; // something got changed

	wxDECLARE_CLASS(OrigineCanvas);
	wxDECLARE_NO_COPY_CLASS(OrigineCanvas);
};

wxIMPLEMENT_CLASS(OrigineCanvas, lkSQL3panel)

OrigineCanvas::OrigineCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbOrig = NULL;
	m_pChkBx = NULL;
	m_pOrigCtrl = NULL;
	m_bChanged = false;
	m_bEnabled = false;

	Create();
}
bool OrigineCanvas::Create()
{
	// we need this for our Validators
	OrigineDocument* orgDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(OrigineDocument)) )
			orgDoc = dynamic_cast<OrigineDocument*>(doc);

		// this forces the document to open the recordset
		if ( orgDoc )
			(void)orgDoc->GetRecordset();
	}
	wxASSERT_MSG(orgDoc != NULL, wxT("wxDocument needs to be of type OrigineDocument"));
	if ( orgDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_ORIGINE()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
		szVer->Add(m_pOrigCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
												lkSQL3TextValidator(orgDoc->m_pFldName, wxFILTER_NONE, &TOrigine::IsUniqueOrigine, orgDoc->GetDB(), NULL)), 0, wxBOTTOM | wxEXPAND, 5);
		szMain->Add(szVer, 0, wxEXPAND | wxLEFT | wxTOP | wxRIGHT, 10);
	}
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->AddSpacer(25);

		szHor->Add(m_pChkBx = new wxCheckBox(this, wxID_ANY, wxT("Set as Default when operating at new media"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE), 0, wxEXPAND);

		szMain->Add(szHor, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	}

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbOrig = new OrigineListbox(orgDoc->GetDB(), orgDoc);
	m_lbOrig->Create(this, ID_ORG_Preview);
	m_lbOrig->SetSelectionBackground(MAKE_RGB(0x80, 0x80, 0xFF));
	szBox->Add(m_lbOrig, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void OrigineCanvas::SetOrigFocus()
{
	if ( m_pOrigCtrl )
		m_pOrigCtrl->SetFocus();
}

//virtual
bool OrigineCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		lkSQL3RecordDocument* doc = GetDocument();
		lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
		bool bChanged = (m_bChanged || (doc && doc->HasChanged()));

		if ( pSet && m_pChkBx )
		{
			wxUint64 fromConf = TOrigine::GetDefaultOrigine(doc->GetDB());
			wxUint64 nRow = pSet->GetActualCurRow();
			m_bEnabled = (fromConf == nRow);

			if ( m_pChkBx->Is3State() )
				m_pChkBx->Set3StateValue(m_bEnabled ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
			else
				m_pChkBx->SetValue(m_bEnabled);
		}

		// update subs listbox
		if ( m_lbOrig )
		{
			lkSQL3RecordView* vw = NULL;
			if ( bChanged || ((vw = GetView()) && vw->IsInitial()) )
				m_lbOrig->BuildList();

			if ( pSet )
				m_lbOrig->ScrollIntoView(m_lbOrig->FindLParam(pSet->GetCurRow()));
		}
		m_bChanged = false;
	}

	return bRet;
}

bool OrigineCanvas::TransferDataFromWindow()
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
				TOrigine::SetDefaultOrigine(x);
				m_bChanged = true;
			}
			m_bEnabled = n;
		}
	}

	return bRet;
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class OrigineDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(OrigineDocument, lkSQL3RecordDocument, TOrigine)


OrigineDocument::OrigineDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TOrigine()
{
	TOrigine::SetDatabase(GetDB());
}

//virtual
lkSQL3RecordSet* OrigineDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void OrigineDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_ORG_Preview);
}

//virtual
void OrigineDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	OrigineView* view = (v && v->IsKindOf(wxCLASSINFO(OrigineView))) ? dynamic_cast<OrigineView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	OrigineCanvas* c = (p && p->IsKindOf(wxCLASSINFO(OrigineCanvas))) ? dynamic_cast<OrigineCanvas*>(p) : NULL;

	if ( c )
		c->SetOrigFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class OrigineView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(OrigineView, lkSQL3RecordView)

OrigineView::OrigineView() : lkSQL3RecordView()
{
}

//virtual
bool OrigineView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 287)
#else
			wxSize(365, 296)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Origines"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 199)
#else
			wxSize(365, 200)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new OrigineCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void OrigineView::OnUpdate(wxView* sender, wxObject* hint)
{
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class OrigineTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(OrigineTemplate, lkChildDocTemplate)

OrigineTemplate::OrigineTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* OrigineTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new OrigineDocument(parent);
}

//static
wxDocTemplate* OrigineTemplate::CreateTemplate(MainDocManager* man)
{
	return new OrigineTemplate(man, wxCLASSINFO(OrigineDocument), wxCLASSINFO(OrigineView));
}

