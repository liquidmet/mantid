#include "MantidQtWidgets/SliceViewer/QPeaksTableModel.h"
#include "MantidAPI/IPeaksWorkspace.h"
#include "MantidGeometry/Crystal/IPeak.h"
#include "MantidGeometry/IComponent.h"
#include "MantidGeometry/Instrument/DetectorInfo.h"
#include "MantidKernel/ConfigService.h"
#include "MantidKernel/FacilityInfo.h"
#include "MantidKernel/InstrumentInfo.h"
#include <boost/lexical_cast.hpp>

using namespace Mantid::API;
using namespace Mantid::Geometry;
using namespace boost;

namespace MantidQt {
namespace SliceViewer {
const QString QPeaksTableModel::RUNNUMBER = "Run";
const QString QPeaksTableModel::DETID = "DetID";
const QString QPeaksTableModel::H = "h";
const QString QPeaksTableModel::K = "k";
const QString QPeaksTableModel::L = "l";
const QString QPeaksTableModel::WAVELENGTH("Wavelength");
const QString QPeaksTableModel::ENERGY("delta E");
const QString QPeaksTableModel::INITIAL_ENERGY("E_i");
const QString QPeaksTableModel::FINAL_ENERGY("E_f");
const QString QPeaksTableModel::TOF("TOF");
const QString QPeaksTableModel::DSPACING = "DSpacing";
const QString QPeaksTableModel::INT = "Int";
const QString QPeaksTableModel::SIGMINT = "SigInt";
const QString QPeaksTableModel::INT_SIGINT("Int/SigInt");
const QString QPeaksTableModel::BINCOUNT("BinCount");
const QString QPeaksTableModel::BANKNAME("BankName");
const QString QPeaksTableModel::ROW("Row");
const QString QPeaksTableModel::COL("Col");
const QString QPeaksTableModel::QLAB = "QLab";
const QString QPeaksTableModel::QSAMPLE = "QSample";

const int QPeaksTableModel::COL_RUNNUMBER(0);
const int QPeaksTableModel::COL_DETID(1);
const int QPeaksTableModel::COL_H(2);
const int QPeaksTableModel::COL_K(3);
const int QPeaksTableModel::COL_L(4);
const int QPeaksTableModel::COL_WAVELENGTH(5);
const int QPeaksTableModel::COL_INITIAL_ENERGY(6);
const int QPeaksTableModel::COL_FINAL_ENERGY(7);
const int QPeaksTableModel::COL_ENERGY(8);
const int QPeaksTableModel::COL_TOF(9);
const int QPeaksTableModel::COL_DSPACING(10);
const int QPeaksTableModel::COL_INT(11);
const int QPeaksTableModel::COL_SIGMINT(12);
const int QPeaksTableModel::COL_INT_SIGINT(13);
const int QPeaksTableModel::COL_BINCOUNT(14);
const int QPeaksTableModel::COL_BANKNAME(15);
const int QPeaksTableModel::COL_ROW(16);
const int QPeaksTableModel::COL_COL(17);
const int QPeaksTableModel::COL_QLAB(18);
const int QPeaksTableModel::COL_QSAMPLE(19);

void QPeaksTableModel::updateDataCache(const Mantid::Geometry::IPeak &peak,
                                       const int row) const {
  // if the index is what is already cached just return
  if (row == m_dataCachePeakIndex)
    return;

  // generate the cache
  m_dataCache.clear();
  m_dataCache.push_back(QString::number(peak.getRunNumber()));
  m_dataCache.push_back(QString::number(peak.getDetectorID()));
  m_dataCache.push_back(QString::number(peak.getH(), 'f', m_hklPrec));
  m_dataCache.push_back(QString::number(peak.getK(), 'f', m_hklPrec));
  m_dataCache.push_back(QString::number(peak.getL(), 'f', m_hklPrec));
  m_dataCache.push_back(QString::number(peak.getWavelength(), 'f', 4));
  double eI = peak.getInitialEnergy();
  double eF = peak.getFinalEnergy();
  m_dataCache.push_back(QString::number(eI, 'f', 4));
  m_dataCache.push_back(QString::number(eF, 'f', 4));
  m_dataCache.push_back(QString::number(eI - eF, 'f', 4));
  m_dataCache.push_back(QString::number(peak.getTOF(), 'f', 1));
  m_dataCache.push_back(QString::number(peak.getDSpacing(), 'f', 4));
  double intensity = peak.getIntensity();
  double sigma = peak.getSigmaIntensity();
  m_dataCache.push_back(QString::number(intensity, 'f', 1));
  m_dataCache.push_back(QString::number(sigma, 'f', 1));
  m_dataCache.push_back(QString::number(intensity / sigma, 'f', 2));
  m_dataCache.push_back(QString::number(peak.getBinCount(), 'g', 2));
  m_dataCache.push_back(QString(peak.getBankName().c_str()));
  m_dataCache.push_back(QString::number(peak.getRow()));
  m_dataCache.push_back(QString::number(peak.getCol()));

  const QString COMMA(",");

  const Mantid::Kernel::V3D qlab = peak.getQLabFrame();
  m_dataCache.push_back(QString::number(qlab.X(), 'f', 4) + COMMA +
                        QString::number(qlab.Y(), 'f', 4) + COMMA +
                        QString::number(qlab.Z(), 'f', 4));

  const Mantid::Kernel::V3D qsample = peak.getQSampleFrame();
  m_dataCache.push_back(QString::number(qsample.X(), 'f', 4) + COMMA +
                        QString::number(qsample.Y(), 'f', 4) + COMMA +
                        QString::number(qsample.Z(), 'f', 4));
}

/**
 * @param column The column to get the number of characters
 * estimated for.
 *
 * @return The number of characters estimated for the column. If
 * it cannot be determined this returns 10.
 */
int QPeaksTableModel::numCharacters(const int column) const {
  if (column == COL_RUNNUMBER)
    return 5;
  else if (column == COL_DETID)
    return 7;
  else if (column == COL_H)
    return 3 + m_hklPrec;
  else if (column == COL_K)
    return 3 + m_hklPrec;
  else if (column == COL_L)
    return 3 + m_hklPrec;
  else if (column == COL_WAVELENGTH)
    return 6;
  else if (column == COL_ENERGY)
    return 6;
  else if (column == COL_INITIAL_ENERGY)
    return 6;
  else if (column == COL_FINAL_ENERGY)
    return 6;
  else if (column == COL_TOF)
    return 6;
  else if (column == COL_DSPACING)
    return 6;
  else if (column == COL_INT)
    return 5;
  else if (column == COL_SIGMINT)
    return 5;
  else if (column == COL_INT_SIGINT)
    return 5;
  else if (column == COL_BINCOUNT)
    return 6;
  else if (column == COL_BANKNAME)
    return 6;
  else if (column == COL_ROW)
    return 3;
  else if (column == COL_COL)
    return 3;
  else if (column == COL_QLAB)
    return 3 * 6;
  else if (column == COL_QSAMPLE)
    return 3 * 6;
  else
    return 3;
}

/**
Constructor
@param peaksWS : Workspace model.
*/
QPeaksTableModel::QPeaksTableModel(
    boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS)
    : QAbstractTableModel(nullptr), m_dataCachePeakIndex(-1),
      m_peaksWS(peaksWS) {
  m_columnNameMap = {{0, RUNNUMBER},
                     {1, DETID},
                     {2, H},
                     {3, K},
                     {4, L},
                     {5, WAVELENGTH},
                     {6, INITIAL_ENERGY},
                     {7, FINAL_ENERGY},
                     {8, ENERGY},
                     {9, TOF},
                     {10, DSPACING},
                     {11, INT},
                     {12, SIGMINT},
                     {13, INT_SIGINT},
                     {14, BINCOUNT},
                     {15, BANKNAME},
                     {16, ROW},
                     {17, COL},
                     {18, QLAB},
                     {19, QSAMPLE}};

  m_sortableColumns = {{RUNNUMBER, true},
                       {DETID, true},
                       {H, true},
                       {K, true},
                       {L, true},
                       {WAVELENGTH, true},
                       {ENERGY, false},
                       {INITIAL_ENERGY, true},
                       {FINAL_ENERGY, true},
                       {TOF, true},
                       {DSPACING, true},
                       {INT, true},
                       {SIGMINT, true},
                       {INT_SIGINT, false},
                       {BINCOUNT, true},
                       {BANKNAME, true},
                       {ROW, true},
                       {COL, true},
                       {QLAB, false},
                       {QSAMPLE, false}};

  if (!Mantid::Kernel::ConfigService::Instance().getValue("PeakColumn.hklPrec",
                                                          m_hklPrec))
    m_hklPrec = 2;
}

void QPeaksTableModel::setPeaksWorkspace(
    boost::shared_ptr<const Mantid::API::IPeaksWorkspace> peaksWS) {
  m_peaksWS = peaksWS;
  this->update();
}

/**
Update the model.
*/
void QPeaksTableModel::update() {
  emit layoutChanged(); // This should tell the view that the data has changed.
}

/**
@return the row count.
*/
int QPeaksTableModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_peaksWS->rowCount());
}

