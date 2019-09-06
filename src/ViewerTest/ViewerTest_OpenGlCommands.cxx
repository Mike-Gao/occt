// Created on: 2012-04-09
// Created by: Sergey ANIKIN
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <ViewerTest.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Image_AlienPixMap.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenFile.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <OpenGl_Group.hxx>
#include <OSD_OpenFile.hxx>

extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

namespace {

//=======================================================================
//function : VUserDraw
//purpose  : Checks availability and operation of UserDraw feature
//=======================================================================

class VUserDrawObj : public AIS_InteractiveObject
{
public:
    // CASCADE RTTI
    DEFINE_STANDARD_RTTI_INLINE(VUserDrawObj,AIS_InteractiveObject);

    VUserDrawObj()
    {
      myCoords[0] = -10.;
      myCoords[1] = -20.;
      myCoords[2] = -30.;
      myCoords[3] =  10.;
      myCoords[4] =  20.;
      myCoords[5] =  30.;
    }

public:
  class Element : public OpenGl_Element
  {
  private:
    Handle(VUserDrawObj) myIObj;

  public:
    Element (const Handle(VUserDrawObj)& theIObj) : myIObj (theIObj) {}

    virtual ~Element() {}

    virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const
    {
      if (!myIObj.IsNull())
        myIObj->Render(theWorkspace);
    }

    virtual void Release (OpenGl_Context*)
    {
      //
    }

  public:
    DEFINE_STANDARD_ALLOC
  };

private:
    // Virtual methods implementation
    void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode) Standard_OVERRIDE;

    void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                           const Standard_Integer theMode) Standard_OVERRIDE;

    // Called by VUserDrawElement
    void Render(const Handle(OpenGl_Workspace)& theWorkspace) const;

private:
    GLfloat myCoords[6];
    friend class Element;
};

void VUserDrawObj::Compute(const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer /*theMode*/)
{
  thePrs->Clear();

  Graphic3d_Vec4 aBndMin (myCoords[0], myCoords[1], myCoords[2], 1.0f);
  Graphic3d_Vec4 aBndMax (myCoords[3], myCoords[4], myCoords[5], 1.0f);
  Handle(OpenGl_Group) aGroup = Handle(OpenGl_Group)::DownCast (thePrs->NewGroup());
  aGroup->SetMinMaxValues (aBndMin.x(), aBndMin.y(), aBndMin.z(),
                           aBndMax.x(), aBndMax.y(), aBndMax.z());
  aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
  VUserDrawObj::Element* anElem = new VUserDrawObj::Element (this);
  aGroup->AddElement(anElem);

  // invalidate bounding box of the scene
  thePrsMgr->StructureManager()->Update();
}

void VUserDrawObj::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                     const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 5);
  aPnts->SetValue(1, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  aPnts->SetValue(2, gp_Pnt(myCoords[3], myCoords[4], myCoords[2]));
  aPnts->SetValue(3, gp_Pnt(myCoords[3], myCoords[4], myCoords[5]));
  aPnts->SetValue(4, gp_Pnt(myCoords[0], myCoords[1], myCoords[5]));
  aPnts->SetValue(5, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  Handle(Select3D_SensitiveCurve) aSensitive = new Select3D_SensitiveCurve(anEntityOwner, aPnts);
  theSelection->Add(aSensitive);
}

void VUserDrawObj::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  // To test linking against OpenGl_Workspace and all aspect classes
  const OpenGl_Aspects* aMA = theWorkspace->Aspects();
  aMA->Aspect()->MarkerType();
  OpenGl_Vec4 aColor = theWorkspace->InteriorColor();

  aCtx->ShaderManager()->BindLineProgram (Handle(OpenGl_TextureSet)(), Aspect_TOL_SOLID,
                                          Graphic3d_TOSM_UNLIT, Graphic3d_AlphaMode_Opaque, false,
                                          Handle(OpenGl_ShaderProgram)());
  aCtx->SetColor4fv (aColor);

  const OpenGl_Vec3 aVertArray[4] =
  {
    OpenGl_Vec3(myCoords[0], myCoords[1], myCoords[2]),
    OpenGl_Vec3(myCoords[3], myCoords[4], myCoords[2]),
    OpenGl_Vec3(myCoords[3], myCoords[4], myCoords[5]),
    OpenGl_Vec3(myCoords[0], myCoords[1], myCoords[5]),
  };
  Handle(OpenGl_VertexBuffer) aVertBuffer = new OpenGl_VertexBuffer();
  aVertBuffer->Init (aCtx, 3, 4, aVertArray[0].GetData());

  // Finally draw something to make sure UserDraw really works
  aVertBuffer->BindAttribute  (aCtx, Graphic3d_TOA_POS);
  glDrawArrays(GL_LINE_LOOP, 0, aVertBuffer->GetElemsNb());
  aVertBuffer->UnbindAttribute(aCtx, Graphic3d_TOA_POS);
  aVertBuffer->Release (aCtx.get());
}

} // end of anonymous namespace

static Standard_Integer VUserDraw (Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "Call 'vinit' before!\n";
    return 1;
  }

  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContext->CurrentViewer()->Driver());
  if (aDriver.IsNull())
  {
    std::cerr << "Graphic driver not available.\n";
    return 1;
  }

  if (argc > 2)
  {
    di << argv[0] << "Wrong number of arguments, only the object name expected\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  VDisplayAISObject(aName, Handle(AIS_InteractiveObject)());

  Handle(VUserDrawObj) anIObj = new VUserDrawObj();
  VDisplayAISObject(aName, anIObj);

  return 0;
}

//==============================================================================
//function : VFeedback
//purpose  :
//==============================================================================

static int VFeedback (Draw_Interpretor& theDI,
                      Standard_Integer  /*theArgNb*/,
                      const char**      /*theArgVec*/)
{
#if !defined(GL_ES_VERSION_2_0)
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  unsigned int aBufferSize = 1024 * 1024;
  for (;;)
  {
    size_t aBytes = (size_t )aBufferSize * sizeof(GLfloat);
    if (aBytes / sizeof(GLfloat) != (size_t )aBufferSize)
    {
      // finito la commedia
      std::cerr << "Can not allocate buffer - requested size ("
                << (double(aBufferSize / (1024 * 1024)) * double(sizeof(GLfloat)))
                << " MiB) is out of address space\n";
      return 1;
    }

    GLfloat* aBuffer = (GLfloat* )Standard::Allocate (aBytes);
    if (aBuffer == NULL)
    {
      // finito la commedia
      std::cerr << "Can not allocate buffer with size ("
                << (double(aBufferSize / (1024 * 1024)) * double(sizeof(GLfloat)))
                << " MiB)\n";
      return 1;
    }

    glFeedbackBuffer ((GLsizei )aBufferSize, GL_2D, aBuffer);
    glRenderMode (GL_FEEDBACK);

    aView->Redraw();

    GLint aResult = glRenderMode (GL_RENDER);
    if (aResult < 0)
    {
      aBufferSize *= 2;

      void* aPtr = aBuffer;
      Standard::Free (aPtr);
      aBuffer = NULL;
      continue;
    }

    std::cout << "FeedBack result= " << aResult << "\n";
    GLint aPntNb     = 0;
    GLint aTriNb     = 0;
    GLint aQuadsNb   = 0;
    GLint aPolyNb    = 0;
    GLint aNodesNb   = 0;
    GLint aLinesNb   = 0;
    GLint aBitmapsNb = 0;
    GLint aPassThrNb = 0;
    GLint aUnknownNb = 0;
    const GLint NODE_VALUES = 2; // GL_2D
    for (GLint anIter = 0; anIter < aResult;)
    {
        const GLfloat aPos = aBuffer[anIter];
        switch ((GLint )aPos)
        {
          case GL_POINT_TOKEN:
          {
            ++aPntNb;
            ++aNodesNb;
            anIter += 1 + NODE_VALUES;
            break;
          }
          case GL_LINE_RESET_TOKEN:
          case GL_LINE_TOKEN:
          {
            ++aLinesNb;
            aNodesNb += 2;
            anIter += 1 + 2 * NODE_VALUES;
            break;
          }
          case GL_POLYGON_TOKEN:
          {
            const GLint aCount = (GLint )aBuffer[++anIter];
            aNodesNb += aCount;
            anIter += aCount * NODE_VALUES + 1;
            if (aCount == 3)
            {
              ++aTriNb;
            }
            else if (aCount == 4)
            {
              ++aQuadsNb;
            }
            else
            {
              ++aPolyNb;
            }
            break;
          }
          case GL_BITMAP_TOKEN:
          case GL_DRAW_PIXEL_TOKEN:
          case GL_COPY_PIXEL_TOKEN:
          {
            ++aBitmapsNb;
            anIter += 1 + NODE_VALUES;
            break;
          }
          case GL_PASS_THROUGH_TOKEN:
          {
            ++aPassThrNb;
            anIter += 2; // header + value
            break;
          }
          default:
          {
            ++anIter;
            ++aUnknownNb;
            break;
          }
       }
    }
    void* aPtr = aBuffer;
    Standard::Free (aPtr);

    // return statistics
    theDI << "Total nodes:   " << aNodesNb   << "\n"
          << "Points:        " << aPntNb     << "\n"
          << "Line segments: " << aLinesNb   << "\n"
          << "Triangles:     " << aTriNb     << "\n"
          << "Quads:         " << aQuadsNb   << "\n"
          << "Polygons:      " << aPolyNb    << "\n"
          << "Bitmap tokens: " << aBitmapsNb << "\n"
          << "Pass through:  " << aPassThrNb << "\n"
          << "UNKNOWN:       " << aUnknownNb << "\n";

    double aLen2D      = double(aNodesNb * 2 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    double aLen3D      = double(aNodesNb * 3 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    double aLen3D_rgba = double(aNodesNb * 7 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    theDI << "Buffer size GL_2D:       " << aLen2D      * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n"
          << "Buffer size GL_3D:       " << aLen3D      * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n"
          << "Buffer size GL_3D_COLOR: " << aLen3D_rgba * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n";
    return 0;
  }
#else
  (void )theDI;
  std::cout << "Command is unsupported on current platform.\n";
  return 1;
#endif
}

//==============================================================================
//function : VImmediateFront
//purpose  :
//==============================================================================

static int VImmediateFront (Draw_Interpretor& /*theDI*/,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();

  if (aDriver.IsNull())
  {
    std::cerr << "Graphic driver not available.\n";
    return 1;
  }

  if (theArgNb < 2)
  {
    std::cerr << "Wrong number of arguments.\n";
    return 1;
  }

  ViewerTest::CurrentView()->View()->SetImmediateModeDrawToFront (atoi(theArgVec[1]) != 0);

  return 0;
}

//! Search the info from the key.
inline TCollection_AsciiString searchInfo (const TColStd_IndexedDataMapOfStringString& theDict,
                                           const TCollection_AsciiString& theKey)
{
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter (theDict); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString (anIter.Key(), theKey, Standard_False))
    {
      return anIter.Value();
    }
  }
  return TCollection_AsciiString();
}

//==============================================================================
//function : VGlInfo
//purpose  :
//==============================================================================

static int VGlInfo (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Graphic3d_DiagnosticInfo anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
  if (theArgNb == 2)
  {
    TCollection_AsciiString aName (theArgVec[1]);
    aName.LowerCase();
    if (aName == "-short")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Short;
    }
    else if (aName == "-basic")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
    }
    else if (aName == "-complete"
          || aName == "-full")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Complete;
    }
  }

  TColStd_IndexedDataMapOfStringString aDict;
  if (anArgIter >= theArgNb)
  {
    aView->DiagnosticInformation (aDict, anInfoLevel);
    TCollection_AsciiString aText;
    for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter (aDict); aValueIter.More(); aValueIter.Next())
    {
      if (!aText.IsEmpty())
      {
        aText += "\n";
      }
      aText += TCollection_AsciiString("  ") + aValueIter.Key() + ": " + aValueIter.Value();
    }

    theDI << "OpenGL info:\n"
          << aText;
    return 0;
  }

  const Standard_Boolean isList = theArgNb >= 3;
  aView->DiagnosticInformation (aDict, Graphic3d_DiagnosticInfo_Complete);
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString aName (theArgVec[anArgIter]);
    aName.UpperCase();
    TCollection_AsciiString aValue;
    if (aName.Search ("VENDOR") != -1)
    {
      aValue = searchInfo (aDict, "GLvendor");
    }
    else if (aName.Search ("RENDERER") != -1)
    {
      aValue = searchInfo (aDict, "GLdevice");
    }
    else if (aName.Search ("SHADING_LANGUAGE_VERSION") != -1
          || aName.Search ("GLSL") != -1)
    {
      aValue = searchInfo (aDict, "GLSLversion");
    }
    else if (aName.Search ("VERSION") != -1)
    {
      aValue = searchInfo (aDict, "GLversion");
    }
    else if (aName.Search ("EXTENSIONS") != -1)
    {
      aValue = searchInfo (aDict, "GLextensions");
    }
    else
    {
      std::cerr << "Unknown key '" << aName.ToCString() << "'\n";
      return 1;
    }

    if (isList)
    {
      theDI << "{" << aValue << "} ";
    }
    else
    {
      theDI << aValue;
    }
  }

  return 0;
}

