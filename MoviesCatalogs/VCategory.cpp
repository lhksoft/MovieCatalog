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
#include "VCategory.h"
#include "MainDocument.h"
#include "lkSQL3Valid.h"
#include "TMovies.h"
#include "SimpleListbox.h"
#include "DlgSubCat.h"
#include "../lkControls/lkCanvas.h"
#include "Backgrounds.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/config.h>
#include <wx/statbox.h>
#include <wx/msgdlg.h>

#include "../lkControls/lkColour.h"
#include "../lkControls/lkChkImgListbox.h"
#include "ImageCtrl.h"

#include "Defines.h"

#include "../lkSQLite3/lkSQL3Validators.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#define conf_PATH	wxT("CategoryView")

//////////////////////////////////////////////////////////////////////////////////////
// class CategoryCanvas

class CategoryCanvas : public lkSQL3panel
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	CategoryCanvas(wxView* view, wxWindow* parent = NULL);

	bool					Create(void);

	virtual bool			TransferDataToWindow() wxOVERRIDE;

	// m_SubsList : from which is selected, returns the lParam - if none selected, return 0
	wxUint32				GetSelectedLParam(void);
	bool					HasSelection(void);
	void					DeSelectAny(void);

	void					SetCatFocus(void);

protected:
	wxTextCtrl*				m_CatCtrl;
	SubCatListbox*			m_SubsList;

// Events
	void					OnUpdateSubAdd(wxUpdateUIEvent& event);
	void					OnUpdateSubEdit(wxUpdateUIEvent& event);
	void					OnUpdateSubDel(wxUpdateUIEvent& event);

	wxDECLARE_EVENT_TABLE();

	wxDECLARE_CLASS(CategoryCanvas);
	wxDECLARE_NO_COPY_CLASS(CategoryCanvas);
};

// /////////////////////////////////////////////////////////////////////////////////////////////
// Class CategoryDocument
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_ABSTRACT_CLASS2(CategoryDocument, lkSQL3RecordDocument, TCategory)

CategoryDocument::CategoryDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TCategory()
{
	TCategory::SetDatabase(GetDB());
	m_curCat = 0;
}

//virtual
lkSQL3RecordSet* CategoryDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
wxString CategoryDocument::GetDefaultFilter() const
{
	wxString whr;
	whr.Printf(wxT("([%s] = 0)"), t3Category_subID);
	return whr;
}

//virtual
wxString CategoryDocument::GetDefaultOrder() const
{
	wxString order;
	order.Printf(_T("[%s] COLLATE lkUTF8compare"), t3Category_NAME);
	return order;
}

//virtual
bool CategoryDocument::LoadData()
{
	bool bRet = TCategory::LoadData();
	if ( bRet && !IsAdding())
		m_curCat = GetCatValue();
	return bRet;
}

//virtual
void CategoryDocument::DoThingsBeforeUpdate()
{
	if ( m_pSet && IsAdding() )
	{
		wxUint32 freeCat = TCategory::GetFreeCatID(GetDB());
		SetCatValue(freeCat);
	}
}

//virtual
void CategoryDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	CategoryView* view = (v && v->IsKindOf(wxCLASSINFO(CategoryView))) ? dynamic_cast<CategoryView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	CategoryCanvas* c = (p && p->IsKindOf(wxCLASSINFO(CategoryCanvas))) ? dynamic_cast<CategoryCanvas*>(p) : NULL;

	if ( c )
		c->SetCatFocus();
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// Class CategoryTemplate
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CategoryTemplate, lkChildDocTemplate)

CategoryTemplate::CategoryTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* CategoryTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new CategoryDocument(parent);
}

//static
wxDocTemplate* CategoryTemplate::CreateTemplate(MainDocManager* man)
{
	return new CategoryTemplate(man, wxCLASSINFO(CategoryDocument), wxCLASSINFO(CategoryView));
}

