#include "DStorage.h"
#include "XPMs.h"
#include <wx/sizer.h>

wxIMPLEMENT_ABSTRACT_CLASS2(DStorage, lkSQL3RecordDialog, TStorage)

DStorage::DStorage(lkSQL3Database* pDB) : lkSQL3RecordDialog(pDB), TStorage(pDB), CStorage()
{
	m_nID_NextID = wxWindow::NewControlId(1);
	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);
}

bool DStorage::Create(wxWindow* parent)
{
	wxSize size = 
#ifdef __WXMSW__
		wxSize(531, 258)
#else
		wxSize(515, 300)
#endif
	;
	if ( !lkSQL3RecordDialog::Create(parent, CStorage::GetBackground(), wxT("Storages Dialog"), wxDefaultPosition, size, wxDEFAULT_DIALOG_STYLE) )
		return false;

	size = 
#ifdef __WXMSW__
		wxSize(515, 170)
#else
		wxSize(515, 200)
#endif
	;
	SetMinClientSize(size);
	SetStatusText(wxEmptyString, 1);

	// this forces to open the recordset and initialize its internal fields
	(void)GetRecordset();

	if ( CStorage::CreateCanvas(this, this) )
	{
		wxSizer* sz = GetSizer();
		sz->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
		Layout();

		return true;
	}

	return false;
}

wxWindowID DStorage::GetID_NextID() const
{
	return m_nID_NextID;
}
wxWindowID DStorage::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID DStorage::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID DStorage::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}

// Overrides of base-class lkSQL3RecordDialog
////////////////////////////////////////////////////////

wxToolBar* DStorage::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordDialog::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_NextID(), wxT(""), Get_Group_Xpm(), wxT("Find an ID"));
		tb->AddSeparator();
		tb->AddTool(GetID_Search(), wxT(""), Get_Search_Xpm(), wxT("Start a Search"));
		tb->AddTool(GetID_SearchNext(), wxT(""), Get_SearchNext_Xpm(), wxT("Search Next"));
		tb->AddTool(GetID_SearchPrev(), wxT(""), Get_SearchPrev_Xpm(), wxT("Search Back"));
	}
	return tb;
}

//virtual
void DStorage::BindButtons()
{
	lkSQL3RecordDialog::BindButtons();

	Bind(wxEVT_UPDATE_UI, &DStorage::OnUpdateNextID, this, GetID_NextID());
	Bind(wxEVT_UPDATE_UI, &DStorage::OnUpdateFind, this, GetID_Search());
	Bind(wxEVT_UPDATE_UI, &DStorage::OnUpdateFindNext, this, GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &DStorage::OnUpdateFindPrev, this, GetID_SearchPrev());

	Bind(wxEVT_TOOL, &DStorage::OnNextID, this, GetID_NextID());
	Bind(wxEVT_TOOL, &DStorage::OnFind, this, GetID_Search());
	Bind(wxEVT_TOOL, &DStorage::OnFindNext, this, GetID_SearchNext());
	Bind(wxEVT_TOOL, &DStorage::OnFindPrev, this, GetID_SearchPrev());

	//// (02-11-22) Appended (was missing) 
	Bind(wxEVT_BUTTON, &CStorage::OnSelectDate, this, m_ID_DateBtn);
}

//virtual
bool DStorage::Validate()
{
	lkSQL3RecordSet* pSet = GetBaseRecordset();
	m_bReqAdd = (pSet && pSet->IsAdding());
	return lkSQL3RecordDialog::Validate();
}

//virtual
bool DStorage::TransferDataToWindow()
{
	CStorage::stTransferDataToWindow(m_bInitial);
	return lkSQL3RecordDialog::TransferDataToWindow();
}

//virtual
wxString DStorage::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
lkSQL3RecordSet* DStorage::GetBaseSet()
{
	return static_cast<lkSQL3RecordSet*>(this);
}

void DStorage::DoThingsBeforeAdd()
{
	CStorage::SetStorageFocus();
}

//virtual
bool DStorage::DoUpdateRecordFind()
{
	return (m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false;
}
//virtual
bool DStorage::DoUpdateRecordFindNext()
{
	return (m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindNext);
}
//virtual
bool DStorage::DoUpdateRecordFindPrev()
{
	return (m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindPrev);
}


// Overrides of base-class CStorage
////////////////////////////////////////////////////////

//virtual
bool DStorage::IsMediumChanged() const
{
	return false;
}
//virtual
bool DStorage::IsLocationChanged() const
{
	return false;
}

lkSQL3Database* DStorage::GetDBase()
{
	return lkSQL3RecordDialog::GetDB();
}

lkSQL3RecordSet* DStorage::GetBaseRecordset()
{
	return GetBaseSet();
}

//virtual
lkSQL3FieldData_Text* DStorage::GetFldStorage()
{
	return TStorage::m_pFldStorage;
}
//virtual
lkSQL3FieldData_Int* DStorage::GetFldLocation()
{
	return TStorage::m_pFldLocation;
}
//virtual
lkSQL3FieldData_Int* DStorage::GetFldMedium()
{
	return TStorage::m_pFldMedium;
}
//virtual
lkSQL3FieldData_Real* DStorage::GetFldCreation()
{
	return TStorage::m_pFldCreation;
}
//virtual
lkSQL3FieldData_Int* DStorage::GetFldInfo()
{
	return TStorage::m_pFldInfo;
}

// Event Handling
////////////////////////////////////////////////////////

void DStorage::OnUpdateNextID(wxUpdateUIEvent& event)
{
	lkTransTextCtrl* t = CStorage::m_pStorageCtrl;
	MediumBox* m = CStorage::m_pMedium;

	bool bEnable = TStorage::IsAdding(); // only allow when adding new data
	if ( bEnable )
		bEnable = (m && (m->GetSelectedLParam() != 0)); // a medium should be selected
	if ( bEnable )
		bEnable = (t && t->GetValue().IsEmpty()); // must be empty to get it work

	event.Enable(bEnable);
}

void DStorage::OnNextID(wxCommandEvent& event)
{
	lkTransTextCtrl* t = CStorage::m_pStorageCtrl;
	MediumBox* m = CStorage::m_pMedium;

	bool bEnable = TStorage::IsAdding(); // only allow when adding new data
	if ( bEnable )
		bEnable = (m && (m->GetSelectedLParam() != 0)); // a medium should be selected
	if ( bEnable )
		bEnable = (t && t->GetValue().IsEmpty()); // must be empty to get it work

	if ( !bEnable )
		return; // just to be sure

	wxUint32 medium = static_cast<wxUint32>(m->GetSelectedLParam());
	wxString volume = (medium > 0) ? TStorage::GetNextLabel(GetDBase(), medium) : wxT("");
	if ( !volume.IsEmpty() )
		t->SetValue(volume);
}

void DStorage::OnFind(wxCommandEvent& WXUNUSED(event))
{
	TStorage* pRS = dynamic_cast<TStorage*>(GetBaseSet());
	wxUint64 _row = DlgStorageFind::FindStorage(this, pRS);

	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDialog::UpdateData(false);
	}
}
