// Created on: 2020-03-19
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

#include <XDEDRAW_Kinematics.hxx>

#include <BRep_Tool.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Geom_Plane.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_KinematicPair.hxx>
#include <XCAFDoc_KinematicPairValue.hxx>
#include <XCAFDoc_KinematicTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFKinematics_HighOrderPairObject.hxx>
#include <XCAFKinematics_LowOrderPairObject.hxx>
#include <XCAFKinematics_LowOrderPairObjectWithCoupling.hxx>
#include <XCAFKinematics_PairObject.hxx>
#include <XCAFKinematics_PairValueObject.hxx>

//=======================================================================
//function : getDocument
//purpose  : auxiliary method
//=======================================================================
static Standard_Boolean getDocument(Draw_Interpretor& di,
                                    const char* theDocName,
                                    Handle(TDocStd_Document)& theDoc)
{
  DDocStd::GetDocument(theDocName, theDoc);
  if (theDoc.IsNull()) {
    di << theDocName << " is not a document\n";
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : getEntry
//purpose  : auxiliary method
//=======================================================================
static TCollection_AsciiString getEntry(const TDF_Label& theLabel)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(theLabel, entry);
  return entry;
}

//=======================================================================
//function : getLabel
//purpose  : auxiliary method
//=======================================================================
static Standard_Boolean getLabel(Draw_Interpretor& di,
                                 const Handle(TDocStd_Document)& theDoc,
                                 const char* theEntry,
                                 TDF_Label& theLabel)
{
  TDF_Tool::Label(theDoc->GetData(), theEntry, theLabel);
  if (theLabel.IsNull()) {
    di << "Invalid label " << theEntry << "\n";
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : addMechanism
//purpose  : 
//=======================================================================
static Standard_Integer addMechanism(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {
    di << "Use: XAddMechanism Doc\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  di << getEntry(aTool->AddMechanism());

  return 0;
}

//=======================================================================
//function : isMechanism
//purpose  : 
//=======================================================================
static Standard_Integer isMechanism(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XIsMechanism Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  di << aTool->IsMechanism(aLabel);

  return 0;
}

//=======================================================================
//function : removeMechanism
//purpose  : 
//=======================================================================
static Standard_Integer removeMechanism(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XRemoveMechanism Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  aTool->RemoveMechanism(aLabel);

  return 0;
}

//=======================================================================
//function : addLink
//purpose  : 
//=======================================================================
static Standard_Integer addLink(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XAddLink Doc ParentMechanism [-Base] [shapeLabel1 .. shapeLabelN]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aMechanism;
  if (!getLabel(di, aDoc, argv[2], aMechanism))
    return 1;
  Standard_Boolean IsBase = argc >= 4 && TCollection_AsciiString(argv[3]).IsEqual("-Base");
  TDF_LabelSequence aShapeArray;
  for (Standard_Integer i = (IsBase) ? 4 : 3; i < argc; i++) {
    TDF_Label aLabel;
    if (!getLabel(di, aDoc, argv[i], aLabel))
      continue;
    aShapeArray.Append(aLabel);
  }

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  if(IsBase)
    di << getEntry(aTool->AddBaseLink(aMechanism, aShapeArray));
  else 
    di << getEntry(aTool->AddLink(aMechanism, aShapeArray));
  return 0;
}

//=======================================================================
//function : setLink
//purpose  : 
//=======================================================================
static Standard_Integer setLink(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XAddLink Doc Link shapeLabel1 .. shapeLabelN\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLink;
  if (!getLabel(di, aDoc, argv[2], aLink))
    return 1;

  TDF_LabelSequence aShapeArray;
  for (Standard_Integer i = 3; i < argc; i++) {
    TDF_Label aLabel;
    if (!getLabel(di, aDoc, argv[i], aLabel))
      continue;
    aShapeArray.Append(aLabel);
  }

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  if (!aTool->SetLink(aLink, aShapeArray))
    return 1;

  return 0;
}

//=======================================================================
//function : isLink
//purpose  : 
//=======================================================================
static Standard_Integer isLink(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XIsLink Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  di << aTool->IsLink(aLabel);

  return 0;
}

//=======================================================================
//function : removeLink
//purpose  : 
//=======================================================================
static Standard_Integer removeLink(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XRemoveLink Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  aTool->RemoveLink(aLabel);

  return 0;
}

//=======================================================================
//function : addJoint
//purpose  : 
//=======================================================================
static Standard_Integer addJoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XAddJoint Doc Mechanism [Link1 Link2]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aMechanism;
  if (!getLabel(di, aDoc, argv[2], aMechanism))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());

  if (argc == 3) {
    di << getEntry(aTool->AddJoint(aMechanism));
    return 0;
  }
  else if (argc == 5) {
    TDF_Label aLink1, aLink2;
    if (!getLabel(di, aDoc, argv[3], aLink1))
      return 1;
    if (!getLabel(di, aDoc, argv[4], aLink2))
      return 1;
    di << getEntry(aTool->AddJoint(aMechanism, aLink1, aLink2));
    return 0;
  }

  return 1;
}

//=======================================================================
//function : isJoint
//purpose  : 
//=======================================================================
static Standard_Integer isJoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XIsJoint Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  di << aTool->IsJoint(aLabel);
  return 0;
}

//=======================================================================
//function : setJoint
//purpose  : 
//=======================================================================
static Standard_Integer setJoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetJoint Doc Joint Link1 Link2\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint))
    return 1;
  TDF_Label aLink1, aLink2;
  if (!getLabel(di, aDoc, argv[3], aLink1))
    return 1;
  if (!getLabel(di, aDoc, argv[4], aLink2))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  if (!aTool->SetJoint(aJoint, aLink1, aLink2))
    return 1;

  return 0;
}

