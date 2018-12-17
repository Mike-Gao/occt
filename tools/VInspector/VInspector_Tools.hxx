// Created on: 2017-06-16
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

#ifndef VInspector_Tools_H
#define VInspector_Tools_H

#include <AIS_InteractiveContext.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Select3D_BndBox3d.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <inspector/VInspector_CallBackMode.hxx>
#include <inspector/VInspector_DisplayActionType.hxx>
#include <inspector/VInspector_SelectionType.hxx>

#include <inspector/ViewControl_PaneCreator.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class ViewControl_TableModelValues;

class Graphic3d_IndexBuffer;
class Graphic3d_Buffer;
class Graphic3d_BoundBuffer;

//! \class VInspector_Tools
//! The class that gives auxiliary methods for Visualization elements manipulation
class VInspector_Tools
{

public:

  //! Returns string value of enumeration value
  //! \param theType a shape type
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString GetShapeTypeInfo (const TopAbs_ShapeEnum& theType);

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value 
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo = true);

  //! Returns number of selected owners for presentation
  //! \param theContext an interactive context
  //! \param theObject a presentation
  //! \param theShapeInfoOnly if true, only BRep owners are taken
  Standard_EXPORT static int SelectedOwners (const Handle(AIS_InteractiveContext)& theContext,
                                             const Handle(AIS_InteractiveObject)& theObject,
                                             const bool theShapeInfoOnly);

  //! Returns true if the owner is selected in the context
  //! \param theContext an interactive context
  //! \param theOwner a selectable owner
  //! \return boolean value
  Standard_EXPORT static bool IsOwnerSelected (const Handle(AIS_InteractiveContext)& theContext,
                                               const Handle(SelectBasics_EntityOwner)& theOwner);

  //! Returns all owners present in the context
  //! \param theContext an interactive context
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectBasics_EntityOwner)> ContextOwners (
                                                 const Handle(AIS_InteractiveContext)& theContext);

  //! Returns active owners in main selector of context
  //! \param theContext an interactive context
  //! \param theEmptySelectableOwners container of owners with NULL presentation or not displayed presentation
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectBasics_EntityOwner)> ActiveOwners (
                            const Handle(AIS_InteractiveContext)& theContext,
                            NCollection_List<Handle(SelectBasics_EntityOwner)>& theEmptySelectableOwners);

  //! Unhighlight selected, set selected the owners
  //! \param theContext an interactive context
  //! \param theOwners a container of owners
  Standard_EXPORT static void AddOrRemoveSelectedShapes (const Handle(AIS_InteractiveContext)& theContext,
                                         const NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners);

  //! Unhighlight selected, set selected presentations
  //! \param theContext an interactive context
  //! \param thePresentations a container of presentations
  Standard_EXPORT static void AddOrRemovePresentations (const Handle(AIS_InteractiveContext)& theContext,
                                        const NCollection_List<Handle(AIS_InteractiveObject)>& thePresentations);

  //! Returns information about presentation: Dynamic Type, Pointer info, Shape type info
  //! \param theObject a presentation
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetInfo (Handle(AIS_InteractiveObject)& theObject);

  //! Returns information about current highlight: Names, Owners, Pointers, Owners
  //! \param theContext an interactive context
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetHighlightInfo (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns information about current selection: Names, Owners, Pointers, Owners
  //! \param theContext an interactive context
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetSelectedInfo (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns the first pointer of selection in the context
  Standard_EXPORT static QString GetSelectedInfoPointers (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns string information of call back mode
  //! \param theMode type of selection
  //! \param theValue a value in selection enumeration
  //! \return information text
  Standard_EXPORT static TCollection_AsciiString ToName (const VInspector_SelectionType theType, const int theValue);

  //! Returns selection information 
  //! \param theMode a selection mode
  //! \param thePresentation a presentation
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString SelectionModeToName (int theMode, const Handle(AIS_InteractiveObject)& thePresentation);

  //! Returns text of orientation
  //! \param theOrientation an orientation value
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString OrientationToName (const TopAbs_Orientation& theOrientation);

  //! Read Shape using BREP reader
  //! \param theFileName a file name
  //! \return shape or NULL
  Standard_EXPORT static TopoDS_Shape ReadShape (const TCollection_AsciiString& theFileName);

  //! Fills container of table values
  //! \param theAlert a message alert
  //! \param theTableValue container of values
  Standard_EXPORT static void GetPropertyTableValues (const TreeModel_ItemBasePtr& theItem,
                                                      const NCollection_List<Handle(ViewControl_PaneCreator)>& theCreators,
                                                      QList<ViewControl_TableModelValues*>& theTableValues);

  //! Returns the string name for a given type.
  //! @param theType action type
  //! @return string identifier from the display action type
  Standard_EXPORT static Standard_CString DisplayActionTypeToString (VInspector_DisplayActionType theType);

  //! Returns the enumeration type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @return string identifier from the display action type
  static VInspector_DisplayActionType DisplayActionTypeFromString (Standard_CString theTypeString)
  {
    VInspector_DisplayActionType aType = VInspector_DisplayActionType_NoneId;
    DisplayActionTypeFromString (theTypeString, aType);
    return aType;
  }

  //! Determines the enumeration type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @param theType detected action type
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean DisplayActionTypeFromString (const Standard_CString theTypeString,
                                                                       VInspector_DisplayActionType& theType);

  //! Build string presentation of bounding box information in form: (xmin, ymin, zmin), (xmax, ymax, zmax)
  //! \param theBoundingBox bounding box
  //! \return string presentation
  Standard_EXPORT static QVariant ToVariant (const Select3D_BndBox3d& theBoundingBox);

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static TopoDS_Shape CreateShape (const Bnd_Box& theBoundingBox);

  //! Creates box shape
  //! \param theBoundingBox box shape parameters
  //! \return created shape
  Standard_EXPORT static TopoDS_Shape CreateShape (const Select3D_BndBox3d& theBoundingBox);

  //! Build string presentation of Graphic3D index buffer
  //! \param theIndexBuffer index buffer
  //! \return string presentation
  Standard_EXPORT static QVariant ToVariant (const Handle(Graphic3d_IndexBuffer)& theIndexBuffer);

  //! Build string presentation of Graphic3D buffer
  //! \param theBuffer index buffer
  //! \return string presentation
  Standard_EXPORT static QVariant ToVariant (const Handle(Graphic3d_Buffer)& theBuffer);

  //! Build string presentation of Graphic3D bound buffer
  //! \param theBoundBuffer index buffer
  //! \return string presentation
  Standard_EXPORT static QVariant ToVariant (const Handle(Graphic3d_BoundBuffer)& theBoundBuffer);

  Standard_EXPORT static QString ToString (const Graphic3d_Attribute& theAttribute);

};

#endif
