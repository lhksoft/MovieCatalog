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
#include "VRating.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/config.h>

#include "ImageCtrl.h"
#include "SimpleListbox.h" // Rating's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"
#include "../lkControls/lkConfigTools.h"

#define config_RATING_PATH		wxT("Ratings")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  RatingCanvas
////

class RatingCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	RatingCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetRatingFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	RatingListbox*			m_lbRating;
	wxTextCtrl*				m_pRtngFlagCtrl;

	wxDECLARE_CLASS(RatingCanvas);
	wxDECLARE_NO_COPY_CLASS(RatingCanvas);
};

wxIMPLEMENT_CLASS(RatingCanvas, lkSQL3panel)

RatingCanvas::RatingCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbRating = NULL;
	m_pRtngFlagCtrl = NULL;
	Create();
}
bool RatingCanvas::Create()
{
	// we need this for our Validators
	RatingDocument* rtgDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(RatingDocument)) )
			rtgDoc = dynamic_cast<RatingDocument*>(doc);

		// this forces the document to open the recordset
		if ( rtgDoc )
			(void)rtgDoc->GetRecordset();
	}
	wxASSERT_MSG(rtgDoc != NULL, wxT("wxDocument needs to be of type RatingDocument"));
	if ( rtgDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_RATING()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL);
	{ // Image
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(rtgDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(config_RATING_PATH);
	}

	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(m_pRtngFlagCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT, 
													lkSQL3TextValidator(rtgDoc->m_pFldFlag, wxFILTER_DIGITS, &TRating::IsUniqueFlag, rtgDoc->GetDB(), NULL)), 0, wxFIXED_MINSIZE | wxRIGHT | wxEXPAND, 5);

		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
								  lkSQL3TextValidator(rtgDoc->m_pFldRating, wxFILTER_NONE, &TRating::IsUniqueRating, rtgDoc->GetDB(), NULL)), 1, wxEXPAND);

		szVer->Add(szHor, 0, wxEXPAND | wxBOTTOM, 5);
	}

	szVer->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, lkSQL3TextValidator(rtgDoc->m_pFldDescription)), 1, wxEXPAND);

	szTop->Add(szVer, 1, wxEXPAND);
	szMain->Add(szTop, 0, wxALL | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbRating = new RatingListbox(rtgDoc->GetDB(), rtgDoc);
	m_lbRating->Create(this, ID_RTG_Preview);
	m_lbRating->SetSelectionBackground(MAKE_RGB(0x5F, 0xC0, 0x00));
	szBox->Add(m_lbRating, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void RatingCanvas::SetRatingFocus()
{
	if ( m_pRtngFlagCtrl )
		m_pRtngFlagCtrl->SetFocus();
}

//virtual
bool RatingCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbRating )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbRating->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if ( pSet )
				m_lbRating->ScrollIntoView(m_lbRating->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class RatingDocument
////

wxIMPLEMENT_ABSTRACT_CLASS2(RatingDocument, lkSQL3RecordDocument, TRating)


RatingDocument::RatingDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TRating()
{
	TRating::SetDatabase(GetDB());
}

//virtual
wxString RatingDocument::GetDefaultOrder() const
{
	wxString order;
	order.Printf(wxT("[%s]"), t3Rating_ID);
	return order;
}

//virtual
lkSQL3RecordSet* RatingDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void RatingDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_RTG_Preview);
}

//virtual
void RatingDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	RatingView* view = (v && v->IsKindOf(wxCLASSINFO(RatingView))) ? dynamic_cast<RatingView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	RatingCanvas* c = (p && p->IsKindOf(wxCLASSINFO(RatingCanvas))) ? dynamic_cast<RatingCanvas*>(p) : NULL;

	if ( c )
		c->SetRatingFocus();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class RatingView
////

wxIMPLEMENT_DYNAMIC_CLASS(RatingView, lkSQL3RecordView)

RatingView::RatingView() : lkSQL3RecordView()
{
}

//virtual
bool RatingView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 277)
#else
			wxSize(355, 283)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Ratings"), pos, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 189)
#else
			wxSize(355, 187)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new RatingCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void RatingView::OnUpdate(wxView* sender, wxObject* hint)
{
}

//virtual
wxString RatingView::GetConfigPath() const
{
	return wxT("VRating");
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// class RatingTemplate
////

wxIMPLEMENT_CLASS(RatingTemplate, lkChildDocTemplate)

RatingTemplate::RatingTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* RatingTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new RatingDocument(parent);
}

//static
wxDocTemplate* RatingTemplate::CreateTemplate(MainDocManager* man)
{
	return new RatingTemplate(man, wxCLASSINFO(RatingDocument), wxCLASSINFO(RatingView));
}