//=======================================================================
//function : removeJoint
//purpose  : 
//=======================================================================
static Standard_Integer removeJoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XRemoveJoint Doc Label\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLabel;
  if (!getLabel(di, aDoc, argv[2], aLabel))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  aTool->RemoveJoint(aLabel);

  return 0;
}

//=======================================================================
//function : getMechanisms
//purpose  : 
//=======================================================================
static Standard_Integer getMechanisms(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {
    di << "Use: XGetMechanisms Doc\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_LabelSequence aMechanismArray = aTool->GetMechanisms();
  for (TDF_LabelSequence::Iterator anIter(aMechanismArray); anIter.More(); anIter.Next()) {
    di << getEntry(anIter.Value()) << " ";
  }

  return 0;
}

//=======================================================================
//function : getLinks
//purpose  : 
//=======================================================================
static Standard_Integer getLinks(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetLinks Doc Mechanism\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aMechanism;
  if (!getLabel(di, aDoc, argv[2], aMechanism))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_LabelSequence aLinksArray = aTool->GetLinks(aMechanism);
  for (TDF_LabelSequence::Iterator anIter(aLinksArray); anIter.More(); anIter.Next()) {
    di << getEntry(anIter.Value()) << " ";
  }

  return 0;
}

//=======================================================================
//function : getJoints
//purpose  : 
//=======================================================================
static Standard_Integer getJoints(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetJoints Doc Mechanism\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aMechanism;
  if (!getLabel(di, aDoc, argv[2], aMechanism))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_LabelSequence aJointsArray = aTool->GetJoints(aMechanism);
  for (TDF_LabelSequence::Iterator anIter(aJointsArray); anIter.More(); anIter.Next()) {
    di << getEntry(anIter.Value()) << " ";
  }

  return 0;
}

//=======================================================================
//function : getLinksOfJoint
//purpose  : 
//=======================================================================
static Standard_Integer getLinksOfJoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetLinksOfJoint Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aLink1, aLink2;
  aTool->GetLinksOfJoint(aJoint, aLink1, aLink2);

  di << getEntry(aLink1) << " " << getEntry(aLink2);

  return 0;
}

//=======================================================================
//function : getJointsOfLink
//purpose  : 
//=======================================================================
static Standard_Integer getJointsOfLink(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetJointsOfLink Doc Link\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLink;
  if (!getLabel(di, aDoc, argv[2], aLink))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_LabelSequence aJointsArray = aTool->GetJointsOfLink(aLink);
  for (TDF_LabelSequence::Iterator anIter(aJointsArray); anIter.More(); anIter.Next()) {
    di << getEntry(anIter.Value()) << " ";
  }

  return 0;
}

//=======================================================================
//function : getRefShapes
//purpose  : 
//=======================================================================
static Standard_Integer getRefShapes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetRefShapes Doc Link\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aLink;
  if (!getLabel(di, aDoc, argv[2], aLink))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_LabelSequence aShapesArray = aTool->GetRefShapes(aLink);
  for (TDF_LabelSequence::Iterator anIter(aShapesArray); anIter.More(); anIter.Next()) {
    di << getEntry(anIter.Value()) << " ";
  }

  return 0;
}

//=======================================================================
//function : setName
//purpose  : 
//=======================================================================
static Standard_Integer setName(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetPairName Doc Joint Name\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  TCollection_AsciiString aName;
  for (Standard_Integer i = 3; i < argc; i++) {
    aName.AssignCat(argv[i]);
    aName.AssignCat(" ");
    if (i == argc - 1)
      aName.Remove(aName.Length(), 1);
  }

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    anObject->SetName(aName);
    aPair->SetObject(anObject);
  }

  return 0;
}

//=======================================================================
//function : getName
//purpose  : 
//=======================================================================
static Standard_Integer getName(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetPairName Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    di << anObject->Name();
  }

  return 0;
}

//=======================================================================
//function : setType
//purpose  : 
//=======================================================================
static Standard_Integer setType(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetPairType Doc Joint Type[1, 19]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    int aType = Draw::Atoi(argv[3]);
    if (aType >= XCAFKinematics_PairType_FullyConstrained &&
        aType <= XCAFKinematics_PairType_Unconstrained)
      anObject = new XCAFKinematics_LowOrderPairObject();
    else if (aType >= XCAFKinematics_PairType_Screw &&
             aType <= XCAFKinematics_PairType_LinearFlexibleAndPinion)
      anObject = new XCAFKinematics_LowOrderPairObjectWithCoupling();
    else if (aType >= XCAFKinematics_PairType_PointOnSurface &&
             aType <= XCAFKinematics_PairType_LinearFlexibleAndPlanarCurve)
      anObject = new XCAFKinematics_HighOrderPairObject();
    anObject->SetType((XCAFKinematics_PairType)aType);
    aPair->SetObject(anObject);
  }

  return 0;
}