/**
@return the number of columns in the model.
*/
int QPeaksTableModel::columnCount(const QModelIndex &) const {
  return static_cast<int>(m_columnNameMap.size());
}

/**
Find the column name at a given column index.
@param colIndex : Index to find column name for.
*/
QString QPeaksTableModel::findColumnName(const int colIndex) const {
  ColumnIndexNameMap::const_iterator foundColName =
      m_columnNameMap.find(colIndex);
  if (foundColName == m_columnNameMap.end()) {
    throw std::runtime_error("Unknown column requested");
  }
  return foundColName->second;
}

/**
Overrident data method, allows consuming view to extract data for an index and
role.
@param index : For which to extract the data
@param role : Role mode
*/
QVariant QPeaksTableModel::data(const QModelIndex &index, int role) const {

  if (role == Qt::TextAlignmentRole)
    return Qt::AlignRight;

  if (role != Qt::DisplayRole)
    return QVariant();

  const int colNumber = index.column();
  const int rowNumber = index.row();

  const IPeak &peak = m_peaksWS->getPeak(rowNumber);
  this->updateDataCache(peak, rowNumber);
  return m_dataCache[colNumber];
}

/**
Get the heading for a given section, orientation and role.
@param section : Column index
@param orientation : Heading orientation
@param role : Role mode of table.
@return HeaderData.
*/
QVariant QPeaksTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return findColumnName(section);
  }
  return QVariant();
}

