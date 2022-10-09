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
#include "VGenre.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

//#include <wx/custombgwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/config.h>

//#include <lkChkImgListbox.h>
#include "ImageCtrl.h"
#include "SimpleListbox.h" // Country's Listbox

#include "../lkControls/lkColour.h"
#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#define conf_GENRES_PATH	wxT("Genres")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  GenreCanvas
////
class GenreCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	GenreCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetGenreFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	GenreListbox*			m_lbGenres;
	wxTextCtrl*				m_pGenreCtrl;

	wxDECLARE_CLASS(GenreCanvas);
	wxDECLARE_NO_COPY_CLASS(GenreCanvas);
};

wxIMPLEMENT_CLASS(GenreCanvas, lkSQL3panel)

GenreCanvas::GenreCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbGenres = NULL;
	m_pGenreCtrl = NULL;
	Create();
}
bool GenreCanvas::Create()
{
	// we need this for our Validators
	GenreDocument* gnrDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(GenreDocument)) )
			gnrDoc = dynamic_cast<GenreDocument*>(doc);

		// this forces the document to open the recordset
		if ( gnrDoc )
			(void)gnrDoc->GetRecordset();
	}
	wxASSERT_MSG(gnrDoc != NULL, wxT("wxDocument needs to be of type GenreDocument"));
	if ( gnrDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_GENRES()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szTop = new wxBoxSizer(wxHORIZONTAL);
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(gnrDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_GENRES_PATH);
	}

	{
		wxBoxSizer* szVer = new wxBoxSizer(wxVERTICAL);

		szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Genre Name :")), 0, wxEXPAND);

		szVer->Add(m_pGenreCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
												 lkSQL3TextValidator(gnrDoc->m_pFldGenre, wxFILTER_NONE, &TGenre::IsUniqueGenre, gnrDoc->GetDB(), NULL)), 0, wxEXPAND);
		szVer->AddSpacer(3);
		szTop->Add(szVer, 1, wxEXPAND);
	}
	szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbGenres = new GenreListbox(gnrDoc->GetDB(), gnrDoc);
	m_lbGenres->Create(this, ID_GNR_Preview, wxSize(-1, 5 * 18 + 4));
	m_lbGenres->SetSelectionBackground(MAKE_RGB(0xE2, 0x75, 0x00));
	szBox->Add(m_lbGenres, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 2, wxALL | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void GenreCanvas::SetGenreFocus()
{
	if ( m_pGenreCtrl )
		m_pGenreCtrl->SetFocus();
}

//virtual
bool GenreCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbGenres )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbGenres->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if ( pSet )
				m_lbGenres->ScrollIntoView(m_lbGenres->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class GenreDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(GenreDocument, lkSQL3RecordDocument, TGenre)

GenreDocument::GenreDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TGenre()
{
	TGenre::SetDatabase(GetDB());
}

//virtual
lkSQL3RecordSet* GenreDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
wxString GenreDocument::GetDefaultOrder() const
{
	wxString order;
	order.Printf(wxT("[%s] COLLATE lkUTF8compare"), t3Genre_GENRE);
	return order;
}

//virtual
void GenreDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_GNR_Preview);
}

//virtual
void GenreDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	GenreView* view = (v && v->IsKindOf(wxCLASSINFO(GenreView))) ? dynamic_cast<GenreView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	GenreCanvas* c = (p && p->IsKindOf(wxCLASSINFO(GenreCanvas))) ? dynamic_cast<GenreCanvas*>(p) : NULL;

	if ( c )
		c->SetGenreFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class GenreView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(GenreView, lkSQL3RecordView)

wxBEGIN_EVENT_TABLE(GenreView, lkSQL3RecordView)
wxEND_EVENT_TABLE()

GenreView::GenreView() : lkSQL3RecordView()
{
}

//virtual
bool GenreView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();
		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 376)
#else
			wxSize(365, 379)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Genres"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 288)
#else
			wxSize(365, 283)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new GenreCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void GenreView::OnUpdate(wxView* sender, wxObject* hint)
{
}

/////////////////////////////////////////////////////////////////////////
// Event Processing

// /////////////////////////////////////////////////////////////////////////////////////////////
// class GenreTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(GenreTemplate, lkChildDocTemplate)

GenreTemplate::GenreTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* GenreTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new GenreDocument(parent);
}

//static
wxDocTemplate* GenreTemplate::CreateTemplate(MainDocManager* man)
{
	return new GenreTemplate(man, wxCLASSINFO(GenreDocument), wxCLASSINFO(GenreView));
}

