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

#include "Viewer3dSamples.h"
#include "MakeBottle.h"

#include <AIS_Shape.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_AmbientLight.hxx>


void Viewer3dSamples::ExecuteSample(TCollection_AsciiString theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "SpotLight3dSample")
    SpotLight3dSample();
  else if (theSampleName == "PositionalLight3dSample")
    PositionalLight3dSample();
  else if (theSampleName == "DirectionalLight3dSample")
    DirectionalLight3dSample();
  else if (theSampleName == "AmbientLight3dSample")
    AmbientLight3dSample();
  else if (theSampleName == "ClearLight3dSample")
    ClearLight3dSample();
  else if (theSampleName == "VerticesSelect3dSample")
    VerticesSelect3dSample();
  else if (theSampleName == "EdgesSelect3dSample")
    EdgesSelect3dSample();
  else if (theSampleName == "FacesSelect3dSample")
    FacesSelect3dSample();
  else if (theSampleName == "NeutralPointSelect3dSample")
    NeutralPointSelect3dSample();
  else if (theSampleName == "WireFramePresentation3dSample")
    WireFramePresentation3dSample();
  else if (theSampleName == "ShadingPresentation3dSample")
    ShadingPresentation3dSample();
  else if (theSampleName == "RedColorPresentation3dSample")
    RedColorPresentation3dSample();
  else if (theSampleName == "GrayColorPresentation3dSample")
    GrayColorPresentation3dSample();
  else if (theSampleName == "PlasticPresentation3dSample")
    PlasticPresentation3dSample();
  else if (theSampleName == "BronzePresentation3dSample")
    BronzePresentation3dSample();
  else if (theSampleName == "OpaquePresentation3dSample")
    OpaquePresentation3dSample();
  else if (theSampleName == "HalfTransparencyPresentation3dSample")
    HalfTransparencyPresentation3dSample();
  else if (theSampleName == "VboOn3dSample")
    VboOn3dSample();
  else if (theSampleName == "VboOff3dSample")
    VboOff3dSample();
  else {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void Viewer3dSamples::AppendBottle()
{
  TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
  Handle(AIS_InteractiveObject) aShape = new AIS_Shape(aBottle);
  myObject3d.Append(aShape);
  myResult << "A bottle shape was created." << std::endl;
}

void Viewer3dSamples::SpotLight3dSample()
{
  // Spot light source creation
  Handle(V3d_SpotLight) aSpotLight =
    new V3d_SpotLight(gp_Pnt(100.0, 0.0, 0.0), gp_Dir(-1.0, 0.0, 0.0), Quantity_NOC_RED);
  aSpotLight->SetIntensity(5000);
  myView->SetLightOn(aSpotLight);
}

void Viewer3dSamples::PositionalLight3dSample()
{
  Handle(V3d_PositionalLight) aPositionalLight =
    new V3d_PositionalLight(gp_Pnt(0.0, -100.0, 5.0), Quantity_NOC_GREEN);
  aPositionalLight->SetAttenuation(1, 0);
  myView->SetLightOn(aPositionalLight);
}

void Viewer3dSamples::DirectionalLight3dSample()
{
  Handle(V3d_DirectionalLight) aDirectionalLight =
    new V3d_DirectionalLight(gp_Dir(-1.0, 0.0, -1.0), Quantity_NOC_BLUE1);
  myView->SetLightOn(aDirectionalLight);
}

void Viewer3dSamples::AmbientLight3dSample()
{
  Handle(V3d_AmbientLight) aAmbientLight = new V3d_AmbientLight(Quantity_NOC_MAGENTA1);
  myView->SetLightOn(aAmbientLight);
}

void Viewer3dSamples::ClearLight3dSample()
{
  //	Setting Off all viewer active lights
  TColStd_ListOfTransient lights;
  for (myView->Viewer()->InitActiveLights();
    myView->Viewer()->MoreActiveLights();
    myView->Viewer()->NextActiveLights()) {
    lights.Append(myView->Viewer()->ActiveLight());
  }
  TColStd_ListIteratorOfListOfTransient itrLights(lights);
  for (; itrLights.More(); itrLights.Next()) {
    Handle(V3d_Light) light = Handle(V3d_Light)::DownCast(itrLights.Value());
    myView->Viewer()->SetLightOff(light);
  }

  //	Setting Off all view active lights
  lights.Clear();
  for (myView->InitActiveLights();
    myView->MoreActiveLights();
    myView->NextActiveLights()) {
    lights.Append(myView->ActiveLight());
  }
  itrLights.Initialize(lights);
  for (; itrLights.More(); itrLights.Next()) {
    Handle(V3d_Light) light = Handle(V3d_Light)::DownCast(itrLights.Value());
    myView->SetLightOff(light);
  }
  myView->Viewer()->SetDefaultLights();// Setting the default lights on
  myView->Update();
}

void Viewer3dSamples::VerticesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_VERTEX));
}