/**
Provide flags on an index by index basis
@param index: To generate a flag for.
*/
Qt::ItemFlags QPeaksTableModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return nullptr;
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/**
 * @return List of columns to hide by default.
 */
std::vector<int> QPeaksTableModel::defaultHideCols() {
  std::vector<int> result;

  // only show bank name for SNS instruments
  Mantid::Geometry::Instrument_const_sptr instr = m_peaksWS->getInstrument();
  std::string instrName = instr->getName();
  try {
    Mantid::Kernel::InstrumentInfo instrInfo =
        Mantid::Kernel::ConfigService::Instance().getInstrument(instrName);
    if (instrInfo.facility().name() != "SNS")
      result.push_back(COL_BANKNAME);

    // hide some columns based on the techniques
    { // shrink variable scope
      auto techniques = instrInfo.techniques();
      // required for showing final and delta energy
      const std::string IGS("TOF Indirect Geometry Spectroscopy");
      // required for showing initial and delta energy
      const std::string DGS("TOF Direct Geometry Spectroscopy");
      bool showEnergy(false);
      if (techniques.find(DGS) == techniques.end())
        result.push_back(COL_FINAL_ENERGY);
      else
        showEnergy = true;
      if (techniques.find(IGS) == techniques.end())
        result.push_back(COL_INITIAL_ENERGY);
      else
        showEnergy = true;
      if (!showEnergy)
        result.push_back(COL_ENERGY);
    }
  } catch (Mantid::Kernel::Exception::NotFoundError &) {
    // Unable to fetch instrument info, so continue without it.
  }

  return result;
}

/// Destructor
QPeaksTableModel::~QPeaksTableModel() {}

/**
 * Overriden sort.
 * @param column
 * @param order
 */
void QPeaksTableModel::sort(int column, Qt::SortOrder order) {
  using namespace Mantid::API;
  const QString columnName = findColumnName(column);
  const bool isSortable = m_sortableColumns[columnName];
  if (isSortable) {
    std::string rationalName(columnName.toStdString());
    if (columnName == QPeaksTableModel::INT)
      rationalName = "Intens";
    else if (columnName == QPeaksTableModel::RUNNUMBER)
      rationalName = "RunNumber";
    // TODO raise event and propagate through to Proper presenter.
    peaksSorted(rationalName, order == Qt::AscendingOrder);

    emit layoutChanged(); // This should tell the view that the data has
                          // changed.
  }
}
}
}
