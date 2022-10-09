/* lkControls - custom controls using wxWidgets
 * Copyright (C) 2022 Laurens Koehoorn (lhksoft)
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "lkFileConfig_private.h"
#include "lkFileConfig.h"

#include <wx/confbase.h>
#include <wx/debug.h>
//#include <wx/log.h>

// ----------------------------------------------------------------------------
// wxFileConfigLineList
// ----------------------------------------------------------------------------

lkFileConfigLineList::lkFileConfigLineList(const wxString& str, lkFileConfigLineList* pNext) : m_strLine(str)
{
	SetNext(pNext);
	SetPrev(NULL);
}

void lkFileConfigLineList::SetNext(lkFileConfigLineList* pNext)
{
	m_pNext = pNext;
}
void lkFileConfigLineList::SetPrev(lkFileConfigLineList* pPrev)
{
	m_pPrev = pPrev;
}

lkFileConfigLineList* lkFileConfigLineList::Next() const
{
	return m_pNext;
}
lkFileConfigLineList* lkFileConfigLineList::Prev() const
{
	return m_pPrev;
}

void lkFileConfigLineList::SetText(const wxString& str)
{
	m_strLine = str;
}
const wxString& lkFileConfigLineList::Text() const
{
	return m_strLine;
}


// ----------------------------------------------------------------------------
// wxFileConfigEntry: a name/value pair
// ----------------------------------------------------------------------------

lkFileConfigEntry::lkFileConfigEntry(lkFileConfigGroup* pParent, const wxString& strName, int nLine) : m_strName(strName)
{
	wxASSERT(!strName.empty());

	m_pParent = pParent;
	m_nLine = nLine;
	m_pLine = NULL;

	m_bHasValue = false;

	m_bImmutable = strName[0] == wxCONFIG_IMMUTABLE_PREFIX;
	if ( m_bImmutable )
		m_strName.erase(0, 1);  // remove first character
}

wxString lkFileConfigEntry::Name() const
{
	return m_strName;
}
wxString lkFileConfigEntry::Value() const
{
	return m_strValue;
}
lkFileConfigGroup* lkFileConfigEntry::Group() const
{
	return m_pParent;
}
bool lkFileConfigEntry::IsImmutable() const
{
	return m_bImmutable;
}
bool lkFileConfigEntry::IsLocal() const
{
	return m_pLine != 0;
}
int lkFileConfigEntry::Line() const
{
	return m_nLine;
}
lkFileConfigLineList* lkFileConfigEntry::GetLine() const
{
	return m_pLine;
}

// second parameter is false if we read the value from file and prevents the
// entry from being marked as 'dirty'
void lkFileConfigEntry::SetValue(const wxString& strValue, bool bUser)
{
	if ( bUser && IsImmutable() )
		return;

	// do nothing if it's the same value: but don't test for it if m_bHasValue
	// hadn't been set yet or we'd never write empty values to the file
	if ( m_bHasValue && strValue == m_strValue )
		return;

	m_bHasValue = true;
	m_strValue = strValue;

	if ( bUser )
	{
		wxString strValFiltered;

		if ( Group()->Config()->GetStyle() & wxCONFIG_USE_NO_ESCAPE_CHARACTERS )
		{
			strValFiltered = strValue;
		}
		else
		{
			strValFiltered = FilterOutValue(strValue);
		}

		wxString    strLine;
		strLine << FilterOutEntryName(m_strName) << wxT('=') << strValFiltered;

		if ( m_pLine )
		{
			// entry was read from the local config file, just modify the line
			m_pLine->SetText(strLine);
		}
		else // this entry didn't exist in the local file
		{
			// add a new line to the file: note that line returned by
			// GetLastEntryLine() may be NULL if we're in the root group and it
			// doesn't have any entries yet, but this is ok as passing NULL
			// line to LineListInsert() means to prepend new line to the list
			lkFileConfigLineList* line = Group()->GetLastEntryLine();
			m_pLine = Group()->Config()->LineListInsert(strLine, line);

			Group()->SetLastEntry(this);
		}
	}
}

void lkFileConfigEntry::SetLine(lkFileConfigLineList* pLine)
{
	m_pLine = pLine;
	Group()->SetLastEntry(this);
}


// ============================================================================
// wxFileConfig::wxFileConfigGroup
// ============================================================================

lkFileConfigGroup::lkFileConfigGroup(lkFileConfigGroup* pParent, const wxString& strName, lkFileConfig* pConfig) :
	m_aEntries(CompareEntries), m_aSubgroups(CompareGroups), m_strName(strName)
{
	m_pConfig = pConfig;
	m_pParent = pParent;
	m_pLine = NULL;

	m_pLastEntry = NULL;
	m_pLastGroup = NULL;
}

// dtor deletes all children
lkFileConfigGroup::~lkFileConfigGroup()
{
	// entries
	size_t n, nCount = m_aEntries.GetCount();
	for ( n = 0; n < nCount; n++ )
		delete m_aEntries[n];

	// subgroups
	nCount = m_aSubgroups.GetCount();
	for ( n = 0; n < nCount; n++ )
		delete m_aSubgroups[n];
}

const wxString& lkFileConfigGroup::Name() const
{
	return m_strName;
}
lkFileConfigGroup* lkFileConfigGroup::Parent() const
{
	return m_pParent;
}
lkFileConfig* lkFileConfigGroup::Config() const
{
	return m_pConfig;
}

const ArrayEntries& lkFileConfigGroup::Entries() const
{
	return m_aEntries;
}
const ArrayGroups& lkFileConfigGroup::Groups() const
{
	return m_aSubgroups;
}
bool lkFileConfigGroup::IsEmpty() const
{
	return (Entries().IsEmpty() && Groups().IsEmpty());
}

void lkFileConfigGroup::SetLine(lkFileConfigLineList* pLine)
{
	// for a normal (i.e. not root) group this method shouldn't be called twice
	// unless we are resetting the line
	wxASSERT_MSG(!m_pParent || !m_pLine || !pLine, wxT("changing line for a non-root group?"));

	m_pLine = pLine;
}

// Return the line which contains "[our name]". If we're still not in the list,
// add our line to it immediately after the last line of our parent group if we
// have it or in the very beginning if we're the root group.
lkFileConfigLineList* lkFileConfigGroup::GetGroupLine()
{
	if ( !m_pLine )
	{
		lkFileConfigGroup* pParent = Parent();

		// this group wasn't present in local config file, add it now
		if ( pParent )
		{
			wxString strFullName;

			// add 1 to the name because we don't want to start with '/'
			strFullName << wxT("[")
				<< FilterOutEntryName(GetFullName().c_str() + 1)
				<< wxT("]");
			m_pLine = m_pConfig->LineListInsert(strFullName, pParent->GetLastGroupLine());
			pParent->SetLastGroup(this);  // we're surely after all the others
		}
		//else: this is the root group and so we return NULL because we don't
		//      have any group line
	}

	return m_pLine;
}

// Return the last line belonging to the subgroups of this group (after which
// we can add a new subgroup), if we don't have any subgroups or entries our
// last line is the group line (m_pLine) itself.
lkFileConfigLineList* lkFileConfigGroup::GetLastGroupLine()
{
	// if we have any subgroups, our last line is the last line of the last
	// subgroup
	if ( m_pLastGroup )
	{
		lkFileConfigLineList* pLine = m_pLastGroup->GetLastGroupLine();

		wxASSERT_MSG(pLine, wxT("last group must have !NULL associated line"));

		return pLine;
	}

	// no subgroups, so the last line is the line of thelast entry (if any)
	return GetLastEntryLine();
}

// return the last line belonging to the entries of this group (after which
// we can add a new entry), if we don't have any entries we will add the new
// one immediately after the group line itself.
lkFileConfigLineList* lkFileConfigGroup::GetLastEntryLine()
{
	if ( m_pLastEntry )
	{
		lkFileConfigLineList* pLine = m_pLastEntry->GetLine();

		wxASSERT_MSG(pLine, wxT("last entry must have !NULL associated line"));

		return pLine;
	}

	// no entries: insert after the group header, if any
	return GetGroupLine();
}

void lkFileConfigGroup::SetLastEntry(lkFileConfigEntry* pEntry)
{
	m_pLastEntry = pEntry;

	if ( !m_pLine )
	{
		// the only situation in which a group without its own line can have
		// an entry is when the first entry is added to the initially empty
		// root pseudo-group
		wxASSERT_MSG(!m_pParent, wxT("unexpected for non root group"));

		// let the group know that it does have a line in the file now
		m_pLine = pEntry->GetLine();
	}
}
void lkFileConfigGroup::SetLastGroup(lkFileConfigGroup* pGroup)
{
	m_pLastGroup = pGroup;
}

void lkFileConfigGroup::UpdateGroupAndSubgroupsLines()
{
	// update the line of this group
	lkFileConfigLineList* line = GetGroupLine();
	wxCHECK_RET(line, wxT("a non root group must have a corresponding line!"));

	// +1: skip the leading '/'
	line->SetText(wxString::Format(wxT("[%s]"), GetFullName().c_str() + 1));


	// also update all subgroups as they have this groups name in their lines
	const size_t nCount = m_aSubgroups.GetCount();
	for ( size_t n = 0; n < nCount; n++ )
	{
		m_aSubgroups[n]->UpdateGroupAndSubgroupsLines();
	}
}

void lkFileConfigGroup::Rename(const wxString& newName)
{
	wxCHECK_RET(m_pParent, wxT("the root group can't be renamed"));

	if ( newName == m_strName )
		return;

	// we need to remove the group from the parent and it back under the new
	// name to keep the parents array of subgroups alphabetically sorted
	m_pParent->m_aSubgroups.Remove(this);

	m_strName = newName;

	m_pParent->m_aSubgroups.Add(this);

	// update the group lines recursively
	UpdateGroupAndSubgroupsLines();
}

wxString lkFileConfigGroup::GetFullName() const
{
	wxString fullname;
	if ( Parent() )
		fullname = Parent()->GetFullName() + wxCONFIG_PATH_SEPARATOR + Name();

	return fullname;
}

// use binary search because the array is sorted
lkFileConfigEntry* lkFileConfigGroup::FindEntry(const wxString& name) const
{
	size_t lo = 0,
		hi = m_aEntries.GetCount();
	lkFileConfigEntry* pEntry;

	while ( lo < hi )
	{
		size_t i;
		i = (lo + hi) / 2;
		pEntry = m_aEntries[i];

		int res;
#if wxCONFIG_CASE_SENSITIVE
		res = pEntry->Name().compare(name);
#else
		res = pEntry->Name().CmpNoCase(name);
#endif

		if ( res > 0 )
			hi = i;
		else if ( res < 0 )
			lo = i + 1;
		else
			return pEntry;
	}

	return NULL;
}

lkFileConfigGroup* lkFileConfigGroup::FindSubgroup(const wxString& name) const
{
	size_t lo = 0,
		hi = m_aSubgroups.GetCount();
	lkFileConfigGroup* pGroup;

	while ( lo < hi )
	{
		size_t i;
		i = (lo + hi) / 2;
		pGroup = m_aSubgroups[i];

		int res;
#if wxCONFIG_CASE_SENSITIVE
		res = pGroup->Name().compare(name);
#else
		res = pGroup->Name().CmpNoCase(name);
#endif

		if ( res > 0 )
			hi = i;
		else if ( res < 0 )
			lo = i + 1;
		else
			return pGroup;
	}

	return NULL;
}

// create a new entry and add it to the current group
lkFileConfigEntry* lkFileConfigGroup::AddEntry(const wxString& strName, int nLine)
{
	wxASSERT(FindEntry(strName) == 0);

	lkFileConfigEntry* pEntry = new lkFileConfigEntry(this, strName, nLine);

	m_aEntries.Add(pEntry);
	return pEntry;
}

// create a new group and add it to the current group
lkFileConfigGroup* lkFileConfigGroup::AddSubgroup(const wxString& strName)
{
	wxASSERT(FindSubgroup(strName) == 0);

	lkFileConfigGroup* pGroup = new lkFileConfigGroup(this, strName, m_pConfig);

	m_aSubgroups.Add(pGroup);
	return pGroup;
}

bool lkFileConfigGroup::DeleteSubgroupByName(const wxString& name)
{
	lkFileConfigGroup* const pGroup = FindSubgroup(name);
	return pGroup ? DeleteSubgroup(pGroup) : false;
}

// Delete the subgroup and remove all references to it from
// other data structures.
bool lkFileConfigGroup::DeleteSubgroup(lkFileConfigGroup* pGroup)
{
	wxCHECK_MSG(pGroup, false, wxT("deleting non existing group?"));

	// delete all entries...
	size_t nCount = pGroup->m_aEntries.GetCount();

	for ( size_t nEntry = 0; nEntry < nCount; nEntry++ )
	{
		lkFileConfigLineList* pLine = pGroup->m_aEntries[nEntry]->GetLine();

		if ( pLine )
			m_pConfig->LineListRemove(pLine);
	}

	// ...and subgroups of this subgroup
	nCount = pGroup->m_aSubgroups.GetCount();

	for ( size_t nGroup = 0; nGroup < nCount; nGroup++ )
		pGroup->DeleteSubgroup(pGroup->m_aSubgroups[0]);

	// and then finally the group itself
	lkFileConfigLineList* pLine = pGroup->m_pLine;
	if ( pLine )
	{
		// notice that we may do this test inside the previous "if"
		// because the last entry's line is surely !NULL
		if ( pGroup == m_pLastGroup )
		{
			// our last entry is being deleted, so find the last one which
			// stays by going back until we find a subgroup or reach the
			// group line
			const size_t nSubgroups = m_aSubgroups.GetCount();

			m_pLastGroup = NULL;
			for ( lkFileConfigLineList* pl = pLine->Prev(); pl && !m_pLastGroup; pl = pl->Prev() )
			{
				// does this line belong to our subgroup?
				for ( size_t n = 0; n < nSubgroups; n++ )
				{
					// do _not_ call GetGroupLine! we don't want to add it to
					// the local file if it's not already there
					if ( m_aSubgroups[n]->m_pLine == pl )
					{
						m_pLastGroup = m_aSubgroups[n];
						break;
					}
				}

				if ( pl == m_pLine )
					break;
			}
		}

		m_pConfig->LineListRemove(pLine);
	}

	m_aSubgroups.Remove(pGroup);
	delete pGroup;

	return true;
}

bool lkFileConfigGroup::DeleteEntry(const wxString& name)
{
	lkFileConfigEntry* pEntry = FindEntry(name);
	if ( !pEntry )
	{
		// entry doesn't exist, nothing to do
		return false;
	}

	lkFileConfigLineList* pLine = pEntry->GetLine();
	if ( pLine != NULL )
	{
		// notice that we may do this test inside the previous "if" because the
		// last entry's line is surely !NULL
		if ( pEntry == m_pLastEntry )
		{
			// our last entry is being deleted - find the last one which stays
			wxASSERT(m_pLine != NULL);  // if we have an entry with !NULL pLine...

			// find the previous entry (if any)
			lkFileConfigEntry* pNewLast = NULL;
			const lkFileConfigLineList* const pNewLastLine = m_pLastEntry->GetLine()->Prev();
			const size_t nEntries = m_aEntries.GetCount();
			for ( size_t n = 0; n < nEntries; n++ )
			{
				if ( m_aEntries[n]->GetLine() == pNewLastLine )
				{
					pNewLast = m_aEntries[n];
					break;
				}
			}

			// pNewLast can be NULL here -- it's ok and can happen if we have no
			// entries left
			m_pLastEntry = pNewLast;

			// For the root group only, we could be removing the first group line
			// here, so update m_pLine to avoid keeping a dangling pointer.
			if ( pLine == m_pLine )
				SetLine(NULL);
		}

		m_pConfig->LineListRemove(pLine);
	}

	m_aEntries.Remove(pEntry);
	delete pEntry;

	return true;
}


// ============================================================================
// global functions
// ============================================================================

// ----------------------------------------------------------------------------
// compare functions for array sorting
// ----------------------------------------------------------------------------

int CompareEntries(lkFileConfigEntry* p1, lkFileConfigEntry* p2)
{
#if wxCONFIG_CASE_SENSITIVE
	return p1->Name().compare(p2->Name());
#else
	return p1->Name().CmpNoCase(p2->Name());
#endif
}

int CompareGroups(lkFileConfigGroup* p1, lkFileConfigGroup* p2)
{
#if wxCONFIG_CASE_SENSITIVE
	return p1->Name().compare(p2->Name());
#else
	return p1->Name().CmpNoCase(p2->Name());
#endif
}

// ----------------------------------------------------------------------------
// filter functions
// ----------------------------------------------------------------------------

// undo FilterOutValue
wxString FilterInValue(const wxString& str)
{
	wxString strResult;
	if ( str.empty() )
		return strResult;

	strResult.reserve(str.length());

	wxString::const_iterator i = str.begin();
	const bool bQuoted = *i == '"';
	if ( bQuoted )
		++i;

	for ( const wxString::const_iterator end = str.end(); i != end; ++i )
	{
		if ( *i == wxT('\\') )
		{
			if ( ++i == end )
				break;

			switch ( (*i).GetValue() )
			{
				case wxT('n'):
					strResult += wxT('\n');
					break;

				case wxT('r'):
					strResult += wxT('\r');
					break;

				case wxT('t'):
					strResult += wxT('\t');
					break;

				case wxT('\\'):
					strResult += wxT('\\');
					break;

				case wxT('"'):
					strResult += wxT('"');
					break;
			}
		}
		else // not a backslash
		{
			if ( *i != wxT('"') || !bQuoted )
			{
				strResult += *i;
			}
			else if ( i != end - 1 )
			{
//				wxLogWarning(_("unexpected \" at position %d in '%s'."), i - str.begin(), str.c_str());
			}
			//else: it's the last quote of a quoted string, ok
		}
	}

	return strResult;
}

// quote the string before writing it to file
wxString FilterOutValue(const wxString& str)
{
	if ( !str )
		return str;

	wxString strResult;
	strResult.Alloc(str.Len());

	// quoting is necessary to preserve spaces in the beginning of the string
	bool bQuote = wxIsspace(str[0]) || str[0] == wxT('"');

	if ( bQuote )
		strResult += wxT('"');

	wxChar c;
	for ( size_t n = 0; n < str.Len(); n++ )
	{
		switch ( str[n].GetValue() )
		{
			case wxT('\n'):
				c = wxT('n');
				break;

			case wxT('\r'):
				c = wxT('r');
				break;

			case wxT('\t'):
				c = wxT('t');
				break;

			case wxT('\\'):
				c = wxT('\\');
				break;

			case wxT('"'):
				if ( bQuote )
				{
					c = wxT('"');
					break;
				}
				wxFALLTHROUGH;

			default:
				strResult += str[n];
				continue;   // nothing special to do
		}

		// we get here only for special characters
		strResult << wxT('\\') << c;
	}

	if ( bQuote )
		strResult += wxT('"');

	return strResult;
}

// undo FilterOutEntryName
wxString FilterInEntryName(const wxString& str)
{
	wxString strResult;
	strResult.Alloc(str.Len());

	for ( const wxChar* pc = str.c_str(); *pc != '\0'; pc++ )
	{
		if ( *pc == wxT('\\') )
		{
			// we need to test it here or we'd skip past the NUL in the loop line
			if ( *++pc == wxT('\0') )
				break;
		}

		strResult += *pc;
	}

	return strResult;
}

// sanitize entry or group name: insert '\\' before any special characters
wxString FilterOutEntryName(const wxString& str)
{
	wxString strResult;
	strResult.Alloc(str.Len());

	for ( const wxChar* pc = str.c_str(); *pc != wxT('\0'); pc++ )
	{
		const wxChar c = *pc;

		// we explicitly allow some of "safe" chars and 8bit ASCII characters
		// which will probably never have special meaning and with which we can't
		// use isalnum() anyhow (in ASCII built, in Unicode it's just fine)
		//
		// NB: note that wxCONFIG_IMMUTABLE_PREFIX and wxCONFIG_PATH_SEPARATOR
		//     should *not* be quoted
		if (
#if !wxUSE_UNICODE
		((unsigned char)c < 127) &&
#endif // ANSI
			!wxIsalnum(c) && !wxStrchr(wxT("@_/-!.*%()"), c) )
		{
			strResult += wxT('\\');
		}

		strResult += c;
	}

	return strResult;
}
