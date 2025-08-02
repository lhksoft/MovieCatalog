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
#include "DMovies.h"
#include "XPMs.h"

#include "DlgFind.h"

#include <wx/sizer.h>

wxIMPLEMENT_ABSTRACT_CLASS2(DMovies, lkSQL3RecordDialog, TMovies)


DMovies::DMovies(lkSQL3Database* pDB) : lkSQL3RecordDialog(pDB), TMovies(pDB), CMovies()
{
	m_bHasMoviesCopy = false;

	m_nID_MoviesCopy = wxWindow::NewControlId(1);
	m_nID_MoviesPaste = wxWindow::NewControlId(1);

	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);
}

bool DMovies::Create(wxWindow* parent)
{
	int height = 0, width = 0;	
#ifdef __WXMSW__
	height = 508;
#else
	height = 575;
#endif
	if ( !lkSQL3RecordDialog::Create(parent, CMovies::GetBackground(), wxT("Movies Dialog"), wxDefaultPosition, wxSize(867, height), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) )
		return false;

#ifdef __WXMSW__
	width = 851;
	height = 420;
#else
	width = 867;
	height = 471;
#endif
	SetMinClientSize(wxSize(width, height));

	// this forces to open the recordset and initialize its internal fields
	(void)GetRecordset();

	if ( CMovies::CreateCanvas(this, this) )
	{
		wxSizer* sz = GetSizer();
		sz->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);
		Layout();

		return true;
	}

	return false;
}

wxWindowID DMovies::GetID_MoviesCopy() const
{
	return m_nID_MoviesCopy;
}
wxWindowID DMovies::GetID_MoviesPaste() const
{
	return m_nID_MoviesPaste;
}
wxWindowID DMovies::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID DMovies::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID DMovies::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}

// Overrides of base-class lkSQL3RecordDialog
////////////////////////////////////////////////////////

wxToolBar* DMovies::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordDialog::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_MoviesCopy(), wxT(""), Get_MoviesCopy_Xpm(), wxT("Copy current record"));
		tb->AddTool(GetID_MoviesPaste(), wxT(""), Get_MoviesPaste_Xpm(), wxT("Paste copied record"));
		tb->AddSeparator();
		tb->AddTool(GetID_Search(), wxT(""), Get_Search_Xpm(), wxT("Start a Search"));
		tb->AddTool(GetID_SearchNext(), wxT(""), Get_SearchNext_Xpm(), wxT("Search Next"));
		tb->AddTool(GetID_SearchPrev(), wxT(""), Get_SearchPrev_Xpm(), wxT("Search Back"));
	}
	return tb;
}

//virtual
void DMovies::BindButtons()
{
	lkSQL3RecordDialog::BindButtons();

	Bind(wxEVT_CHECKBOX, &CMovies::OnYearCheck, this, m_ID_YearChkb);
	Bind(wxEVT_BUTTON, &CMovies::OnSelectDate, this, m_ID_DateBtn);
	Bind(wxEVT_BUTTON, &CMovies::OnPickCountry, this, m_ID_CountryBtn);

	Bind(wxEVT_UPDATE_UI, &DMovies::OnUpdateFind, this, GetID_Search());
	Bind(wxEVT_UPDATE_UI, &DMovies::OnUpdateFindNext, this, GetID_SearchNext());
	Bind(wxEVT_UPDATE_UI, &DMovies::OnUpdateFindPrev, this, GetID_SearchPrev());

	Bind(wxEVT_UPDATE_UI, &DMovies::OnUpdateMoviesCopy, this, GetID_MoviesCopy());
	Bind(wxEVT_UPDATE_UI, &DMovies::OnUpdateMoviesPaste, this, GetID_MoviesPaste());

	Bind(wxEVT_TOOL, &DMovies::OnFind, this, GetID_Search());
	Bind(wxEVT_TOOL, &DMovies::OnFindNext, this, GetID_SearchNext());
	Bind(wxEVT_TOOL, &DMovies::OnFindPrev, this, GetID_SearchPrev());

	Bind(wxEVT_TOOL, &DMovies::OnMoviesCopy, this, GetID_MoviesCopy());
	Bind(wxEVT_TOOL, &DMovies::OnMoviesPaste, this, GetID_MoviesPaste());
}

//virtual
bool DMovies::Validate()
{
	lkSQL3RecordSet* pSet = GetBaseRecordset();
	m_bReqAdd = (pSet && pSet->IsAdding());
	return lkSQL3RecordDialog::Validate();
}

//virtual
bool DMovies::TransferDataToWindow()
{
	bool bRet = CMovies::mvTransferDataToWindow(m_bInitial);
	if ( bRet )
		bRet = lkSQL3RecordDialog::TransferDataToWindow();

	return bRet;
}

//virtual
wxString DMovies::GetDefaultOrder() const
{
	return wxString(wxT("[ROWID]"));
}

