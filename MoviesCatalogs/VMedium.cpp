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
#include "VMedium.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/config.h>

//#include <lkChkImgListbox.h>
#include "ImageCtrl.h"
#include "SimpleListbox.h" // Country's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"

#define conf_MEDIUM_PATH	wxT("Media")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MediumCanvas
////
class MediumCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	MediumCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetMediumFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	MediumListbox*			m_lbMedia;
	wxTextCtrl*				m_pShortCtrl;

	wxDECLARE_CLASS(MediumCanvas);
	wxDECLARE_NO_COPY_CLASS(MediumCanvas);
};

wxIMPLEMENT_CLASS(MediumCanvas, lkSQL3panel)

MediumCanvas::MediumCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbMedia = NULL;
	m_pShortCtrl = NULL;
	Create();
}
bool MediumCanvas::Create()
{
	// we need this for our Validators
	MediumDocument* mdmDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(MediumDocument)) )
			mdmDoc = dynamic_cast<MediumDocument*>(doc);

		// this forces the document to open the recordset
		if ( mdmDoc )
			(void)mdmDoc->GetRecordset();
	}
	wxASSERT_MSG(mdmDoc != NULL, wxT("wxDocument needs to be of type MediumDocument"));
	if ( mdmDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_MEDIA()) )
		return false;

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL);
	{ // Image
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(mdmDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_MEDIUM_PATH);
	}

int width = 80;
#ifdef __WXMSW__
	width = 70;
#endif
	wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL), * szHor;
	{ // 1st Line : Short Name
		szHor = new wxBoxSizer(wxHORIZONTAL);

		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Short Name :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxRIGHT, 5);

		szHor->Add(m_pShortCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
												 lkSQL3TextValidator(mdmDoc->m_pFldCode, wxFILTER_NONE, &TMedium::IsUniqueCode, mdmDoc->GetDB(), NULL)), 1, wxEXPAND);
		szVer->Add(szHor, 0, wxEXPAND);
	}
	{ // 2nd Line : Long Name
		szHor = new wxBoxSizer(wxHORIZONTAL);

		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Long Name :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxTOP | wxRIGHT, 5);

		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, lkSQL3TextValidator(mdmDoc->m_pFldName)), 1, wxEXPAND | wxTOP, 5);
		szVer->Add(szHor, 0, wxEXPAND);
	}
	{ // 3th Line : Likes / Not Likes
		szHor = new wxBoxSizer(wxHORIZONTAL);
		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Prefix :"), wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxTOP | wxRIGHT, 5);
		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(35, -1), wxTE_LEFT, lkSQL3TextValidator(mdmDoc->m_pFldLikes)), 1, wxEXPAND | wxTOP, 5);

		szHor->Add(new wxStaticText(this, wxID_ANY, wxT("Dislikes :"), wxDefaultPosition, wxSize(70, -1), wxALIGN_RIGHT | wxST_NO_AUTORESIZE), 0, wxALIGN_CENTER | wxTOP | wxRIGHT, 5);
		szHor->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(35, -1), wxTE_LEFT, lkSQL3TextValidator(mdmDoc->m_pFldNotLikes)), 1, wxEXPAND | wxTOP, 5);
		szVer->Add(szHor, 0, wxEXPAND);
	}
	szTop->Add(szVer, 1, wxEXPAND);
	szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbMedia = new MediumListbox(mdmDoc->GetDB(), mdmDoc);
	m_lbMedia->Create(this, ID_MDM_Preview);
	m_lbMedia->SetSelectionBackground(MAKE_RGB(0x68, 0x88, 0x6F));
	szBox->Add(m_lbMedia, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxALL | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void MediumCanvas::SetMediumFocus()
{
	if ( m_pShortCtrl )
		m_pShortCtrl->SetFocus();
}

//virtual
bool MediumCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbMedia )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbMedia->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if ( pSet )
				m_lbMedia->ScrollIntoView(m_lbMedia->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MediumDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(MediumDocument, lkSQL3RecordDocument, TMedium)


MediumDocument::MediumDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TMedium()
{
	TMedium::SetDatabase(GetDB());
}

//virtual
wxString MediumDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* MediumDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void MediumDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_MDM_Preview);
}

//virtual
void MediumDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	MediumView* view = (v && v->IsKindOf(wxCLASSINFO(MediumView))) ? dynamic_cast<MediumView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	MediumCanvas* c = (p && p->IsKindOf(wxCLASSINFO(MediumCanvas))) ? dynamic_cast<MediumCanvas*>(p) : NULL;

	if ( c )
		c->SetMediumFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MediumView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(MediumView, lkSQL3RecordView)

MediumView::MediumView() : lkSQL3RecordView()
{
}

//virtual
bool MediumView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();
		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 305)
#else
			wxSize(365, 316)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Media"), pos, size);
		size =
#ifdef __WXMSW__
			wxSize(349, 217)
#else
			wxSize(365, 220)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new MediumCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void MediumView::OnUpdate(wxView* sender, wxObject* hint)
{
}

//virtual
wxString MediumView::GetConfigPath() const
{
	return wxT("VMedium");
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class MediumTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(MediumTemplate, lkChildDocTemplate)

MediumTemplate::MediumTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* MediumTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new MediumDocument(parent);
}

//static
wxDocTemplate* MediumTemplate::CreateTemplate(MainDocManager* man)
{
	return new MediumTemplate(man, wxCLASSINFO(MediumDocument), wxCLASSINFO(MediumView));
}

