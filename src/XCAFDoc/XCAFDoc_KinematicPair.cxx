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

#include <XCAFDoc_KinematicPair.hxx>

#include <BRep_Builder.hxx>
#include <gp_Pln.hxx>
#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <XCAFDoc.hxx>
#include <XCAFKinematics_HighOrderPairObject.hxx>
#include <XCAFKinematics_LowOrderPairObject.hxx>
#include <XCAFKinematics_LowOrderPairObjectWithCoupling.hxx>
#include <XCAFKinematics_PairObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_KinematicPair, TDF_Attribute)

enum ChildLab
{
  ChildLab_FirstTrsf = 1,
  ChildLab_SecondTrsf,
  ChildLab_FirstGeomParam,
  ChildLab_SecondGeomParam,
  ChildLab_Value
};

//=======================================================================
//function : XCAFDoc_KinematicPair
//purpose  : 
//=======================================================================
XCAFDoc_KinematicPair::XCAFDoc_KinematicPair()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPair::GetID()
{
  static Standard_GUID PairID ("89FB0996-A8B8-4085-87A9-63AB5D56D8C1");
  return PairID; 
}

//=======================================================================
//function : getLimitsID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPair::getLimitsID()
{
  static Standard_GUID KinematicLimitsID("8A9E9B60-7CA1-45F1-882A-42390D4DB894");
  return KinematicLimitsID;
}

