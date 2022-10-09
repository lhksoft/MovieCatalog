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
#include "VMovies_private.h"
#include "VMovies.h"

#include "XPMs.h"

// /////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesFrame
////
wxIMPLEMENT_CLASS(MoviesFrame, lkSQL3RecordChildFrame)

MoviesFrame::MoviesFrame() : lkSQL3RecordChildFrame()
{
	m_nID_MoviesCopy = wxWindow::NewControlId(1);
	m_nID_MoviesPaste = wxWindow::NewControlId(1);

	m_nID_Filter = wxWindow::NewControlId(1);
	m_nID_FilterToggle = wxWindow::NewControlId(1);

	m_nID_Search = wxWindow::NewControlId(1);
	m_nID_SearchNext = wxWindow::NewControlId(1);
	m_nID_SearchPrev = wxWindow::NewControlId(1);
}

wxToolBar* MoviesFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
	wxToolBar* tb = lkSQL3RecordChildFrame::OnCreateToolBar(style, id, name);
	if ( tb )
	{
		tb->AddSeparator();
		tb->AddTool(GetID_MoviesCopy(), wxT(""), Get_MoviesCopy_Xpm(), wxT("Copy current record"));
		tb->AddTool(GetID_MoviesPaste(), wxT(""), Get_MoviesPaste_Xpm(), wxT("Paste copied record"));
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

wxWindowID MoviesFrame::GetID_MoviesCopy() const
{
	return m_nID_MoviesCopy;
}
wxWindowID MoviesFrame::GetID_MoviesPaste() const
{
	return m_nID_MoviesPaste;
}
wxWindowID MoviesFrame::GetID_Filter() const
{
	return m_nID_Filter;
}
wxWindowID MoviesFrame::GetID_FilterToggle() const
{
	return m_nID_FilterToggle;
}
wxWindowID MoviesFrame::GetID_Search() const
{
	return m_nID_Search;
}
wxWindowID MoviesFrame::GetID_SearchNext() const
{
	return m_nID_SearchNext;
}
wxWindowID MoviesFrame::GetID_SearchPrev() const
{
	return m_nID_SearchPrev;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesCanvas
////
wxIMPLEMENT_CLASS(MoviesCanvas, lkSQL3panel)

MoviesCanvas::MoviesCanvas(wxView* view, wxWindow* parent) : lkSQL3panel(view, parent), MoviesPanel()
{
	Create();
}

bool MoviesCanvas::Create()
{
	if ( !lkSQL3panel::Create(GetParent(), MoviesPanel::GetBackground()) )
		return false;

	SetParentView(m_pView);
	bool bRet = MoviesPanel::CreateCanvas(GetView()->GetFrame(), this, GetDocument());

	Bind(wxEVT_CHECKBOX, &MoviesPanel::OnYearCheck, this, m_ID_YearChkb);
	Bind(wxEVT_BUTTON, &MoviesPanel::OnSelectDate, this, m_ID_DateBtn);
	Bind(wxEVT_BUTTON, &MoviesPanel::OnPickCountry, this, m_ID_CountryBtn);

	return bRet;
}

//virtual
bool MoviesCanvas::Validate()
{
	lkSQL3RecordSet* pSet = GetBaseRecordset();
	m_bReqAdd = (pSet && pSet->IsAdding());
	return lkSQL3panel::Validate();
}

//virtual
bool MoviesCanvas::TransferDataToWindow()
{
	bool bInitial = false;
	{
		lkSQL3RecordView* vw = GetView();
		bInitial = (vw && vw->IsInitial());
	}

	bool bRet = MoviesPanel::mvTransferDataToWindow(bInitial);
	if ( bRet )
		bRet = lkSQL3panel::TransferDataToWindow();

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// class MoviesPanel
////
MoviesPanel::MoviesPanel() : CMovies()
{
	m_pParentDoc = NULL;
	m_pParentView = NULL;
}
MoviesPanel::~MoviesPanel()
{
}

bool MoviesPanel::IsCategoryChanged() const
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	if ( movDoc )
	{
		bool b = movDoc->IsCategoryChanged();
		movDoc->SetCategoryChanged(false);
		return b;
	}
	//else..
	return false;
}

bool MoviesPanel::IsJudgesChanged() const
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	if ( movDoc )
	{
		bool b = movDoc->IsJudgesChanged();
		movDoc->SetJudgesChanged(false);
		return b;
	}
	//else..
	return false;
}

bool MoviesPanel::IsRatingChanged() const
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	if ( movDoc )
	{
		bool b = movDoc->IsRatingChanged();
		movDoc->SetRatingChanged(false);
		return b;
	}
	//else..
	return false;
}

bool MoviesPanel::IsGenresChanged() const
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	if ( movDoc )
	{
		bool b = movDoc->IsGenresChanged();
		movDoc->SetGenresChanged(false);
		return b;
	}
	//else..
	return false;
}

bool MoviesPanel::GetYearValue() const
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->GetYearValue() : false;
}

lkSQL3Database* MoviesPanel::GetDBase()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->GetDB() : NULL;
}

lkSQL3RecordSet* MoviesPanel::GetBaseRecordset()
{
	lkSQL3RecordSet* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(lkSQL3RecordSet)) )
		movDoc = dynamic_cast<lkSQL3RecordSet*>(m_pParentDoc);

	return movDoc;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldCover()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldCover : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldTitle()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldTitle : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldSubTtl()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldSubTtl : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldAlias()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldAlias : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldEpisode()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldEp : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldCat()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldCat : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldSubCat()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldSub : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldGenre()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldGenre : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldCountry()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldCountry : NULL;
}

lkSQL3FieldData_Real* MoviesPanel::GetFldDate()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldDate : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldYear()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldYear : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldSerie()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldSerie : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldURL()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldURL : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldJudge()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldJudge : NULL;
}

lkSQL3FieldData_Int* MoviesPanel::GetFldRating()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldRating : NULL;
}

lkSQL3FieldData_Text* MoviesPanel::GetFldInfo()
{
	MoviesDocument* movDoc = NULL;

	if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
		movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

	return (movDoc) ? movDoc->m_pFldInfo : NULL;
}

void MoviesPanel::SetParentView(wxView* v)
{
	m_pParentView = v;
}

bool MoviesPanel::CreateCanvas(wxWindow* parentFrame, wxWindow* parentPanel, lkSQL3RecordDocument* doc) // doc needs to be MoviesDocument
{
	if ( !(doc && doc->IsKindOf(wxCLASSINFO(MoviesDocument))) )
		return false;

	m_pParentDoc = doc;

	// we need this for our Validators
	{
		MoviesDocument* movDoc = NULL;
		if ( m_pParentDoc && m_pParentDoc->IsKindOf(wxCLASSINFO(MoviesDocument)) )
			movDoc = dynamic_cast<MoviesDocument*>(m_pParentDoc);

		// this forces the document to open the recordset
		if ( movDoc )
			(void)movDoc->GetRecordset();

		wxASSERT_MSG(movDoc != NULL, wxT("wxDocument needs to be of type MoviesDocument"));
		if ( movDoc == NULL )
			return false; // seriously ?
	}

	if ( CMovies::CreateCanvas(parentFrame, parentPanel) )
	{ // >> Create parent
		if ( m_pParentView && m_pImage )
			m_pImage->SetView(static_cast<lkView*>(m_pParentView));

		return true;
	}
	//else..
	return false;
}

