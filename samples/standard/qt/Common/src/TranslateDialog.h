#ifndef TRANSLATEDIALOG_H
#define TRANSLATEDIALOG_H

#include <QFileDialog>
#include <QWidget>
#include <QShowEvent>
#include <QListView>
#include <QComboBox>
#include <QList>

class TranslateDialog : public QFileDialog
{
public:
  TranslateDialog(QWidget* = 0, Qt::WindowFlags flags = 0, bool = true);
  ~TranslateDialog();
  int                   getMode() const;
  void                  setMode(const int);
  void                  addMode(const int, const QString&);
  void                  clear();

protected:
  void                  showEvent(QShowEvent* event);

private:
  QListView*            findListView(const QObjectList&);

private:
  QComboBox*            myBox;
  QList<int>            myList;
};

#endif // TRANSLATEDIALOG_H
