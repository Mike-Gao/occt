// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshData_Edge_HeaderFile
#define _IMeshData_Edge_HeaderFile

#include <IMeshData_TessellatedShape.hxx>
#include <IMeshData_StatusOwner.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <IMeshData_Curve.hxx>
#include <IMeshData_PCurve.hxx>
#include <IMeshData_Types.hxx>
#include <BRep_Tool.hxx>

class IMeshData_Face;

//! Interface class representing discrete model of an edge.
class IMeshData_Edge : public IMeshData_TessellatedShape, public IMeshData_StatusOwner
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshData_Edge()
  {
  }

  //! Returns TopoDS_Edge attached to model.
  inline const TopoDS_Edge& GetEdge () const
  {
    return TopoDS::Edge (GetShape ());
  }

  //! Returns number of pcurves assigned to current edge.
  Standard_EXPORT virtual Standard_Integer PCurvesNb () const = 0;

  //! Adds discrete pcurve for the specifed discrete face.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& AddPCurve (
    const IMeshData::IFacePtr& theDFace,
    const TopAbs_Orientation   theOrientation) = 0;

  //! Returns pcurve for the specified discrete face.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& GetPCurve (
    const IMeshData::IFacePtr& theDFace,
    const TopAbs_Orientation   theOrientation) const = 0;

  //! Returns pcurve with the given index.
  Standard_EXPORT virtual const IMeshData::IPCurveHandle& GetPCurve (
    const Standard_Integer theIndex) const = 0;

  //! Clears curve and all pcurves assigned to the edge from discretization.
  inline void Clear(const Standard_Boolean isKeepEndPoints)
  {
    myCurve->Clear(isKeepEndPoints);
    for (Standard_Integer aPCurveIt = 0; aPCurveIt < PCurvesNb(); ++aPCurveIt)
    {
      GetPCurve(aPCurveIt)->Clear(isKeepEndPoints);
    }
  }

  //! Returns true in case if the edge is free one, i.e. it does not have pcurves.
  inline Standard_Boolean IsFree () const
  {
    return (PCurvesNb () == 0);
  }

  //! Sets 3d curve associated with current edge.
  inline void SetCurve (const IMeshData::ICurveHandle& theCurve)
  {
    myCurve = theCurve;
  }

  //! Returns 3d curve associated with current edge.
  inline const IMeshData::ICurveHandle& GetCurve () const
  {
    return myCurve;
  }

  //! Gets value of angular deflection for the discrete model.
  inline Standard_Real GetAngularDeflection () const
  {
    return myAngDeflection;
  }

  //! Sets value of angular deflection for the discrete model.
  inline void SetAngularDeflection (const Standard_Real theValue)
  {
    myAngDeflection = theValue;
  }

  //! Returns same param flag.
  //! By default equals to flag stored in topological shape.
  inline Standard_Boolean GetSameParam () const
  {
    return mySameParam;
  }

  //! Updates same param flag.
  inline void SetSameParam (const Standard_Boolean theValue)
  {
    mySameParam = theValue;
  }

  //! Returns same range flag.
  //! By default equals to flag stored in topological shape.
  inline Standard_Boolean GetSameRange () const
  {
    return mySameRange;
  }

  //! Updates same range flag.
  inline void SetSameRange (const Standard_Boolean theValue)
  {
    mySameRange = theValue;
  }

  //! Returns degenerative flag.
  //! By default equals to flag stored in topological shape.
  inline Standard_Boolean GetDegenerated () const
  {
    return myDegenerated;
  }

  //! Updates degenerative flag.
  inline void SetDegenerated (const Standard_Boolean theValue)
  {
    myDegenerated = theValue;
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI_INLINE(IMeshData_Edge, IMeshData_TessellatedShape)

protected:

  //! Constructor.
  //! Initializes empty model.
  Standard_EXPORT IMeshData_Edge (const TopoDS_Edge& theEdge)
    : IMeshData_TessellatedShape(theEdge),
      mySameParam  (BRep_Tool::SameParameter(theEdge)),
      mySameRange  (BRep_Tool::SameRange    (theEdge)),
      myDegenerated(BRep_Tool::Degenerated  (theEdge)),
      myAngDeflection(RealLast())
  {
  }

private:

  Standard_Boolean        mySameParam;
  Standard_Boolean        mySameRange;
  Standard_Boolean        myDegenerated;
  Standard_Real           myAngDeflection;
  IMeshData::ICurveHandle myCurve;
};

#endif