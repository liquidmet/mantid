#ifndef LINEVIEWER_H
#define LINEVIEWER_H

#include "DllOption.h"
#include "MantidAPI/IMDWorkspace.h"
#include "MantidKernel/VMD.h"
#include "ui_LineViewer.h"
#include <QtGui/QWidget>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>

namespace MantidQt
{
namespace SliceViewer
{

class EXPORT_OPT_MANTIDQT_SLICEVIEWER LineViewer : public QWidget
{
    Q_OBJECT

public:
    LineViewer(QWidget *parent = 0);
    ~LineViewer();

    void setWorkspace(Mantid::API::IMDWorkspace_sptr ws);
    void setStart(Mantid::Kernel::VMD start);
    void setEnd(Mantid::Kernel::VMD end);
    void setWidth(Mantid::Kernel::VMD width);
    void setPlanarWidth(double width);
    void setNumBins(size_t numBins);
    void setFreeDimensions(bool all, int dimX, int dimY);

    void showPreview();
    void showFull();

    double getPlanarWidth() const;
    Mantid::Kernel::VMD getWidth() const;

private:
    void createDimensionWidgets();
    void updateFreeDimensions();
    void updateStartEnd();
    void readTextboxes();
    void calculateCurve(Mantid::API::IMDWorkspace_sptr ws, Mantid::Kernel::VMD start, Mantid::Kernel::VMD end,
        size_t minNumPoints, QwtPlotCurve * curve);

public slots:
    void startEndTextEdited();
    void widthTextEdited();
    void startLinkedToEndText();
    void apply();
    void numBinsChanged();
    void adaptiveBinsChanged();
    void setFreeDimensions(size_t dimX, size_t dimY);

signals:
    /// Signal emitted when the planar width changes
    void changedPlanarWidth(double);
    /// Signal emitted when the start or end position has changed
    void changedStartOrEnd(Mantid::Kernel::VMD, Mantid::Kernel::VMD);


private:
    // -------------------------- Widgets ----------------------------

    /// Auto-generated UI controls.
    Ui::LineViewerClass ui;

    /// Layout containing the plot
    QHBoxLayout * m_plotLayout;

    /// Main plot object
    QwtPlot * m_plot;

    /// Curve of the preview
    QwtPlotCurve * m_previewCurve;

    /// Curve of the full integrated
    QwtPlotCurve * m_fullCurve;

    /// Vector of labels with the dimension names
    QVector<QLabel *> m_dimensionLabel;
    /// Vector of text boxes with the start point
    QVector<QLineEdit *> m_startText;
    /// Vector of text boxes with the end point
    QVector<QLineEdit *> m_endText;
    /// Vector of text boxes with the widths
    QVector<QLineEdit *> m_widthText;


    // -------------------------- Data Members ----------------------------

    /// Workspace being sliced
    Mantid::API::IMDWorkspace_sptr m_ws;

    /// Workspace of the slice
    Mantid::API::IMDWorkspace_sptr m_sliceWS;

    /// Start point of the line
    Mantid::Kernel::VMD m_start;
    /// End point of the line
    Mantid::Kernel::VMD m_end;
    /// Width in each dimension (some will be ignored)
    Mantid::Kernel::VMD m_width;


    /// Number of bins (for regular spacing)
    size_t m_numBins;

    /// Flag that is true when all dimensions are allowed to change
    bool m_allDimsFree;
    /// Index of the X dimension in the 2D slice
    int m_freeDimX;
    /// Index of the Y dimension in the 2D slice
    int m_freeDimY;

};

} //namespace
}
#endif // LINEVIEWER_H