//virtual
bool DMovies::UpdateData(bool bSaveAndValidate)
{
	if ( MoviesCopyInProgress() )
		MoviesCopyDisable();

	return lkSQL3RecordDialog::UpdateData(bSaveAndValidate);
}

//virtual
lkSQL3RecordSet* DMovies::GetBaseSet()
{
	return static_cast<lkSQL3RecordSet*>(this);
}

void DMovies::DoThingsBeforeAdd()
{
	CMovies::SetMoviesFocus();
}

// Overrides of base-class CMovies
////////////////////////////////////////////////////////

bool DMovies::IsCategoryChanged() const
{
	return false;
}

bool DMovies::IsJudgesChanged() const
{
	return false;
}

bool DMovies::IsRatingChanged() const
{
	return false;
}

bool DMovies::IsGenresChanged() const
{
	return false;
}

bool DMovies::GetYearValue() const
{
	return TMovies::GetYearValue();
}

lkSQL3Database* DMovies::GetDBase()
{
	return lkSQL3RecordDialog::GetDB();
}

lkSQL3RecordSet* DMovies::GetBaseRecordset()
{
	return GetBaseSet();
}

lkSQL3FieldData_Text* DMovies::GetFldCover()
{
	return TMovies::m_pFldCover;
}

lkSQL3FieldData_Text* DMovies::GetFldTitle()
{
	return TMovies::m_pFldTitle;
}

lkSQL3FieldData_Text* DMovies::GetFldSubTtl()
{
	return TMovies::m_pFldSubTtl;
}

lkSQL3FieldData_Text* DMovies::GetFldAlias()
{
	return TMovies::m_pFldAlias;
}

lkSQL3FieldData_Int* DMovies::GetFldEpisode()
{
	return TMovies::m_pFldEp;
}

lkSQL3FieldData_Int* DMovies::GetFldCat()
{
	return TMovies::m_pFldCat;
}

lkSQL3FieldData_Int* DMovies::GetFldSubCat()
{
	return TMovies::m_pFldSub;
}

lkSQL3FieldData_Int* DMovies::GetFldGenre()
{
	return TMovies::m_pFldGenre;
}

lkSQL3FieldData_Text* DMovies::GetFldCountry()
{
	return TMovies::m_pFldCountry;
}

lkSQL3FieldData_Real* DMovies::GetFldDate()
{
	return TMovies::m_pFldDate;
}

lkSQL3FieldData_Int* DMovies::GetFldYear()
{
	return TMovies::m_pFldYear;
}

lkSQL3FieldData_Int* DMovies::GetFldSerie()
{
	return TMovies::m_pFldSerie;
}

lkSQL3FieldData_Text* DMovies::GetFldURL()
{
	return TMovies::m_pFldURL;
}

lkSQL3FieldData_Int* DMovies::GetFldJudge()
{
	return TMovies::m_pFldJudge;
}

lkSQL3FieldData_Int* DMovies::GetFldRating()
{
	return TMovies::m_pFldRating;
}

lkSQL3FieldData_Text* DMovies::GetFldInfo()
{
	return TMovies::m_pFldInfo;
}


// Overrides of base-class lkRecordDlg
////////////////////////////////////////////////////////
// Event Handling

//virtual
bool DMovies::DoUpdateRecordFind()
{
	return (m_pSet) ? (!m_pSet->IsAdding() && !m_pSet->IsEmpty()) : false;
}
//virtual
bool DMovies::DoUpdateRecordFindNext()
{
	return (m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindNext);
}
//virtual
bool DMovies::DoUpdateRecordFindPrev()
{
	return (m_pSet && !m_pSet->IsEmpty() && !m_pSet->IsAdding() && m_pSet->m_GotFindPrev);
}

void DMovies::OnUpdateMoviesCopy(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? !m_pSet->IsAdding() : false);
}
void DMovies::OnUpdateMoviesPaste(wxUpdateUIEvent& event)
{
	event.Enable((m_pSet) ? (m_pSet->IsAdding() && m_bHasMoviesCopy) : false);
}

void DMovies::OnFind(wxCommandEvent& WXUNUSED(event))
{
	TMovies* pRS = dynamic_cast<TMovies*>(GetBaseSet());
	wxUint64 _row = DlgMoviesFind::FindMovie(this, pRS);

	if ( _row > 0 )
	{
		pRS->Move(_row);
		lkSQL3RecordDialog::UpdateData(false);
	}
}

void DMovies::OnMoviesCopy(wxCommandEvent& event)
{
	wxASSERT(!m_pSet->IsAdding());
	if ( m_pSet->IsAdding() )
		return;

	MoviesCopyEnable();
	TransferDataFromWindow();
	m_bHasMoviesCopy = true;
	MoviesCopyDisable();
}
void DMovies::OnMoviesPaste(wxCommandEvent& event)
{
	wxASSERT(m_pSet->IsAdding());
	if ( !m_pSet->IsAdding() )
		return;

	m_bHasMoviesCopy = false;
	MoviesCopyEnable();
	TransferDataToWindow();
	MoviesCopyDisable();
}