//=======================================================================
//function : getType
//purpose  : 
//=======================================================================
static Standard_Integer getType(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetPairType Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    di << anObject->Type();
  }

  return 0;
}

//=======================================================================
//function : setTrsf
//purpose  : 
//=======================================================================
static Standard_Integer setTrsf(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetPairTransformation Doc Joint TrsfNumber[1/2] Plane\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    Standard_Integer aTrsfNb = Draw::Atoi(argv[3]);
    Handle(Geom_Plane) aSurface = Handle(Geom_Plane)::DownCast(DrawTrSurf::GetSurface(argv[4]));
    if (aSurface.IsNull()) {
      di << "Invalid transformation\n";
      return 1;
    }
    switch (aTrsfNb) {
    case 1: anObject->SetFirstTransformation(aSurface->Position());
      break;
    case 2: anObject->SetSecondTransformation(aSurface->Position());
      break;
    default:
      di << "Invalid number of transformation\n";
      return 1;
    }
    aPair->SetObject(anObject);
  }

  return 0;
}

//=======================================================================
//function : getTrsf
//purpose  : 
//=======================================================================
static Standard_Integer getTrsf(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XGetPairTransformation Doc Joint TrsfNumber[1/2] PlaneName\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    Standard_Integer aTrsfNb = Draw::Atoi(argv[3]);
    Standard_CString aName = argv[4];
    switch (aTrsfNb) {
    case 1: DrawTrSurf::Set(aName, new Geom_Plane(anObject->FirstTransformation()));
      break;
    case 2: DrawTrSurf::Set(aName, new Geom_Plane(anObject->SecondTransformation()));
      break;
    default:
      di << "Invalid number of transformation\n";
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : setLimits
//purpose  : 
//=======================================================================
static Standard_Integer setLimits(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetPairLimits Doc Joint Value1 Value2...\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(TColStd_HArray1OfReal) aLimitArray = new TColStd_HArray1OfReal(1, argc - 3);
  for (Standard_Integer i = 3; i < argc; i++) {
    aLimitArray->ChangeValue(i - 2) = Draw::Atof(argv[i]);
  }

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    anObject->SetAllLimits(aLimitArray);
    aPair->SetObject(anObject);
  }

  return 0;
}

//=======================================================================
//function : getLimits
//purpose  : 
//=======================================================================
static Standard_Integer getLimits(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetPairLimits Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (!aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    di << "Invalid Pair object\n";
    return 1;
  }

  Handle(TColStd_HArray1OfReal) aLimitArray;
  Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
  aLimitArray = anObject->GetAllLimits();
  for (Standard_Integer i = 1; i <= aLimitArray->Length(); i++)
    di << aLimitArray->Value(i) << " ";

  return 0;
}

//=======================================================================
//function : setParameters
//purpose  : 
//=======================================================================
static Standard_Integer setParameters(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetPairParams Doc Joint Value1 Value2...\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(TColStd_HArray1OfReal) aParamArray = new TColStd_HArray1OfReal(1, argc - 3);
  for (Standard_Integer i = 3; i < argc; i++) {
    aParamArray->ChangeValue(i - 2) = Draw::Atof(argv[i]);
  }

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_LowOrderPairObjectWithCoupling) anObject = 
      Handle(XCAFKinematics_LowOrderPairObjectWithCoupling)::DownCast(aPair->GetObject());
    if (!anObject.IsNull()) {
      anObject->SetAllParams(aParamArray);
      aPair->SetObject(anObject);
      return 0;
    }
    Handle(XCAFKinematics_LowOrderPairObject) aLowObject =
      Handle(XCAFKinematics_LowOrderPairObject)::DownCast(aPair->GetObject());
    if (!aLowObject.IsNull()) {
      aLowObject->SetSkewAngle(aParamArray->First());
      aPair->SetObject(aLowObject);
      return 0;
    }
  }

  di << "Wrong type of object\n";
  return 1;
}