// /////////////////////////////////////////////////////////////////////////////////////////////
// class CategoryCanvas
// /////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CategoryCanvas, lkSQL3panel)
	EVT_UPDATE_UI(ID_CAT_SUBS_Add, CategoryCanvas::OnUpdateSubAdd)
	EVT_UPDATE_UI(ID_CAT_SUBS_Edit, CategoryCanvas::OnUpdateSubEdit)
	EVT_UPDATE_UI(ID_CAT_SUBS_Del, CategoryCanvas::OnUpdateSubDel)
wxEND_EVENT_TABLE()

wxIMPLEMENT_CLASS(CategoryCanvas, lkSQL3panel)

CategoryCanvas::CategoryCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent)
{
	m_SubsList = NULL;
	m_CatCtrl = NULL;
	Create();
}

bool CategoryCanvas::Create()
{
	// we need this for our Validators
	CategoryDocument* catDoc = NULL;
	{
//		wxWindow* p = GetParent();
		wxDocument* doc = GetDocument();
//			(m_pView) ? m_pView->GetDocument() :
//			((p && p->IsKindOf(wxCLASSINFO(lkSQL3RecordChildFrame))) ? (dynamic_cast<lkSQL3RecordChildFrame*>(p))->GetDocument() : NULL);

		if ( doc && doc->IsKindOf(wxCLASSINFO(CategoryDocument)) )
			catDoc = dynamic_cast<CategoryDocument*>(doc);

		// this forces the document to open the recordset
		if ( catDoc )
			(void)catDoc->GetRecordset();
	}
	wxASSERT_MSG(catDoc != NULL, wxT("wxDocument needs to be of type CategoryDocument"));
	if ( catDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetImage_FBG_CATEGORY()) )
		return false;

	wxBoxSizer* szMain = new wxBoxSizer(wxVERTICAL), * szVer;

	wxBoxSizer* szTop = new wxBoxSizer(wxHORIZONTAL);
	{
		ImageCtrl* iCtrl;
		szTop->Add(iCtrl = new ImageCtrl(this, wxSize(36, 36), wxID_ANY, lkSQL3ImageValidator(catDoc->m_pFldImage)), 0, wxRIGHT | wxFIXED_MINSIZE | wxALIGN_CENTER, 10);
		iCtrl->SetView(static_cast<lkView*>(m_pView));
		iCtrl->SetConfigSection(conf_PATH);
	}
	szVer = new wxBoxSizer(wxVERTICAL);
	/* *******************************************************************************
	 * Category Name (EditCtrl) + Sort-By checkbox
	 * ******************************************************************************* */
	szVer->Add(new wxStaticText(this, wxID_ANY, wxT("Category :"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0);
	szVer->Add(m_CatCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_LEFT,
										  lkSQL3TextValidator(catDoc->m_pFldName, wxFILTER_NONE, &TCategory::IsUniqueName, catDoc->GetDB(), NULL)), 0, wxEXPAND);
	szTop->Add(szVer, 1, wxBOTTOM | wxEXPAND, 5);
	szMain->Add(szTop, 0, wxLEFT | wxTOP | wxRIGHT | wxEXPAND, 10);
	/* *******************************************************************************
	 * SubCategory Box
	 * ******************************************************************************* */
	wxStaticBoxSizer* szBox = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT(" SubCategories "));
	wxWindow* bxParent = szBox->GetStaticBox();
	m_SubsList = new SubCatListbox(catDoc->GetDB(), &(catDoc->m_curCat));
	m_SubsList->Create(bxParent, ID_CAT_SubsList, wxSize(-1, 4 * 18 + 4));
	m_SubsList->SetSelectionBackground(MAKE_RGB(0x00, 0x88, 0xC0));
	szBox->Add(m_SubsList, 1, wxEXPAND | wxALL, 5);
	// Buttons for Record Navigation of SubsBox
	szVer = new wxBoxSizer(wxVERTICAL);
	szVer->Add(new wxButton(bxParent, ID_CAT_SUBS_Add, wxT("Add")), 0, wxBOTTOM, 5);
	szVer->Add(new wxButton(bxParent, ID_CAT_SUBS_Edit, wxT("Edit")), 0, wxBOTTOM, 5);
	szVer->Add(new wxButton(bxParent, ID_CAT_SUBS_Del, wxT("Delete")), 0);
	szBox->Add(szVer, 0, wxLEFT | wxTOP | wxRIGHT, 5);

	szMain->Add(szBox, 2, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	SetSizer(szMain);
	Layout();

	return true;
}