void Viewer3dSamples::EdgesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_EDGE));
}

void Viewer3dSamples::FacesSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(AIS_Shape::SelectionMode(TopAbs_FACE));
}

void Viewer3dSamples::NeutralPointSelect3dSample()
{
  myContext->Deactivate();
  myContext->Activate(0);
}

void Viewer3dSamples::WireFramePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetDisplayMode(anAisObject, 0, false); // set wireframe
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::ShadingPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetDisplayMode(anAisObject, 1, false); // set shading
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::RedColorPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList) {
    Quantity_Color aShapeColor;
    myContext->Color(anAisObject, aShapeColor);
    myResult << "A Current shape color: Red = " << aShapeColor.Red()
      << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
    aShapeColor.SetValues(0.8, 0.1, 0.1, Quantity_TOC_RGB);
    myContext->SetColor(anAisObject, aShapeColor, Standard_False);
    myResult << "A New shape color: Red = " << aShapeColor.Red()
      << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
  }
}

void Viewer3dSamples::GrayColorPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList) {
    Quantity_Color aShapeColor;
    myContext->Color(anAisObject, aShapeColor);
    myResult << "A Current shape color: Hue = " << aShapeColor.Hue()
      << " Light = " << aShapeColor.Light() << " Saturation = " << aShapeColor.Saturation() << std::endl;
    aShapeColor.SetValues(0.0, 0.3, 0.1, Quantity_TOC_HLS);
    myContext->SetColor(anAisObject, aShapeColor, Standard_False);
    myResult << "A New shape color: Hue = " << aShapeColor.Hue()
      << " Light = " << aShapeColor.Light() << " Saturation = " << aShapeColor.Saturation() << std::endl;
  }
}

void Viewer3dSamples::PlasticPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Graphic3d_NameOfMaterial aMaterial = Graphic3d_NOM_PLASTIC;
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetMaterial(anAisObject, aMaterial, Standard_False);
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::BronzePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  Graphic3d_NameOfMaterial aMaterial = Graphic3d_NOM_BRONZE;
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetMaterial(anAisObject, aMaterial, Standard_False);
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::OpaquePresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetTransparency(anAisObject, 0.0, Standard_False);
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::HalfTransparencyPresentation3dSample()
{
  AIS_ListOfInteractive anAisObjectsList;
  myContext->DisplayedObjects(anAisObjectsList);
  for (Handle(AIS_InteractiveObject) anAisObject : anAisObjectsList)
    myContext->SetTransparency(anAisObject, 0.5, Standard_False);
  myContext->UpdateCurrentViewer();
}

void Viewer3dSamples::VboOn3dSample()
{
  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver());

  if (!aDriver.IsNull()) {
    aDriver->ChangeOptions().vboDisable = Standard_False;
  }
}

void Viewer3dSamples::VboOff3dSample()
{
  Handle(OpenGl_GraphicDriver) aDriver =
    Handle(OpenGl_GraphicDriver)::DownCast(myContext->CurrentViewer()->Driver());

  if (!aDriver.IsNull()) {
    aDriver->ChangeOptions().vboDisable = Standard_True;
  }
}