//=======================================================================
//function : getParameters
//purpose  : 
//=======================================================================
static Standard_Integer getParameters(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetPairParams Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_LowOrderPairObjectWithCoupling) anObject =
      Handle(XCAFKinematics_LowOrderPairObjectWithCoupling)::DownCast(aPair->GetObject());
    if (anObject.IsNull()) {
      Handle(XCAFKinematics_LowOrderPairObject) aLowObject =
        Handle(XCAFKinematics_LowOrderPairObject)::DownCast(aPair->GetObject());
      if (!aLowObject.IsNull() &&
          (aLowObject->Type() == XCAFKinematics_PairType_Universal ||
           aLowObject->Type() == XCAFKinematics_PairType_Homokinetic)) {
        di << "Skew Angle = " << aLowObject->SkewAngle();
        return 0;
      }
      else {
        di << "Wrong type of object\n";
        return 1;
      }
    }
    switch (anObject->Type()) {
    case XCAFKinematics_PairType_Screw:
      di << "Pitch = " << anObject->Pitch();
      break;
    case XCAFKinematics_PairType_RackAndPinion:
      di << "Pinion Radius = " << anObject->PinionRadius();
      break;
    case XCAFKinematics_PairType_Gear:
      di << "First Link Radius = " << anObject->RadiusFirstLink() << "\n";
      di << "Second Link Radius = " << anObject->RadiusSecondLink() << "\n";
      di << "Bevel = " << anObject->Bevel() << "\n";
      di << "Helical Angle = " << anObject->HelicalAngle() << "\n";
      di << "Gear Ratio = " << anObject->GearRatio();
      break;
    }
  }

  return 0;
}

//=======================================================================
//function : setOrientation
//purpose  : 
//=======================================================================
static Standard_Integer setOrientation(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetPairOrientation Doc Joint Orientation[0/1]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    Handle(XCAFKinematics_HighOrderPairObject) aHighObject =
      Handle(XCAFKinematics_HighOrderPairObject)::DownCast(anObject);
    if (aHighObject.IsNull()) {
      di << "Error: Orientation could be applied only for high order kinematic pair\n";
      return 1;
    }
    aHighObject->SetOrientation(Draw::Atoi(argv[3]) != 0 );
    aPair->SetObject(anObject);
  }

  return 0;
}

//=======================================================================
//function : getOrientation
//purpose  : 
//=======================================================================
static Standard_Integer getOrientation(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetPairOrientation Doc Joint\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;

  Handle(XCAFDoc_KinematicPair) aPair;
  if (aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    Handle(XCAFKinematics_PairObject) anObject = aPair->GetObject();
    Handle(XCAFKinematics_HighOrderPairObject) aHighObject =
      Handle(XCAFKinematics_HighOrderPairObject)::DownCast(anObject);
    if (aHighObject.IsNull()) {
      di << "Error: Orientation could be applied only for high order kinematic pair\n";
      return 1;
    }
    di << aHighObject->Orientation();
  }

  return 0;
}

//=======================================================================
//function : setGeomParam
//purpose  : 
//=======================================================================
static Standard_Integer setGeomParam(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetPairGeomParam Doc Joint Number[1/2] Surface/Face/Curve/Edge\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;
  Handle(XCAFDoc_KinematicPair) aPair;
  if (!aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    di << "Invalid kinematic pair object\n";
    return 1;
  }
  Handle(XCAFKinematics_HighOrderPairObject) anObject = 
    Handle(XCAFKinematics_HighOrderPairObject)::DownCast(aPair->GetObject());
  if (anObject.IsNull()) {
    di << "Wrong type of kinematic pair\n";
    return 1;
  }

  Standard_Integer aParamNb = Draw::Atoi(argv[3]);

  if (anObject->Type() >= XCAFKinematics_PairType_PointOnSurface &&
      anObject->Type() <= XCAFKinematics_PairType_RollingSurface)
  {
    // Surface
    Handle(Geom_Surface) aSurface = DrawTrSurf::GetSurface(argv[4]);
    if (aSurface.IsNull()) {
      // Try to retrieve as a face
      TopoDS_Face aFace = TopoDS::Face(DBRep::Get(argv[4], TopAbs_FACE));
      if (!aFace.IsNull()) {
        TopLoc_Location aLoc;
        aSurface = BRep_Tool::Surface(aFace, aLoc);
      }
    }
    if (aSurface.IsNull()) {
      di << "Invalid geometric argument\n";
      return 1;
    }
    if (anObject->Type() == XCAFKinematics_PairType_PointOnSurface)
      anObject->SetSurface(aSurface);
    else {
      switch (aParamNb) {
      case 1: anObject->SetFirstSurface(aSurface);
        break;
      case 2: anObject->SetSecondSurface(aSurface);
        break;
      default:
        di << "Invalid number of geometric argument\n";
        return 1;
      }
    }
  }
  else {
    // Curve
    Handle(Geom_Curve) aCurve = DrawTrSurf::GetCurve(argv[4]);
    if (aCurve.IsNull()) {
      // Try to retrieve as an adge
      TopoDS_Edge anEdge = TopoDS::Edge(DBRep::Get(argv[4], TopAbs_EDGE));
      if (!anEdge.IsNull()) {
        TopLoc_Location aLoc;
        Standard_Real aFirst, aLast;
        aCurve = BRep_Tool::Curve(anEdge, aLoc, aFirst, aLast);
      }
    }
    if (aCurve.IsNull()) {
      di << "Invalid geometric argument\n";
      return 1;
    }
    if (anObject->Type() == XCAFKinematics_PairType_PointOnPlanarCurve)
      anObject->SetCurve(aCurve);
    else {
      switch (aParamNb) {
      case 1: anObject->SetFirstCurve(aCurve);
        break;
      case 2: anObject->SetSecondCurve(aCurve);
        break;
      default:
        di << "Invalid number of geometric argument\n";
        return 1;
      }
    }
  }

  aPair->SetObject(anObject);

  return 0;
}