//! Parse shader type argument.
static bool parseShaderTypeArg (Graphic3d_TypeOfShaderObject& theType,
                                const TCollection_AsciiString& theArg)
{
  if (theArg == "-vertex"
   || theArg == "-vert")
  {
    theType = Graphic3d_TOS_VERTEX;
  }
  else if (theArg == "-tessevaluation"
        || theArg == "-tesseval"
        || theArg == "-evaluation"
        || theArg == "-eval")
  {
    theType = Graphic3d_TOS_TESS_EVALUATION;
  }
  else if (theArg == "-tesscontrol"
        || theArg == "-tessctrl"
        || theArg == "-control"
        || theArg == "-ctrl")
  {
    theType = Graphic3d_TOS_TESS_CONTROL;
  }
  else if (theArg == "-geometry"
        || theArg == "-geom")
  {
    theType = Graphic3d_TOS_GEOMETRY;
  }
  else if (theArg == "-fragment"
        || theArg == "-frag")
  {
    theType = Graphic3d_TOS_FRAGMENT;
  }
  else if (theArg == "-compute"
        || theArg == "-comp")
  {
    theType = Graphic3d_TOS_COMPUTE;
  }
  else
  {
    return false;
  }
  return true;
}

//==============================================================================
//function : VShaderProg
//purpose  : Sets the pair of vertex and fragment shaders for the object
//==============================================================================
static Standard_Integer VShaderProg (Draw_Interpretor& theDI,
                                     Standard_Integer  theArgNb,
                                     const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view.\n";
    return 1;
  }
  else if (theArgNb < 2)
  {
    std::cout << "Syntax error: lack of arguments\n";
    return 1;
  }

  bool isExplicitShaderType = false;
  Handle(Graphic3d_ShaderProgram) aProgram = new Graphic3d_ShaderProgram();
  NCollection_Sequence<Handle(AIS_InteractiveObject)> aPrsList;
  Graphic3d_GroupAspect aGroupAspect = Graphic3d_ASPECT_FILL_AREA;
  bool isSetGroupAspect = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    Graphic3d_TypeOfShaderObject aShaderTypeArg = Graphic3d_TypeOfShaderObject(-1);
    if (!aProgram.IsNull()
      && anArg == "-uniform"
      && anArgIter + 2 < theArgNb)
    {
      TCollection_AsciiString aName = theArgVec[++anArgIter];
      aProgram->PushVariableFloat (aName, float (Draw::Atof (theArgVec[++anArgIter])));
    }
    else if (anArg == "-list"
          || ((anArg == "-update"
            || anArg == "-dump"
            || anArg == "-debug"
            || anArg == "-reload"
            || anArg == "-load")
           && anArgIter + 1 < theArgNb))
    {
      Handle(OpenGl_Context) aGlCtx;
      if (Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aCtx->CurrentViewer()->Driver()))
      {
        aGlCtx = aDriver->GetSharedContext();
      }
      if (aGlCtx.IsNull())
      {
        std::cout << "Error: no OpenGl_Context\n";
        return 1;
      }

      if (anArg == "-list")
      {
        for (OpenGl_Context::OpenGl_ResourcesMap::Iterator aResIter (aGlCtx->SharedResources()); aResIter.More(); aResIter.Next())
        {
          if (Handle(OpenGl_ShaderProgram) aResProg = Handle(OpenGl_ShaderProgram)::DownCast (aResIter.Value()))
          {
            theDI << aResProg->ResourceId() << " ";
          }
        }
      }
      else
      {
        TCollection_AsciiString aShaderName = theArgVec[++anArgIter];
        Handle(OpenGl_ShaderProgram) aResProg;
        if (!aGlCtx->GetResource (aShaderName, aResProg))
        {
          std::cout << "Syntax error: shader resource '" << aShaderName << "' is not found\n";
          return 1;
        }
        if (aResProg->UpdateDebugDump (aGlCtx, "", false, anArg == "-dump"))
        {
          aCtx->UpdateCurrentViewer();
        }
      }
      if (anArgIter + 1 < theArgNb)
      {
        std::cout << "Syntax error: wrong number of arguments\n";
        return 1;
      }
      return 0;
    }
    else if (!aProgram.IsNull()
          &&  aProgram->ShaderObjects().IsEmpty()
          && (anArg == "-off"
           || anArg ==  "off"))
    {
      aProgram.Nullify();
    }
    else if (!aProgram.IsNull()
          &&  aProgram->ShaderObjects().IsEmpty()
          && (anArg == "-phong"
           || anArg ==  "phong"))
    {
      const TCollection_AsciiString& aShadersRoot = Graphic3d_ShaderProgram::ShadersFolder();
      if (aShadersRoot.IsEmpty())
      {
        std::cout << "Error: both environment variables CSF_ShadersDirectory and CASROOT are undefined!\n"
                     "At least one should be defined to load Phong program.\n";
        return 1;
      }

      const TCollection_AsciiString aSrcVert = aShadersRoot + "/PhongShading.vs";
      const TCollection_AsciiString aSrcFrag = aShadersRoot + "/PhongShading.fs";
      if (!aSrcVert.IsEmpty()
       && !OSD_File (aSrcVert).Exists())
      {
        std::cout << "Error: PhongShading.vs is not found\n";
        return 1;
      }
      if (!aSrcFrag.IsEmpty()
       && !OSD_File (aSrcFrag).Exists())
      {
        std::cout << "Error: PhongShading.fs is not found\n";
        return 1;
      }

      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile (Graphic3d_TOS_VERTEX,   aSrcVert));
      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile (Graphic3d_TOS_FRAGMENT, aSrcFrag));
    }
    else if (aPrsList.IsEmpty()
          && anArg == "*")
    {
      //
    }
    else if (!isSetGroupAspect
          &&  anArgIter + 1 < theArgNb
          && (anArg == "-primtype"
           || anArg == "-primitivetype"
           || anArg == "-groupaspect"
           || anArg == "-aspecttype"
           || anArg == "-aspect"))
    {
      isSetGroupAspect = true;
      TCollection_AsciiString aPrimTypeStr (theArgVec[++anArgIter]);
      aPrimTypeStr.LowerCase();
      if (aPrimTypeStr == "line")
      {
        aGroupAspect = Graphic3d_ASPECT_LINE;
      }
      else if (aPrimTypeStr == "tris"
            || aPrimTypeStr == "triangles"
            || aPrimTypeStr == "fill"
            || aPrimTypeStr == "fillarea"
            || aPrimTypeStr == "shading"
            || aPrimTypeStr == "shade")
      {
        aGroupAspect = Graphic3d_ASPECT_FILL_AREA;
      }
      else if (aPrimTypeStr == "text")
      {
        aGroupAspect = Graphic3d_ASPECT_TEXT;
      }
      else if (aPrimTypeStr == "marker"
            || aPrimTypeStr == "point"
            || aPrimTypeStr == "pnt")
      {
        aGroupAspect = Graphic3d_ASPECT_MARKER;
      }
      else
      {
        std::cerr << "Syntax error at '" << aPrimTypeStr << "'\n";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb
         && !aProgram.IsNull()
         &&  aProgram->Header().IsEmpty()
         &&  (anArg == "-version"
           || anArg == "-glslversion"
           || anArg == "-header"
           || anArg == "-glslheader"))
    {
      TCollection_AsciiString aHeader (theArgVec[++anArgIter]);
      if (aHeader.IsIntegerValue())
      {
        aHeader = TCollection_AsciiString ("#version ") + aHeader;
      }
      aProgram->SetHeader (aHeader);
    }
    else if (!anArg.StartsWith ("-")
          && GetMapOfAIS().IsBound2 (theArgVec[anArgIter]))
    {
      Handle(AIS_InteractiveObject) anIO = GetMapOfAIS().Find2 (theArgVec[anArgIter]);
      if (anIO.IsNull())
      {
        std::cerr << "Syntax error: " << theArgVec[anArgIter] << " is not an AIS object\n";
        return 1;
      }
      aPrsList.Append (anIO);
    }
    else if (!aProgram.IsNull()
           && ((anArgIter + 1 < theArgNb && parseShaderTypeArg (aShaderTypeArg, anArg))
            || (!isExplicitShaderType && aProgram->ShaderObjects().Size() < 2)))
    {
      TCollection_AsciiString aShaderPath (theArgVec[anArgIter]);
      if (aShaderTypeArg != Graphic3d_TypeOfShaderObject(-1))
      {
        aShaderPath = (theArgVec[++anArgIter]);
        isExplicitShaderType = true;
      }

      const bool isSrcFile = OSD_File (aShaderPath).Exists();
      Handle(Graphic3d_ShaderObject) aShader = isSrcFile
                                             ? Graphic3d_ShaderObject::CreateFromFile  (Graphic3d_TOS_VERTEX, aShaderPath)
                                             : Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, aShaderPath);
      const TCollection_AsciiString& aShaderSrc = aShader->Source();

      const bool hasVertPos   = aShaderSrc.Search ("gl_Position")  != -1;
      const bool hasFragColor = aShaderSrc.Search ("occSetFragColor") != -1
                             || aShaderSrc.Search ("occFragColor") != -1
                             || aShaderSrc.Search ("gl_FragColor") != -1
                             || aShaderSrc.Search ("gl_FragData")  != -1;
      Graphic3d_TypeOfShaderObject aShaderType = aShaderTypeArg;
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        if (hasVertPos
        && !hasFragColor)
        {
          aShaderType = Graphic3d_TOS_VERTEX;
        }
        if (hasFragColor
        && !hasVertPos)
        {
          aShaderType = Graphic3d_TOS_FRAGMENT;
        }
      }
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        std::cerr << "Error: non-existing or invalid shader source\n";
        return 1;
      }

      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aShaderType, aShaderSrc));
    }
    else
    {
      std::cerr << "Syntax error at '" << anArg << "'\n";
      return 1;
    }
  }

  if (!aProgram.IsNull()
    && ViewerTest::CurrentView()->RenderingParams().TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
  {
    aProgram->SetNbFragmentOutputs (2);
    aProgram->SetWeightOitOutput (true);
  }

  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName aGlobalPrsIter (GetMapOfAIS());
  NCollection_Sequence<Handle(AIS_InteractiveObject)>::Iterator aPrsIter (aPrsList);
  const bool isGlobalList = aPrsList.IsEmpty();
  for (;;)
  {
    Handle(AIS_InteractiveObject) anIO;
    if (isGlobalList)
    {
      if (!aGlobalPrsIter.More())
      {
        break;
      }
      anIO = aGlobalPrsIter.Key1();
      aGlobalPrsIter.Next();
      if (anIO.IsNull())
      {
        continue;
      }
    }
    else
    {
      if (!aPrsIter.More())
      {
        break;
      }
      anIO = aPrsIter.Value();
      aPrsIter.Next();
    }

    if (anIO->Attributes()->SetShaderProgram (aProgram, aGroupAspect, true))
    {
      aCtx->Redisplay (anIO, Standard_False);
    }
    else
    {
      anIO->SynchronizeAspects();
    }
  }

  aCtx->UpdateCurrentViewer();
  return 0;
}

