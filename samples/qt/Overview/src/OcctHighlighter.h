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

#ifndef OCCTHIGHLIGHTER_H
#define OCCTHIGHLIGHTER_H

#include <Standard_WarningsDisable.hxx>
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>
#include <QString>
#include <QTextCharFormat>
#include <Standard_WarningsRestore.hxx>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class OcctHighlighter: public QSyntaxHighlighter
{
  Q_OBJECT

public:
  OcctHighlighter(QTextDocument *parent = 0);

protected:
  void highlightBlock(const QString &theText) override;

private:
  struct HighlightingRule
  {
    QRegularExpression myPattern;
    QTextCharFormat myFormat;
  };
  QVector<HighlightingRule> myHighlightingRules;

  QRegularExpression myCommentStartExpression;
  QRegularExpression myCommentEndExpression;

  QTextCharFormat myKeywordFormat;
  QTextCharFormat mySingleLineCommentFormat;
  QTextCharFormat myMultiLineCommentFormat;
  QTextCharFormat myQuotationFormat;
  QTextCharFormat myFunctionFormat;  
  QTextCharFormat myOcctFormat;
  QTextCharFormat myMemberFormat;
  QTextCharFormat myLocalFormat;
  QTextCharFormat myHelperFormat;
};

#endif