//=======================================================================
//function : getGeomParam
//purpose  : 
//=======================================================================
static Standard_Integer getGeomParam(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XGetPairGeomParam Doc Joint Number[1/2] Name\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;
  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[2], aJoint) || !aTool->IsJoint(aJoint))
    return 1;
  Handle(XCAFDoc_KinematicPair) aPair;
  if (!aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    di << "Invalid kinematic pair object\n";
    return 1;
  }
  Handle(XCAFKinematics_HighOrderPairObject) anObject =
    Handle(XCAFKinematics_HighOrderPairObject)::DownCast(aPair->GetObject());
  if (anObject.IsNull()) {
    di << "Wrong type of kinematic pair\n";
    return 1;
  }

  Standard_Integer aTrsfNb = Draw::Atoi(argv[3]);

  if (anObject->Type() >= XCAFKinematics_PairType_PointOnSurface &&
      anObject->Type() <= XCAFKinematics_PairType_RollingSurface)
  {
    // Surface
    Handle(Geom_Surface) aSurface;
    if (anObject->Type() == XCAFKinematics_PairType_PointOnSurface)
      aSurface = anObject->Surface();
    else {
      switch (aTrsfNb) {
      case 1: aSurface = anObject->FirstSurface();
        break;
      case 2: aSurface = anObject->SecondSurface();
        break;
      default:
        di << "Invalid number of geometric argument\n";
        return 1;
      }
    }
    DrawTrSurf::Set(argv[4], aSurface);
  }
  else {
    // Curve
    Handle(Geom_Curve) aCurve;
    if (anObject->Type() == XCAFKinematics_PairType_PointOnPlanarCurve)
      aCurve = anObject->Curve();
    else {
      switch (aTrsfNb) {
      case 1: aCurve = anObject->FirstCurve();
        break;
      case 2: aCurve = anObject->SecondCurve();
        break;
      default:
        di << "Invalid number of geometric argument\n";
        return 1;
      }
    }
    DrawTrSurf::Set(argv[4], aCurve);
  }

  return 0;
}

//=======================================================================
//function : addMechanismState
//purpose  : 
//=======================================================================
static Standard_Integer addMechanismState(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XAddMechanismState Doc ParentMechanism\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;

  TDF_Label aMechanism;
  if (!getLabel(di, aDoc, argv[2], aMechanism))
    return 1;

  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  di << getEntry(aTool->AddState(aMechanism));
  return 0;
}

//=======================================================================
//function : addValues
//purpose  : 
//=======================================================================
static Standard_Integer addValues(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XAddPairValues Doc State [Joint -Key1 Values1... -KeyN ValuesN]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;
  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aState;
  if (!getLabel(di, aDoc, argv[2], aState))
    return 1;
  TDF_Label aMechanism = aState.Father().Father();

  TDF_TagSource aTag;
  TDF_Label aValue = aTag.NewChild(aState);
  if (argc == 3)
  {
    Handle(XCAFDoc_KinematicPairValue) aPairValue;
    XCAFDoc_KinematicPairValue::Set(aValue);
  }
  else {
    TDF_Label aJoint;
    if (!getLabel(di, aDoc, argv[3], aJoint) || !aTool->IsJoint(aJoint))
      return 1;

    Handle(XCAFKinematics_PairValueObject) anObject;
    Handle(XCAFDoc_KinematicPairValue) aPairValue;
    Handle(XCAFDoc_KinematicPair) aPair;
    if (!aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
      di << "Invalid kinematic pair object\n";
      return 1;
    }
    Handle(XCAFKinematics_PairObject) aPairObject = aPair->GetObject();
    anObject = new XCAFKinematics_PairValueObject();
    anObject->SetType(aPairObject->Type());
    aPairValue = XCAFDoc_KinematicPairValue::Set(aValue, aJoint);

    try {
      Standard_Integer anIt = 4;
      while (anIt < argc) {
        TCollection_AsciiString aKey = argv[anIt];
        if (aKey.IsEqual("-rotation"))
          anObject->SetRotation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-first_rotation"))
          anObject->SetFirstRotation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-second_rotation"))
          anObject->SetSecondRotation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-translation"))
          anObject->SetTranslation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-first_translation"))
          anObject->SetFirstTranslation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-second_translation"))
          anObject->SetSecondTranslation(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-point_on_surface")) {
          anObject->SetPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
          anIt += 2;
        }
        else if (aKey.IsEqual("-first_point_on_surface")) {
          anObject->SetFirstPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
          anIt += 2;
        }
        else if (aKey.IsEqual("-second_point_on_surface")) {
          anObject->SetSecondPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
          anIt += 2;
        }
        else if (aKey.IsEqual("-point_on_curve"))
          anObject->SetPointOnCurve(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-first_point_on_curve"))
          anObject->SetFirstPointOnCurve(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-second_point_on_curve"))
          anObject->SetSecondPointOnCurve(Atof(argv[++anIt]));
        else if (aKey.IsEqual("-ypr")) {
          anObject->SetYPR(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]), Atof(argv[anIt + 3]));
          anIt += 3;
        }
        else if (aKey.IsEqual("-trsf")) {
          gp_Pnt aLoc(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]), Atof(argv[anIt + 3]));
          gp_Dir aDir(Atof(argv[anIt + 4]), Atof(argv[anIt + 5]), Atof(argv[anIt + 6]));
          gp_Dir aXDir(Atof(argv[anIt + 7]), Atof(argv[anIt + 8]), Atof(argv[anIt + 9]));
          anIt += 9;
          gp_Ax2 aResult = gp_Ax2(aLoc, aDir, aXDir);
          anObject->SetTransformation(aResult);
        }
        anIt++;
      }
    }
    catch (...) {
      di << "Invalid number of parameters";
      return 1;
    }

    aPairValue->SetObject(anObject);
  }

  di << getEntry(aValue);
  return 0;
}