//=======================================================================
//function : getParamsID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPair::getParamsID()
{
  static Standard_GUID KinematicParamsID("6BE4AAD0-36BE-4D17-B65C-0B2062E04D92");
  return KinematicParamsID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_KinematicPair) XCAFDoc_KinematicPair::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_KinematicPair) anAttr;
  if (!theLabel.FindAttribute(XCAFDoc_KinematicPair::GetID(), anAttr)) {
    anAttr = new XCAFDoc_KinematicPair();
    theLabel.AddAttribute(anAttr);
  }
  return anAttr;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPair::SetObject(const Handle(XCAFKinematics_PairObject)& theObject)
{
  Backup();

  for (TDF_ChildIterator anIter(Label()); anIter.More(); anIter.Next())
  {
    // Check if it needs to clear value label
    if (anIter.Value().IsEqual(Label().FindChild(ChildLab_Value))) {
      Handle(TDataStd_Integer) aTypeAttr;
      if (Label().FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
      {
        int aType = aTypeAttr->Get();
        if (aType != theObject->Type())
          anIter.Value().ForgetAllAttributes();
      }
    }
    else
      anIter.Value().ForgetAllAttributes();
  }

  // Common attributes
  if (!theObject->Name().IsEmpty())
    TDataStd_Name::Set(Label(), theObject->Name());
  TDataStd_Integer::Set(Label(), theObject->Type());
  TDataXtd_Plane::Set(Label().FindChild(ChildLab_FirstTrsf), gp_Pln(theObject->FirstTransformation()));
  TDataXtd_Plane::Set(Label().FindChild(ChildLab_SecondTrsf), gp_Pln(theObject->SecondTransformation()));

  if (!theObject->GetAllLimits().IsNull() && theObject->GetAllLimits()->Length() > 0) {
    Handle(TDataStd_RealArray) aLimitsAttr;
    aLimitsAttr = TDataStd_RealArray::Set(Label(), getLimitsID(), 1, theObject->GetAllLimits()->Length());
    aLimitsAttr->ChangeArray(theObject->GetAllLimits());
  }

  // Low order pairs
  if (theObject->Type() == XCAFKinematics_PairType_Universal ||
      theObject->Type() == XCAFKinematics_PairType_Homokinetic) {
    Handle(XCAFKinematics_LowOrderPairObject) anObject =
      Handle(XCAFKinematics_LowOrderPairObject)::DownCast(theObject);
    TDataStd_Real::Set(Label(), getParamsID(), anObject->SkewAngle());
  }

  // Low order pairs with motion coupling
  if (theObject->Type() >= XCAFKinematics_PairType_Screw &&
      theObject->Type() <= XCAFKinematics_PairType_Gear) {
    Handle(XCAFKinematics_LowOrderPairObjectWithCoupling) anObject =
      Handle(XCAFKinematics_LowOrderPairObjectWithCoupling)::DownCast(theObject);
    if (!anObject->GetAllParams().IsNull() && anObject->GetAllParams()->Upper() > 0) {
      Handle(TDataStd_RealArray) aParamsAttr;
      aParamsAttr = TDataStd_RealArray::Set(Label(), getParamsID(), 1, anObject->GetAllParams()->Upper());
      aParamsAttr->ChangeArray(anObject->GetAllParams());
    }
  }

  // High order pairs
  if (theObject->Type() >= XCAFKinematics_PairType_PointOnSurface &&
      theObject->Type() <= XCAFKinematics_PairType_RollingCurve) {
    Handle(XCAFKinematics_HighOrderPairObject) anObject =
      Handle(XCAFKinematics_HighOrderPairObject)::DownCast(theObject);
    TDataStd_Integer::Set(Label(), getParamsID(), (Standard_Integer)anObject->Orientation());
    BRep_Builder aBuilder;
    if (theObject->Type() >= XCAFKinematics_PairType_PointOnSurface && !anObject->Surface().IsNull())
    {
      TopoDS_Face aFace;
      aBuilder.MakeFace(aFace, anObject->Surface(), Precision::Confusion());
      TNaming_Builder aTNBuild(Label().FindChild(ChildLab_FirstGeomParam));
      aTNBuild.Generated(aFace);
    }
    if (theObject->Type() == XCAFKinematics_PairType_SlidingSurface ||
        theObject->Type() == XCAFKinematics_PairType_RollingSurface)
    {
      TopoDS_Face aFace1, aFace2;
      if (!anObject->FirstSurface().IsNull()) {
        aBuilder.MakeFace(aFace1, anObject->FirstSurface(), Precision::Confusion());
        TNaming_Builder aTNBuild1(Label().FindChild(ChildLab_FirstGeomParam));
        aTNBuild1.Generated(aFace1);
      }
      if (!anObject->SecondSurface().IsNull()) {
        aBuilder.MakeFace(aFace2, anObject->SecondSurface(), Precision::Confusion());
        TNaming_Builder aTNBuild2(Label().FindChild(ChildLab_SecondGeomParam));
        aTNBuild2.Generated(aFace2);
      }
    }
    if (theObject->Type() == XCAFKinematics_PairType_PointOnPlanarCurve && !anObject->Curve().IsNull())
    {
      TopoDS_Edge anEdge;
      aBuilder.MakeEdge(anEdge, anObject->Curve(), Precision::Confusion());
      TNaming_Builder aTNBuild(Label().FindChild(ChildLab_FirstGeomParam));
      aTNBuild.Generated(anEdge);
    }
    if (theObject->Type() == XCAFKinematics_PairType_SlidingCurve ||
        theObject->Type() == XCAFKinematics_PairType_RollingCurve)
    {
      TopoDS_Edge anEdge1, anEdge2;
      if (!anObject->FirstCurve().IsNull()) {
        aBuilder.MakeEdge(anEdge1, anObject->FirstCurve(), Precision::Confusion());
        TNaming_Builder aTNBuild1(Label().FindChild(ChildLab_FirstGeomParam));
        aTNBuild1.Generated(anEdge1);
      }
      if (!anObject->SecondCurve().IsNull()) {
        aBuilder.MakeEdge(anEdge2, anObject->SecondCurve(), Precision::Confusion());
        TNaming_Builder aTNBuild2(Label().FindChild(ChildLab_SecondGeomParam));
        aTNBuild2.Generated(anEdge2);
      }
    }
  }
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================
Handle(XCAFKinematics_PairObject) XCAFDoc_KinematicPair::GetObject()  const
{
  // Type
  Handle(TDataStd_Integer) aTypeAttr;
  Handle(XCAFKinematics_PairObject) anObject = new XCAFKinematics_PairObject();
  if (Label().FindAttribute(TDataStd_Integer::GetID(), aTypeAttr))
  {
    int aType = aTypeAttr->Get();
    if (aType >= XCAFKinematics_PairType_FullyConstrained &&
        aType <= XCAFKinematics_PairType_Unconstrained)
      anObject = new XCAFKinematics_LowOrderPairObject();
    else if (aType >= XCAFKinematics_PairType_Screw &&
             aType <= XCAFKinematics_PairType_Gear)
      anObject = new XCAFKinematics_LowOrderPairObjectWithCoupling();
    else if (aType >= XCAFKinematics_PairType_PointOnSurface &&
             aType <= XCAFKinematics_PairType_RollingCurve)
      anObject = new XCAFKinematics_HighOrderPairObject();
    anObject->SetType((XCAFKinematics_PairType)aType);
  }

  // Name
  Handle(TDataStd_Name) aNameAttr;
  if (Label().FindAttribute(TDataStd_Name::GetID(), aNameAttr))
    anObject->SetName(aNameAttr->Get());

  //Transformations
  Handle(TDataXtd_Plane) aTrsfAttr;
  if (Label().FindChild(ChildLab_FirstTrsf).FindAttribute(TDataXtd_Plane::GetID(), aTrsfAttr)) {
    gp_Pln aPlane;
    TDataXtd_Geometry::Plane(aTrsfAttr->Label(), aPlane);
    anObject->SetFirstTransformation(aPlane.Position());
  }
  if (Label().FindChild(ChildLab_SecondTrsf).FindAttribute(TDataXtd_Plane::GetID(), aTrsfAttr)) {
    gp_Pln aPlane;
    TDataXtd_Geometry::Plane(aTrsfAttr->Label(), aPlane);
    anObject->SetSecondTransformation(aPlane.Position());
  }

  // Limits
  Handle(TDataStd_RealArray) aLimitsAttr;
  if (Label().FindAttribute(getLimitsID(), aLimitsAttr)) {
    Handle(TColStd_HArray1OfReal) aLimitsArray = aLimitsAttr->Array();
    anObject->SetAllLimits(aLimitsArray);
  }

  // Low order pairs
  if (anObject->Type() == XCAFKinematics_PairType_Universal ||
      anObject->Type() == XCAFKinematics_PairType_Homokinetic) {
    Handle(XCAFKinematics_LowOrderPairObject) aDefObject =
      Handle(XCAFKinematics_LowOrderPairObject)::DownCast(anObject);
    Handle(TDataStd_Real) aParamAttr;
    if (Label().FindAttribute(getParamsID(), aParamAttr)) {
      aDefObject->SetSkewAngle(aParamAttr->Get());
    }
  }

  // Low order pairs with motion coupling
  if (anObject->Type() >= XCAFKinematics_PairType_Screw &&
      anObject->Type() <= XCAFKinematics_PairType_Gear) {
    Handle(XCAFKinematics_LowOrderPairObjectWithCoupling) aDefObject =
      Handle(XCAFKinematics_LowOrderPairObjectWithCoupling)::DownCast(anObject);
    Handle(TDataStd_RealArray) aParamsAttr;
    if (Label().FindAttribute(getParamsID(), aParamsAttr)) {
      Handle(TColStd_HArray1OfReal) aParamsArray = aParamsAttr->Array();
      aDefObject->SetAllParams(aParamsAttr->Array());
    }
  }

  // High order pairs
  if (anObject->Type() >= XCAFKinematics_PairType_PointOnSurface &&
      anObject->Type() <= XCAFKinematics_PairType_RollingCurve) {
    Handle(XCAFKinematics_HighOrderPairObject) aDefObject =
      Handle(XCAFKinematics_HighOrderPairObject)::DownCast(anObject);
    Handle(TDataStd_Integer) anOrienAttr;
    if (Label().FindAttribute(getParamsID(), anOrienAttr)) {
      aDefObject->SetOrientation(anOrienAttr->Get() != 0);
    }

    Handle(TNaming_NamedShape) aNS;
    if (anObject->Type() == XCAFKinematics_PairType_PointOnSurface)
    {
      if (Label().FindChild(ChildLab_FirstGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetSurface(BRep_Tool::Surface(aFace));
      }
    }
    if (anObject->Type() == XCAFKinematics_PairType_SlidingSurface ||
        anObject->Type() == XCAFKinematics_PairType_RollingSurface)
    {
      if (Label().FindChild(ChildLab_FirstGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetFirstSurface(BRep_Tool::Surface(aFace));
      }
      if (Label().FindChild(ChildLab_SecondGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Face aFace = TopoDS::Face(TNaming_Tool::GetShape(aNS));
        aDefObject->SetSecondSurface(BRep_Tool::Surface(aFace));
      }
    }
    if (anObject->Type() == XCAFKinematics_PairType_PointOnPlanarCurve)
    {
      if (Label().FindChild(ChildLab_FirstGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
    }
    if (anObject->Type() == XCAFKinematics_PairType_SlidingCurve ||
        anObject->Type() == XCAFKinematics_PairType_RollingCurve)
    {
      if (Label().FindChild(ChildLab_FirstGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetFirstCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
      if (Label().FindChild(ChildLab_SecondGeomParam).FindAttribute(TNaming_NamedShape::GetID(), aNS))
      {
        TopoDS_Edge anEdge = TopoDS::Edge(TNaming_Tool::GetShape(aNS));
        Standard_Real aFirst, aLast;
        aDefObject->SetSecondCurve(BRep_Tool::Curve(anEdge, aFirst, aLast));
      }
    }
  }

  return anObject;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicPair::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPair::Restore(const Handle(TDF_Attribute)& /*With*/)
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_KinematicPair::NewEmpty() const
{
  return new XCAFDoc_KinematicPair();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPair::Paste(const Handle(TDF_Attribute)& /*Into*/,
                                  const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicPair::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TDF_Attribute)
}