//! Print triplet of values.
template<class S, class T> static S& operator<< (S& theStream, const NCollection_Vec3<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2];
  return theStream;
}

//! Print 4 values.
template<class S, class T> static S& operator<< (S& theStream, const NCollection_Vec4<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2] << " " << theVec[3];
  return theStream;
}

//! Print fresnel model.
static const char* fresnelModelString (const Graphic3d_FresnelModel theModel)
{
  switch (theModel)
  {
    case Graphic3d_FM_SCHLICK:    return "SCHLICK";
    case Graphic3d_FM_CONSTANT:   return "CONSTANT";
    case Graphic3d_FM_CONDUCTOR:  return "CONDUCTOR";
    case Graphic3d_FM_DIELECTRIC: return "DIELECTRIC";
  }
  return "N/A";
}

//! Create a colored rectangle SVG element.
static TCollection_AsciiString formatSvgColoredRect (const Quantity_Color& theColor)
{
  return TCollection_AsciiString()
       + "<svg width='20px' height='20px'><rect width='20px' height='20px' fill='" + Quantity_Color::ColorToHex (theColor) + "' /></svg>";
}

//==============================================================================
//function : VListMaterials
//purpose  :
//==============================================================================
static Standard_Integer VListMaterials (Draw_Interpretor& theDI,
                                        Standard_Integer  theArgNb,
                                        const char**      theArgVec)
{
  TCollection_AsciiString aDumpFile;
  NCollection_Sequence<Graphic3d_NameOfMaterial> aMatList;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    Graphic3d_NameOfMaterial aMat = Graphic3d_MaterialAspect::MaterialFromName (theArgVec[anArgIter]);
    if (aMat != Graphic3d_NOM_DEFAULT)
    {
      aMatList.Append (aMat);
    }
    else if (anArg == "*")
    {
      for (Standard_Integer aMatIter = 0; aMatIter < (Standard_Integer )Graphic3d_NOM_DEFAULT; ++aMatIter)
      {
        aMatList.Append ((Graphic3d_NameOfMaterial )aMatIter);
      }
    }
    else if (aDumpFile.IsEmpty()
          && (anArg.EndsWith (".obj")
           || anArg.EndsWith (".mtl")
           || anArg.EndsWith (".htm")
           || anArg.EndsWith (".html")))
    {
      aDumpFile = theArgVec[anArgIter];
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aMatList.IsEmpty())
  {
    if (aDumpFile.IsEmpty())
    {
      for (Standard_Integer aMatIter = 1; aMatIter <= Graphic3d_MaterialAspect::NumberOfMaterials(); ++aMatIter)
      {
        theDI << Graphic3d_MaterialAspect::MaterialName (aMatIter) << " ";
      }
      return 0;
    }

    for (Standard_Integer aMatIter = 0; aMatIter < (Standard_Integer )Graphic3d_NOM_DEFAULT; ++aMatIter)
    {
      aMatList.Append ((Graphic3d_NameOfMaterial )aMatIter);
    }
  }

  // geometry for dumping
  const Graphic3d_Vec3 aBoxVerts[8] =
  {
    Graphic3d_Vec3( 1, -1, -1),
    Graphic3d_Vec3( 1, -1,  1),
    Graphic3d_Vec3(-1, -1,  1),
    Graphic3d_Vec3(-1, -1, -1),
    Graphic3d_Vec3( 1,  1, -1),
    Graphic3d_Vec3( 1,  1,  1),
    Graphic3d_Vec3(-1,  1,  1),
    Graphic3d_Vec3(-1,  1, -1)
  };

  const Graphic3d_Vec4i aBoxQuads[6] =
  {
    Graphic3d_Vec4i (1, 2, 3, 4),
    Graphic3d_Vec4i (5, 8, 7, 6),
    Graphic3d_Vec4i (1, 5, 6, 2),
    Graphic3d_Vec4i (2, 6, 7, 3),
    Graphic3d_Vec4i (3, 7, 8, 4),
    Graphic3d_Vec4i (5, 1, 4, 8)
  };

  std::ofstream aMatFile, anObjFile, anHtmlFile;
  if (aDumpFile.EndsWith (".obj")
   || aDumpFile.EndsWith (".mtl"))
  {
    const TCollection_AsciiString aMatFilePath  = aDumpFile.SubString (1, aDumpFile.Length() - 3) + "mtl";
    const TCollection_AsciiString anObjFilePath = aDumpFile.SubString (1, aDumpFile.Length() - 3) + "obj";

    OSD_OpenStream (aMatFile,  aMatFilePath.ToCString(),  std::ios::out | std::ios::binary);
    if (!aMatFile.is_open())
    {
      std::cout << "Error: unable creating material file\n";
      return 0;
    }
    if (!aDumpFile.EndsWith (".mtl"))
    {
      OSD_OpenStream (anObjFile, anObjFilePath.ToCString(), std::ios::out | std::ios::binary);
      if (!anObjFile.is_open())
      {
        std::cout << "Error: unable creating OBJ file\n";
        return 0;
      }

      TCollection_AsciiString anMtlName, aFolder;
      OSD_Path::FolderAndFileFromPath (aMatFilePath, aFolder, anMtlName);
      anObjFile << "mtllib " << anMtlName << "\n";
    }
  }
  else if (aDumpFile.EndsWith (".htm")
        || aDumpFile.EndsWith (".html"))
  {
    OSD_OpenStream (anHtmlFile, aDumpFile.ToCString(), std::ios::out | std::ios::binary);
    if (!anHtmlFile.is_open())
    {
      std::cout << "Error: unable creating HTML file\n";
      return 0;
    }
    anHtmlFile << "<html>\n"
                  "<head><title>OCCT Material table</title></head>\n"
                  "<body>\n"
                  "<table border='1'><tbody>\n"
                  "<tr>\n"
                  "<th rowspan='2'><div title='Material name.\n"
                                              "See also Graphic3d_NameOfMaterial enumeration'>"
                                   "Name</div></th>\n"
                  "<th rowspan='2'><div title='Material type: PHYSIC or ASPECT.\n"
                                              "ASPECT material does not define final colors, it is taken from Internal Color instead.\n"
                                              "See also Graphic3d_TypeOfMaterial enumeration'>"
                                   "Type</div></th>\n"
                  "<th rowspan='2'>Transparency</th>\n"
                  "<th colspan='5'><div title='PBR Metallic-Roughness'>"
                                   "PBR Metallic-Roughness</div></th>\n"
                  "<th colspan='5'><div title='Common material definition for Phong shading model'>"
                                   "Common (Blinn-Phong)</div></th>\n"
                  "<th colspan='10'><div title='BSDF (Bidirectional Scattering Distribution Function).\n"
                                              "Used for physically-based rendering (in path tracing engine).\n"
                                              "BSDF is represented as weighted mixture of basic BRDFs/BTDFs (Bidirectional Reflectance (Transmittance) Distribution Functions).\n"
                                              "See also Graphic3d_BSDF structure.'>"
                                   "BSDF (Bidirectional Scattering Distribution Function)</div></th>\n"
                  "</tr>\n"
                  "<tr>\n"
                  "<th>Color</th>\n"
                  "<th>Metallic</th>\n"
                  "<th>Roughness</th>\n"
                  "<th>Emission</th>\n"
                  "<th><div title='Index of refraction'>"
                       "IOR</div></th>\n"
                  "<th>Ambient</th>\n"
                  "<th>Diffuse</th>\n"
                  "<th>Specular</th>\n"
                  "<th>Emissive</th>\n"
                  "<th>Shiness</th>\n"
                  "<th><div title='Weight of coat specular/glossy BRDF'>"
                       "Kc</div></th>\n"
                  "<th><div title='Weight of base diffuse BRDF'>"
                       "Kd</div></th>\n"
                  "<th><div title='Weight of base specular/glossy BRDF'>"
                       "Ks</div></th>\n"
                  "<th><div title='Weight of base specular/glossy BTDF'>"
                       "Kt</div></th>\n"
                  "<th><div title='Radiance emitted by the surface'>"
                       "Le</div></th>\n"
                  "<th><div title='Volume scattering color/density'>"
                       "Absorption</div></th>\n"
                  "<th><div title='Parameters of Fresnel reflectance of coat layer'>"
                       "FresnelCoat</div></th>\n"
                  "<th><div title='Parameters of Fresnel reflectance of base layer'>"
                       "FresnelBase</div></th>\n"
                  "<th>Refraction Index</th>\n"
                  "</tr>\n";
  }
  else if (!aDumpFile.IsEmpty())
  {
    std::cout << "Syntax error: unknown output file format\n";
    return 1;
  }

  Standard_Integer aMatIndex = 0, anX = 0, anY = 0;
  for (NCollection_Sequence<Graphic3d_NameOfMaterial>::Iterator aMatIter (aMatList); aMatIter.More(); aMatIter.Next(), ++aMatIndex)
  {
    Graphic3d_MaterialAspect aMat (aMatIter.Value());
    const TCollection_AsciiString aMatName = aMat.StringName();
    const Graphic3d_Vec3 anAmbient  = (Graphic3d_Vec3 )aMat.AmbientColor();
    const Graphic3d_Vec3 aDiffuse   = (Graphic3d_Vec3 )aMat.DiffuseColor();
    const Graphic3d_Vec3 aSpecular  = (Graphic3d_Vec3 )aMat.SpecularColor();
    const Graphic3d_Vec3 anEmission = (Graphic3d_Vec3 )aMat.EmissiveColor();
    const Standard_Real  aShiness  = aMat.Shininess() * 1000.0;
    if (aMatFile.is_open())
    {
      aMatFile << "newmtl " << aMatName << "\n";
      aMatFile << "Ka " << Quantity_Color::Convert_LinearRGB_To_sRGB (anAmbient) << "\n";
      aMatFile << "Kd " << Quantity_Color::Convert_LinearRGB_To_sRGB (aDiffuse)  << "\n";
      aMatFile << "Ks " << Quantity_Color::Convert_LinearRGB_To_sRGB (aSpecular) << "\n";
      aMatFile << "Ns " << aShiness  << "\n";
      if (aMat.Transparency() >= 0.0001)
      {
        aMatFile << "Tr " << aMat.Transparency() << "\n";
      }
      aMatFile << "\n";
    }
    else if (anHtmlFile.is_open())
    {
      anHtmlFile << "<tr>\n";
      anHtmlFile << "<td>" << aMat.StringName() << "</td>\n";
      anHtmlFile << "<td>" << (aMat.MaterialType() == Graphic3d_MATERIAL_PHYSIC ? "PHYSIC" : "ASPECT")  << "</td>\n";
      anHtmlFile << "<td>" << aMat.Transparency() << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (aMat.PBRMaterial().Color().GetRGB()) << (Graphic3d_Vec3 )aMat.PBRMaterial().Color().GetRGB() << "</td>\n";
      anHtmlFile << "<td>" << aMat.PBRMaterial().Metallic() << "</td>\n";
      anHtmlFile << "<td>" << aMat.PBRMaterial().NormalizedRoughness() << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (Quantity_Color (aMat.PBRMaterial().Emission())) << aMat.PBRMaterial().Emission() << "</td>\n";
      anHtmlFile << "<td>" << aMat.PBRMaterial().IOR() << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (Quantity_Color (anAmbient))  << anAmbient  << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (Quantity_Color (aDiffuse))   << aDiffuse   << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (Quantity_Color (aSpecular))  << aSpecular  << "</td>\n";
      anHtmlFile << "<td>" << formatSvgColoredRect (Quantity_Color (anEmission)) << anEmission << "</td>\n";
      anHtmlFile << "<td>" << aMat.Shininess() << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Kc << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Kd << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Ks << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Kt << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Le << "</td>\n";
      anHtmlFile << "<td>" << aMat.BSDF().Absorption << "</td>\n";
      anHtmlFile << "<td>" << fresnelModelString (aMat.BSDF().FresnelCoat.FresnelType()) << "</td>\n";
      anHtmlFile << "<td>" << fresnelModelString (aMat.BSDF().FresnelBase.FresnelType()) << "</td>\n";
      anHtmlFile << "<td>" << aMat.RefractionIndex() << "</td>\n";
      anHtmlFile << "</tr>\n";
    }
    else
    {
      theDI << aMat.StringName() << "\n";
      theDI << "  Transparency:           " << aMat.Transparency() << "\n";
      theDI << "  PBR.BaseColor:          " << (Graphic3d_Vec3 )aMat.PBRMaterial().Color().GetRGB() << "\n";
      theDI << "  PBR.Metallic:           " << aMat.PBRMaterial().Metallic() << "\n";
      theDI << "  PBR.Roughness:          " << aMat.PBRMaterial().NormalizedRoughness() << "\n";
      theDI << "  PBR.Emission:           " << aMat.PBRMaterial().Emission() << "\n";
      theDI << "  PBR.IOR:                " << aMat.PBRMaterial().IOR() << "\n";
      theDI << "  Common.Ambient:         " << anAmbient << "\n";
      theDI << "  Common.Diffuse:         " << aDiffuse  << "\n";
      theDI << "  Common.Specular:        " << aSpecular << "\n";
      theDI << "  Common.Emissive:        " << anEmission << "\n";
      theDI << "  Common.Shiness:         " << aMat.Shininess() << "\n";
      theDI << "  BSDF.Kc:                " << aMat.BSDF().Kc << "\n";
      theDI << "  BSDF.Kd:                " << aMat.BSDF().Kd << "\n";
      theDI << "  BSDF.Ks:                " << aMat.BSDF().Ks << "\n";
      theDI << "  BSDF.Kt:                " << aMat.BSDF().Kt << "\n";
      theDI << "  BSDF.Le:                " << aMat.BSDF().Le << "\n";
      theDI << "  BSDF.Absorption:        " << aMat.BSDF().Absorption << "\n";
      theDI << "  BSDF.FresnelCoat:       " << fresnelModelString (aMat.BSDF().FresnelCoat.FresnelType()) << "\n";
      theDI << "  BSDF.FresnelBase:       " << fresnelModelString (aMat.BSDF().FresnelBase.FresnelType()) << "\n";
      theDI << "  RefractionIndex:        " << aMat.RefractionIndex() << "\n";
    }

    if (anObjFile.is_open())
    {
      anObjFile << "g " << aMatName << "\n";
      anObjFile << "usemtl " << aMatName << "\n";
      for (Standard_Integer aVertIter = 0; aVertIter < 8; ++aVertIter)
      {
        anObjFile << "v " << (aBoxVerts[aVertIter] + Graphic3d_Vec3 (3.0f * anX, -3.0f * anY, 0.0f)) << "\n";
      }
      anObjFile << "s off\n";
      for (Standard_Integer aFaceIter = 0; aFaceIter < 6; ++aFaceIter)
      {
        anObjFile << "f " << (aBoxQuads[aFaceIter] + Graphic3d_Vec4i (8 * aMatIndex)) << "\n";
      }
      anObjFile << "\n";
      if (++anX > 5)
      {
        anX = 0;
        ++anY;
      }
    }
  }

  if (anHtmlFile.is_open())
  {
    anHtmlFile << "</tbody></table>\n</body>\n</html>\n";
  }
  return 0;
}