//virtual
bool CategoryCanvas::TransferDataToWindow()
{
	bool bRet = lkSQL3panel::TransferDataToWindow();
	if ( bRet )
	{
		// update subs listbox
		if ( m_SubsList )
		{
			m_SubsList->BuildList();
//			m_SubsList->Refresh();
		}
	}

	return bRet;
}

wxUint32 CategoryCanvas::GetSelectedLParam()
{
	if ( !m_SubsList || !HasSelection() )
		return 0;

	wxASSERT(!m_SubsList->HasMultipleSelection());
	int sel = m_SubsList->GetSelection();
	if ( sel == wxNOT_FOUND )
		return 0;
	else
		return (wxUint32)(m_SubsList->GetLParam((unsigned int)sel));
}
bool CategoryCanvas::HasSelection()
{
	return (m_SubsList && (m_SubsList->GetSelectedCount() > 0));
}
void CategoryCanvas::DeSelectAny()
{
	if ( m_SubsList && !m_SubsList->IsEmpty() )
		m_SubsList->SetSelection(-1);
}

void CategoryCanvas::SetCatFocus()
{
	if ( m_CatCtrl )
		m_CatCtrl->SetFocus();
}

void CategoryCanvas::OnUpdateSubAdd(wxUpdateUIEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;

	bool b = (doc && !(doc->IsAdding()) && doc->CanUpdate());
	if ( b )
		b = (doc->m_pFldCatID->GetValue2() > 0);

	event.Enable(b);
}
void CategoryCanvas::OnUpdateSubEdit(wxUpdateUIEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;
//	CategoryCanvas* canvas = dynamic_cast<CategoryCanvas*>(m_pCanvas);

	bool b = (doc && !(doc->IsAdding()) && doc->CanUpdate());
	if ( b )
		b = (doc->m_pFldCatID->GetValue2() > 0);
	if ( b )
		b = HasSelection();

	event.Enable(b);
}
void CategoryCanvas::OnUpdateSubDel(wxUpdateUIEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;
//	CategoryCanvas* canvas = dynamic_cast<CategoryCanvas*>(m_pCanvas);
	wxUint32 nCat, nSub;

	bool b = (doc && !(doc->IsAdding()) && doc->CanUpdate());
	if ( b )
		b = ((nCat = doc->m_pFldCatID->GetValue2()) > 0);
	if ( b )
		b = HasSelection();

	if ( b )
	{
		nSub = GetSelectedLParam();
		b = !(TMovies::HasCat(doc->GetDB(), nCat, nSub));
	}

	event.Enable(b);
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// Class CategoryView
// /////////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_DYNAMIC_CLASS(CategoryView, lkSQL3RecordView)

wxBEGIN_EVENT_TABLE(CategoryView, lkSQL3RecordView)
	EVT_BUTTON(ID_CAT_SUBS_Add, CategoryView::OnSubAdd)
	EVT_BUTTON(ID_CAT_SUBS_Edit, CategoryView::OnSubEdit)
	EVT_BUTTON(ID_CAT_SUBS_Del, CategoryView::OnSubDel)
wxEND_EVENT_TABLE()

CategoryView::CategoryView() : lkSQL3RecordView()
{
}

//virtual
bool CategoryView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		lkSQL3RecordChildFrame* frame = new lkSQL3RecordChildFrame();

		wxSize size =
#ifdef __WXMSW__		
			wxSize(350, 278)
#else
			wxSize(350, 315)
#endif
			;
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Categories"), wxDefaultPosition, size);

		size =
#ifdef __WXMSW__
			wxSize(334, 190)
#else
			wxSize(350, 219)
#endif
			;
		frame->SetMinClientSize(size);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new CategoryCanvas(this, frame);
		frame->Show(true);

		InitialUpdate();
/*
		wxInitDialogEvent* event = new wxInitDialogEvent(0);
		event->SetEventObject(this);
		wxQueueEvent(this, event);
*/
		return true;
	}


	return false;
}

