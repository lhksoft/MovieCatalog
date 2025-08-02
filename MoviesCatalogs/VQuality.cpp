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
#include "VQuality.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/config.h>

#include "ImageCtrl.h"
#include "SimpleListbox.h" // Quality's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  QualityCanvas
////
class QualityCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	QualityCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetQltyFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;
	virtual bool			TransferDataFromWindow() wxOVERRIDE;

private:
	QualityListbox*			m_lbQlty;
	wxCheckBox*				m_pChkBx;
	wxTextCtrl*				m_pQltyCtrl;

	bool					m_bEnabled; // only true if current record is the default
	bool					m_bChanged; // something got changed

	wxDECLARE_CLASS(QualityCanvas);
	wxDECLARE_NO_COPY_CLASS(QualityCanvas);
};

wxIMPLEMENT_CLASS(QualityCanvas, lkSQL3panel)

QualityCanvas::QualityCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbQlty = NULL;
	m_pChkBx = NULL;
	m_pQltyCtrl = NULL;
	m_bChanged = false;
	m_bEnabled = false;

	Create();
}
bool QualityCanvas::Create()
{
	// we need this for our Validators
	QualityDocument* qltDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(QualityDocument)) )
			qltDoc = dynamic_cast<QualityDocument*>(doc);

		// this forces the document to open the recordset
		if ( qltDoc )
			(void)qltDoc->GetRecordset();
	}
	wxASSERT_MSG(qltDoc != NULL, wxT("wxDocument needs to be of type QualityDocument"));
	if ( qltDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_QUALITY()) )
		return false;

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL);
	{ // Image
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(qltDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
	}

	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
	szVer->Add(m_pQltyCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT,
											lkSQL3TextValidator(qltDoc->m_pFldQuality, wxFILTER_NONE, &TQuality::IsUniqueQuality, qltDoc->GetDB(), NULL)), 0, wxBOTTOM | wxEXPAND, 5);
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->AddSpacer(25);

		szHor->Add(m_pChkBx = new wxCheckBox(this, wxID_ANY, wxT("Set as Default when operating at new media"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE), 0, wxEXPAND);

		szVer->Add(szHor, 0, wxEXPAND);
	}
	szTop->Add(szVer, 0, wxEXPAND);
	szMain->Add(szTop, 0, wxALL | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbQlty = new QualityListbox(qltDoc->GetDB(), qltDoc);
	m_lbQlty->Create(this, ID_QLT_Preview);
	m_lbQlty->SetSelectionBackground(MAKE_RGB(0x00, 0x77, 0xAE));
	szBox->Add(m_lbQlty, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void QualityCanvas::SetQltyFocus()
{
	if ( m_pQltyCtrl )
		m_pQltyCtrl->SetFocus();
}

//virtual
bool QualityCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		lkSQL3RecordDocument* doc = GetDocument();
		lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
		bool bChanged = (m_bChanged || (doc && doc->HasChanged()));

		if ( pSet && m_pChkBx )
		{
			wxInt64 fromConf = TQuality::GetDefaultQuality(doc->GetDB());
			wxUint64 nRow = pSet->GetActualCurRow();
			m_bEnabled = (static_cast<wxUint64>(fromConf) == nRow);

			if ( m_pChkBx->Is3State() )
				m_pChkBx->Set3StateValue(m_bEnabled ? wxCheckBoxState::wxCHK_CHECKED : wxCheckBoxState::wxCHK_UNCHECKED);
			else
				m_pChkBx->SetValue(m_bEnabled);
		}

		// update subs listbox
		if ( m_lbQlty )
		{
			lkSQL3RecordView* vw = NULL;
			if ( bChanged || ((vw = GetView()) && vw->IsInitial()) )
				m_lbQlty->BuildList();

			if ( pSet )
				m_lbQlty->ScrollIntoView(m_lbQlty->FindLParam(pSet->GetCurRow()));
		}
		m_bChanged = false;
	}

	return bRet;
}

bool QualityCanvas::TransferDataFromWindow()
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
				TQuality::SetDefaultQuality(x);
				m_bChanged = true;
			}
			m_bEnabled = n;
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class QualityDocument
////

wxIMPLEMENT_ABSTRACT_CLASS2(QualityDocument, lkSQL3RecordDocument, TQuality)


QualityDocument::QualityDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TQuality()
{
	TQuality::SetDatabase(GetDB());
}

//virtual
lkSQL3RecordSet* QualityDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void QualityDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_QLT_Preview);
}

//virtual
void QualityDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	QualityView* view = (v && v->IsKindOf(wxCLASSINFO(QualityView))) ? dynamic_cast<QualityView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	QualityCanvas* c = (p && p->IsKindOf(wxCLASSINFO(QualityCanvas))) ? dynamic_cast<QualityCanvas*>(p) : NULL;

	if ( c )
		c->SetQltyFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class QualityView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(QualityView, lkSQL3RecordView)

QualityView::QualityView() : lkSQL3RecordView()
{
}

//virtual
bool QualityView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 269)
#else
			wxSize(397, 278)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Qualities"), pos, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 181)
#else
			wxSize(397, 182)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new QualityCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void QualityView::OnUpdate(wxView* sender, wxObject* hint)
{
}

//virtual
wxString QualityView::GetConfigPath() const
{
	return wxT("VQuality");
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class QualityTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(QualityTemplate, lkChildDocTemplate)

QualityTemplate::QualityTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* QualityTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new QualityDocument(parent);
}

//static
wxDocTemplate* QualityTemplate::CreateTemplate(MainDocManager* man)
{
	return new QualityTemplate(man, wxCLASSINFO(QualityDocument), wxCLASSINFO(QualityView));
}