//==============================================================================
//function : VListColors
//purpose  :
//==============================================================================
static Standard_Integer VListColors (Draw_Interpretor& theDI,
                                     Standard_Integer  theArgNb,
                                     const char**      theArgVec)
{
  TCollection_AsciiString aDumpFile;
  NCollection_Sequence<Quantity_NameOfColor> aColList;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    Quantity_NameOfColor aName;
    if (Quantity_Color::ColorFromName (theArgVec[anArgIter], aName))
    {
      aColList.Append (aName);
    }
    else if (anArg == "*")
    {
      for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer )Quantity_NOC_WHITE; ++aColIter)
      {
        aColList.Append ((Quantity_NameOfColor )aColIter);
      }
    }
    else if (aDumpFile.IsEmpty()
          && (anArg.EndsWith (".htm")
           || anArg.EndsWith (".html")))
    {
      aDumpFile = theArgVec[anArgIter];
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  if (aColList.IsEmpty())
  {
    if (aDumpFile.IsEmpty())
    {
      for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer )Quantity_NOC_WHITE; ++aColIter)
      {
        theDI << Quantity_Color::StringName (Quantity_NameOfColor (aColIter)) << " ";
      }
      return 0;
    }

    for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer )Quantity_NOC_WHITE; ++aColIter)
    {
      aColList.Append ((Quantity_NameOfColor )aColIter);
    }
  }

  std::ofstream anHtmlFile;
  TCollection_AsciiString aFileNameBase, aFolder;
  if (aDumpFile.EndsWith (".htm")
   || aDumpFile.EndsWith (".html"))
  {
    OSD_Path::FolderAndFileFromPath (aDumpFile, aFolder, aFileNameBase);
    aFileNameBase = aFileNameBase.SubString (1, aFileNameBase.Length() -  (aDumpFile.EndsWith (".htm") ? 4 : 5));
  }
  else if (!aDumpFile.IsEmpty())
  {
    std::cout << "Syntax error: unknown output file format\n";
    return 1;
  }

  Standard_Integer aMaxNameLen = 1;
  for (NCollection_Sequence<Quantity_NameOfColor>::Iterator aColIter (aColList); aColIter.More(); aColIter.Next())
  {
    aMaxNameLen = Max (aMaxNameLen, TCollection_AsciiString (Quantity_Color::StringName (aColIter.Value())).Length());
  }

  V3d_ImageDumpOptions anImgParams;
  anImgParams.Width  = 60;
  anImgParams.Height = 30;
  anImgParams.BufferType = Graphic3d_BT_RGB;
  anImgParams.StereoOptions  = V3d_SDO_MONO;
  anImgParams.ToAdjustAspect = Standard_True;
  Handle(V3d_View) aView;
  if (!aDumpFile.IsEmpty())
  {
    ViewerTest::ViewerInit (0, 0, anImgParams.Width, anImgParams.Height, "TmpDriver/TmpViewer/TmpView");
    aView = ViewerTest::CurrentView();
    aView->SetImmediateUpdate (false);
    aView->SetBgGradientStyle (Aspect_GFM_NONE, false);
  }

  if (!aDumpFile.IsEmpty())
  {
    OSD_OpenStream (anHtmlFile, aDumpFile.ToCString(), std::ios::out | std::ios::binary);
    if (!anHtmlFile.is_open())
    {
      std::cout << "Error: unable creating HTML file\n";
      return 0;
    }
    anHtmlFile << "<html>\n"
               << "<head><title>OCCT Color table</title></head>\n"
               << "<body>\n"
               << "<table border='1'><tbody>\n"
               << "<tr>\n"
               << "<th>HTML</th>\n"
               << "<th>OCCT</th>\n"
               << "<th>Color name</th>\n"
               << "<th>sRGB hex</th>\n"
               << "<th>sRGB dec</th>\n"
               << "<th>RGB linear</th>\n"
               << "</tr>\n";
  }

  Image_AlienPixMap anImg;
  Standard_Integer aColIndex = 0;
  for (NCollection_Sequence<Quantity_NameOfColor>::Iterator aColIter (aColList); aColIter.More(); aColIter.Next(), ++aColIndex)
  {
    Quantity_Color aCol (aColIter.Value());
    const TCollection_AsciiString aColName  = Quantity_Color::StringName (aColIter.Value());
    const TCollection_AsciiString anSRgbHex = Quantity_Color::ColorToHex (aCol);
    const Graphic3d_Vec3i anSRgbInt ((Graphic3d_Vec3 )aCol * 255.0f);
    if (anHtmlFile.is_open())
    {
      const TCollection_AsciiString anImgPath = aFileNameBase + "_" + aColName + ".png";
      if (!aView.IsNull())
      {
        aView->SetImmediateUpdate (false);
        aView->SetBackgroundColor (aCol);
        if (!aView->ToPixMap (anImg, anImgParams)
         || !anImg.Save (aFolder + anImgPath))
        {
          theDI << "Error: image dump failed\n";
          return 0;
        }
      }

      anHtmlFile << "<tr>\n";
      anHtmlFile << "<td style='background-color:" << anSRgbHex << "'><pre>       </pre></td>\n";
      anHtmlFile << "<td><img src='" << (!aView.IsNull() ? anImgPath : "") << "'></img></td>\n";
      anHtmlFile << "<td style='text-align:left'>" << aColName << "</td>\n";
      anHtmlFile << "<td style='text-align:left'><pre>" << anSRgbHex << "</pre></td>\n";
      anHtmlFile << "<td style='text-align:left'>(" << anSRgbInt.r() << " " << anSRgbInt.g() << " " << anSRgbInt.b() << ")</td>\n";
      anHtmlFile << "<td style='text-align:left'>(" << aCol.Red() << " " << aCol.Green() << " " << aCol.Blue() << ")</td>\n";
      anHtmlFile << "</tr>\n";
    }
    else
    {
      TCollection_AsciiString aColNameLong (aColName);
      aColNameLong.RightJustify (aMaxNameLen, ' ');
      theDI << aColNameLong << " [" << anSRgbHex << "]: " << aCol.Red() << " " << aCol.Green() << " " << aCol.Blue() << "\n";
    }
  }

  if (!aView.IsNull())
  {
    ViewerTest::RemoveView (aView);
  }

  if (anHtmlFile.is_open())
  {
    anHtmlFile << "</tbody></table>\n</body>\n</html>\n";
  }
  return 0;
}

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Text.hxx>

