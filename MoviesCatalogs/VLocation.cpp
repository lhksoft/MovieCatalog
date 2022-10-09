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
#include "VLocation.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "Defines.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/config.h>

#include "SimpleListbox.h" // Location's Listbox

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"
#include "../lkControls/lkColour.h"

#define conf_LOCATION_PATH		wxT("Locations")

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class  LocationCanvas
////
class LocationCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	LocationCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);
	void					SetLocFocus(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

private:
	LocationListbox*		m_lbLoc;
	wxTextCtrl*				m_pLocCtrl;

	wxDECLARE_CLASS(LocationCanvas);
	wxDECLARE_NO_COPY_CLASS(LocationCanvas);
};

wxIMPLEMENT_CLASS(LocationCanvas, lkSQL3panel)

LocationCanvas::LocationCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_lbLoc = NULL;
	m_pLocCtrl = NULL;
	Create();
}
bool LocationCanvas::Create()
{
	// we need this for our Validators
	LocationDocument* locDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(LocationDocument)) )
			locDoc = dynamic_cast<LocationDocument*>(doc);

		// this forces the document to open the recordset
		if ( locDoc )
			(void)locDoc->GetRecordset();
	}
	wxASSERT_MSG(locDoc != NULL, wxT("wxDocument needs to be of type LocationDocument"));
	if ( locDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_LOCATION()) )
		return false;


	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL);
	{
		szMain->Add(m_pLocCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT, 
												lkSQL3TextValidator(locDoc->m_pFldLocation, wxFILTER_NONE, &TLocation::IsUniqueLocation, locDoc->GetDB(), NULL)), 0, wxALL | wxEXPAND, 10);
	}

	/* *******************************************************************************
	 * Preview Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" Preview "));
	m_lbLoc = new LocationListbox(locDoc->GetDB(), locDoc);
	m_lbLoc->Create(this, ID_LOC_Preview);
	m_lbLoc->SetSelectionBackground(MAKE_RGB(0x00, 0x80, 0x00));
	szBox->Add(m_lbLoc, 1, wxEXPAND | wxALL, 5);

	szMain->Add(szBox, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

void LocationCanvas::SetLocFocus()
{
	if ( m_pLocCtrl )
		m_pLocCtrl->SetFocus();
}

//virtual
bool LocationCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_lbLoc )
		{
			lkSQL3RecordDocument* doc = NULL;
			lkSQL3RecordView* vw = NULL;
			if ( ((doc = GetDocument()) && doc->HasChanged()) || ((vw = GetView()) && vw->IsInitial()) )
				m_lbLoc->BuildList();

			lkSQL3RecordSet* pSet = (doc) ? doc->GetRecordset() : NULL;
			if ( pSet )
				m_lbLoc->ScrollIntoView(m_lbLoc->FindLParam(pSet->GetCurRow()));
		}
	}

	return bRet;
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class LocationDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(LocationDocument, lkSQL3RecordDocument, TLocation)


LocationDocument::LocationDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TLocation()
{
	TLocation::SetDatabase(GetDB());
}

//virtual
wxString LocationDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* LocationDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void LocationDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	Bind(lkEVT_SQL3MOVE, &lkSQL3RecordDocument::OnSQL3Move, this, ID_LOC_Preview);
}

//virtual
void LocationDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	LocationView* view = (v && v->IsKindOf(wxCLASSINFO(LocationView))) ? dynamic_cast<LocationView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	LocationCanvas* c = (p && p->IsKindOf(wxCLASSINFO(LocationCanvas))) ? dynamic_cast<LocationCanvas*>(p) : NULL;

	if ( c )
		c->SetLocFocus();
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class LocationView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(LocationView, lkSQL3RecordView)

LocationView::LocationView() : lkSQL3RecordView()
{
}

//virtual
bool LocationView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(365, 285)
#else
			wxSize(365, 287)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Locations"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(349, 197)
#else
			wxSize(365, 191)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new LocationCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void LocationView::OnUpdate(wxView* sender, wxObject* hint)
{
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class LocationTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(LocationTemplate, lkChildDocTemplate)

LocationTemplate::LocationTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* LocationTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new LocationDocument(parent);
}

//static
wxDocTemplate* LocationTemplate::CreateTemplate(MainDocManager* man)
{
	return new LocationTemplate(man, wxCLASSINFO(LocationDocument), wxCLASSINFO(LocationView));
}

