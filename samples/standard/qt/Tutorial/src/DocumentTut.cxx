#include "DocumentTut.h"

#include <Standard_WarningsDisable.hxx>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <Standard_WarningsRestore.hxx>

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

TopoDS_Shape
MakeBottle(const Standard_Real myWidth , const Standard_Real myHeight , const Standard_Real myThickness);

DocumentTut::DocumentTut(ApplicationCommonWindow* app)
: DocumentCommon(app)
{
}

DocumentTut::~DocumentTut()
{
}

void DocumentTut::onMakeBottle()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    TopoDS_Shape aBottle=MakeBottle(50,70,30);
    Handle(AIS_Shape) AISBottle=new AIS_Shape(aBottle);
    myContext->SetMaterial (AISBottle, Graphic3d_NOM_GOLD, Standard_False);
    myContext->SetDisplayMode(AISBottle, 1, Standard_False);
    myContext->Display(AISBottle, Standard_False);
    const Handle(AIS_InteractiveObject)& anIOAISBottle = AISBottle;
    myContext->SetSelected(anIOAISBottle,Standard_False);
    emit selectionChanged();
    QApplication::restoreOverrideCursor();
}