//! Create an array of triangles defining a rectangle.
static void addRectangleToArray (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                                 const Graphic3d_Vec2i& theLower,
                                 const Graphic3d_Vec2i& theUpper)
{
  const int aFirst = theTris->VertexNumber() + 1;
  theTris->AddVertex (gp_Pnt (theLower.x(), theLower.y(), 0.0), gp_Pnt2d (0.0, 1.0));
  theTris->AddVertex (gp_Pnt (theLower.x(), theUpper.y(), 0.0), gp_Pnt2d (0.0, 0.0));
  theTris->AddVertex (gp_Pnt (theUpper.x(), theUpper.y(), 0.0), gp_Pnt2d (1.0, 0.0));
  theTris->AddVertex (gp_Pnt (theUpper.x(), theLower.y(), 0.0), gp_Pnt2d (1.0, 1.0));
  theTris->AddEdges (aFirst + 0, aFirst + 1, aFirst + 2);
  theTris->AddEdges (aFirst + 0, aFirst + 2, aFirst + 3);
}

//! Create an array of triangles defining a rectangle.
static Handle(Graphic3d_ArrayOfTriangles) createRectangleArray (const Graphic3d_Vec2i& theLower,
                                                                const Graphic3d_Vec2i& theUpper,
                                                                Graphic3d_ArrayFlags theFlags)
{
  Handle(Graphic3d_ArrayOfTriangles) aRectTris = new Graphic3d_ArrayOfTriangles (4, 6, theFlags);
  addRectangleToArray (aRectTris, theLower, theUpper);
  return aRectTris;
}

//! Material widget action.
enum AIS_MaterialEditorWidgetAction
{
  AIS_MaterialEditorWidgetAction_Reset,
  AIS_MaterialEditorWidgetAction_Transparency,
  AIS_MaterialEditorWidgetAction_PbrAlbedo,
  AIS_MaterialEditorWidgetAction_PbrMetallic,
  AIS_MaterialEditorWidgetAction_PbrRoughness,
  AIS_MaterialEditorWidgetAction_PbrEmission,
  AIS_MaterialEditorWidgetAction_PbrIOR,
  AIS_MaterialEditorWidgetAction_CommonShininess,
  AIS_MaterialEditorWidgetAction_CommonAmbient,
  AIS_MaterialEditorWidgetAction_CommonDiffuse,
  AIS_MaterialEditorWidgetAction_CommonSpecular,
  AIS_MaterialEditorWidgetAction_CommonEmissive,
  AIS_MaterialEditorWidgetAction_NB
};

//! Entity owner holding AIS_MaterialEditorWidget action.
class AIS_MaterialEditorWidgetOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTI_INLINE(AIS_MaterialEditorWidgetOwner, SelectMgr_EntityOwner)
public:

  //! Main constructor.
  //AIS_MaterialEditorWidgetOwner (const Handle(AIS_MaterialEditorWidget)& theSel)
  AIS_MaterialEditorWidgetOwner (const Handle(AIS_InteractiveObject)& theSel,
                                 const Handle(Graphic3d_Aspects)& theAspects,
                                 AIS_MaterialEditorWidgetAction theAction,
                                 Standard_Integer theComponent = 0)
  : SelectMgr_EntityOwner (theSel, 5),
    myAspects (theAspects),
    myAction (theAction),
    myComponent (theComponent) {}

  //! Set last picking results.
  void SetPickedPoint (const gp_Pnt& thePickedCenterPnt,
                       const gp_Pnt& thePnt,
                       const gp_XY& theUV)
  {
    myPickedCenterPnt = thePickedCenterPnt;
    myPickedPnt = thePnt;
    myPickedUV  = theUV;
  }

  //! Reset highlighting.
  virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& ,
                          const Standard_Integer ) Standard_OVERRIDE {}

  //! Handle highlighting.
  virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                 const Handle(Prs3d_Drawer)& theStyle,
                                 const Standard_Integer theMode) Standard_OVERRIDE;
private:
  Handle(Graphic3d_Aspects) myAspects;
  AIS_MaterialEditorWidgetAction myAction;
  Standard_Integer myComponent;
  gp_Pnt myPickedCenterPnt;
  gp_Pnt myPickedPnt;
  gp_XY  myPickedUV;
};

//! Sensitive quad.
class AIS_MaterialEditorWidgetSensitiveQuad : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTI_INLINE(AIS_MaterialEditorWidgetSensitiveQuad, Select3D_SensitiveEntity)
public:

  //! Main constructor.
  AIS_MaterialEditorWidgetSensitiveQuad (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                         const Graphic3d_Vec2i& theLower,
                                         const Graphic3d_Vec2i& theUpper)
  : Select3D_SensitiveEntity (theOwnerId)
  {
    myPoints[0].SetCoord (theLower.x(), theLower.y(), 0.0);
    myPoints[1].SetCoord (theLower.x(), theUpper.y(), 0.0);
    myPoints[2].SetCoord (theUpper.x(), theUpper.y(), 0.0);
    myPoints[3].SetCoord (theUpper.x(), theLower.y(), 0.0);
  }

  //! Checks whether the triangle overlaps current selecting volume
  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE
  {
    if (!theMgr.IsOverlapAllowed())
    {
      return Standard_False;
    }

    if (!theMgr.Overlaps (myPoints[0], myPoints[1], myPoints[2], Select3D_TOS_INTERIOR, thePickResult)
     && !theMgr.Overlaps (myPoints[0], myPoints[2], myPoints[3], Select3D_TOS_INTERIOR, thePickResult))
    {
      return Standard_False;
    }
    if (!thePickResult.HasPickedPoint())
    {
      return Standard_False;
    }

    if (AIS_MaterialEditorWidgetOwner* anOwner = dynamic_cast<AIS_MaterialEditorWidgetOwner* >(myOwnerId.get()))
    {
      gp_Vec aDirX (myPoints[0], gp_Pnt (myPoints[2].X(), myPoints[0].Y(), myPoints[0].Z()));
      gp_Vec aDirY (myPoints[0], gp_Pnt (myPoints[0].X(), myPoints[2].Y(), myPoints[0].Z()));
      aDirX.Normalize();
      aDirY.Normalize();
      const gp_Vec aDirP (myPoints[0], thePickResult.PickedPoint());
      const gp_Pnt aSize = myPoints[2].XYZ() - myPoints[0].XYZ();
      anOwner->SetPickedPoint (CenterOfGeometry(),
                               thePickResult.PickedPoint(),
                               gp_XY (Max (0.0, Min (1.0, aDirP.Dot (aDirX) / aSize.X())),
                                      Max (0.0, Min (1.0, aDirP.Dot (aDirY) / aSize.Y()))));
    }

    //thePickResult.SetDistToGeomCenter (theMgr.DistToGeometryCenter (CenterOfGeometry()));
    thePickResult.SetDistToGeomCenter (0.0);
    return Standard_True;
  }

  //! Returns the copy of this
  virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE
  {
    Handle(Select3D_SensitiveEntity) aCopy = new AIS_MaterialEditorWidgetSensitiveQuad (*this);
    return aCopy;
  }

  //! Returns bounding box of the triangle. If location transformation is set, it will be applied
  virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE
  {
    const SelectMgr_Vec3 aMinPnt (myPoints[0].X(), myPoints[0].Y(), myPoints[0].Z());
    const SelectMgr_Vec3 aMaxPnt (myPoints[2].X(), myPoints[2].Y(), myPoints[2].Z());
    return Select3D_BndBox3d (aMinPnt, aMaxPnt);
  }

  //! Returns the amount of points
  virtual Standard_Integer NbSubElements() Standard_OVERRIDE { return 4; }

  virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE { return (myPoints[0].XYZ() + myPoints[2].XYZ()) * 0.5; }

