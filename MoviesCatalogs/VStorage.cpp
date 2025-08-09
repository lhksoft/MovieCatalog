#include "VStorage.h"
#include "CStorage.h"
#include "MainDocument.h"
#include "Defines.h"
#include "XPMs.h" // resources


#include "../lkSQLite3/lkDateTime.h"
#include "../lkSQLite3/lkSQL3Exception.h"

#include "TLocation.h"
#include "TMedium.h"

#include "DlgStorageFilter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageFrame
////
class StorageFrame : public lkSQL3RecordChildFrame
{
public:
	StorageFrame();

	virtual wxToolBar*		OnCreateToolBar(long style, wxWindowID winid, const wxString& name) wxOVERRIDE;

protected:
	wxWindowID				m_nID_NextID;
	wxWindowID				m_nID_Filter;
	wxWindowID				m_nID_FilterToggle;
	wxWindowID				m_nID_Search;
	wxWindowID				m_nID_SearchNext;
	wxWindowID				m_nID_SearchPrev;

public:
	wxWindowID				GetID_NextID(void) const;
	wxWindowID				GetID_Filter(void) const;
	wxWindowID				GetID_FilterToggle(void) const;
	wxWindowID				GetID_Search(void) const;
	wxWindowID				GetID_SearchNext(void) const;
	wxWindowID				GetID_SearchPrev(void) const;

private:
	wxDECLARE_CLASS(StorageFrame);
	wxDECLARE_NO_COPY_CLASS(StorageFrame);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageCanvas
////
class StorageCanvas : public lkSQL3panel, public CStorage
{
public:
	// view may be NULL if we're not associated with one yet, but parent must then be a valid pointer
	StorageCanvas(wxView* view, wxWindow* parent = NULL);

	bool							Create(void);

	virtual bool					Validate() wxOVERRIDE;
	virtual bool					TransferDataToWindow() wxOVERRIDE;

	MediumBox*						GetMedium(void);
	lkTransTextCtrl*				GetStorage(void);

protected:
	virtual bool					IsMediumChanged(void) const wxOVERRIDE;
	virtual bool					IsLocationChanged(void) const wxOVERRIDE;

	virtual lkSQL3Database*			GetDBase(void) wxOVERRIDE;
	virtual lkSQL3RecordSet*		GetBaseRecordset(void) wxOVERRIDE;

