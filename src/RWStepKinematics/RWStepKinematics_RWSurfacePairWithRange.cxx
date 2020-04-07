// Created on : Mon Apr 13 15:22:03 2020 
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
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
#include <RWStepKinematics_RWSurfacePairWithRange.hxx>
#include <StepKinematics_SurfacePairWithRange.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepKinematics_KinematicJoint.hxx>
#include <StepGeom_Surface.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <Standard_Real.hxx>

//=======================================================================
//function : RWStepKinematics_RWSurfacePairWithRange
//purpose  : 
//=======================================================================

RWStepKinematics_RWSurfacePairWithRange::RWStepKinematics_RWSurfacePairWithRange() {}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSurfacePairWithRange::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                        const Standard_Integer num,
                                                        Handle(Interface_Check)& ach,
                                                        const Handle(StepKinematics_SurfacePairWithRange)& ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,13,ach,"surface_pair_with_range") ) return;

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

  // Inherited fields of SurfacePair

  Handle(StepGeom_Surface) aSurfacePair_Surface1;
  data->ReadEntity (num, 7, "surface_pair.surface1", ach, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface1);

  Handle(StepGeom_Surface) aSurfacePair_Surface2;
  data->ReadEntity (num, 8, "surface_pair.surface2", ach, STANDARD_TYPE(StepGeom_Surface), aSurfacePair_Surface2);

  Standard_Boolean aSurfacePair_Orientation;
  data->ReadBoolean (num, 9, "surface_pair.orientation", ach, aSurfacePair_Orientation);

  // Own fields of SurfacePairWithRange

  Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface1;
  data->ReadEntity (num, 10, "range_on_surface1", ach, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface1);

  Handle(StepGeom_RectangularTrimmedSurface) aRangeOnSurface2;
  data->ReadEntity (num, 11, "range_on_surface2", ach, STANDARD_TYPE(StepGeom_RectangularTrimmedSurface), aRangeOnSurface2);

  Standard_Real aLowerLimitActualRotation;
  Standard_Boolean hasLowerLimitActualRotation = Standard_True;
  if ( data->IsParamDefined (num,12) ) {
    data->ReadReal (num, 12, "lower_limit_actual_rotation", ach, aLowerLimitActualRotation);
  }
  else {
    hasLowerLimitActualRotation = Standard_False;
    aLowerLimitActualRotation = 0;
  }

  Standard_Real aUpperLimitActualRotation;
  Standard_Boolean hasUpperLimitActualRotation = Standard_True;
  if ( data->IsParamDefined (num,13) ) {
    data->ReadReal (num, 13, "upper_limit_actual_rotation", ach, aUpperLimitActualRotation);
  }
  else {
    hasUpperLimitActualRotation = Standard_False;
    aUpperLimitActualRotation = 0;
  }

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aItemDefinedTransformation_Name,
            hasItemDefinedTransformation_Description,
            aItemDefinedTransformation_Description,
            aItemDefinedTransformation_TransformItem1,
            aItemDefinedTransformation_TransformItem2,
            aKinematicPair_Joint,
            aSurfacePair_Surface1,
            aSurfacePair_Surface2,
            aSurfacePair_Orientation,
            aRangeOnSurface1,
            aRangeOnSurface2,
            hasLowerLimitActualRotation,
            aLowerLimitActualRotation,
            hasUpperLimitActualRotation,
            aUpperLimitActualRotation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSurfacePairWithRange::WriteStep (StepData_StepWriter& SW,
                                                         const Handle(StepKinematics_SurfacePairWithRange)& ent) const
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

  // Own fields of SurfacePair

  SW.Send (ent->Surface1());

  SW.Send (ent->Surface2());

  SW.SendBoolean (ent->Orientation());

  // Own fields of SurfacePairWithRange

  SW.Send (ent->RangeOnSurface1());

  SW.Send (ent->RangeOnSurface2());

  if ( ent->HasLowerLimitActualRotation() ) {
    SW.Send (ent->LowerLimitActualRotation());
  }
  else SW.SendUndef();

  if ( ent->HasUpperLimitActualRotation() ) {
    SW.Send (ent->UpperLimitActualRotation());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepKinematics_RWSurfacePairWithRange::Share (const Handle(StepKinematics_SurfacePairWithRange)& ent,
                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of ItemDefinedTransformation

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem1());

  iter.AddItem (ent->ItemDefinedTransformation()->TransformItem2());

  // Inherited fields of KinematicPair

  iter.AddItem (ent->StepKinematics_KinematicPair::Joint());

  // Inherited fields of SurfacePair

  iter.AddItem (ent->StepKinematics_SurfacePair::Surface1());

  iter.AddItem (ent->StepKinematics_SurfacePair::Surface2());

  // Own fields of SurfacePairWithRange

  iter.AddItem (ent->RangeOnSurface1());

  iter.AddItem (ent->RangeOnSurface2());
}
