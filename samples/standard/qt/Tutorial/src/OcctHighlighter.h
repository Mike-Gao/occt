#ifndef OCCTHIGHLIGHTER_H
#define OCCTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextDocument>
#include <QString>
#include <QTextCharFormat>

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
