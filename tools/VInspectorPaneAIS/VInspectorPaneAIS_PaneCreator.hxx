// Created on: 2018-12-08
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#ifndef VInspectorPaneAIS_PaneCreator_H
#define VInspectorPaneAIS_PaneCreator_H

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <inspector/ViewControl_PaneCreator.hxx>
#include <inspector/ViewControl_Pane.hxx>

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>

DEFINE_STANDARD_HANDLE (VInspectorPaneAIS_PaneCreator, ViewControl_PaneCreator)

//! \class VInspectorPaneAIS_PaneCreator
//! \brief An interface to create custom panes by transient object name.
class VInspectorPaneAIS_PaneCreator : public ViewControl_PaneCreator
{
public:

  //! Constructor
  VInspectorPaneAIS_PaneCreator() {}

  //! Destructor
  virtual ~VInspectorPaneAIS_PaneCreator() {}

  //! Returns pane for the name, creates a new pane if it does not exist and possible to create
  //! \param theName type of the pane
  //! \return a pane instance or NULL
  virtual ViewControl_Pane* GetPane (const Standard_CString& theName);

  DEFINE_STANDARD_RTTIEXT(VInspectorPaneAIS_PaneCreator, ViewControl_PaneCreator)

protected:
  //! Creates new pane for the name
  //! \return pane instance or NULL
  ViewControl_Pane* createPane (const Standard_CString& theName);

private:
  NCollection_DataMap<TCollection_AsciiString, ViewControl_Pane*> myPanes; //!< created panes
};

#endif 