//=======================================================================
//function : setValues
//purpose  : 
//=======================================================================
static Standard_Integer setValues(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di << "Use: XSetPairValues Doc Value Joint -Key1 Values1... -KeyN ValuesN\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;
  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aJoint;
  if (!getLabel(di, aDoc, argv[3], aJoint) || !aTool->IsJoint(aJoint))
    return 1;
  TDF_Label aMechanism = aJoint.Father().Father();
  TDF_Label aValue;
  if (!getLabel(di, aDoc, argv[2], aValue) || !aTool->IsValue(aValue))
    return 1;

  Handle(XCAFKinematics_PairValueObject) anObject;
  Handle(XCAFDoc_KinematicPairValue) aPairValue;
  Handle(XCAFDoc_KinematicPair) aPair;

  if (!aJoint.FindAttribute(XCAFDoc_KinematicPair::GetID(), aPair)) {
    di << "Invalid kinematic pair object\n";
    return 1;
  }
  Handle(XCAFKinematics_PairObject) aPairObject = aPair->GetObject();

  if (aValue.FindAttribute(XCAFDoc_KinematicPairValue::GetID(), aPairValue))
  {
    aPairValue = XCAFDoc_KinematicPairValue::Set(aValue, aJoint);
    anObject = aPairValue->GetObject();
    if (anObject->GetAllValues().IsNull())
      anObject->SetType(aPairObject->Type());
  }
  else {
    anObject = new XCAFKinematics_PairValueObject();
    aPairValue = XCAFDoc_KinematicPairValue::Set(aValue,aJoint);
    anObject->SetType(aPairObject->Type());
  }

  try {
    Standard_Integer anIt = 4;
    while (anIt < argc) {
      TCollection_AsciiString aKey = argv[anIt];
      if (aKey.IsEqual("-rotation"))
        anObject->SetRotation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-first_rotation"))
        anObject->SetFirstRotation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-second_rotation"))
        anObject->SetSecondRotation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-translation"))
        anObject->SetTranslation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-first_translation"))
        anObject->SetFirstTranslation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-second_translation"))
        anObject->SetSecondTranslation(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-point_on_surface")) {
        anObject->SetPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
        anIt += 2;
      }
      else if (aKey.IsEqual("-first_point_on_surface")) {
        anObject->SetFirstPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
        anIt += 2;
      }
      else if (aKey.IsEqual("-second_point_on_surface")) {
        anObject->SetSecondPointOnSurface(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]));
        anIt += 2;
      }
      else if (aKey.IsEqual("-point_on_curve"))
        anObject->SetPointOnCurve(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-first_point_on_curve"))
        anObject->SetFirstPointOnCurve(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-second_point_on_curve"))
        anObject->SetSecondPointOnCurve(Atof(argv[++anIt]));
      else if (aKey.IsEqual("-ypr")) {
        anObject->SetYPR(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]), Atof(argv[anIt + 3]));
        anIt += 3;
      }
      else if (aKey.IsEqual("-trsf")) {
        gp_Pnt aLoc(Atof(argv[anIt + 1]), Atof(argv[anIt + 2]), Atof(argv[anIt + 3]));
        gp_Dir aDir(Atof(argv[anIt + 4]), Atof(argv[anIt + 5]), Atof(argv[anIt + 6]));
        gp_Dir aXDir(Atof(argv[anIt + 7]), Atof(argv[anIt + 8]), Atof(argv[anIt + 9]));
        anIt += 9;
        gp_Ax2 aResult = gp_Ax2(aLoc, aDir, aXDir);
        anObject->SetTransformation(aResult);
      }
      anIt++;
    }
  }
  catch (...) {
    di << "Invalid number of parameters";
    return 1;
  }

  aPairValue->SetObject(anObject);

  return 0;
}

