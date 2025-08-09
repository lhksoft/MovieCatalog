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
#ifndef __LK_CTRLDATA_H__
#define __LK_CTRLDATA_H__

#pragma once

#include <wx/clntdata.h>

/* *****************************************************************************
 * Class lkCtrlData
 * ----------------
 * Combined mix-in classes of wxStringClientData and wxClientDataContainer
 * ----------------
 * This class is internally used in class <lkItemContainer>
 * ***************************************************************************** */

class lkCtrlData : public wxStringClientData, public wxClientDataContainer
{
public:
	lkCtrlData();
	lkCtrlData(const wxString&);
    lkCtrlData(const lkCtrlData&);
	virtual ~lkCtrlData();

    // The caller is responsable for deleting the returned object, internal it will be set to NULL
    wxClientData*           DetachClientObject(void);

    bool                    IsChecked(void) const;
    void                    SetCheck(bool);

protected:
    // client data accessors
    // ---------------------

    // if calling this function will change internal data-type if necessary - if previous data was wxClientData_Void it will be overrided
    virtual void            DoSetClientObject(wxClientData* data) wxOVERRIDE;

    // if calling this function will change internal data-type if necessary - if previous data was wxClientData_Object it will be removed first
    virtual void            DoSetClientData(void* data) wxOVERRIDE;

    virtual wxClientData*   DoGetClientObject() const wxOVERRIDE;
    virtual void*           DoGetClientData() const wxOVERRIDE;

private:
    bool                    m_bChecked; // state of a checkbox (true=checked, false=unchecked)
};

#endif // !__LK_CTRLDATA_H__