private:

  //! Copy constructor.
  AIS_MaterialEditorWidgetSensitiveQuad (const AIS_MaterialEditorWidgetSensitiveQuad& theCopy)
  : Select3D_SensitiveEntity (theCopy.myOwnerId)
  {
    myPoints[0] = theCopy.myPoints[0];
    myPoints[1] = theCopy.myPoints[1];
    myPoints[2] = theCopy.myPoints[2];
    myPoints[3] = theCopy.myPoints[3];
  }

private:

  gp_Pnt myPoints[4];

};

//! Widget for configuring material properties.
class AIS_MaterialEditorWidget : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(AIS_MaterialEditorWidget, AIS_InteractiveObject)
public:

  //! Empty constructor.
  AIS_MaterialEditorWidget()
  : myBackAspect (new Graphic3d_Aspects()),
    mySlideBackAspect (new Graphic3d_Aspects()),
    mySlideFillAspect (new Graphic3d_Aspects()),
    myCaptionTextAspect (new Graphic3d_AspectText3d()),
    mySlideTextAspect (new Graphic3d_AspectText3d()),
    myWidth (200),
    mySlideHeight (30)
  {
    myInfiniteState = true;
    myIsMutable = true;
    myDrawer->SetZLayer (Graphic3d_ZLayerId_TopOSD);
    myTransformPersistence = new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_RIGHT_LOWER);
    myTransformPersistence->SetOffset2d (Graphic3d_Vec2i (myWidth, 200));
    
    myBackAspect->SetInteriorColor (Quantity_NOC_GRAY80);
    myBackAspect->SetShadingModel (Graphic3d_TOSM_UNLIT);
    //myBackAspect->ChangeFrontMaterial().SetTransparency (0.5f);
    //myBackAspect->SetAlphaMode (Graphic3d_AlphaMode_Blend);

    mySlideBackAspect->SetInteriorColor (Quantity_NOC_GRAY30);
    mySlideBackAspect->SetShadingModel (Graphic3d_TOSM_UNLIT);

    mySlideFillAspect->SetInteriorColor (Quantity_NOC_STEELBLUE4);
    mySlideFillAspect->SetShadingModel (Graphic3d_TOSM_UNLIT);

    myCaptionTextAspect->SetFont (Font_NOF_SANS_SERIF);
    myCaptionTextAspect->SetTextDisplayType (Aspect_TODT_SHADOW);
    myCaptionTextAspect->SetColor (Quantity_NOC_WHITE);
    myCaptionTextAspect->SetColorSubTitle (Quantity_NOC_BLACK);
    myCaptionTextAspect->SetTextFontAspect (Font_FontAspect_Bold);

    *mySlideTextAspect = *myCaptionTextAspect;
    mySlideTextAspect->SetTextFontAspect (Font_FontAspect_Regular);
  }

  //! Set new widget width.
  Standard_Integer WidgetWidth() const { return myWidth; }

  //! Set new widget width.
  void SetWidgetWidth (Standard_Integer theWidth)
  {
    myWidth = theWidth;
    SetToUpdate();
  }

  //! Return list of attached presentations.
  const AIS_ListOfInteractive& ListOfAttachments() const { return myAttachedPrsList; }

  //! Attach widget to specified presentation list.
  void Attach (const AIS_ListOfInteractive& thePrsList)
  {
    myAttachedPrsList = thePrsList;
    myEditAspects.Nullify();
    SetToUpdate();
    if (thePrsList.IsEmpty())
    {
      return;
    }

    if (thePrsList.First()->Attributes()->SetupOwnShadingAspect())
    {
      thePrsList.First()->SetToUpdate();
    }
    Handle(Prs3d_ShadingAspect) anAspect = thePrsList.First()->Attributes()->ShadingAspect(); 
    myEditAspects = anAspect->Aspect();
    for (AIS_ListOfInteractive::Iterator aPrsIter (thePrsList); aPrsIter.More(); aPrsIter.Next())
    {
      const Handle(AIS_InteractiveObject)& aPrs = aPrsIter.Value();
      if (aPrs->Attributes()->ShadingAspect() != anAspect)
      {
        aPrs->Attributes()->SetShadingAspect (anAspect);
        aPrs->SetToUpdate();
      }
      if (aPrs->HasInteractiveContext())
      {
        if (aPrs->ToBeUpdated())
        {
          aPrs->GetContext()->Display (aPrs, false);
        }
        else
        {
          aPrs->SynchronizeAspects();
        }
      }
    }
  }