//=======================================================================
//function : getValues
//purpose  : 
//=======================================================================
static Standard_Integer getValues(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XGetPairValues Doc Value -Key\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!getDocument(di, argv[1], aDoc))
    return 1;
  Handle(XCAFDoc_KinematicTool) aTool = XCAFDoc_DocumentTool::KinematicTool(aDoc->Main());
  TDF_Label aValue;
  if (!getLabel(di, aDoc, argv[2], aValue) || !aTool->IsValue(aValue))
    return 1;


  Handle(XCAFKinematics_PairValueObject) anObject;
  Handle(XCAFDoc_KinematicPairValue) aPairValue;
  if (aValue.FindAttribute(XCAFDoc_KinematicPairValue::GetID(), aPairValue))
    anObject = aPairValue->GetObject();
  if (anObject.IsNull()) {
    di << "Invalid value object";
    return 1;
  }

  TCollection_AsciiString aKey = argv[3];
  if (aKey.IsEqual("-rotation"))
    di << anObject->GetRotation();
  else if (aKey.IsEqual("-first_rotation"))
    di << anObject->GetFirstRotation();
  else if (aKey.IsEqual("-second_rotation"))
    di << anObject->GetSecondRotation();
  else if (aKey.IsEqual("-translation"))
    di << anObject->GetTranslation();
  else if (aKey.IsEqual("-first_translation"))
    di << anObject->GetFirstTranslation();
  else if (aKey.IsEqual("-second_translation"))
    di << anObject->GetSecondTranslation();
  else if (aKey.IsEqual("-point_on_surface")) {
    Standard_Real anU, aV;
    if (anObject->GetPointOnSurface(anU, aV))
      di << anU << " " << aV;
  }
  else if (aKey.IsEqual("-first_point_on_surface")) {
    Standard_Real anU, aV;
    if (anObject->GetFirstPointOnSurface(anU, aV))
      di << anU << " " << aV;
  }
  else if (aKey.IsEqual("-second_point_on_surface")) {
    Standard_Real anU, aV;
    if (anObject->GetSecondPointOnSurface(anU, aV))
      di << anU << " " << aV;
  }
  else if (aKey.IsEqual("-point_on_curve"))
    di << anObject->GetPointOnCurve();
  else if (aKey.IsEqual("-first_point_on_curve"))
    di << anObject->GetFirstPointOnCurve();
  else if (aKey.IsEqual("-second_point_on_curve"))
    di << anObject->GetSecondPointOnCurve();
  else if (aKey.IsEqual("-ypr")) {
    Standard_Real anYaw, aPitch, aRoll;
    if (anObject->GetYPR(anYaw, aPitch, aRoll))
      di << anYaw << " " << aPitch << " " << aRoll;
  }
  else if (aKey.IsEqual("-trsf")) {
    gp_Ax2 aResult = anObject->GetTransformation();
    di << aResult.Location().X() << " " << aResult.Location().Y() << " " << aResult.Location().Z() << "\n";
    di << aResult.Direction().X() << " " << aResult.Direction().Y() << " " << aResult.Direction().Z() << "\n";
    di << aResult.XDirection().X() << " " << aResult.XDirection().Y() << " " << aResult.XDirection().Z();
  }

  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_Kinematics::InitCommands(Draw_Interpretor& di)
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  Standard_CString g = "XDE Kinematics commands";

  di.Add("XAddMechanism", "XAddMechanism Doc",
    __FILE__, addMechanism, g);

  di.Add("XIsMechanism", "XIsMechanism Doc Label",
    __FILE__, isMechanism, g);

  di.Add("XRemoveMechanism", "XRemoveMechanism Doc Label",
    __FILE__, removeMechanism, g);

  di.Add("XAddLink", "XAddLink Doc ParentMechanism [-Base] [shapeLabel1 .. shapeLabelN]",
    __FILE__, addLink, g);

  di.Add("XSetLink", "XSetLink Doc Link shapeLabel1 .. shapeLabelN",
    __FILE__, setLink, g);

  di.Add("XIsLink", "XIsLink Doc Label",
    __FILE__, isLink, g);

  di.Add("XRemoveLink", "XRemoveLink Doc Label",
    __FILE__, removeLink, g);

  di.Add("XAddJoint", "XAddJoint Doc Mechanism [Link1 Link2]",
    __FILE__, addJoint, g);

  di.Add("XSetJoint", "XSetJoint Doc Joint Link1 Link2",
    __FILE__, setJoint, g);

  di.Add("XIsJoint", "XIsJoint Doc Label",
    __FILE__, isJoint, g);

  di.Add("XRemoveJoint", "XRemoveJoint Doc Label",
    __FILE__, removeJoint, g);

  di.Add("XGetMechanisms", "XGetMechanisms Doc",
    __FILE__, getMechanisms, g);

  di.Add("XGetLinks", "XGetLinks Doc Mechanism",
    __FILE__, getLinks, g);

  di.Add("XGetJoints", "XGetJoints Doc Mechanism",
    __FILE__, getJoints, g);

  di.Add("XGetLinksOfJoint", "XGetLinksOfJoint Doc Joint",
    __FILE__, getLinksOfJoint, g);

  di.Add("XGetJointsOfLink", "XGetJointsOfLink Doc Link",
    __FILE__, getJointsOfLink, g);

  di.Add("XGetRefShapes", "XGetRefShapes Doc Link",
    __FILE__, getRefShapes, g);

  di.Add("XSetPairName", "XSetPairName Doc Joint Name",
    __FILE__, setName, g);

  di.Add("XGetPairName", "XGetPairName Doc Joint",
    __FILE__, getName, g);

  di.Add("XSetPairType", "XSetPairType Doc Joint Type[0..19]"
    "Values:\n"
    "\t  0 type is absent\n"
    "\t  1 FullyConstrained\n"
    "\t  2 Revolute\n"
    "\t  3 Prismatic\n"
    "\t  4 Cylindrical\n"
    "\t  5 Universal\n"
    "\t  6 Homokinetic\n"
    "\t  7 SphericalWithPin\n"
    "\t  8 Spherical\n"
    "\t  9 Planar\n"
    "\t 10 Unconstrained\n"
    "\t 11 Screw\n"
    "\t 12 RackAndPinion\n"
    "\t 13 Gear\n"
    "\t 14 LinearFlexibleAndPinion\n"
    "\t 15 PointOnSurface\n"
    "\t 16 SlidingSurface\n"
    "\t 17 RollingSurface\n"
    "\t 18 PointOnPlanarCurve\n"
    "\t 19 SlidingCurve\n"
    "\t 20 RollingCurve\n"
    __FILE__, setType, g);

  di.Add("XGetPairType", "XGetPairType Doc Joint",
    __FILE__, getType, g);

  di.Add("XSetPairTransformation", "XSetPairTransformation Doc Joint TrsfNb[1/2] Plane",
    __FILE__, setTrsf, g);

  di.Add("XGetPairTransformation", "XGetPairTransformation Doc Joint TrsfNb[1/2] PlaneName",
    __FILE__, getTrsf, g);

  di.Add("XSetPairOrientation", "XSetPairOrientation Doc Joint Orientation[0/1]",
    __FILE__, setOrientation, g);

  di.Add("XGetPairOrientation", "XGetPairOrientation Doc Joint",
    __FILE__, getOrientation, g);

  di.Add("XSetPairLimits", "XSetPairLimits Doc Joint Value1 Value2..."
    "Values:\n"
    "\t Low order pair - 12 values \n"
    "\t Low order pair with motion coupling - 2 values \n"
    "\t High order pair - 2 or 6 values\n"
    __FILE__, setLimits, g);

  di.Add("XGetPairLimits", "XGetPairLimits Doc Joint",
    __FILE__, getLimits, g);

  di.Add("XSetPairParams", "XSetPairParams Doc Joint Value1 Value2..."
    "\tScrew - Pitch\n"
    "\tRackAndPinion - PinionRadius\n"
    "\tGear -FirstLinkRadius SecondLinkRadius Bevel HelicalAngle GearRatio \n"
    __FILE__, setParameters, g);

  di.Add("XGetPairParams", "XGetPairParams Doc Joint",
    __FILE__, getParameters, g);

  di.Add("XSetPairGeomParam", "XSetPairGeomParam Doc Joint Number[1/2] Surface/Face/Curve/Edge",
    __FILE__, setGeomParam, g);

  di.Add("XGetPairGeomParam", "XGetPairGeomParam Doc Joint Number[1/2] Name",
    __FILE__, getGeomParam, g);

  di.Add("XAddMechanismState", "XAddMechanismState Doc Mechanism",
    __FILE__, addMechanismState, g);

  di.Add("XAddPairValues", "XAddPairValues Doc State [Joint -Key1 Values1 -KeyN ValuesN]"
    "\t-[first_/second_]rotation - current rotation - 1 number"
    "\t-[first_/second_]translation - current translation - 1 number"
    "\t-[first_/second_]point_on_surface - current (u,v) - 2 numbers"
    "\t-[first_/second_]point_on_curve - current (t) - 1 number"
    "\t-ypr - current yaw pitch roll angles - 3 numbers"
    "\t-trsf - for unconstrained only current location direction xdirection - 9 numbers",
    __FILE__, addValues, g);

  di.Add("XSetPairValues", "XSetPairValues Doc Value Joint -Key1 Values1 -KeyN ValuesN"
    "\t-[first_/second_]rotation - current rotation - 1 number"
    "\t-[first_/second_]translation - current translation - 1 number"
    "\t-[first_/second_]point_on_surface - current (u,v) - 2 numbers"
    "\t-[first_/second_]point_on_curve - current (t) - 1 number"
    "\t-ypr - current yaw pitch roll angles - 3 numbers"
    "\t-trsf - for unconstrained only current location direction xdirection - 9 numbers",
    __FILE__, setValues, g);

  di.Add("XGetPairValues", "XGetPairValues Doc Value -Key"
    "\t-[first_/second_]rotation - current rotation - 1 number"
    "\t-[first_/second_]translation - current translation - 1 number"
    "\t-[first_/second_]point_on_surface - current (u,v) - 2 numbers"
    "\t-[first_/second_]point_on_curve - current (t) - 1 number"
    "\t-ypr - current yaw pitch roll angles - 3 numbers"
    "\t-trsf - for unconstrained only current location direction xdirection - 9 numbers",
    __FILE__, getValues, g);
}
