// Created on: 2020-03-16
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_KinematicTool_HeaderFile
#define _XCAFDoc_KinematicTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <TDF_LabelSequence.hxx>
#include <Standard_Integer.hxx>

class XCAFDoc_ShapeTool;
class TDF_Label;
class Standard_GUID;
class TDF_Attribute;


class XCAFDoc_KinematicTool;
DEFINE_STANDARD_HANDLE(XCAFDoc_KinematicTool, TDF_Attribute)

//! Provides tools to store and retrieve Kinematics in and from TDocStd_Document.
//! Kinematics data storage consists of several mechanisms,
//! each mechanish has its ows sets of links and joints.
class XCAFDoc_KinematicTool : public TDF_Attribute
{

public:

  
  Standard_EXPORT XCAFDoc_KinematicTool();
  
  //! Creates (if not exist) KinematicTool
  Standard_EXPORT static Handle(XCAFDoc_KinematicTool) Set(const TDF_Label& theLabel);
  
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Returns the label under which Kinematics is stored
  Standard_EXPORT TDF_Label BaseLabel() const;

  //! Creates new mechanism - a child under BaseLabel
  Standard_EXPORT TDF_Label AddMechanism();

  //! Checks is the given label a mechanism
  Standard_EXPORT Standard_Boolean IsMechanism(const TDF_Label& theLabel) const;

  //! Removes given mechanism with all underlying information
  Standard_EXPORT void RemoveMechanism(const TDF_Label& theLabel);

  //! Adds new empty label for link.
  //! \param[in] theMechanism parent mechanism
  //! \param[in] isBase link is base of mechanism
  //! \return created link label
  Standard_EXPORT TDF_Label AddLink(const TDF_Label& theMechanism,
                                    const Standard_Boolean& isBase = Standard_False);

  //! Adds new label with link.
  //! \param[in] theMechanism parent mechanism
  //! \param[in] theShapes all details of link
  //! \param[in] isBase link is base of mechanism
  //! \return created link label
  Standard_EXPORT TDF_Label AddLink(const TDF_Label& theMechanism,
                                    const TDF_LabelSequence& theShapes,
                                    const Standard_Boolean& isBase = Standard_False);

  //! Adds new label with link.
  //! \param[in] theMechanism parent mechanism
  //! \param[in] theShape singular detail of link
  //! \param[in] isBase link is base of mechanism
  //! \return created link label
  Standard_EXPORT TDF_Label AddLink(const TDF_Label& theMechanism,
                                    const TDF_Label& theShape,
                                    const Standard_Boolean& isBase = Standard_False);

  //! Checks is the given label a link
  //! \return the result of check
  Standard_EXPORT Standard_Boolean IsLink(const TDF_Label& theLink) const;

  //! Sets shapes references for link
  //! \param[in] theLink link to set shapes
  //! \param[in] theShapes all details of link
  //! \return true in case of success, false otherwise
  Standard_EXPORT Standard_Boolean SetLink(const TDF_Label& theLink,
                                           const TDF_LabelSequence& theShapes);

  //! Sets shapes references for link
  //! \param[in] theLink link to set base
  //! \return true in case of success, false otherwise
  Standard_EXPORT Standard_Boolean SetBaseLink(const TDF_Label& theLink);

  //! Sets shape reference for link
  //! \param[in] theLink link to set shape
  //! \param[in] theShape singular detail of link
  //! \return true in case of success, false otherwise
  Standard_EXPORT Standard_Boolean SetLink(const TDF_Label& theLink,
                                           const TDF_Label& theShape);

  //! Removes the given link
  //! \param[in] theLink link to remove
  Standard_EXPORT void RemoveLink(const TDF_Label& theLink);

  //! Adds new empty label for joint.
  //! \param[in] theMechanism parent mechanism
  //! \return created joint label
  Standard_EXPORT TDF_Label AddJoint(const TDF_Label& theMechanism);

  //! Adds new label with joint.
  //! \param[in] theMechanism parent mechanism
  //! \param[in] theLink1 first link
  //! \param[in] theLink2 second link
  //! \return created joint label
  Standard_EXPORT TDF_Label AddJoint(const TDF_Label& theMechanism,
                                     const TDF_Label& theLink1,
                                     const TDF_Label& theLink2);