	virtual lkSQL3FieldData_Text*	GetFldStorage(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldLocation(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldMedium(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Real*	GetFldCreation(void) wxOVERRIDE;
	virtual lkSQL3FieldData_Int*	GetFldInfo(void) wxOVERRIDE;

private:
	StorageDocument*				m_StorDoc;

	wxDECLARE_CLASS(StorageCanvas);
	wxDECLARE_NO_COPY_CLASS(StorageCanvas);
};

wxIMPLEMENT_CLASS(StorageCanvas, lkSQL3panel)

StorageCanvas::StorageCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent), CStorage()
{
	m_StorDoc = NULL;
	{
		lkSQL3RecordDocument* doc = GetDocument();

		if ( doc && doc->IsKindOf(wxCLASSINFO(StorageDocument)) )
			m_StorDoc = dynamic_cast<StorageDocument*>(doc);
	}

	Create();
}

MediumBox* StorageCanvas::GetMedium()
{
	return m_pMedium;
}
lkTransTextCtrl* StorageCanvas::GetStorage()
{
	return m_pStorageCtrl;
}

bool StorageCanvas::Create()
{
	// we need this for our Validators
	if ( m_StorDoc )
			(void)m_StorDoc->GetRecordset();

	wxASSERT_MSG(m_StorDoc != NULL, wxT("wxDocument needs to be of type StorageDocument"));
	if ( m_StorDoc == NULL )
		return false; // seriously ?

	if ( !lkSQL3panel::Create(GetParent(), GetBackground()) )
		return false;

	CStorage::CreateCanvas(GetView()->GetFrame(), this);

	Bind(wxEVT_BUTTON, &CStorage::OnSelectDate, this, m_ID_DateBtn);

	return true;
}

//virtual
bool StorageCanvas::Validate()
{
	m_bReqAdd = (m_StorDoc && m_StorDoc->IsAdding());
	return lkSQL3panel::Validate();
}

//virtual
bool StorageCanvas::TransferDataToWindow()
{
	wxASSERT(m_StorDoc != NULL);
	if ( !m_StorDoc )
		return false;

	bool bInitial = false;
	{
		lkSQL3RecordView* vw = GetView();
		bInitial = (vw && vw->IsInitial());
	}

	stTransferDataToWindow(bInitial);

	m_StorDoc->SetMediumChanged(false);
	m_StorDoc->SetLocationChanged(false);

	bool bRet = lkSQL3panel::TransferDataToWindow();
	return bRet;
}


//virtual
bool StorageCanvas::IsMediumChanged() const
{
	if ( m_StorDoc )
		return m_StorDoc->MediumChanged();
	return false;
}
//virtual
bool StorageCanvas::IsLocationChanged() const
{
	if ( m_StorDoc )
		return m_StorDoc->LocationChanged();
	return false;
}

//virtual
lkSQL3Database* StorageCanvas::GetDBase()
{
	if ( m_StorDoc )
		return m_StorDoc->GetDB();
	return NULL;
}
//virtual
lkSQL3RecordSet* StorageCanvas::GetBaseRecordset()
{
	return dynamic_cast<lkSQL3RecordSet*>(m_StorDoc);
}

//virtual
lkSQL3FieldData_Text* StorageCanvas::GetFldStorage()
{
	if ( m_StorDoc )
		return m_StorDoc->m_pFldStorage;
	return NULL;
}
//virtual
lkSQL3FieldData_Int* StorageCanvas::GetFldLocation()
{
	if ( m_StorDoc )
		return m_StorDoc->m_pFldLocation;
	return NULL;
}
//virtual
lkSQL3FieldData_Int* StorageCanvas::GetFldMedium()
{
	if ( m_StorDoc )
		return m_StorDoc->m_pFldMedium;
	return NULL;
}
//virtual
lkSQL3FieldData_Real* StorageCanvas::GetFldCreation()
{
	if ( m_StorDoc )
		return m_StorDoc->m_pFldCreation;
	return NULL;
}
//virtual
lkSQL3FieldData_Int* StorageCanvas::GetFldInfo()
{
	if ( m_StorDoc )
		return m_StorDoc->m_pFldInfo;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageDocument
////
wxIMPLEMENT_ABSTRACT_CLASS2(StorageDocument, lkSQL3RecordDocument, TStorage)

StorageDocument::StorageDocument(wxDocument* pParent) : lkSQL3RecordDocument(pParent), TStorage()
{
	m_bMedium = m_bLocation = false;
	TStorage::SetDatabase(GetDB());
}

//virtual
wxString StorageDocument::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* StorageDocument::GetBaseSet()
{
	return dynamic_cast<lkSQL3RecordSet*>(this);
}

//virtual
void StorageDocument::BindButtons()
{
	lkSQL3RecordDocument::BindButtons();

	StorageFrame* frame = NULL;
	{
		lkSQL3RecordChildFrame* f = dynamic_cast<lkSQL3RecordChildFrame*>(GetFrame());
		if ( f && f->IsKindOf(wxCLASSINFO(StorageFrame)) )
			frame = dynamic_cast<StorageFrame*>(f);
	}
	if ( !frame )
		return;

	Bind(wxEVT_UPDATE_UI, &StorageDocument::OnUpdateFind, this, frame->GetID_Search());
	Bind(wxEVT_UPDATE_UI, &StorageDocument::OnUpdateFindNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &StorageDocument::OnUpdateFindPrev, this, frame->GetID_SearchPrev());

	Bind(wxEVT_UPDATE_UI, &StorageDocument::OnUpdateFilter, this, frame->GetID_Filter());
	Bind(wxEVT_UPDATE_UI, &StorageDocument::OnUpdateFilterToggle, this, frame->GetID_FilterToggle());

	Bind(wxEVT_TOOL, &StorageDocument::OnSearch, this, frame->GetID_Search());
	Bind(wxEVT_TOOL, &StorageDocument::OnFilter, this, frame->GetID_Filter());
	Bind(wxEVT_TOOL, &StorageDocument::OnFilterToggle, this, frame->GetID_FilterToggle());
	Bind(wxEVT_TOOL, &StorageDocument::OnSearchNext, this, frame->GetID_SearchNext());
	Bind(wxEVT_TOOL, &StorageDocument::OnSearchPrev, this, frame->GetID_SearchPrev());
}

bool StorageDocument::MediumChanged() const
{
	return m_bMedium;
}
void StorageDocument::SetMediumChanged(bool b)
{
	m_bMedium = b;
}

bool StorageDocument::LocationChanged() const
{
	return m_bLocation;
}
void StorageDocument::SetLocationChanged(bool b)
{
	m_bLocation = b;
}

//virtual
void StorageDocument::DoThingsBeforeAdd()
{
	wxView* v = GetFirstView();
	StorageView* view = (v && v->IsKindOf(wxCLASSINFO(StorageView))) ? dynamic_cast<StorageView*>(v) : NULL;
	lkSQL3panel* p = (view) ? view->GetCanvas() : NULL;
	StorageCanvas* c = (p && p->IsKindOf(wxCLASSINFO(StorageCanvas))) ? dynamic_cast<StorageCanvas*>(p) : NULL;

	if ( c )
		c->SetStorageFocus();
}

void StorageDocument::OnUpdateFilter(wxUpdateUIEvent& event)
{
	event.Enable(!m_bFilterActive && (m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false);
}
void StorageDocument::OnUpdateFilterToggle(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty() && (m_bFilterActive || !m_sFltFilter.IsEmpty())) : false);
}

void StorageDocument::OnSearch(wxCommandEvent& WXUNUSED(event))
{
	TStorage* pRS = dynamic_cast<TStorage*>(GetBaseSet());
	wxUint64 _row = DlgStorageFind::FindStorage(GetFrame(), pRS);

	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDocument::UpdateData(false);
	}
}

void StorageDocument::OnFilter(wxCommandEvent& WXUNUSED(event))
{
	StorageFilterDlg filter(GetDB());
	if ( filter.Create(GetFrame()) && (filter.ShowModal() == wxID_OK) )
	{
		SetFltFilter(filter.GetFilter());
		SetFltOrder(filter.GetOrder());
		SetFilterActive(true);

		try
		{
			Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			SetFilterActive(false);
			SetFltFilter(wxEmptyString);
			SetFltOrder(wxEmptyString);
		}

		lkSQL3RecordDocument::UpdateData(false);
	}
}

void StorageDocument::OnFilterToggle(wxCommandEvent& WXUNUSED(event))
{
	if ( m_bFilterActive || !m_sFltFilter.IsEmpty() )
	{
		SetFilterActive(!m_bFilterActive);

		try
		{
			Requery();
		}
		catch ( const lkSQL3Exception& e )
		{
			((lkSQL3Exception*)&e)->ReportError();
			SetFilterActive(!m_bFilterActive);
		}

		lkSQL3RecordDocument::UpdateData(false);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class StorageView
////

wxIMPLEMENT_DYNAMIC_CLASS(StorageView, lkSQL3RecordView)

StorageView::StorageView() : lkSQL3RecordView()
{
}

//virtual
bool StorageView::OnCreate(wxDocument* doc, long flags)
{
	if ( !lkSQL3RecordView::OnCreate(doc, flags) )
		return false;

	wxDocParentFrame* parent = GetParentFrame();
	if ( parent )
	{
		StorageFrame* frame = new StorageFrame();

		wxSize size =
#ifdef __WXMSW__
			wxSize(531, 219)
#else
			wxSize(541, 245)
#endif
			;

		wxPoint pos = GetConfigPosition();
		(void)frame->Create(doc, this, static_cast<wxDocParentFrame*>(parent), wxT("Storage"), pos, size, (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR);

		size =
#ifdef __WXMSW__
			wxSize(515, 125)
#else
			wxSize(541,149)
#endif
			;
		frame->SetMinClientSize(size);
		frame->SetStatusText(wxEmptyString, 1);
		SetFrame(frame);

		// Bind events in our document
		(dynamic_cast<lkSQL3RecordDocument*>(doc))->BindButtons();

		m_pCanvas = new StorageCanvas(this, frame);
		frame->Show(true);

		Bind(wxEVT_UPDATE_UI, &StorageView::OnUpdateNextID, this, frame->GetID_NextID());
		Bind(wxEVT_TOOL, &StorageView::OnNextID, this, frame->GetID_NextID());

		InitialUpdate();
		return true;
	}

	return false;
}

//virtual
void StorageView::InitialUpdate()
{
	wxDocument* doc = GetDocument();
	StorageDocument* strDoc = (doc && doc->IsKindOf(wxCLASSINFO(StorageDocument))) ? dynamic_cast<StorageDocument*>(doc) : NULL;
	if ( strDoc )
		strDoc->MoveLast();

	lkSQL3RecordView::InitialUpdate();
}

//virtual
wxString StorageView::GetConfigPath() const
{
	return wxT("VStorage");
}

//virtual
void StorageView::ProcessRecordset(lkSQL3RecordSet* pSet)
{
	if ( m_bClosing )
		return;

	wxDocument* doc = GetDocument();
	StorageDocument* strDoc = (doc && doc->IsKindOf(wxCLASSINFO(StorageDocument))) ? dynamic_cast<StorageDocument*>(doc) : NULL;

	if ( strDoc && pSet )
	{
		if ( pSet->IsKindOf(wxCLASSINFO(TLocation)) )
			strDoc->SetLocationChanged(true);
		else if ( pSet->IsKindOf(wxCLASSINFO(TMedium)) )
			strDoc->SetMediumChanged(true);
		else
			return;

		if ( UpdateData(true) )
			UpdateData(false);
	}
}

void StorageView::OnUpdateNextID(wxUpdateUIEvent& event)
{
	StorageCanvas* canvas = (m_pCanvas && m_pCanvas->IsKindOf(wxCLASSINFO(StorageCanvas))) ? dynamic_cast<StorageCanvas*>(m_pCanvas) : NULL;
	wxDocument* d = GetDocument();
	StorageDocument* doc = (d && d->IsKindOf(wxCLASSINFO(StorageDocument))) ? dynamic_cast<StorageDocument*>(d) : NULL;
	lkTransTextCtrl* t;
	MediumBox* m;

	bool bEnable = (doc && doc->IsAdding()); // only allow when adding new data
	if ( bEnable )
		bEnable = (canvas && (m = canvas->GetMedium()) && (m->GetSelectedLParam() != 0)); // a medium should be selected
	if ( bEnable )
		bEnable = (canvas && (t = canvas->GetStorage()) && t->GetValue().IsEmpty()); // must be empty to get it work

	event.Enable(bEnable);
}

void StorageView::OnNextID(wxCommandEvent& event)
{
	StorageCanvas* canvas = (m_pCanvas && m_pCanvas->IsKindOf(wxCLASSINFO(StorageCanvas))) ? dynamic_cast<StorageCanvas*>(m_pCanvas) : NULL;
	wxDocument* d = GetDocument();
	StorageDocument* doc = (d && d->IsKindOf(wxCLASSINFO(StorageDocument))) ? dynamic_cast<StorageDocument*>(d) : NULL;
	lkTransTextCtrl* t = (canvas) ? canvas->GetStorage() : NULL;
	MediumBox* m = (canvas) ? canvas->GetMedium() : NULL;

	bool bEnable = (doc && doc->IsAdding()); // only allow when adding new data
	if ( bEnable )
		bEnable = (m && (m->GetSelectedLParam() != 0)); // a medium should be selected
	if ( bEnable )
		bEnable = (t && t->GetValue().IsEmpty()); // must be empty to get it work

	if ( !bEnable )
		return; // just to be sure

	wxUint32 medium = static_cast<wxUint32>(m->GetSelectedLParam());
	wxString volume = (medium > 0) ? TStorage::GetNextLabel(GetDB(), medium) : wxT("");
	if ( !volume.IsEmpty() )
		t->SetValue(volume);
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class StorageTemplate
////

wxIMPLEMENT_CLASS(StorageTemplate, lkChildDocTemplate)

StorageTemplate::StorageTemplate(MainDocManager* manager, wxClassInfo* parentDocClassInfo, wxClassInfo* viewClassInfo) :
	lkChildDocTemplate(manager, parentDocClassInfo, viewClassInfo)
{
}

//virtual
wxDocument* StorageTemplate::DoCreateDocument()
{
	wxASSERT(m_documentManager != NULL);

	wxDocument* parent = NULL;
	if ( m_documentManager && m_documentManager->IsKindOf(wxCLASSINFO(MainDocManager)) )
		parent = (dynamic_cast<MainDocManager*>(m_documentManager))->GetMainDocument(wxCLASSINFO(MainDocument));
	wxASSERT(parent != NULL);

	return new StorageDocument(parent);
}

//static
wxDocTemplate* StorageTemplate::CreateTemplate(MainDocManager* man)
{
	return new StorageTemplate(man, wxCLASSINFO(StorageDocument), wxCLASSINFO(StorageView));
}


// /////////////////////////////////////////////////////////////////////////////////////////////
// class StorageFrame
////
wxIMPLEMENT_CLASS(StorageFrame, lkSQL3RecordChildFrame)

StorageFrame::StorageFrame() : lkSQL3RecordChildFrame()
{
	m_nID_Filter = wxWindow::NewControlId(1);
	m_nID_FilterToggle = wxWindow::NewControlId(1);

	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);

	m_nID_NextID = wxWindow::NewControlId(1);
}

wxToolBar* StorageFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordChildFrame::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_NextID(), wxT(""), Get_Group_Xpm(), wxT("Find an ID"));
		tb->AddSeparator();
		tb->AddTool(GetID_Filter(), wxT(""), Get_Filter_Xpm(), wxT("Define a Filter"));
		tb->AddTool(GetID_FilterToggle(), wxT(""), Get_FilterTogle_Xpm(), wxT("Toggle Filter"));
		tb->AddSeparator();
		tb->AddTool(GetID_Search(), wxT(""), Get_Search_Xpm(), wxT("Start a Search"));
		tb->AddTool(GetID_SearchNext(), wxT(""), Get_SearchNext_Xpm(), wxT("Search Next"));
		tb->AddTool(GetID_SearchPrev(), wxT(""), Get_SearchPrev_Xpm(), wxT("Search Back"));
	}
	return tb;
}

wxWindowID StorageFrame::GetID_NextID() const
{
	return m_nID_NextID;
}
wxWindowID StorageFrame::GetID_Filter() const
{
	return m_nID_Filter;
}
wxWindowID StorageFrame::GetID_FilterToggle() const
{
	return m_nID_FilterToggle;
}
wxWindowID StorageFrame::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID StorageFrame::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID StorageFrame::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}


