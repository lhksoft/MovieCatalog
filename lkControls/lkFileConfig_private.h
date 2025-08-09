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
/* *************************************************************
 * Private classes for lkFileConfig
 * *********************************************************** */
#ifndef __LK_FILECONFIG_PRIVATE_H__
#define __LK_FILECONFIG_PRIVATE_H__
#pragma once

#include <wx/string.h>
#include <wx/dynarray.h>

class lkFileConfig;

// ----------------------------------------------------------------------------
// wxFileConfigLineList
// ----------------------------------------------------------------------------

// we store all lines of the local config file as a linked list in memory
class lkFileConfigLineList
{
public:
	// ctor
	lkFileConfigLineList(const wxString& str, lkFileConfigLineList* pNext = NULL);

	void								SetNext(lkFileConfigLineList* pNext);
	void								SetPrev(lkFileConfigLineList* pPrev);

	// next/prev nodes in the linked list
	lkFileConfigLineList*				Next(void) const;
	lkFileConfigLineList*				Prev(void) const;

	// get/change lines text
	void								SetText(const wxString& str);
	const wxString&						Text(void) const;

private:
	wxString							m_strLine;			// line contents
	lkFileConfigLineList*				m_pNext,			// next node
										* m_pPrev;			// previous one

	wxDECLARE_NO_COPY_CLASS(lkFileConfigLineList);
};

// ----------------------------------------------------------------------------
// wxFileConfigEntry: a name/value pair
// ----------------------------------------------------------------------------

class lkFileConfigGroup;
class lkFileConfigEntry
{
public:
	lkFileConfigEntry(lkFileConfigGroup* pParent, const wxString& strName, int nLine);

	// simple accessors
	wxString							Name(void) const;
	wxString							Value(void) const;
	lkFileConfigGroup*					Group(void) const;
	bool								IsImmutable(void) const;
	bool								IsLocal(void) const;
	int									Line(void) const;
	lkFileConfigLineList*				GetLine(void) const;

	// modify entry attributes
	void								SetValue(const wxString& strValue, bool bUser = true);
	void								SetLine(lkFileConfigLineList* pLine);

private:
	lkFileConfigGroup*					m_pParent;			// group that contains us

	wxString							m_strName,			// entry name
										m_strValue;			//       value
	bool								m_bImmutable : 1,	// can be overridden locally?
										m_bHasValue : 1;	// set after first call to SetValue()

	int									m_nLine;			// used if m_pLine == NULL only

	// pointer to our line in the linked list or NULL if it was found in global file (which we don't modify)
	lkFileConfigLineList*				m_pLine;

	wxDECLARE_NO_COPY_CLASS(lkFileConfigEntry);
};

// ----------------------------------------------------------------------------
// wxFileConfigGroup: container of entries and other groups
// ----------------------------------------------------------------------------

WX_DEFINE_SORTED_ARRAY(lkFileConfigEntry*, ArrayEntries);
WX_DEFINE_SORTED_ARRAY(lkFileConfigGroup*, ArrayGroups);

class lkFileConfigGroup
{
public:
	// ctor
	lkFileConfigGroup(lkFileConfigGroup* pParent, const wxString& strName, lkFileConfig*);

	// dtor deletes all entries and subgroups also
	virtual ~lkFileConfigGroup();

	// simple accessors
	const wxString&						Name(void) const;
	lkFileConfigGroup*					Parent(void) const;
	lkFileConfig*						Config(void) const;

	const ArrayEntries&					Entries(void) const;
	const ArrayGroups&					Groups(void) const;
	bool								IsEmpty(void) const;

	void								SetLine(lkFileConfigLineList* pLine);

	// get the last line belonging to an entry/subgroup of this group
	lkFileConfigLineList*				GetGroupLine(void); // line which contains [group] -- may be NULL for "/" only
	lkFileConfigLineList*				GetLastGroupLine(void); // after which the next group starts
	lkFileConfigLineList*				GetLastEntryLine(void); // after which our subgroups start

	// called by entries/subgroups when they're created/deleted
	void								SetLastEntry(lkFileConfigEntry* pEntry);
	void								SetLastGroup(lkFileConfigGroup* pGroup);

	// used by Rename()
	void								UpdateGroupAndSubgroupsLines(void);

	// rename: no checks are done to ensure that the name is unique!
	void								Rename(const wxString& newName);

	wxString							GetFullName(void) const;

	// find entry/subgroup (NULL if not found)
	lkFileConfigEntry*					FindEntry(const wxString& name) const;
	lkFileConfigGroup*					FindSubgroup(const wxString& name) const;

	// create new entry/subgroup returning pointer to newly created element
	lkFileConfigEntry*					AddEntry(const wxString& strName, int nLine = wxNOT_FOUND);
	lkFileConfigGroup*					AddSubgroup(const wxString& strName);

	// delete entry/subgroup, return false if doesn't exist
	bool								DeleteSubgroupByName(const wxString& name);
	bool								DeleteEntry(const wxString& name);

	// DeleteSubgroupByName helper
	bool								DeleteSubgroup(lkFileConfigGroup* pGroup);


private:
	lkFileConfig*						m_pConfig;			// config object we belong to
	lkFileConfigGroup*					m_pParent;			// parent group (NULL for root group)
	ArrayEntries						m_aEntries;			// entries in this group
	ArrayGroups							m_aSubgroups;		// subgroups
	wxString							m_strName;			// group's name
	lkFileConfigLineList*				m_pLine;			// pointer to our line in the linked list
	lkFileConfigEntry*					m_pLastEntry;		// last entry/subgroup of this group in the
	lkFileConfigGroup*					m_pLastGroup;		// local file (we insert new ones after it)

	wxDECLARE_NO_COPY_CLASS(lkFileConfigGroup);
};


// ----------------------------------------------------------------------------
// global functions declarations
// ----------------------------------------------------------------------------

// compare functions for sorting the arrays
int CompareEntries(lkFileConfigEntry* p1, lkFileConfigEntry* p2);
int CompareGroups(lkFileConfigGroup* p1, lkFileConfigGroup* p2);

// filter strings
wxString FilterInValue(const wxString& str);
wxString FilterOutValue(const wxString& str);

wxString FilterInEntryName(const wxString& str);
wxString FilterOutEntryName(const wxString& str);

#endif // !__LK_FILECONFIG_PRIVATE_H__
