// Created on : Sat May 02 12:41:15 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <RWStepKinematics_RWPlanarCurvePair.hxx>
#include <StepKinematics_PlanarCurvePair.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Curve.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//function : RWStepKinematics_RWPlanarCurvePair
//purpose  : 
//=======================================================================

RWStepKinematics_RWPlanarCurvePair::RWStepKinematics_RWPlanarCurvePair() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPlanarCurvePair::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                   const Standard_Integer num,
                                                   Handle(Interface_Check)& ach,
                                                   const Handle(StepKinematics_PlanarCurvePair)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,9,ach,"planar_curve_pair") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Inherited fields of ItemDefinedTransformation

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Name;
  data->ReadString (num, 2, "item_defined_transformation.name", ach, aItemDefinedTransformation_Name);

  Handle(TCollection_HAsciiString) aItemDefinedTransformation_Description;
  Standard_Boolean hasItemDefinedTransformation_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "item_defined_transformation.description", ach, aItemDefinedTransformation_Description);
  }
  else {
    hasItemDefinedTransformation_Description = Standard_False;
    aItemDefinedTransformation_Description.Nullify();
  }

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem1;
  data->ReadEntity (num, 4, "item_defined_transformation.transform_item1", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem1);

  Handle(StepRepr_RepresentationItem) aItemDefinedTransformation_TransformItem2;
  data->ReadEntity (num, 5, "item_defined_transformation.transform_item2", ach, STANDARD_TYPE(StepRepr_RepresentationItem), aItemDefinedTransformation_TransformItem2);

  // Inherited fields of KinematicPair

  Handle(StepKinematics_KinematicJoint) aKinematicPair_Joint;
  data->ReadEntity (num, 6, "kinematic_pair.joint", ach, STANDARD_TYPE(StepKinematics_KinematicJoint), aKinematicPair_Joint);

  // Own fields of PlanarCurvePair

  Handle(StepGeom_Curve) aCurve1;
  data->ReadEntity (num, 7, "curve1", ach, STANDARD_TYPE(StepGeom_Curve), aCurve1);

  Handle(StepGeom_Curve) aCurve2;
  data->ReadEntity (num, 8, "curve2", ach, STANDARD_TYPE(StepGeom_Curve), aCurve2);

  Standard_Boolean aOrientation;
  data->ReadBoolean (num, 9, "orientation", ach, aOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aCurve1,
            aCurve2,
            aOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPlanarCurvePair::WriteStep (StepData_StepWriter& SW,
                                                    const Handle(StepKinematics_PlanarCurvePair)& ent) const
{

  // Own fields of RepresentationItem

  SW.Send (ent->Name());

  // Inherited fields of ItemDefinedTransformation

  SW.Send (ent->ItemDefinedTransformation()->Name());

  if ( ent->ItemDefinedTransformation()->HasDescription() ) {
    SW.Send (ent->ItemDefinedTransformation()->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->ItemDefinedTransformation()->TransformItem1());

  SW.Send (ent->ItemDefinedTransformation()->TransformItem2());

  // Own fields of KinematicPair

  SW.Send (ent->Joint());

  // Own fields of PlanarCurvePair

  SW.Send (ent->Curve1());

  SW.Send (ent->Curve2());

  SW.SendBoolean (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWPlanarCurvePair::Share (const Handle(StepKinematics_PlanarCurvePair)& ent,
                                                Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Own fields of PlanarCurvePair

  iter.AddItem (ent->Curve1());

  iter.AddItem (ent->Curve2());
}