//virtual
void CategoryView::OnUpdate(wxView* sender, wxObject* hint)
{
}


/////////////////////////////////////////////////////////////////////////
// Event Processing

void CategoryView::OnSubAdd(wxCommandEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;
	wxUint32 c = (doc) ? doc->m_pFldCatID->GetValue2() : 0;
	if ( (!doc || doc->IsAdding()) || (c == 0) )
		return;

	SubCatDlg* dlg = new SubCatDlg(GetDB(), c, 0);
	dlg->SetConfigSection(conf_PATH);
	if ( dlg->Create(GetFrame()) )
	{
		SetCanClose(false);
		int retVal = dlg->ShowModal();
		dlg->Destroy();
		SetCanClose(true);

		if ( retVal == wxID_OK )
		{
			UpdateData(false);
			d->UpdateAllViews(this, doc->GetRecordset());
		}
	}
	delete dlg;
}
void CategoryView::OnSubEdit(wxCommandEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;
	CategoryCanvas* canvas = dynamic_cast<CategoryCanvas*>(m_pCanvas);

	wxUint32 c = (doc) ? doc->m_pFldCatID->GetValue2() : 0;
	wxUint32 s = canvas->GetSelectedLParam();

	if ( (!doc || doc->IsAdding()) || (c == 0) || (s == 0) )
		return;

	SubCatDlg* dlg = new SubCatDlg(GetDB(), c, s);
	dlg->SetConfigSection(conf_PATH);
	if ( dlg->Create(GetFrame()) )
	{
		SetCanClose(false);
		int retVal = dlg->ShowModal();
		dlg->Destroy();
		SetCanClose(true);

		if ( retVal == wxID_OK )
		{
			UpdateData(false);
			d->UpdateAllViews(this, doc->GetRecordset());
		}
	}
	delete dlg;
}

void CategoryView::OnSubDel(wxCommandEvent& event)
{
	wxDocument* d = GetDocument();
	CategoryDocument* doc = (d && d->IsKindOf(wxCLASSINFO(CategoryDocument))) ? dynamic_cast<CategoryDocument*>(d) : NULL;
	CategoryCanvas* canvas = dynamic_cast<CategoryCanvas*>(m_pCanvas);

	wxUint32 c = (doc) ? doc->m_pFldCatID->GetValue2() : 0;
	wxUint32 s = canvas->GetSelectedLParam();

	if ( (!doc || doc->IsAdding()) || (c == 0) || (s == 0) )
		return;

	wxString ttl = wxT("Confirm"), msg;
	if ( TCategory::HasImages(GetDB(), c, s) )
	{
		msg = wxT("The selected SubCategory still has an accosiated image.\nAre you sure you want to delete it and so loosing this image?\n\n" \
				  "You can save this image via the <Edit> button, then right-click the image.");

		SetCanClose(false);
		int nRet = wxMessageBox(msg, ttl, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, GetFrame());
		SetCanClose(true);

		if ( nRet == wxNO )
			return;
		// else...
	}
	else
	{
		msg = wxT("This action will permanently destroy this SubCategory.\nAre you sure you want to DELETE this item ?");

		SetCanClose(false);
		int nRet = wxMessageBox(msg, ttl, wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, GetFrame());
		SetCanClose(true);

		if ( nRet == wxNO )
			return;
		// else...
	}

	TCategory pSub(GetDB());
	{
		wxString qry;
		qry.Printf(wxT("([%s] = %u) AND ([%s] == %u)"), t3Category_catID, c, t3Category_subID, s);
		pSub.SetFilter(qry);
	}
	try
	{
		pSub.Open();
	}
	catch ( const lkSQL3Exception& e )
	{
		((lkSQL3Exception*)&e)->ReportError();
		return;
	}

	if ( pSub.CanDelete() )
	{
		try
		{
			pSub.Delete();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			return;
		}
		UpdateData(false);
		canvas->DeSelectAny();
	}
}
