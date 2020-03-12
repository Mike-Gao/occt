// Created on: 2020-03-16
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

#include <XCAFDoc_KinematicTool.hxx>

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Integer.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_KinematicPair.hxx>
#include <XCAFDoc_KinematicPairValue.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_KinematicTool, TDF_Attribute)

//=======================================================================
//function : XCAFDoc_KinematicTool
//purpose  : 
//=======================================================================
XCAFDoc_KinematicTool::XCAFDoc_KinematicTool()
{
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_KinematicTool) XCAFDoc_KinematicTool::Set(const TDF_Label& theLabel) 
{
  Handle(XCAFDoc_KinematicTool) anAttr;
  if (!theLabel.FindAttribute(XCAFDoc_KinematicTool::GetID(), anAttr)) {
    anAttr = new XCAFDoc_KinematicTool();
    theLabel.AddAttribute(anAttr);
  }
  return anAttr;
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicTool::GetID() 
{
  static Standard_GUID KinematicToolID("322D8F8D-8E93-4F6A-A98B-CCC2EBBCC34D");
  return KinematicToolID; 
}


//=======================================================================
//function : BaseLabel
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::BaseLabel() const
{
  return Label();
}

//=======================================================================
//function : AddMechanism
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddMechanism()
{
  TDF_TagSource aTag;
  TDF_Label aMechanismL = aTag.NewChild(Label());
  TCollection_AsciiString aName = "Mechanism ";
  aName.AssignCat(TCollection_AsciiString(Label().NbChildren()));
  TDataStd_Name::Set(aMechanismL, aName);
  TDF_Label aRootOfLinks = aMechanismL.FindChild(1);
  TDataStd_Name::Set(aRootOfLinks, "Links");
  TDF_Label aRootOfJoints = aMechanismL.FindChild(2);
  TDataStd_Name::Set(aRootOfJoints, "Joints");
  return aMechanismL;
}

//=======================================================================
//function : IsMechanism
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::IsMechanism(const TDF_Label& theLabel) const
{
  if (!Label().IsEqual(theLabel.Father()))
    return Standard_False;

  return (theLabel.NbChildren() == 2 || theLabel.NbChildren() == 3);
}

//=======================================================================
//function : RemoveMechanism
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::RemoveMechanism(const TDF_Label& theLabel)
{
  if (!IsMechanism(theLabel))
    return;

  for (TDF_ChildIterator aLinkIt(getRootOfLinks(theLabel)); aLinkIt.More(); aLinkIt.Next())
    RemoveLink(aLinkIt.Value());
  for (TDF_ChildIterator aJointIt(getRootOfJoints(theLabel)); aJointIt.More(); aJointIt.Next())
    RemoveJoint(aJointIt.Value());
  if (!getRootOfStates(theLabel).IsNull())
    for (TDF_ChildIterator aStateIt(getRootOfStates(theLabel)); aStateIt.More(); aStateIt.Next())
      RemoveState(aStateIt.Value());

  theLabel.ForgetAllAttributes();
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddLink(const TDF_Label& theMechanism,
                                         const Standard_Boolean& isBase)
{
  if (!IsMechanism(theMechanism))
    return TDF_Label();
  TDF_TagSource aTag;
  TDF_Label aLink = aTag.NewChild(getRootOfLinks(theMechanism));
  if (isBase)
  {
    SetBaseLink(aLink);
  }
  else
  {
    TCollection_AsciiString aName = "Link ";
    aName.AssignCat(TCollection_AsciiString(aLink.Tag()));
    TDataStd_Name::Set(aLink, aName);
  }
  return aLink;
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddLink(const TDF_Label& theMechanism,
                                         const TDF_LabelSequence& theShapes,
                                         const Standard_Boolean& isBase)
{
  TDF_Label aNewLink = AddLink(theMechanism, isBase);
  if (aNewLink.IsNull() || theShapes.Length() == 0)
    return aNewLink;

  SetLink(aNewLink, theShapes);
  return aNewLink;
}

//=======================================================================
//function : AddLink
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddLink(const TDF_Label& theMechanism,
                                         const TDF_Label& theShape,
                                         const Standard_Boolean& isBase)
{
  TDF_LabelSequence anAuxSequence;
  anAuxSequence.Append(theShape);
  return AddLink(theMechanism, anAuxSequence, isBase);
}

//=======================================================================
//function : SetLink
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::SetLink(const TDF_Label& theLink,
                                                const TDF_LabelSequence& theShapes)
{
  if (theShapes.Length() == 0)
    return Standard_False;

  Handle(XCAFDoc_GraphNode) aLinkGNode;
  Handle(XCAFDoc_GraphNode) aShapesGNode;

  // Clear link from old references to shapes
  if (theLink.FindAttribute(XCAFDoc::KinematicRefShapeGUID(), aLinkGNode)) {
    while (aLinkGNode->NbFathers() > 0) {
      aShapesGNode = aLinkGNode->GetFather(1);
      aShapesGNode->UnSetChild(aLinkGNode);
      if (aShapesGNode->NbChildren() == 0)
        aShapesGNode->ForgetAttribute(XCAFDoc::KinematicRefShapeGUID());
    }
    theLink.ForgetAttribute(XCAFDoc::KinematicRefShapeGUID());
  }

  // Create new references
  aLinkGNode = new XCAFDoc_GraphNode;
  aLinkGNode = XCAFDoc_GraphNode::Set(theLink);
  aLinkGNode->SetGraphID(XCAFDoc::KinematicRefShapeGUID());

  for (Standard_Integer i = theShapes.Lower(); i <= theShapes.Upper(); i++) {
    if (!theShapes.Value(i).FindAttribute(XCAFDoc::KinematicRefShapeGUID(), aShapesGNode)) {
      aShapesGNode = new XCAFDoc_GraphNode;
      aShapesGNode = XCAFDoc_GraphNode::Set(theShapes.Value(i));
      aShapesGNode->SetGraphID(XCAFDoc::KinematicRefShapeGUID());
    }
    aShapesGNode->SetChild(aLinkGNode);
    aLinkGNode->SetFather(aShapesGNode);
  }

  return Standard_True;
}

//=======================================================================
//function : SetBaseLink
//purpose  : 
//=======================================================================
Standard_EXPORT Standard_Boolean XCAFDoc_KinematicTool::SetBaseLink(const TDF_Label& theLink)
{
  TDF_Label aMechanism = theLink.Father().Father();
  if (!IsMechanism(aMechanism))
    return Standard_False;
  TDF_LabelSequence aLinks = GetLinks(aMechanism);
  for (Standard_Integer anInd = 1; anInd <= aLinks.Length(); ++anInd)
  {
    Handle(TDataStd_Integer) aBase;
    if (aLinks(anInd).FindAttribute(TDataStd_Integer::GetID(), aBase))
    {
      aLinks(anInd).ForgetAttribute(aBase);
      Handle(TDataStd_Name) aLinkName;
      if (aLinks(anInd).FindAttribute(TDataStd_Name::GetID(), aLinkName) && !aLinkName->Get().IsDifferent("Base"))
      {
        TCollection_AsciiString aName = "Link ";
        aName.AssignCat(TCollection_AsciiString(aLinks(anInd).Tag()));
        TDataStd_Name::Set(aLinks(anInd), aName);
      }
    }
  }
  TDataStd_Name::Set(theLink, "Base");
  TDataStd_Integer::Set(theLink, 1);
}

//=======================================================================
//function : SetLink
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::SetLink(const TDF_Label& theLink,
                                                const TDF_Label& theShape)
{
  TDF_LabelSequence anAuxSequence;
  anAuxSequence.Append(theShape);
  return SetLink(theLink, anAuxSequence);
}

//=======================================================================
//function : IsLink
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::IsLink(const TDF_Label& theLink) const
{
  Handle(XCAFDoc_GraphNode) aLinkGNode;
  return theLink.FindAttribute(XCAFDoc::KinematicRefShapeGUID(), aLinkGNode);
}

//=======================================================================
//function : RemoveLink
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::RemoveLink(const TDF_Label& theLink)
{
  if (!IsLink(theLink))
    return;

  Handle(XCAFDoc_GraphNode) aLinkGNode;
  Handle(XCAFDoc_GraphNode) aShapesGNode;

  // Remove all referenced joints
  TDF_LabelSequence aRefJoints = GetJointsOfLink(theLink);
  for (TDF_LabelSequence::Iterator anIt(aRefJoints); anIt.More(); anIt.Next())
    RemoveJoint(anIt.Value());

  // Clear link from old references to shapes
  if (theLink.FindAttribute(XCAFDoc::KinematicRefShapeGUID(), aLinkGNode)) {
    while (aLinkGNode->NbFathers() > 0) {
      aShapesGNode = aLinkGNode->GetFather(1);
      aShapesGNode->UnSetChild(aLinkGNode);
      if (aShapesGNode->NbChildren() == 0)
        aShapesGNode->ForgetAttribute(XCAFDoc::KinematicRefShapeGUID());
    }
  }
  theLink.ForgetAllAttributes();
}

//=======================================================================
//function : AddJoint
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddJoint(const TDF_Label& theMechanism)
{
  if (!IsMechanism(theMechanism))
    return TDF_Label();

  TDF_TagSource aTag;
  TDF_Label aJoint = aTag.NewChild(getRootOfJoints(theMechanism));
  Handle(XCAFDoc_KinematicPair) aPair = XCAFDoc_KinematicPair::Set(aJoint);
  TCollection_AsciiString aName = "Joint ";
  aName.AssignCat(TCollection_AsciiString(aJoint.Tag()));
  TDataStd_Name::Set(aJoint, aName);
  return aJoint;
}

//=======================================================================
//function : AddJoint
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddJoint(const TDF_Label& theMechanism,
                                          const TDF_Label& theLink1,
                                          const TDF_Label& theLink2)
{
  TDF_Label aNewJoint = AddJoint(theMechanism);
  if (aNewJoint.IsNull())
    return TDF_Label();

  SetJoint(aNewJoint, theLink1, theLink2);
  return aNewJoint;
}

//=======================================================================
//function : IsJoint
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::IsJoint(const TDF_Label& theJoint) const
{
  Handle(TDataStd_TreeNode) anAuxAttr;
  return (theJoint.FindAttribute(XCAFDoc::KinematicRefLink1GUID(), anAuxAttr) &&
          theJoint.FindAttribute(XCAFDoc::KinematicRefLink2GUID(), anAuxAttr));
}

//=======================================================================
//function : SetJoint
//purpose  : 
//=======================================================================
Standard_Boolean XCAFDoc_KinematicTool::SetJoint(const TDF_Label& theJoint,
                                                 const TDF_Label& theLink1,
                                                 const TDF_Label& theLink2)
{
  if (!IsLink(theLink1) || !IsLink(theLink2))
    return Standard_False;


  Handle(TDataStd_TreeNode) aJointNode, aLinkNode;
  // Add reference to first link
  aJointNode = TDataStd_TreeNode::Set(theJoint, XCAFDoc::KinematicRefLink1GUID());
  aLinkNode = TDataStd_TreeNode::Set(theLink1, XCAFDoc::KinematicRefLink1GUID());
  aJointNode->Remove(); // fix against bug in TreeNode::Append()
  aLinkNode->Append(aJointNode);

  // Add reference to second link
  aJointNode = TDataStd_TreeNode::Set(theJoint, XCAFDoc::KinematicRefLink2GUID());
  aLinkNode = TDataStd_TreeNode::Set(theLink2, XCAFDoc::KinematicRefLink2GUID());
  aJointNode->Remove(); // fix against bug in TreeNode::Append()
  aLinkNode->Append(aJointNode);

  TCollection_AsciiString aName = "Joint ";
  aName.AssignCat(TCollection_AsciiString(theJoint.Tag()));
  TDataStd_Name::Set(theJoint, aName);
  return Standard_True;
}

//=======================================================================
//function : RemoveJoint
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::RemoveJoint(const TDF_Label& theJoint)
{
  if (!IsJoint(theJoint))
    return;

  Handle(TDataStd_TreeNode) aNode;
  theJoint.FindAttribute(XCAFDoc::KinematicRefLink1GUID(), aNode);
  aNode->Remove();
  theJoint.FindAttribute(XCAFDoc::KinematicRefLink2GUID(), aNode);
  aNode->Remove();

  theJoint.ForgetAllAttributes();

}

Standard_Boolean XCAFDoc_KinematicTool::IsValue(const TDF_Label& theValue) const
{
  Handle(XCAFDoc_KinematicPairValue) anAttr;
  return theValue.FindAttribute(XCAFDoc_KinematicPairValue::GetID(), anAttr);
}

//=======================================================================
//function : GetMechanisms
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetMechanisms() const
{
  TDF_LabelSequence aMechArray;
  for (TDF_ChildIterator anIt(Label()); anIt.More(); anIt.Next()) {
    TDF_Label aMechanism = anIt.Value();
    if (IsMechanism(aMechanism))
      aMechArray.Append(aMechanism);
  }

  return aMechArray;
}

//=======================================================================
//function : GetLinks
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetLinks(const TDF_Label& theMechanism) const
{
  TDF_LabelSequence aLinkArray;
  if (!IsMechanism(theMechanism))
    return aLinkArray;

  for (TDF_ChildIterator anIt(getRootOfLinks(theMechanism)); anIt.More(); anIt.Next()) {
    TDF_Label aLink = anIt.Value();
    if (IsLink(aLink))
      aLinkArray.Append(aLink);
  }

  return aLinkArray;
}

//=======================================================================
//function : GetJoints
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetJoints(const TDF_Label& theMechanism) const
{
  TDF_LabelSequence aJointArray;
  if (!IsMechanism(theMechanism))
    return aJointArray;

  for (TDF_ChildIterator anIt(getRootOfJoints(theMechanism)); anIt.More(); anIt.Next()) {
    TDF_Label aJoint = anIt.Value();
    if (IsJoint(aJoint))
      aJointArray.Append(aJoint);
  }

  return aJointArray;
}

//=======================================================================
//function : GetLinksOfJoint
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::GetLinksOfJoint(const TDF_Label& theJoint,
                                            TDF_Label& theLink1,
                                            TDF_Label& theLink2) const
{
  if (!IsJoint(theJoint))
    return;

  Handle(TDataStd_TreeNode) aNode;

  if (theJoint.FindAttribute(XCAFDoc::KinematicRefLink1GUID(), aNode))
    theLink1 = aNode->Father()->Label();

  if (theJoint.FindAttribute(XCAFDoc::KinematicRefLink2GUID(), aNode))
    theLink2 = aNode->Father()->Label();
}

//=======================================================================
//function : GetJointsOfLink
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetJointsOfLink(const TDF_Label& theLink,
                                                         const Standard_Boolean& toFirst,
                                                         const Standard_Boolean& toSecond) const
{
  TDF_LabelSequence aJointsArray;
  if (!IsLink(theLink))
    return aJointsArray;

  Handle(TDataStd_TreeNode) aNode, aChildNode;

  if (toFirst && theLink.FindAttribute(XCAFDoc::KinematicRefLink1GUID(), aNode)) {
    aChildNode = aNode->First();
    while (!aChildNode.IsNull()) {
      aJointsArray.Append(aChildNode->Label());
      aChildNode = aChildNode->Next();
    }
  }

  if (toSecond && theLink.FindAttribute(XCAFDoc::KinematicRefLink2GUID(), aNode)) {
    aChildNode = aNode->First();
    while (!aChildNode.IsNull()) {
      aJointsArray.Append(aChildNode->Label());
      aChildNode = aChildNode->Next();
    }
  }

  return aJointsArray;
}

//=======================================================================
//function : AddState
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddState(const TDF_Label& theMechanism)
{
  if (!IsMechanism(theMechanism))
    return TDF_Label();

  TDF_Label aRootOfState = getRootOfStates(theMechanism);
  if (aRootOfState.IsNull())
  {
    aRootOfState = theMechanism.FindChild(3);
    TDataStd_Name::Set(aRootOfState, "States");
  }
  TDF_TagSource aTag;
  TDF_Label aState = aTag.NewChild(aRootOfState);
  TCollection_AsciiString aName = "State ";
  aName.AssignCat(TCollection_AsciiString(aState.Tag()));
  TDataStd_Name::Set(aState, aName);
  return aState;
}

//=======================================================================
//function : AddValue
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::AddValue(const TDF_Label& theState)
{
  if(!IsMechanism(theState.Father().Father()) &&
     theState.Father().Tag() != 3)
    return TDF_Label();

  return theState.NewChild();
}

//=======================================================================
//function : RemoveValue
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::RemoveValue(const TDF_Label& theValue)
{
  if (!IsValue(theValue))
    return;

  Handle(TDataStd_TreeNode) aNode;
  theValue.FindAttribute(XCAFDoc::KinematicRefJointGUID(), aNode);
  aNode->Remove();


  theValue.ForgetAllAttributes();
}

//=======================================================================
//function : GetStates
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetStates(const TDF_Label& theMechanism) const
{
  TDF_LabelSequence aStatesArray;
  if (!IsMechanism(theMechanism))
    return aStatesArray;

  TDF_Label aRootOfState = getRootOfStates(theMechanism);
  if (!IsMechanism(theMechanism) || aRootOfState.IsNull())
    return aStatesArray;

  for (TDF_ChildIterator anIt(aRootOfState); anIt.More(); anIt.Next()) {
    TDF_Label aState = anIt.Value();
    if(!GetValuesOfState(aState).IsEmpty())
      aStatesArray.Append(aState);
  }
  return aStatesArray;
}

//=======================================================================
//function : GetStateOfValue
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::GetStateOfValue(const TDF_Label& theValue) const
{
  TDF_Label aState;
  if (!IsValue(theValue))
    return aState;
  aState = theValue.Father();
  return aState;
}

//=======================================================================
//function : GetValuesOfState
//purpose  : 
//=======================================================================
Standard_EXPORT TDF_LabelSequence XCAFDoc_KinematicTool::GetValuesOfState(const TDF_Label& theState) const
{
  TDF_LabelSequence aValueArray;
  if (!IsMechanism(theState.Father().Father()) || theState.Father().Tag() != 3)
    return aValueArray;

  for (TDF_ChildIterator anIt(theState); anIt.More(); anIt.Next()) {
    TDF_Label aValue = anIt.Value();
    if (IsValue(aValue))
      aValueArray.Append(aValue);
  }

  return aValueArray;
}

//=======================================================================
//function : GetJointOfValue
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::GetJointOfValue(const TDF_Label& theValue) const
{
  TDF_Label aJoint;
  if (!IsValue(theValue))
    return aJoint;

  Handle(TDataStd_TreeNode) aNode, aChildNode;

  if (theValue.FindAttribute(XCAFDoc::KinematicRefJointGUID(), aNode))
    aJoint = aNode->Father()->Label();
  return aJoint;
}

//=======================================================================
//function : GetValuesOfJoint
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetValuesOfJoint(const TDF_Label& theJoint) const
{
  TDF_LabelSequence aValuesArray;
  if (!IsJoint(theJoint))
    return aValuesArray;

  Handle(TDataStd_TreeNode) aNode, aChildNode;

  if (theJoint.FindAttribute(XCAFDoc::KinematicRefJointGUID(), aNode)) {
    aChildNode = aNode->First();
    while (!aChildNode.IsNull()) {
      if (IsValue(aChildNode->Label()))
        aValuesArray.Append(aChildNode->Label());
      aChildNode = aChildNode->Next();
    }
  }
  return aValuesArray;
}

//=======================================================================
//function : RemoveState
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::RemoveState(const TDF_Label& theState)
{
  TDF_Label aMechanism = theState.Father().Father();
  if (!IsMechanism(aMechanism) || theState.Father().Tag() != 3)
    return;
  Handle(TDataStd_TreeNode) aNode;
  TDF_LabelSequence aSeqOfValues = GetValuesOfState(theState);
  for (Standard_Integer anInd = 1; anInd <= aSeqOfValues.Length(); ++anInd)
    RemoveValue(aSeqOfValues.Value(anInd));
  theState.ForgetAllAttributes();
}

//=======================================================================
//function : GetRefShapes
//purpose  : 
//=======================================================================
TDF_LabelSequence XCAFDoc_KinematicTool::GetRefShapes(const TDF_Label& theLink) const
{
  Handle(XCAFDoc_GraphNode) aGNode;
  TDF_LabelSequence aShapeArray;
  if (theLink.FindAttribute(XCAFDoc::KinematicRefShapeGUID(), aGNode) && aGNode->NbFathers() > 0) {
    for(Standard_Integer i = 1; i <= aGNode->NbFathers(); i++)
      aShapeArray.Append(aGNode->GetFather(i)->Label());
  }
  return aShapeArray;
}

//=======================================================================
//function : getRootOfLinks
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::getRootOfLinks(const TDF_Label& theMechanism) const
{
  return theMechanism.FindChild(1, Standard_False);
}

//=======================================================================
//function : getRootOfJoints
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::getRootOfJoints(const TDF_Label& theMechanism) const
{
  return theMechanism.FindChild(2, Standard_False);
}

//=======================================================================
//function : getRootOfStates
//purpose  : 
//=======================================================================
TDF_Label XCAFDoc_KinematicTool::getRootOfStates(const TDF_Label& theMechanism) const
{
  return theMechanism.FindChild(3, Standard_False);
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& XCAFDoc_KinematicTool::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::Restore(const Handle(TDF_Attribute)& /*with*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XCAFDoc_KinematicTool::NewEmpty() const
{
  return new XCAFDoc_KinematicTool;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::Paste(const Handle(TDF_Attribute)& /*into*/,
                             const Handle(TDF_RelocationTable)& /*RT*/) const
{
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void XCAFDoc_KinematicTool::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, TDF_Attribute)
}
