// Created on: 2018-08-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef ViewControl_ColorSelector_H
#define ViewControl_ColorSelector_H

#include <inspector/ViewControl.hxx>

#include <Quantity_ColorRGBA.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QColor>
#include <QDialog>
//#include <QPushButton>
#include <QItemSelection>
#include <QString>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class ViewControl_ColorPicker;
class QDialogButtonBox;
class QTableView;

//! \class ViewControl_ColorSelector
//! \brief Selector of OCCT color
class VIEWCONTROL_EXPORT ViewControl_ColorSelector : public QDialog
{
  Q_OBJECT
public:

  //! Constructor
  ViewControl_ColorSelector (QWidget* theParent);

  //! Destructor
  virtual ~ViewControl_ColorSelector() Standard_OVERRIDE {}

  //! Inits control by the color value
  //! \param theColor text color value
  void SetColor (const QString& theColor);

  //! Returns current selected color value
  //! \return text color value
  QString GetColor() const;

  //! Updates OCCT color model by changing color in parameter model
  void ParameterColorChanged();

  //! Converts color to string value in form: r;g;b
  //! \param theColor color value
  //! \return text value
  static QString ColorToString (const Quantity_Color& theColor);

  //! Converts color to string value in form: r;g;b;alpha
  //! \param theColor color value
  //! \return text value
  static QString ColorToString (const Quantity_ColorRGBA& theColor);

  //! Converts color to QColor value in form: r;g;b
  //! \param theColor color value
  //! \return qt color value
  static QColor ColorToQColor (const Quantity_Color& theColor);

  //! Converts color to QColor value in form: r;g;b;a
  //! \param theColor color value
  //! \return qt color value
  static QColor ColorToQColor (const Quantity_ColorRGBA& theColor);

  //! Converts string to color value from a form: r;g;b;a
  //! \param theColor text color value
  //! \return color value
  static Quantity_Color StringToColor (const QString& theColor, Standard_ShortReal& theAlpha);

  //! Converts string to color value from a form: r;g;b
  //! \param theColor text color value
  //! \return color value
  static Quantity_Color StringToColor (const QString& theColor);

  //! Converts string to color value from a form: r;g;b;a
  //! \param theColor text color value
  //! \return color value
  static Quantity_ColorRGBA StringToColorRGBA (const QString& theColor);

  static Standard_Boolean IsExactColorName (const Quantity_ColorRGBA& theColor,
                                            Quantity_NameOfColor& theColorName);

private:
  //! Returns symbol used as a separtor of color components in string conversion
  //! \return symbol value
  static QString ColorSeparator() { return ";"; }

private slots:
  //! Slots listen selection change and update the current control content by selection
  //! \param theSelected container of selected items
  //! \param theDeselected container of items that become deselected
  void onParametersTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

  //! Slots listen selection change and update the current control content by selection
  //! \param theSelected container of selected items
  //! \param theDeselected container of items that become deselected
  void onOCCTColorsTableSelectionChanged (const QItemSelection& theSelected, const QItemSelection& theDeselected);

private:
  QTableView* myParameters; //! current color parameters (RGB, alpha, color name)
  ViewControl_ColorPicker* myColorPicker; //! color picker
  QTableView* myOCCTColors; //! OCCT color values
  QDialogButtonBox* myDialogButtons; //! OK/Cancel buttons
};
#endif