  //! Checks is the given label a joint
  //! \return the result of check
  Standard_EXPORT Standard_Boolean IsJoint(const TDF_Label& theJoint) const;

  //! Sets references to links to joint.
  //! \param[in] theJoint joint to set
  //! \param[in] theLink1 first link of joint
  //! \param[in] theLink2 second link of joint
  //! \return true in case of success, false otherwise
  Standard_EXPORT Standard_Boolean SetJoint(const TDF_Label& theJoint,
                                            const TDF_Label& theLink1,
                                            const TDF_Label& theLink2);

  //! Removes the given joint
  //! \param[in] thejoint joint to remove
  Standard_EXPORT void RemoveJoint(const TDF_Label& theJoint);

  //! Checks is the given label a Value
  //! \return the result of check
  Standard_EXPORT Standard_Boolean IsValue(const TDF_Label& theValue) const;

  //! Adds new empty label for State.
  //! \param[in] theMechanism parent mechanism
  //! \return created state label
  Standard_EXPORT TDF_Label AddState(const TDF_Label& theMechanism);

  // Retrieves all States labels of the given mechanism
  //! \param[in] theMechanism parent mechanism
  //! \return state set
  Standard_EXPORT TDF_LabelSequence GetStates(const TDF_Label& theMechanism) const;

  // Retrieves all values labels of the given state
  //! \param[in] theState parent state
  //! \return set of values
  Standard_EXPORT TDF_LabelSequence GetValuesOfState(const TDF_Label& theState) const;

  //! Gets references to state of the given value.
  //! \param[in] theValue value to get
  //! \return state
  Standard_EXPORT TDF_Label GetStateOfValue(const TDF_Label& theValue) const;

  //! Gets references to joints of the given value.
  //! \param[in] theValue value to get
  //! \return a joint
  Standard_EXPORT TDF_Label GetJointOfValue(const TDF_Label& theValue) const;

  //! Gets references to links of the given joint.
  //! \param[in] theJoint joint to get
  //! \return set of values
  Standard_EXPORT TDF_LabelSequence GetValuesOfJoint(const TDF_Label& theJoint) const;

  //! Removes the given state
  //! \param[in] theState state to remove
  Standard_EXPORT void RemoveState(const TDF_Label& theState);
  
  //! Retrieves all mechanisms labels
  //! \return mechanisms set
  Standard_EXPORT TDF_LabelSequence GetMechanisms() const;

  //! Retrieves all links labels of the given mechanism
  //! \param[in] theMechanism parent mechanism
  //! \return links set
  Standard_EXPORT TDF_LabelSequence GetLinks(const TDF_Label& theMechanism) const;

  // Retrieves all joints labels of the given mechanism
  //! \param[in] theMechanism parent mechanism
  //! \return joints set
  Standard_EXPORT TDF_LabelSequence GetJoints(const TDF_Label& theMechanism) const;

  //! Gets references to links of the given joint.
  //! \param[in] theJoint joint to get
  //! \param[out] theLink1 first link of joint
  //! \param[out] theLink2 second link of joint
  Standard_EXPORT void GetLinksOfJoint(const TDF_Label& theJoint,
                                       TDF_Label& theLink1,
                                       TDF_Label& theLink2) const;

  //! Gets references to joints of the given link.
  //! \param[in] theLink link to get
  //! \return set of joints
  Standard_EXPORT TDF_LabelSequence GetJointsOfLink(const TDF_Label& theLink) const;

  //! Gets reference shapes of the given link.
  //! \param[in] theLink link to get
  //! \return set of shapes
  Standard_EXPORT TDF_LabelSequence GetRefShapes(const TDF_Label& theLink) const;

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  private:
  
  Standard_EXPORT TDF_Label getRootOfLinks(const TDF_Label& theMechanism) const;
  
  Standard_EXPORT TDF_Label getRootOfJoints(const TDF_Label& theMechanism) const;

  Standard_EXPORT TDF_Label getRootOfStates(const TDF_Label& theMechanism) const;

  DEFINE_STANDARD_RTTIEXT(XCAFDoc_KinematicTool, TDF_Attribute)

};
#endif // _XCAFDoc_KinematicTool_HeaderFile
