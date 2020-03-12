// Created on: 2020-03-17
// Created by: Irina KRYLOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_KinematicPair_HeaderFile
#define _XCAFDoc_KinematicPair_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class XCAFKinematics_PairObject;

// resolve name collisions with WinAPI headers
#ifdef GetObject
  #undef GetObject
#endif

class XCAFDoc_KinematicPair;
DEFINE_STANDARD_HANDLE(XCAFDoc_KinematicPair, TDF_Attribute)

//! Attribute to store kinematic pair
class XCAFDoc_KinematicPair : public TDF_Attribute
{

public:

  Standard_EXPORT XCAFDoc_KinematicPair();
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT static Handle(XCAFDoc_KinematicPair) Set (const TDF_Label& theLabel);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Updates parent's label and its sub-labels with data taken from thePairObject.
  //! Old data associated with the label will be lost.
  Standard_EXPORT void SetObject(const Handle(XCAFKinematics_PairObject)& thePairObject);
  
  //! Returns kinematic pair object data taken from the parent's label and its sub-labels.
  Standard_EXPORT Handle(XCAFKinematics_PairObject) GetObject() const;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  Standard_EXPORT static const Standard_GUID& getLimitsID();

  Standard_EXPORT static const Standard_GUID& getParamsID();

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_KinematicPair, TDF_Attribute)

};

#endif
