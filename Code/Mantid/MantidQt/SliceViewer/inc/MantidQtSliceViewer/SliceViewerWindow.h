#ifndef SLICEVIEWERWINDOW_H
#define SLICEVIEWERWINDOW_H

#include "MantidAPI/IMDWorkspace.h"
#include "MantidQtAPI/WorkspaceObserver.h"
#include "MantidQtSliceViewer/LineViewer.h"
#include "MantidQtSliceViewer/SliceViewer.h"
#include <QtGui/QMainWindow>
#include <QtGui/QSplitter>
#include <QtGui/qdialog.h>
#include <qmainwindow.h>
#include <QShowEvent>
#include "DllOption.h"
#include "MantidKernel/VMD.h"


/** A window that contains a SliceViewer and a LineViewer widget,
 * linked together.
 *
 * @author Janik Zikovsky
 * @date October 13, 2011
 */
class EXPORT_OPT_MANTIDQT_SLICEVIEWER SliceViewerWindow : public QMainWindow, public MantidQt::API::WorkspaceObserver
{
    Q_OBJECT

public:
  SliceViewerWindow(const QString& wsName, QWidget *app , const QString& label = QString() , Qt::WFlags f=0);
  ~SliceViewerWindow();

private:
  void setLineViewerValues(QPointF start2D, QPointF end2D, double width);
  void resizeEvent(QResizeEvent * event);

signals:
  void needToClose();
  void needToUpdate();

protected slots:
  void closeWindow();
  void updateWorkspace();
  void changedSlicePoint(Mantid::Kernel::VMD);
  void lineChanging(QPointF start, QPointF end, double width);
  void lineChanged(QPointF start, QPointF end, double width);
  void showLineViewer(bool);
  void changeStartOrEnd(Mantid::Kernel::VMD, Mantid::Kernel::VMD);
  void changePlanarWidth(double);

protected:
  void deleteHandle(const std::string& wsName,const boost::shared_ptr<Mantid::API::Workspace> ws);
  void afterReplaceHandle(const std::string& wsName,const boost::shared_ptr<Mantid::API::Workspace> ws);

  /// The SliceViewer
  MantidQt::SliceViewer::SliceViewer * m_slicer;

  /// The LineViewer
  MantidQt::SliceViewer::LineViewer * m_liner;

  /// Horizontal splitter between slice viewer and LineViewer
  QSplitter * m_splitter;

  /// Workspace being looked at
  Mantid::API::IMDWorkspace_sptr m_ws;
  /// Name of the workspace being viewed
  std::string m_wsName;

  /// Width of the LineViewer last time it was open
  int m_lastLinerWidth;

};

#endif // SLICEVIEWERWINDOW_H
