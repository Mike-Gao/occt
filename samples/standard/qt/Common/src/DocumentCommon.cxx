#include "DocumentCommon.h"

#include "ApplicationCommon.h"
#include "Transparency.h"
#include "Material.h"

#include <Standard_WarningsDisable.hxx>
#include <QStatusBar>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <Standard_WarningsRestore.hxx>

#include <Aspect_DisplayConnection.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
#include <OSD_Environment.hxx>
#endif
#include <TCollection_AsciiString.hxx>

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer (const Standard_ExtString ,
                                           const Standard_CString ,
                                           const Standard_Real theViewSize,
                                           const V3d_TypeOfOrientation theViewProj,
                                           const Standard_Boolean theComputedMode,
                                           const Standard_Boolean theDefaultComputedMode )
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;

  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection (OSD_Environment ("DISPLAY").Value());
#endif
    aGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);
  }

  Handle(V3d_Viewer) aViewer = new V3d_Viewer (aGraphicDriver);
  aViewer->SetDefaultViewSize (theViewSize);
  aViewer->SetDefaultViewProj (theViewProj);
  aViewer->SetComputedMode (theComputedMode);
  aViewer->SetDefaultComputedMode (theDefaultComputedMode);
  return aViewer;
}

DocumentCommon::DocumentCommon(ApplicationCommonWindow* app ): 
  QObject( app ),
  myApp( app ),
  myContextIsEmpty(true)
{
  TCollection_ExtendedString a3DName ("Visu3D");

  myViewer = Viewer (a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  myContext = new AIS_InteractiveContext (myViewer);
}

DocumentCommon::~DocumentCommon()
{
}

ApplicationCommonWindow* DocumentCommon::getApplication()
{
  return myApp;
}

Handle(AIS_InteractiveContext) DocumentCommon::getContext()
{
  return myContext;
}

void DocumentCommon::SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects)
{
  myContext->RemoveAll(Standard_True);
  if (theObjects.IsEmpty())
    myContextIsEmpty = true;
  else 
    myContextIsEmpty = false;
  for (const Handle(AIS_InteractiveObject) aObject : theObjects)
  {
    myContext->Display(aObject, Standard_True);
  }
}

void DocumentCommon::onWireframe()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected() )
        myContext->SetDisplayMode( myContext->SelectedInteractive(), 0, false );
    myContext->UpdateCurrentViewer();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onShading()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected() )
        myContext->SetDisplayMode( myContext->SelectedInteractive(), 1, false );
    myContext->UpdateCurrentViewer();
    QApplication::restoreOverrideCursor();
}

void DocumentCommon::onColor()
{
    QColor aColor ;
    myContext->InitSelected();
    Handle(AIS_InteractiveObject) Current = myContext->SelectedInteractive();
    if (!Current)
      return;
    if ( Current->HasColor () )
    {
        Quantity_Color aShapeColor;
        myContext->Color( Current, aShapeColor );
        aColor.setRgb( (Standard_Integer)(aShapeColor.Red() * 255), (Standard_Integer)(aShapeColor.Green() * 255),
                       (Standard_Integer)(aShapeColor.Blue() * 255));
    }
    else
        aColor.setRgb( 255, 255, 255 );

    QColor aRetColor = QColorDialog::getColor( aColor );
    if ( aRetColor.isValid() )
    {
        Quantity_Color color( aRetColor.red() / 255., aRetColor.green() / 255.,
                        aRetColor.blue() / 255., Quantity_TOC_RGB );
        for (; myContext->MoreSelected(); myContext->NextSelected() )
            myContext->SetColor( myContext->SelectedInteractive(), color, Standard_False);
        myContext->UpdateCurrentViewer();
    }
}

void DocumentCommon::onMaterial( int theMaterial )
{
    for ( myContext->InitSelected(); myContext->MoreSelected (); myContext->NextSelected () )
        myContext->SetMaterial( myContext->SelectedInteractive(), (Graphic3d_NameOfMaterial)theMaterial, Standard_False);
    myContext->UpdateCurrentViewer();
}

void DocumentCommon::onMaterial()
{
    DialogMaterial* m = new DialogMaterial();
    connect( m, SIGNAL( sendMaterialChanged( int ) ), this, SLOT( onMaterial( int ) ) );
    m->exec();
}

void DocumentCommon::onTransparency( int theTrans )
{
    for( myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected() )
        myContext->SetTransparency (myContext->SelectedInteractive(), ((Standard_Real)theTrans) / 10.0, Standard_False);
    myContext->UpdateCurrentViewer();
}

void DocumentCommon::onTransparency()
{
    DialogTransparency* aDialog = new DialogTransparency();
    connect( aDialog, SIGNAL( sendTransparencyChanged( int ) ), this, SLOT( onTransparency( int ) ) );
    aDialog->exec();
}

void DocumentCommon::onDelete()
{
    myContext->EraseSelected (Standard_False);
    myContext->ClearSelected (Standard_False);
    myContext->UpdateCurrentViewer();
}