public:

  //! Accept only display mode 0.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Global selection has no meaning for this class.
  virtual Handle(SelectMgr_EntityOwner) GlobalSelOwner() const Standard_OVERRIDE { return Handle(SelectMgr_EntityOwner)(); }

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE
  {
    if (theMode != 0)
    {
      return;
    }

    bool toShowCommon = true, toShowPbr = false;
    Handle(V3d_View) aView = GetContext()->LastActiveView();
    if (aView.IsNull())
    {
      aView = GetContext()->CurrentViewer()->ActiveViews().First();
    }
    if (!aView.IsNull())
    {
      toShowPbr = aView->ShadingModel() == Graphic3d_TOSM_PBR
               || aView->ShadingModel() == Graphic3d_TOSM_PBR_FACET;
      toShowCommon = !toShowPbr;
    }

    thePrs->SetInfiniteState (true);
    mySensQuads.Clear();
    mySelOwners.Clear();

    Handle(Graphic3d_Aspects) anEditAspects = myEditAspects;
    if (anEditAspects.IsNull())
    {
      anEditAspects = new Graphic3d_Aspects();
    }

    Handle(Graphic3d_Group) aBackGroup = thePrs->NewGroup();
    aBackGroup->SetGroupPrimitivesAspect (myBackAspect);

    Handle(Graphic3d_Group) aSlideBackGroup = thePrs->NewGroup();
    aSlideBackGroup->SetGroupPrimitivesAspect (mySlideBackAspect);

    Handle(Graphic3d_Group) aSlideFillGroup = thePrs->NewGroup();
    aSlideFillGroup->SetGroupPrimitivesAspect (mySlideFillAspect);

    Handle(Graphic3d_Group) aCapGroup = thePrs->NewGroup();
    aCapGroup->SetGroupPrimitivesAspect (myCaptionTextAspect);

    Handle(Graphic3d_Group) aLabGroup  = thePrs->NewGroup();
    aLabGroup->SetGroupPrimitivesAspect (mySlideTextAspect);

    Graphic3d_Vec2i aLower  (0, 0);
    Graphic3d_Vec2i anUpper (myWidth, 0);
    {
      //Handle(Graphic3d_ArrayOfTriangles) aTris = createRectangleArray (aLower, anUpper, Graphic3d_ArrayFlags_None);
      //aBackGroup->AddPrimitiveArray (aTris);
    }

    Standard_Integer aTopIter = anUpper.y();
    addCaption (aTopIter, NULL, aCapGroup, "Material");
    addCaption (aTopIter, NULL, aLabGroup, anEditAspects->FrontMaterial().StringName());
    addSlider (aTopIter, aSlideBackGroup, NULL, aLabGroup, "RESET",
               AIS_MaterialEditorWidgetAction_Reset, 0, -Precision::Infinite());

    addCaption (aTopIter, NULL, aCapGroup, "Transparency");
    addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "",
               AIS_MaterialEditorWidgetAction_Transparency, 0, anEditAspects->FrontMaterial().Transparency());

    if (toShowPbr)
    {
      const Graphic3d_Vec3 aBaseColVec3 = anEditAspects->FrontMaterial().PBRMaterial().Color().GetRGB();
      addCaption (aTopIter, NULL, aCapGroup, "Albedo");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_PbrAlbedo, -1, aBaseColVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_PbrAlbedo, 0, aBaseColVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_PbrAlbedo, 1, aBaseColVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_PbrAlbedo, 2, aBaseColVec3.b());

      addCaption (aTopIter, NULL, aCapGroup, "Metallic");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "",
                 AIS_MaterialEditorWidgetAction_PbrMetallic, 0, anEditAspects->FrontMaterial().PBRMaterial().Metallic());
      addCaption (aTopIter, NULL, aCapGroup, "Roughness");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "",
                 AIS_MaterialEditorWidgetAction_PbrRoughness, 0, anEditAspects->FrontMaterial().PBRMaterial().NormalizedRoughness());
      addCaption (aTopIter, NULL, aCapGroup, "IOR");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "",
                 AIS_MaterialEditorWidgetAction_PbrIOR, 0,
                 anEditAspects->FrontMaterial().PBRMaterial().IOR(), Bnd_Range (1, 3));

      const Graphic3d_Vec3 anEmissVec3 = anEditAspects->FrontMaterial().PBRMaterial().Emission();
      addCaption (aTopIter, NULL, aCapGroup, "Emission");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_PbrEmission, -1, anEmissVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_PbrEmission, 0, anEmissVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_PbrEmission, 1, anEmissVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_PbrEmission, 2, anEmissVec3.b());
    }

    if (toShowCommon)
    {
      addCaption (aTopIter, NULL, aCapGroup, "Shininess");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "",
                 AIS_MaterialEditorWidgetAction_CommonShininess, 0,
                 anEditAspects->FrontMaterial().Shininess() * 128.0, Bnd_Range (0, 128));

      const Graphic3d_Vec3 anAmbVec3 = anEditAspects->FrontMaterial().AmbientColor();
      addCaption (aTopIter, NULL, aCapGroup, "Ambient");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_CommonAmbient, -1, anAmbVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_CommonAmbient, 0, anAmbVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_CommonAmbient, 1, anAmbVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_CommonAmbient, 2, anAmbVec3.b());

      const Graphic3d_Vec3 aDiffVec3 = anEditAspects->FrontMaterial().DiffuseColor();
      addCaption (aTopIter, NULL, aCapGroup, "Diffuse");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_CommonDiffuse, -1, aDiffVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_CommonDiffuse, 0, aDiffVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_CommonDiffuse, 1, aDiffVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_CommonDiffuse, 2, aDiffVec3.b());

      const Graphic3d_Vec3 aSpecVec3 = anEditAspects->FrontMaterial().SpecularColor();
      addCaption (aTopIter, NULL, aCapGroup, "Specular");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_CommonSpecular, -1, aSpecVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_CommonSpecular, 0, aSpecVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_CommonSpecular, 1, aSpecVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_CommonSpecular, 2, aSpecVec3.b());

      const Graphic3d_Vec3 anEmissVec3 = anEditAspects->FrontMaterial().EmissiveColor();
      addCaption (aTopIter, NULL, aCapGroup, "Emissive");
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "%",
                 AIS_MaterialEditorWidgetAction_CommonEmissive, -1, anEmissVec3.maxComp() * 100.0, Bnd_Range (0, 100));
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "r",
                 AIS_MaterialEditorWidgetAction_CommonEmissive, 0, anEmissVec3.r());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "g",
                 AIS_MaterialEditorWidgetAction_CommonEmissive, 1, anEmissVec3.g());
      addSlider (aTopIter, aSlideBackGroup, aSlideFillGroup, aLabGroup, "b",
                 AIS_MaterialEditorWidgetAction_CommonEmissive, 2, anEmissVec3.b());
    }

    myTransformPersistence->SetOffset2d (Graphic3d_Vec2i (myWidth, Abs (aTopIter)));
  }

  void addCaption (Standard_Integer& theTopIter,
                   const Handle(Graphic3d_Group)& theBackGroup,
                   const Handle(Graphic3d_Group)& theLabelsGroup,
                   const TCollection_AsciiString& theText)
  {
    addSlider (theTopIter, theBackGroup, NULL, theLabelsGroup, theText,
               AIS_MaterialEditorWidgetAction_Transparency, -1, -Precision::Infinite());
  }

  void addSlider (Standard_Integer& theTopIter,
                  const Handle(Graphic3d_Group)& theSlideBackGroup,
                  const Handle(Graphic3d_Group)& theSlideFillGroup,
                  const Handle(Graphic3d_Group)& theLabelsGroup,
                  const TCollection_AsciiString& theText,
                  const AIS_MaterialEditorWidgetAction theAction,
                  const Standard_Integer theActionComponent,
                  const Standard_Real theValue,
                  const Bnd_Range& theValueRange = Bnd_Range (0.0, 1.0))
  {
    const Standard_Integer aSlideGap = 8;
    const Graphic3d_Vec2i aSlideUpper (myWidth - aSlideGap, theTopIter - aSlideGap);
    const Graphic3d_Vec2i aSlideLower (aSlideGap, theTopIter - mySlideHeight + aSlideGap);
    if (!theSlideBackGroup.IsNull())
    {
      Handle(Graphic3d_ArrayOfTriangles) aTris = createRectangleArray (aSlideLower, aSlideUpper, Graphic3d_ArrayFlags_None);
      theSlideBackGroup->AddPrimitiveArray (aTris);

      Handle(AIS_MaterialEditorWidgetOwner) aSensOwner =
        new AIS_MaterialEditorWidgetOwner (this, myEditAspects, theAction, theActionComponent);
      Handle(AIS_MaterialEditorWidgetSensitiveQuad) aSensQuad = new AIS_MaterialEditorWidgetSensitiveQuad (aSensOwner, aSlideLower, aSlideUpper);
      mySelOwners.Append (aSensOwner);
      mySensQuads.Append (aSensQuad);
    }

    TCollection_AsciiString aTextVal = theText;
    if (!theSlideFillGroup.IsNull()
     && !Precision::IsInfinite (theValue))
    {
      if (!aTextVal.IsEmpty())
      {
        aTextVal += ": ";
      }
      aTextVal += theValue;

      Standard_Real aMin = 0.0, aMax = 1.0;
      theValueRange.GetBounds (aMin, aMax);
      Standard_Real aVal = (Max (Min (theValue, aMax), aMin) - aMin) / (aMax - aMin);
      const Graphic3d_Vec2i aFillUpper (aSlideLower.x() + Standard_Integer(aVal * (myWidth - aSlideGap * 2)),
                                        aSlideUpper.y());
      Handle(Graphic3d_ArrayOfTriangles) aTrisFill = createRectangleArray (aSlideLower, aFillUpper, Graphic3d_ArrayFlags_None);
      theSlideFillGroup->AddPrimitiveArray (aTrisFill);
    }

    if (!aTextVal.IsEmpty()
     && !theLabelsGroup.IsNull())
    {
      Handle(Graphic3d_Text) aText = new Graphic3d_Text ((float )myDrawer->TextAspect()->Height());
      aText->SetText (aTextVal);
      aText->SetHorizontalAlignment (Graphic3d_HTA_CENTER);
      aText->SetVerticalAlignment (Graphic3d_VTA_CENTER);
      aText->SetPosition (gp_Pnt (myWidth / 2, theTopIter - mySlideHeight / 2, 0));
      theLabelsGroup->AddText (aText);
    }

    theTopIter -= mySlideHeight;
  }

  //! Compute selection
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) Standard_OVERRIDE
  {
    if (theMode != 0)
    {
      return;
    }

    for (NCollection_Sequence<Handle(AIS_MaterialEditorWidgetSensitiveQuad)>::Iterator aSensIter (mySensQuads);
         aSensIter.More(); aSensIter.Next())
    { 
      theSel->Add (aSensIter.Value());
    }
  }

protected:

  Handle(Graphic3d_Aspects)      myEditAspects;       //!< aspects to edit
  AIS_ListOfInteractive          myAttachedPrsList;   //!< list of attached presentations

  Handle(Graphic3d_Aspects)      myBackAspect;        //!< widget background aspect
  Handle(Graphic3d_Aspects)      mySlideBackAspect;   //!< slider background aspect
  Handle(Graphic3d_Aspects)      mySlideFillAspect;   //!< slider progress aspect
  Handle(Graphic3d_AspectText3d) myCaptionTextAspect; //!< caption text aspect
  Handle(Graphic3d_AspectText3d) mySlideTextAspect;   //!< slider  text aspect
  Standard_Integer               myWidth;             //!< widget width
  Standard_Integer               mySlideHeight;       //!< slider height

  NCollection_Sequence<Handle(AIS_MaterialEditorWidgetSensitiveQuad)> mySensQuads;
  NCollection_Sequence<Handle(AIS_MaterialEditorWidgetOwner)> mySelOwners;
};

void AIS_MaterialEditorWidgetOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                                                      const Handle(Prs3d_Drawer)& ,
                                                      const Standard_Integer )
{
  if (thePrsMgr->IsImmediateModeOn())
  {
    return;
  }

  Handle(AIS_MaterialEditorWidget) aWidget = Handle(AIS_MaterialEditorWidget)::DownCast (Selectable());
  if (aWidget.IsNull())
  {
    throw Standard_ProgramError ("Internal Error within AIS_MaterialWidgetOwnerOwner::HilightWithColor()!");
  }

  float aClamped = (float )Max (Min (myPickedUV.X(), 1.0), 0.0);
  switch (myAction)
  {
    case AIS_MaterialEditorWidgetAction_Reset:
    {
      myAspects->ChangeFrontMaterial() = myAspects->FrontMaterial().RequestedName();
      break;
    }
    case AIS_MaterialEditorWidgetAction_Transparency:
    {
      myAspects->ChangeFrontMaterial().SetTransparency (aClamped);
      break;
    }
    case AIS_MaterialEditorWidgetAction_PbrMetallic:
    {
      Graphic3d_PBRMaterial aPbr = myAspects->ChangeFrontMaterial().PBRMaterial();
      aPbr.SetMetallic (aClamped);
      myAspects->ChangeFrontMaterial().SetPBRMaterial (aPbr);
      break;
    }
    case AIS_MaterialEditorWidgetAction_PbrRoughness:
    {
      Graphic3d_PBRMaterial aPbr = myAspects->ChangeFrontMaterial().PBRMaterial();
      aPbr.SetRoughness (aClamped);
      myAspects->ChangeFrontMaterial().SetPBRMaterial (aPbr);
      break;
    }
    case AIS_MaterialEditorWidgetAction_PbrIOR:
    {
      Graphic3d_PBRMaterial aPbr = myAspects->ChangeFrontMaterial().PBRMaterial();
      aPbr.SetIOR (1.0f + aClamped * 2.0f);
      myAspects->ChangeFrontMaterial().SetPBRMaterial (aPbr);
      break;
    }
    case AIS_MaterialEditorWidgetAction_PbrAlbedo:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().PBRMaterial().Color().GetRGB();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }

      Graphic3d_PBRMaterial aPbr = myAspects->ChangeFrontMaterial().PBRMaterial();
      aPbr.SetColor (Quantity_Color (aVec));
      myAspects->ChangeFrontMaterial().SetPBRMaterial (aPbr);
      break;
    }
    case AIS_MaterialEditorWidgetAction_PbrEmission:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().PBRMaterial().Emission();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }

      Graphic3d_PBRMaterial aPbr = myAspects->ChangeFrontMaterial().PBRMaterial();
      aPbr.SetEmission (aVec);
      myAspects->ChangeFrontMaterial().SetPBRMaterial (aPbr);
      break;
    }
    case AIS_MaterialEditorWidgetAction_CommonShininess:
    {
      myAspects->ChangeFrontMaterial().SetShininess (aClamped);
      break;
    }
    case AIS_MaterialEditorWidgetAction_CommonAmbient:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().AmbientColor();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }
      myAspects->ChangeFrontMaterial().SetAmbientColor (Quantity_Color (aVec));
      break;
    }
    case AIS_MaterialEditorWidgetAction_CommonDiffuse:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().DiffuseColor();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }
      myAspects->ChangeFrontMaterial().SetDiffuseColor (Quantity_Color (aVec));
      break;
    }
    case AIS_MaterialEditorWidgetAction_CommonSpecular:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().SpecularColor();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }
      myAspects->ChangeFrontMaterial().SetSpecularColor (Quantity_Color (aVec));
      break;
    }
    case AIS_MaterialEditorWidgetAction_CommonEmissive:
    {
      Graphic3d_Vec3 aVec = myAspects->FrontMaterial().EmissiveColor();
      if (myComponent == -1)
      {
        const float aMaxComp = aVec.maxComp() > 0.0f ? aVec.maxComp() : 1.0f;
        aVec = (aVec / aMaxComp) * aClamped;
      }
      else
      {
        aVec[myComponent] = aClamped;
      }
      myAspects->ChangeFrontMaterial().SetEmissiveColor (Quantity_Color (aVec));
      break;
    }
    case AIS_MaterialEditorWidgetAction_NB:
      return;
  }

  for (AIS_ListOfInteractive::Iterator aPrsIter (aWidget->ListOfAttachments()); aPrsIter.More(); aPrsIter.Next())
  {
    aPrsIter.Value()->SynchronizeAspects();
  }
  aWidget->GetContext()->Redisplay (aWidget, false);
}

//==============================================================================
//function : VMaterialWidget
//purpose  :
//==============================================================================
static Standard_Integer VMaterialWidget (Draw_Interpretor& ,
                                         Standard_Integer  theArgNb,
                                         const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view\n";
    return 1;
  }

  TCollection_AsciiString aWidgetName;
  Handle(AIS_MaterialEditorWidget) aWidget;
  AIS_ListOfInteractive aPrsList;
  Standard_Integer aWidth = -1;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    if (anArgCase == "-width"
     && anArgIter + 1 < theArgNb)
    {
      aWidth = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else if (aWidgetName.IsEmpty())
    {
      aWidgetName = theArgVec[anArgIter];
      Handle(AIS_InteractiveObject) aPrs;
      if (GetMapOfAIS().Find2 (aWidgetName, aPrs))
      {
        aWidget = Handle(AIS_MaterialEditorWidget)::DownCast (aPrs);
      }
      if (aWidget.IsNull())
      {
        aWidget = new AIS_MaterialEditorWidget();
      }
      else
      {
        aWidget->SetToUpdate();
      }
    }
    else
    {
      Handle(AIS_InteractiveObject) aPrs;
      if (GetMapOfAIS().Find2 (theArgVec[anArgIter], aPrs))
      {
        aPrsList.Append (aPrs);
      }
      else
      {
        std::cout << "Syntax error: object '" << theArgVec[anArgIter] << "' is not displayed\n";
        return 1;
      }
    }
  }
  if (aWidget.IsNull())
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }
  if (aWidth != -1)
  {
    aWidget->SetWidgetWidth (aWidth);
  }
  aWidget->Attach (aPrsList);

  ViewerTest::Display (aWidgetName, aWidget);
  return 0;
}

//==============================================================================
//function : envlutWriteToFile
//purpose  :
//==============================================================================
static std::string envLutWriteToFile (Standard_ShortReal theValue)
{
  std::stringstream aStream;
  aStream << theValue;
  if (aStream.str().length() == 1)
  {
    aStream << '.';
  }
  aStream << 'f';
  return aStream.str();
}

//==============================================================================
//function : VGenEnvLUT
//purpose  :
//==============================================================================
static Standard_Integer VGenEnvLUT (Draw_Interpretor&,
                                    Standard_Integer  theArgNb,
                                    const char**      theArgVec)
{
  Standard_Integer aTableSize = -1;
  Standard_Integer aNbSamples = -1;
  TCollection_AsciiString aFilePath = Graphic3d_TextureRoot::TexturesFolder() + "/Textures_EnvLUT.pxx";

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();

    if (anArg == "-size"
     || anArg == "-s")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cerr << "Syntax error: size of PBR environment look up table is undefined" << "\n";
        return 1;
      }

      aTableSize = Draw::Atoi(theArgVec[++anArgIter]);

      if (aTableSize < 16)
      {
        std::cerr << "Error: size of PBR environment look up table must be greater or equal 16\n";
        return 1;
      }
    }
    else if (anArg == "-nbsamples"
          || anArg == "-samples")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cerr << "Syntax error: number of samples to generate PBR environment look up table is undefined" << "\n";
        return 1;
      }

      aNbSamples = Draw::Atoi(theArgVec[++anArgIter]);

      if (aNbSamples < 1)
      {
        std::cerr << "Syntax error: number of samples to generate PBR environment look up table must be greater than 1\n" << "\n";
        return 1;
      }
    }
    else
    {
      std::cerr << "Syntax error: unknown argument " << anArg << ";\n";
      return 1;
    }
  }

  if (aTableSize < 0)
  {
    aTableSize = 128;
  }

  if (aNbSamples < 0)
  {
    aNbSamples = 1024;
  }

  std::ofstream aFile;

  OSD_OpenStream (aFile, aFilePath, std::ios::out | std::ios::binary);

  if (!aFile.good())
  {
    std::cerr << "Error: unable to write to " << aFilePath << "\n";
    return 1;
  }

  aFile << "//this file has been generated by vgenenvlut draw command\n";
  aFile << "static unsigned int Textures_EnvLUTSize = " << aTableSize << ";\n\n";
  aFile << "static float Textures_EnvLUT[] =\n";
  aFile << "{\n";

  Handle(Image_PixMap) aPixMap = new Image_PixMap();
  aPixMap->InitZero (Image_Format_RGF, aTableSize, aTableSize);
  Graphic3d_PBRMaterial::GenerateEnvLUT (aPixMap, aNbSamples);

  const Standard_Integer aNumbersInRow = 5;
  Standard_Integer aCounter = 0;

  for (int y = 0; y < aTableSize - 1; ++y)
  {
    aCounter = 0;
    for (int x = 0; x < aTableSize; ++x)
    {
      aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(aTableSize - 1 - y, x).x()) << ",";
      aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(aTableSize - 1 - y, x).y()) << ",";
      if (++aCounter % aNumbersInRow == 0)
      {
        aFile << "\n";
      }
      else if (x != aTableSize - 1)
      {
        aFile << " ";
      }
    }
    aFile << "\n";
    if (aTableSize % aNumbersInRow != 0)
    {
      aFile << "\n";
    }
  }

  aCounter = 0;
  for (int x = 0; x < aTableSize - 1; ++x)
  {
    aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(0, x).x()) << ",";
    aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(0, x).y()) << ",";
    if (++aCounter % aNumbersInRow == 0)
    {
      aFile << "\n";
    }
    else
    {
      aFile << " ";
    }
  }

  aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(0, aTableSize - 1).x()) << ",";
  aFile << envLutWriteToFile (aPixMap->Value<Graphic3d_Vec3>(0, aTableSize - 1).y()) << "\n";

  aFile << "};";
  aFile.close();

  return 0;
}

//=======================================================================
//function : OpenGlCommands
//purpose  :
//=======================================================================

void ViewerTest::OpenGlCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup ="Commands for low-level TKOpenGl features";

  theCommands.Add("vuserdraw",
    "vuserdraw : name - simulates drawing with help of UserDraw",
    __FILE__, VUserDraw, aGroup);
  theCommands.Add("vfeedback",
    "vfeedback       : perform test GL feedback rendering",
    __FILE__, VFeedback, aGroup);
  theCommands.Add("vimmediatefront",
    "vimmediatefront : render immediate mode to front buffer or to back buffer",
    __FILE__, VImmediateFront, aGroup);
  theCommands.Add("vglinfo",
                "vglinfo [-short|-basic|-complete]"
        "\n\t\t:         [GL_VENDOR] [GL_RENDERER] [GL_VERSION]"
        "\n\t\t:         [GL_SHADING_LANGUAGE_VERSION] [GL_EXTENSIONS]"
        "\n\t\t: print OpenGL info",
    __FILE__, VGlInfo, aGroup);
  theCommands.Add("vshader",
                  "vshader name -vert VertexShader -frag FragmentShader [-geom GeometryShader]"
                  "\n\t\t:   [-off] [-phong] [-aspect {shading|line|point|text}=shading]"
                  "\n\t\t:   [-header VersionHeader]"
                  "\n\t\t:   [-tessControl TessControlShader -tesseval TessEvaluationShader]"
                  "\n\t\t:   [-uniform Name FloatValue]"
                  "\n\t\t: Assign custom GLSL program to presentation aspects."
                  "\nvshader [-list] [-dump] [-reload] ShaderId"
                  "\n\t\t:  -list   prints the list of registered GLSL programs"
                  "\n\t\t:  -dump   dumps specified GLSL program (for debugging)"
                  "\n\t\t:  -reload restores dump of specified GLSL program",
    __FILE__, VShaderProg, aGroup);
  theCommands.Add("vshaderprog", "Alias for vshader", __FILE__, VShaderProg, aGroup);
  theCommands.Add("vlistmaterials",
                  "vlistmaterials [*] [MaterialName1 [MaterialName2 [...]]] [dump.obj|dump.html]"
                  "\n\t\t: Without arguments, command prints the list of standard materials."
                  "\n\t\t: Otherwise, properties of specified materials will be printed"
                  "\n\t\t: or dumped into specified file."
                  "\n\t\t: * can be used to refer to complete list of standard materials.",
                  __FILE__, VListMaterials, aGroup);
  theCommands.Add("vlistcolors",
                  "vlistcolors [*] [ColorName1 [ColorName2 [...]]] [dump.html]"
                  "\n\t\t: Without arguments, command prints the list of standard colors."
                  "\n\t\t: Otherwise, properties of specified colors will be printed"
                  "\n\t\t: or dumped into specified file."
                  "\n\t\t: * can be used to refer to complete list of standard colors.",
                  __FILE__, VListColors, aGroup);
  theCommands.Add("vmaterialwidget",
                  "vmaterialwidget widgetName prsName1 [prsName2 [...]]"
                  "\n\t\t: Create a widget for configuring material properties of specified presentations.",
                  __FILE__, VMaterialWidget, aGroup);
  theCommands.Add("vgenenvlut",
                  "vgenenvlut [-size size = 128] [-nbsamples nbsamples = 1024]"
                  "\n\t\t: Generates PBR environment look up table."
                  "\n\t\t: Saves it as C++ source file which is expected to be included in code."
                  "\n\t\t: The path where result will be located is 'Graphic3d_TextureRoot::TexturesFolder()'."
                  "\n\t\t:  -size size of one side of resulted square table"
                  "\n\t\t:  -nbsamples number of samples used in Monte-Carlo integration",
                  __FILE__, VGenEnvLUT, aGroup);
}
