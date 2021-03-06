#include "ConvFit.h"

#include "../General/UserInputValidator.h"

#include "MantidQtWidgets/Common/RangeSelector.h"

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/TableRow.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidGeometry/Instrument.h"

#include <QDoubleValidator>
#include <QMenu>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

using namespace Mantid::API;

namespace {
Mantid::Kernel::Logger g_log("ConvFit");
}

namespace MantidQt {
namespace CustomInterfaces {
namespace IDA {

ConvFit::ConvFit(QWidget *parent)
    : IndirectDataAnalysisTab(parent), m_stringManager(nullptr),
      m_cfTree(nullptr), m_fixedProps(), m_confitResFileType(), m_runMin(-1),
      m_runMax(-1) {
  m_uiForm.setupUi(parent);
}

void ConvFit::setup() {
  // Create Property Managers
  m_stringManager = new QtStringPropertyManager();
  m_runMin = 0;
  m_runMax = 0;

  // Initialise fitTypeStrings
  m_fitStrings = {"", "1L", "2L", "IDS", "IDC", "EDS", "EDC", "SFT"};
  // All Parameters in tree that should be defaulting to 1
  QMap<QString, double> m_defaultParams;
  m_defaultParams = createDefaultParamsMap(m_defaultParams);

  // Create TreeProperty Widget
  m_cfTree = new QtTreePropertyBrowser();
  m_uiForm.properties->addWidget(m_cfTree);

  // add factories to managers
  m_cfTree->setFactoryForManager(m_blnManager, m_blnEdFac);
  m_cfTree->setFactoryForManager(m_dblManager, m_dblEdFac);

  // Create Range Selectors
  auto fitRangeSelector = m_uiForm.ppPlot->addRangeSelector("ConvFitRange");
  auto backRangeSelector = m_uiForm.ppPlot->addRangeSelector(
      "ConvFitBackRange", MantidWidgets::RangeSelector::YSINGLE);
  auto hwhmRangeSelector = m_uiForm.ppPlot->addRangeSelector("ConvFitHWHM");
  backRangeSelector->setColour(Qt::darkGreen);
  backRangeSelector->setRange(0.0, 1.0);
  hwhmRangeSelector->setColour(Qt::red);

  // Populate Property Widget

  // Option to convolve members
  m_properties["Convolve"] = m_blnManager->addProperty("Convolve");
  m_cfTree->addProperty(m_properties["Convolve"]);
  m_blnManager->setValue(m_properties["Convolve"], true);

  // Option to extract members
  m_properties["ExtractMembers"] = m_blnManager->addProperty("ExtractMembers");
  m_cfTree->addProperty(m_properties["ExtractMembers"]);
  m_blnManager->setValue(m_properties["ExtractMembers"], false);

  // Max iterations option
  m_properties["MaxIterations"] = m_dblManager->addProperty("Max Iterations");
  m_dblManager->setDecimals(m_properties["MaxIterations"], 0);
  m_dblManager->setValue(m_properties["MaxIterations"], 500);
  m_cfTree->addProperty(m_properties["MaxIterations"]);

  // Fitting range
  m_properties["FitRange"] = m_grpManager->addProperty("Fitting Range");
  m_properties["StartX"] = m_dblManager->addProperty("StartX");
  m_dblManager->setDecimals(m_properties["StartX"], NUM_DECIMALS);
  m_properties["EndX"] = m_dblManager->addProperty("EndX");
  m_dblManager->setDecimals(m_properties["EndX"], NUM_DECIMALS);
  m_properties["FitRange"]->addSubProperty(m_properties["StartX"]);
  m_properties["FitRange"]->addSubProperty(m_properties["EndX"]);
  m_cfTree->addProperty(m_properties["FitRange"]);

  // FABADA
  initFABADAOptions();

  // Background type
  m_properties["LinearBackground"] = m_grpManager->addProperty("Background");
  m_properties["BGA0"] = m_dblManager->addProperty("A0");
  m_dblManager->setDecimals(m_properties["BGA0"], NUM_DECIMALS);
  m_properties["BGA1"] = m_dblManager->addProperty("A1");
  m_dblManager->setDecimals(m_properties["BGA1"], NUM_DECIMALS);
  m_properties["LinearBackground"]->addSubProperty(m_properties["BGA0"]);
  m_properties["LinearBackground"]->addSubProperty(m_properties["BGA1"]);
  m_cfTree->addProperty(m_properties["LinearBackground"]);

  // Delta Function
  m_properties["Delta Function"] = m_grpManager->addProperty("Delta Function");
  m_properties["UseDeltaFunc"] = m_blnManager->addProperty("Use");
  m_properties["Delta Function"]->addSubProperty(m_properties["UseDeltaFunc"]);
  m_properties["Delta Function"] =
      createFitType(m_properties["Delta Function"], false);
  m_cfTree->addProperty(m_properties["Delta Function"]);

  // Fit functions
  m_properties["Lorentzian 1"] = createFitType("Lorentzian 1");
  m_properties["Lorentzian 2"] = createFitType("Lorentzian 2");
  m_properties["DiffSphere"] = createFitType("DiffSphere");
  m_properties["DiffRotDiscreteCircle"] =
      createFitType("DiffRotDiscreteCircle");
  m_properties["ElasticDiffSphere"] = createFitType("ElasticDiffSphere");
  m_properties["ElasticDiffRotDiscreteCircle"] =
      createFitType("ElasticDiffRotDiscreteCircle");
  m_properties["InelasticDiffSphere"] = createFitType("InelasticDiffSphere");
  m_properties["InelasticDiffRotDiscreteCircle"] =
      createFitType("InelasticDiffRotDiscreteCircle");
  m_properties["StretchedExpFT"] = createFitType("StretchedExpFT");

  // Instrument resolution
  m_properties["InstrumentResolution"] =
      m_dblManager->addProperty("InstrumentResolution");

  // Update fit parameters in browser when function is selected
  connect(m_uiForm.cbFitType, SIGNAL(currentIndexChanged(int)), this,
          SLOT(fitFunctionSelected(int)));
  fitFunctionSelected(m_uiForm.cbFitType->currentIndex());

  m_uiForm.leTempCorrection->setValidator(new QDoubleValidator(m_parentWidget));

  // Connections
  connect(fitRangeSelector, SIGNAL(minValueChanged(double)), this,
          SLOT(minChanged(double)));
  connect(fitRangeSelector, SIGNAL(maxValueChanged(double)), this,
          SLOT(maxChanged(double)));
  connect(backRangeSelector, SIGNAL(minValueChanged(double)), this,
          SLOT(backgLevel(double)));
  connect(hwhmRangeSelector, SIGNAL(minValueChanged(double)), this,
          SLOT(hwhmChanged(double)));
  connect(hwhmRangeSelector, SIGNAL(maxValueChanged(double)), this,
          SLOT(hwhmChanged(double)));
  connect(m_dblManager, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(updateRS(QtProperty *, double)));
  connect(m_blnManager, SIGNAL(valueChanged(QtProperty *, bool)), this,
          SLOT(checkBoxUpdate(QtProperty *, bool)));
  connect(m_uiForm.ckTempCorrection, SIGNAL(toggled(bool)),
          m_uiForm.leTempCorrection, SLOT(setEnabled(bool)));

  // Update guess curve when certain things happen
  connect(m_dblManager, SIGNAL(propertyChanged(QtProperty *)), this,
          SLOT(plotGuess()));
  connect(m_uiForm.cbFitType, SIGNAL(currentIndexChanged(int)), this,
          SLOT(plotGuess()));
  connect(m_uiForm.ckPlotGuess, SIGNAL(stateChanged(int)), this,
          SLOT(plotGuess()));

  // Have FWHM Range linked to Fit Start/End Range
  connect(fitRangeSelector, SIGNAL(rangeChanged(double, double)),
          hwhmRangeSelector, SLOT(setRange(double, double)));
  hwhmRangeSelector->setRange(-1.0, 1.0);
  hwhmUpdateRS(0.02);

  typeSelection(m_uiForm.cbFitType->currentIndex());
  bgTypeSelection(m_uiForm.cbBackground->currentIndex());

  // Replot input automatically when file / spec no changes
  connect(m_uiForm.spPlotSpectrum, SIGNAL(valueChanged(int)), this,
          SLOT(updatePlot()));
  connect(m_uiForm.spPlotSpectrum, SIGNAL(valueChanged(int)), this,
          SLOT(updateProperties(int)));
  connect(m_uiForm.spPlotSpectrum, SIGNAL(valueChanged(int)), this,
          SLOT(IndirectDataAnalysisTab::setSelectedSpectrum(int)));

  connect(m_uiForm.dsSampleInput, SIGNAL(dataReady(const QString &)), this,
          SLOT(newDataLoaded(const QString &)));
  connect(m_uiForm.dsSampleInput, SIGNAL(dataReady(const QString &)), this,
          SLOT(updatePlotRange()));
  connect(m_uiForm.dsSampleInput, SIGNAL(dataReady(const QString &)), this,
          SLOT(extendResolutionWorkspace()));
  connect(m_uiForm.dsResInput, SIGNAL(dataReady(const QString &)), this,
          SLOT(extendResolutionWorkspace()));

  connect(m_uiForm.spSpectraMin, SIGNAL(valueChanged(int)), this,
          SLOT(specMinChanged(int)));
  connect(m_uiForm.spSpectraMax, SIGNAL(valueChanged(int)), this,
          SLOT(specMaxChanged(int)));

  connect(m_uiForm.cbFitType, SIGNAL(currentIndexChanged(int)), this,
          SLOT(typeSelection(int)));
  connect(m_uiForm.cbBackground, SIGNAL(currentIndexChanged(int)), this,
          SLOT(bgTypeSelection(int)));
  connect(m_uiForm.pbSingleFit, SIGNAL(clicked()), this, SLOT(singleFit()));

  // Context menu
  m_cfTree->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(m_cfTree, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(fitContextMenu(const QPoint &)));

  // Tie
  connect(m_uiForm.cbFitType, SIGNAL(currentIndexChanged(QString)),
          SLOT(showTieCheckbox(QString)));
  showTieCheckbox(m_uiForm.cbFitType->currentText());

  // Post Plot and Save
  connect(m_uiForm.pbSave, SIGNAL(clicked()), this, SLOT(saveClicked()));
  connect(m_uiForm.pbPlot, SIGNAL(clicked()), this, SLOT(plotClicked()));
  connect(m_uiForm.pbPlotPreview, SIGNAL(clicked()), this,
          SLOT(plotCurrentPreview()));

  m_uiForm.ckTieCentres->setChecked(true);

  updatePlotOptions();
}

/** Setup FABADA minimizer options
 *
 */
void ConvFit::initFABADAOptions() {

  m_properties["FABADA"] = m_grpManager->addProperty("Bayesian");
  m_properties["UseFABADA"] = m_blnManager->addProperty("Use FABADA");
  m_properties["FABADA"]->addSubProperty(m_properties["UseFABADA"]);

  // Output chain
  m_properties["OutputFABADAChain"] = m_blnManager->addProperty("Output Chain");
  // Chain length
  m_properties["FABADAChainLength"] = m_dblManager->addProperty("Chain Length");
  m_dblManager->setDecimals(m_properties["FABADAChainLength"], 0);
  m_dblManager->setValue(m_properties["FABADAChainLength"], 1000000);
  // Convergence criteria
  m_properties["FABADAConvergenceCriteria"] =
      m_dblManager->addProperty("Convergence Criteria");
  m_dblManager->setValue(m_properties["FABADAConvergenceCriteria"], 0.1);
  // Jump acceptance rate
  m_properties["FABADAJumpAcceptanceRate"] =
      m_dblManager->addProperty("Acceptance Rate");
  m_dblManager->setValue(m_properties["FABADAJumpAcceptanceRate"], 0.25);

  // Advanced options
  m_properties["FABADAAdvanced"] = m_blnManager->addProperty("Advanced");
  m_blnManager->setValue(m_properties["FABADAAdvanced"], false);
  // Steps between values
  m_properties["FABADAStepsBetweenValues"] =
      m_dblManager->addProperty("Steps Between Values");
  m_dblManager->setDecimals(m_properties["FABADAStepsBetweenValues"], 0);
  m_dblManager->setValue(m_properties["FABADAStepsBetweenValues"], 10);
  // Inactive convergence criterion
  m_properties["FABADAInactiveConvergenceCriterion"] =
      m_dblManager->addProperty("Inactive Convergence Criterion");
  m_dblManager->setDecimals(m_properties["FABADAInactiveConvergenceCriterion"],
                            0);
  m_dblManager->setValue(m_properties["FABADAInactiveConvergenceCriterion"], 5);
  // Simulated annealing applied
  m_properties["FABADASimAnnealingApplied"] =
      m_blnManager->addProperty("Sim Annealing Applied");
  // Maximum temperature
  m_properties["FABADAMaximumTemperature"] =
      m_dblManager->addProperty("Maximum Temperature");
  m_dblManager->setValue(m_properties["FABADAMaximumTemperature"], 10.0);
  // Number of regrigeration steps
  m_properties["FABADANumRefrigerationSteps"] =
      m_dblManager->addProperty("Num Refrigeration Steps");
  m_dblManager->setDecimals(m_properties["FABADANumRefrigerationSteps"], 0);
  m_dblManager->setValue(m_properties["FABADANumRefrigerationSteps"], 5);
  // Simulated annealing iterations
  m_properties["FABADASimAnnealingIterations"] =
      m_dblManager->addProperty("Sim Annealing Iterations");
  m_dblManager->setDecimals(m_properties["FABADASimAnnealingIterations"], 0);
  m_dblManager->setValue(m_properties["FABADASimAnnealingIterations"], 10000);
  // Overexploration
  m_properties["FABADAOverexploration"] =
      m_blnManager->addProperty("Overexploration");
  m_cfTree->addProperty(m_properties["FABADA"]);
  // Number of bins in PDF
  m_properties["FABADANumberBinsPDF"] =
      m_dblManager->addProperty("Number Bins PDF");
  m_dblManager->setDecimals(m_properties["FABADANumberBinsPDF"], 0);
  m_dblManager->setValue(m_properties["FABADANumberBinsPDF"], 20);
}

/**
 * Handles the initial set up and running of the ConvolutionFitSequential
 * algorithm
 */
void ConvFit::run() {
  // Get input from interface
  m_runMin = m_uiForm.spSpectraMin->value();
  m_runMax = m_uiForm.spSpectraMax->value();
  const auto specMin = m_uiForm.spSpectraMin->text().toStdString();
  const auto specMax = m_uiForm.spSpectraMax->text().toStdString();
  m_fitFunctions = indexToFitFunctions(m_uiForm.cbFitType->currentIndex());
  auto cfs = sequentialFit(specMin, specMax, m_baseName);

  // Add to batch alg runner and execute
  m_batchAlgoRunner->addAlgorithm(cfs);
  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(sequentialFitComplete(bool)));
  m_batchAlgoRunner->executeBatchAsync();
}

IAlgorithm_sptr ConvFit::sequentialFit(const std::string &specMin,
                                       const std::string &specMax,
                                       QString &outputWSName) {
  const auto func = createFunction(m_uiForm.ckTieCentres->isChecked() &&
                                   m_uiForm.ckTieCentres->isVisible());
  const auto function = std::string(func->asString());

  // Construct expected name
  outputWSName = QString::fromStdString(inputWorkspace()->getName());

  // Remove _red
  const auto cutIndex = outputWSName.lastIndexOf("_");
  if (cutIndex != -1) {
    outputWSName = outputWSName.left(cutIndex + 1);
  }

  // Add fit specific suffix
  const auto bgType = backgroundString();
  const auto fitType = fitTypeString();
  outputWSName += "conv_";
  outputWSName += fitType;
  outputWSName += bgType;
  outputWSName += QString::fromStdString(specMin);

  if (specMin != specMax) {
    outputWSName += "_to_";
    outputWSName += QString::fromStdString(specMax);
  }

  // Run ConvolutionFitSequential Algorithm
  auto cfs = AlgorithmManager::Instance().create("ConvolutionFitSequential");
  cfs->initialize();
  cfs->setProperty("InputWorkspace", inputWorkspace());
  cfs->setProperty("Function", function);
  cfs->setProperty("PassWSIndexToFunction", true);
  cfs->setProperty("BackgroundType",
                   m_uiForm.cbBackground->currentText().toStdString());
  cfs->setProperty("StartX", m_properties["StartX"]->valueText().toStdString());
  cfs->setProperty("EndX", m_properties["EndX"]->valueText().toStdString());
  cfs->setProperty("SpecMin", specMin);
  cfs->setProperty("SpecMax", specMax);
  cfs->setProperty("Convolve", true);
  cfs->setProperty("ExtractMembers", static_cast<bool>(m_blnManager->value(
                                         m_properties["ExtractMembers"])));
  cfs->setProperty("Minimizer",
                   minimizerString("$outputname_$wsindex").toStdString());
  cfs->setProperty("MaxIterations", static_cast<int>(m_dblManager->value(
                                        m_properties["MaxIterations"])));
  cfs->setProperty("OutputWorkspace", (outputWSName.toStdString() + "_Result"));
  return cfs;
}

void ConvFit::sequentialFitComplete(bool error) {
  disconnect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
             SLOT(sequentialFitComplete(bool)));
  algorithmComplete(error, m_baseName);
}

/**
 * Handles saving the workspace when save is clicked
 */
void ConvFit::saveClicked() {
  // check workspace exists
  const auto resultName = m_baseName.toStdString() + "_Result";
  const auto wsFound = checkADSForPlotSaveWorkspace(resultName, false);
  // process workspace after check
  if (wsFound) {
    QString saveDir = QString::fromStdString(
        Mantid::Kernel::ConfigService::Instance().getString(
            "defaultsave.directory"));
    // Check validity of save path
    QString QresultWsName = QString::fromStdString(resultName);
    const auto fullPath = saveDir.append(QresultWsName).append(".nxs");
    addSaveWorkspaceToQueue(QresultWsName, fullPath);
    m_batchAlgoRunner->executeBatchAsync();
  } else {
    return;
  }
}

/**
 * Handles plotting the workspace when plot is clicked
 */
void ConvFit::plotClicked() {

  // check workspace exists
  const auto resultName = m_baseName.toStdString() + "_Result";
  const auto wsFound = checkADSForPlotSaveWorkspace(resultName, true);
  if (wsFound) {
    MatrixWorkspace_sptr resultWs =
        AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(resultName);
    const auto plot = m_uiForm.cbPlotType->currentText().toStdString();

    // Handle plot result
    if (!(plot.compare("None") == 0)) {
      if (plot.compare("All") == 0) {
        const auto specEnd = (int)resultWs->getNumberHistograms();
        for (int i = 0; i < specEnd; i++) {
          IndirectTab::plotSpectrum(QString::fromStdString(resultWs->getName()),
                                    i, i);
        }
      } else {
        const auto specNumber = m_uiForm.cbPlotType->currentIndex();
        IndirectTab::plotSpectrum(QString::fromStdString(resultWs->getName()),
                                  specNumber, specNumber);
        // Plot results for both Lorentzians if "Two Lorentzians"
        if (m_uiForm.cbFitType->currentIndex() == 2) {
          IndirectTab::plotSpectrum(QString::fromStdString(resultWs->getName()),
                                    specNumber + 2, specNumber + 2);
        }
      }
    }
  } else {
    return;
  }
}

/**
 * Handles completion of the ConvolutionFitSequential algorithm.
 *
 * @param error True if the algorithm was stopped due to error, false otherwise
 * @param outputWSName The name of the output workspace created from running the
 *                     algorithm.
 */
void ConvFit::algorithmComplete(bool error, const QString &outputWSName) {

  if (error) {
    return;
  }

  std::string outputPrefix = outputWSName.toStdString();

  const auto resultName = outputPrefix + "_Result";
  MatrixWorkspace_sptr resultWs =
      AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(resultName);

  // Name for GroupWorkspace
  const auto groupName = outputPrefix + "_Workspaces";
  // Add Sample logs for ResolutionFiles
  const auto resFile = m_uiForm.dsResInput->getCurrentDataName().toStdString();
  addSampleLogsToWorkspace(resultName, "resolution_filename", resFile,
                           "String");
  addSampleLogsToWorkspace(groupName, "resolution_filename", resFile, "String");
  bool usedTemperature = false;

  // Check if temperature is used and is valid
  if (m_uiForm.ckTempCorrection->isChecked()) {
    const QString temperature = m_uiForm.leTempCorrection->text();
    double temp = 0.0;

    if (!temperature.isEmpty()) {
      temp = temperature.toDouble();
    }

    if (temp != 0.0) {
      usedTemperature = true;
      // Add sample logs for temperature
      const auto temperatureStr = temperature.toStdString();
      addSampleLogsToWorkspace(resultName, "temperature_correction", "true",
                               "String");
      addSampleLogsToWorkspace(groupName, "temperature_correction", "true",
                               "String");
      addSampleLogsToWorkspace(resultName, "temperature_value", temperatureStr,
                               "Number");

      addSampleLogsToWorkspace(resultName, "temperature_value", temperatureStr,
                               "Number");
    }
  }
  m_batchAlgoRunner->executeBatchAsync();

  updatePlot();
  updatePlotRange();

  std::string paramWsName = outputPrefix + "_Parameters_new";

  if (AnalysisDataService::Instance().doesExist(paramWsName)) {
    QString prefixPrefix = "f1.f1.";
    QString prefixSuffix = usedTemperature ? "f1." : "f0.";

    m_propertyToParameter = createPropertyToParameterMap(
        m_fitFunctions, prefixPrefix, prefixSuffix);
    m_parameterValues = IndirectTab::extractParametersFromTable(
        paramWsName, m_propertyToParameter.values().toSet(), m_runMin,
        m_runMax);

    updateProperties(m_uiForm.spPlotSpectrum->value());
  }

  m_uiForm.pbSave->setEnabled(true);
  m_uiForm.pbPlot->setEnabled(true);
}

QHash<QString, QString>
ConvFit::createPropertyToParameterMap(const QVector<QString> &functionNames,
                                      const QString &prefixPrefix,
                                      const QString &prefixSuffix) {
  QHash<QString, QString> propertyToParameter;

  if (functionNames.size() == 1) {
    QString prefix = prefixPrefix;

    if (functionNames[0] != "Delta Function") {
      prefix += prefixSuffix;
    }

    extendPropertyToParameterMap(functionNames[0], prefixPrefix + prefixSuffix,
                                 propertyToParameter);
  } else {

    for (int i = 0; i < functionNames.size(); ++i) {
      extendPropertyToParameterMap(functionNames[i], i, prefixPrefix,
                                   prefixSuffix, propertyToParameter);
    }
  }

  propertyToParameter["f0.A0"] = "BackgroundA0";
  return propertyToParameter;
}

void ConvFit::extendPropertyToParameterMap(
    const QString &functionName, const int &funcIndex,
    const QString &prefixPrefix, const QString &prefixSuffix,
    QHash<QString, QString> &propertyToParameter) {
  QString prefix;

  if (functionName == "Delta Function") {
    prefix = prefixPrefix + prefixSuffix;
  } else {
    prefix =
        prefixPrefix + "f" + QString::number(funcIndex) + "." + prefixSuffix;
  }

  extendPropertyToParameterMap(functionName, prefix, propertyToParameter);
}

void ConvFit::extendPropertyToParameterMap(
    const QString &functionName, const QString &prefix,
    QHash<QString, QString> &propertyToParameter) {

  for (auto &paramName : getFunctionParameters(functionName)) {
    propertyToParameter[functionName + "." + paramName] = prefix + paramName;
  }
}

/**
 * Sets up and add an instance of the AddSampleLog algorithm to the batch
 * algorithm runner
 * @param workspaceName	:: The name of the workspace to add the log to
 * @param logName		:: The title of the log input
 * @param logText		:: The information to match the title
 * @param logType		:: The type of information (String, Number)
 */
void ConvFit::addSampleLogsToWorkspace(const std::string &workspaceName,
                                       const std::string &logName,
                                       const std::string &logText,
                                       const std::string &logType) {

  auto addSampleLog = AlgorithmManager::Instance().create("AddSampleLog");
  addSampleLog->setLogging(false);
  addSampleLog->setProperty("Workspace", workspaceName);
  addSampleLog->setProperty("LogName", logName);
  addSampleLog->setProperty("LogText", logText);
  addSampleLog->setProperty("LogType", logType);
  m_batchAlgoRunner->addAlgorithm(addSampleLog);
}

/**
 * Validates the user's inputs in the ConvFit tab.
 * @return If the validation was successful
 */
bool ConvFit::validate() {
  UserInputValidator uiv;

  const QString fitType = fitTypeString();
  if (fitType == "") {
    uiv.addErrorMessage("No fit type defined");
  }

  uiv.checkDataSelectorIsValid("Sample", m_uiForm.dsSampleInput);
  uiv.checkDataSelectorIsValid("Resolution", m_uiForm.dsResInput);

  const auto range = std::make_pair(m_dblManager->value(m_properties["StartX"]),
                                    m_dblManager->value(m_properties["EndX"]));
  uiv.checkValidRange("Fitting Range", range);

  // Enforce the rule that at least one fit is needed; either a delta function,
  // one or two Lorentzian functions,
  // or both.  (The resolution function must be convolved with a model.)
  if (m_uiForm.cbFitType->currentIndex() == 0 &&
      !m_blnManager->value(m_properties["UseDeltaFunc"]))
    uiv.addErrorMessage("No fit function has been selected.");

  if (m_uiForm.ckTempCorrection->isChecked()) {
    if (m_uiForm.leTempCorrection->text().compare("") == 0) {
      uiv.addErrorMessage("Temperature correction has been checked in the "
                          "interface, but no value has been given.");
    }
  }

  const auto error = uiv.generateErrorMessage();
  showMessageBox(error);

  return error.isEmpty();
}

/**
 * Reads in settings files
 * @param settings The name of the QSettings object to retrieve data from
 */
void ConvFit::loadSettings(const QSettings &settings) {
  m_uiForm.dsSampleInput->readSettings(settings.group());
  m_uiForm.dsResInput->readSettings(settings.group());
}

/**
 * Called when new data has been loaded by the data selector.
 *
 * Configures ranges for spin boxes before raw plot is done.
 *
 * @param wsName Name of new workspace loaded
 */
void ConvFit::newDataLoaded(const QString wsName) {
  auto inputWs = AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(
      wsName.toStdString());
  setInputWorkspace(inputWs);

  const int maxWsIndex = static_cast<int>(inputWs->getNumberHistograms()) - 1;

  m_uiForm.spPlotSpectrum->setMaximum(maxWsIndex);
  m_uiForm.spPlotSpectrum->setMinimum(0);
  m_uiForm.spPlotSpectrum->setValue(0);

  m_uiForm.spSpectraMin->setMaximum(maxWsIndex);
  m_uiForm.spSpectraMin->setMinimum(0);

  m_uiForm.spSpectraMax->setMaximum(maxWsIndex);
  m_uiForm.spSpectraMax->setMinimum(0);
  m_uiForm.spSpectraMax->setValue(maxWsIndex);

  updatePlot();
}

/**
 * Create a resolution workspace with the same number of histograms as in the
 * sample, if the resolution and sample differ in their number of histograms.
 *
 * Needed to allow DiffSphere and DiffRotDiscreteCircle fit functions to work as
 * they need to have the WorkspaceIndex attribute set.
 */
void ConvFit::extendResolutionWorkspace() {
  auto inputWs = inputWorkspace();

  if (inputWs && m_uiForm.dsResInput->isValid()) {
    const std::string resWsName =
        m_uiForm.dsResInput->getCurrentDataName().toStdString();
    // Check spectra consistency between resolution and sample
    auto resolutionInputWS =
        AnalysisDataService::Instance().retrieveWS<MatrixWorkspace>(resWsName);
    size_t resolutionNumHist = resolutionInputWS->getNumberHistograms();
    size_t numHist = inputWs->getNumberHistograms();
    if (resolutionNumHist != 1 && resolutionNumHist != numHist) {
      std::string msg(
          "Resolution must have either one or as many spectra as the sample");
      throw std::runtime_error(msg);
    }
    // Clone resolution workspace
    IAlgorithm_sptr cloneAlg =
        AlgorithmManager::Instance().create("CloneWorkspace");
    cloneAlg->setLogging(false);
    cloneAlg->initialize();
    cloneAlg->setProperty("InputWorkspace", resWsName);
    cloneAlg->setProperty("OutputWorkspace", "__ConvFit_Resolution");
    cloneAlg->execute();
    // Append to cloned workspace if necessary
    if (resolutionNumHist == 1 && numHist > 1) {
      IAlgorithm_sptr appendAlg =
          AlgorithmManager::Instance().create("AppendSpectra");
      appendAlg->setLogging(false);
      appendAlg->initialize();
      appendAlg->setPropertyValue("InputWorkspace1", "__ConvFit_Resolution");
      appendAlg->setPropertyValue("InputWorkspace2", resWsName);
      appendAlg->setProperty("Number", static_cast<int>(numHist - 1));
      appendAlg->setPropertyValue("OutputWorkspace", "__ConvFit_Resolution");
      appendAlg->execute();
    }
  }
}

namespace {
////////////////////////////
// Anon Helper functions. //
////////////////////////////

/**
 * Takes an index and a name, and constructs a single level parameter name
 * for use with function ties, etc.
 *
 * @param index :: the index of the function in the first level.
 * @param name  :: the name of the parameter inside the function.
 *
 * @returns the constructed function parameter name.
 */
std::string createParName(size_t index, const std::string &name = "") {
  std::stringstream prefix;
  prefix << "f" << index << "." << name;
  return prefix.str();
}

/**
 * Takes an index, a sub index and a name, and constructs a double level
 * (nested) parameter name for use with function ties, etc.
 *
 * @param index    :: the index of the function in the first level.
 * @param subIndex :: the index of the function in the second level.
 * @param name     :: the name of the parameter inside the function.
 *
 * @returns the constructed function parameter name.
 */
std::string createParName(size_t index, size_t subIndex,
                          const std::string &name = "") {
  std::stringstream prefix;
  prefix << "f" << index << ".f" << subIndex << "." << name;
  return prefix.str();
}
} // namespace

/**
 * Creates a function to carry out the fitting in the "ConvFit" tab.  The
 * function consists of various sub functions, with the following structure:
 *
 * Composite
 *  |
 *  +- LinearBackground
 *  +- Convolution
 *      |
 *      +- Resolution
 *      +- Model (AT LEAST one delta function or one/two lorentzians.)
 *          |
 *          +- Delta Function(yes/no)
 *				+- ProductFunction
 *					|
 *					+- Lorentzian 1(yes/no)
 *					+- Temperature Correction(yes/no)
 *				+- ProductFunction
 *					|
 *					+- Lorentzian 2(yes/no)
 *					+- Temperature Correction(yes/no)
 *				+- ProductFunction
 *					|
 *					+- InelasticDiffSphere(yes/no)
 *					+- Temperature Correction(yes/no)
 *				+- ProductFunction
 *					|
 *					+- InelasticDiffRotDisCircle(yes/no)
 *					+- Temperature Correction(yes/no)
 *
 * @param tieCentres :: whether to tie centres of the two lorentzians.
 *
 * @returns the composite fitting function.
 */
CompositeFunction_sptr ConvFit::createFunction(bool tieCentres) {
  auto conv = boost::dynamic_pointer_cast<CompositeFunction>(
      FunctionFactory::Instance().createFunction("Convolution"));
  CompositeFunction_sptr comp(new CompositeFunction);

  IFunction_sptr func;
  size_t index = 0;

  // -------------------------------------
  // --- Composite / Linear Background ---
  // -------------------------------------
  func = FunctionFactory::Instance().createFunction("LinearBackground");
  comp->addFunction(func);

  // 0 = Fixed Flat, 1 = Fit Flat, 2 = Fit all
  const int bgType = m_uiForm.cbBackground->currentIndex();

  if (bgType == 0 || !m_properties["BGA0"]->subProperties().isEmpty()) {
    comp->tie("f0.A0", m_properties["BGA0"]->valueText().toStdString());
  } else {
    func->setParameter("A0", m_properties["BGA0"]->valueText().toDouble());
  }

  if (bgType != 2) {
    comp->tie("f0.A1", "0.0");
  } else {
    if (!m_properties["BGA1"]->subProperties().isEmpty()) {
      comp->tie("f0.A1", m_properties["BGA1"]->valueText().toStdString());
    } else {
      func->setParameter("A1", m_properties["BGA1"]->valueText().toDouble());
    }
  }

  // --------------------------------------------
  // --- Composite / Convolution / Resolution ---
  // --------------------------------------------
  func = FunctionFactory::Instance().createFunction("Resolution");
  conv->addFunction(func);

  // add resolution file
  IFunction::Attribute attr("__ConvFit_Resolution");
  func->setAttribute("Workspace", attr);

  // --------------------------------------------------------
  // --- Composite / Convolution / Model / Delta Function ---
  // --------------------------------------------------------
  CompositeFunction_sptr model(new CompositeFunction);

  bool useDeltaFunc = m_blnManager->value(m_properties["UseDeltaFunc"]);

  if (useDeltaFunc) {
    func = FunctionFactory::Instance().createFunction("DeltaFunction");
    index = model->addFunction(func);
    std::string parName = createParName(index);
    populateFunction(func, model, m_properties["Delta Function"], parName,
                     false);
  }

  // ------------------------------------------------------------
  // --- Composite / Convolution / Model / Temperature Factor ---
  // ------------------------------------------------------------

  // create temperature correction function to multiply with the lorentzians
  IFunction_sptr tempFunc;
  QString temperature = m_uiForm.leTempCorrection->text();
  bool useTempCorrection =
      (!temperature.isEmpty() && m_uiForm.ckTempCorrection->isChecked());

  // -----------------------------------------------------
  // --- Composite / Convolution / Model / Lorentzians ---
  // -----------------------------------------------------
  std::string prefix1;
  std::string prefix2;

  int fitTypeIndex = m_uiForm.cbFitType->currentIndex();
  if (fitTypeIndex > 0) {
    size_t subIndex = 0;
    auto product = boost::dynamic_pointer_cast<CompositeFunction>(
        FunctionFactory::Instance().createFunction("ProductFunction"));

    if (useTempCorrection) {
      createTemperatureCorrection(product);
    }

    // Add 1st Lorentzian

    // if temperature not included then product is Lorentzian * 1
    // create product function for temp * Lorentzian

    std::string functionName = m_uiForm.cbFitType->currentText().toStdString();

    if (fitTypeIndex == 1 || fitTypeIndex == 2) {
      functionName = "Lorentzian";
    }
    func = FunctionFactory::Instance().createFunction(functionName);
    subIndex = product->addFunction(func);
    index = model->addFunction(product);
    prefix1 = createParName(index, subIndex);

    populateFunction(func, model, m_properties["FitFunction1"], prefix1, false);

    // Add 2nd Lorentzian
    if (fitTypeIndex == 2) {
      // if temperature not included then product is Lorentzian * 1
      // create product function for temp * Lorentzian
      auto product = boost::dynamic_pointer_cast<CompositeFunction>(
          FunctionFactory::Instance().createFunction("ProductFunction"));

      if (useTempCorrection) {
        createTemperatureCorrection(product);
      }

      func = FunctionFactory::Instance().createFunction(functionName);
      subIndex = product->addFunction(func);
      index = model->addFunction(product);
      prefix2 = createParName(index, subIndex);

      populateFunction(func, model, m_properties["FitFunction2"], prefix2,
                       false);
    }
  }

  conv->addFunction(model);
  comp->addFunction(conv);

  // Tie PeakCentres together
  if (tieCentres) {
    std::string tieL = prefix1 + "PeakCentre";
    std::string tieR = prefix2 + "PeakCentre";
    model->tie(tieL, tieR);
  }

  comp->applyTies();
  return comp;
}

/**
 * Creates the correction for the temperature
 */
void ConvFit::createTemperatureCorrection(CompositeFunction_sptr product) {
  // create temperature correction function to multiply with the lorentzians
  IFunction_sptr tempFunc;
  QString temperature = m_uiForm.leTempCorrection->text();

  // create user function for the exponential correction
  // (x*temp) / 1-exp(-(x*temp))
  tempFunc = FunctionFactory::Instance().createFunction("UserFunction");
  // 11.606 is the conversion factor from meV to K
  std::string formula = "((x*11.606)/Temp) / (1 - exp(-((x*11.606)/Temp)))";
  IFunction::Attribute att(formula);
  tempFunc->setAttribute("Formula", att);
  tempFunc->setParameter("Temp", temperature.toDouble());

  product->addFunction(tempFunc);
  product->tie("f0.Temp", temperature.toStdString());
  product->applyTies();
}

/**
 * Obtains the instrument resolution from the provided workspace
 * @param workspaceName The workspaces which holds the instrument
 *                      resolution
 * @return The resolution of the instrument. returns 0 if no resolution data
 * could be found
 */
double ConvFit::getInstrumentResolution(MatrixWorkspace_sptr workspace) {
  using namespace Mantid::API;

  double resolution = 0.0;
  try {
    Mantid::Geometry::Instrument_const_sptr inst = workspace->getInstrument();
    std::vector<std::string> analysers = inst->getStringParameter("analyser");
    if (analysers.empty()) {
      g_log.warning("Could not load instrument resolution from parameter file");
      return 0.0;
    }

    std::string analyser = analysers[0];
    std::string idfDirectory =
        Mantid::Kernel::ConfigService::Instance().getString(
            "instrumentDefinition.directory");

    // If the analyser component is not already in the data file then load it
    // from the parameter file
    if (inst->getComponentByName(analyser) == NULL ||
        inst->getComponentByName(analyser)
                ->getNumberParameter("resolution")
                .size() == 0) {
      std::string reflection = inst->getStringParameter("reflection")[0];

      IAlgorithm_sptr loadParamFile =
          AlgorithmManager::Instance().create("LoadParameterFile");
      loadParamFile->setChild(true);
      loadParamFile->initialize();
      loadParamFile->setProperty("Workspace", workspace);
      loadParamFile->setProperty(
          "Filename", idfDirectory + inst->getName() + "_" + analyser + "_" +
                          reflection + "_Parameters.xml");
      loadParamFile->execute();

      if (!loadParamFile->isExecuted()) {
        g_log.warning("Could not load parameter file, ensure instrument "
                      "directory is in data search paths.");
        return 0.0;
      }

      inst = workspace->getInstrument();
    }
    if (inst->getComponentByName(analyser) != NULL) {
      resolution = inst->getComponentByName(analyser)
                       ->getNumberParameter("resolution")[0];
    } else {
      resolution = inst->getNumberParameter("resolution")[0];
    }
  } catch (Mantid::Kernel::Exception::NotFoundError &e) {
    UNUSED_ARG(e);

    g_log.warning("Could not load instrument resolution from parameter file");
    resolution = 0.0;
  }

  return resolution;
}

/**
 * Initialises the property values for any of the fit type
 * @param propName The name of the property group
 * @return The populated property group representing a fit type
 */
QtProperty *ConvFit::createFitType(const QString &propName) {
  return createFitType(m_grpManager->addProperty(propName));
}

QtProperty *ConvFit::createFitType(QtProperty *fitTypeGroup,
                                   const bool &addProperties) {
  QString cbName = fitTypeGroup->propertyName();
  auto params = getFunctionParameters(cbName);

  for (auto it = params.begin(); it != params.end(); ++it) {
    QString paramName = cbName + "." + *it;
    m_properties[paramName] = m_dblManager->addProperty(*it);
    m_dblManager->setDecimals(m_properties[paramName], NUM_DECIMALS);
    if (QString(*it).compare("FWHM") == 0) {
      m_dblManager->setValue(m_properties[paramName], 0.02);
    }

    if (addProperties)
      fitTypeGroup->addSubProperty(m_properties[paramName]);
  }
  return fitTypeGroup;
}

/**
 * Populates the properties of a function with given values
 * @param func The function currently being added to the composite
 * @param comp A composite function of the previously called functions
 * @param group The QtProperty representing the fit type
 * @param pref The index of the functions eg. (f0.f1)
 * @param tie Bool to state if parameters are to be tied together
 */
void ConvFit::populateFunction(IFunction_sptr func, IFunction_sptr comp,
                               QtProperty *group, const std::string &pref,
                               bool tie) {
  // Get sub-properties of group and apply them as parameters on the function
  // object
  QList<QtProperty *> props = group->subProperties();

  for (int i = 0; i < props.size(); i++) {
    if (tie || !props[i]->subProperties().isEmpty()) {
      std::string name = pref + props[i]->propertyName().toStdString();
      std::string value = props[i]->valueText().toStdString();
      comp->tie(name, value);
    } else {
      std::string propName = props[i]->propertyName().toStdString();
      double propValue = props[i]->valueText().toDouble();
      if (propValue != 0.0) {
        if (func->hasAttribute(propName))
          func->setAttributeValue(propName, propValue);
        else
          func->setParameter(propName, propValue);
      }
    }
  }
}

/**
 * Generate a string to describe the fit type selected by the user.
 * Used when naming the resultant workspaces.
 *
 * Assertions used to guard against any future changes that don't take
 * workspace naming into account.
 *
 * @returns the generated QString.
 */
QString ConvFit::fitTypeString() const {
  QString fitType("");

  if (m_blnManager->value(m_properties["UseDeltaFunc"]))
    fitType += "Delta";

  fitType += m_fitStrings.at(m_uiForm.cbFitType->currentIndex());

  return fitType;
}

/**
 * Generate a string to describe the background selected by the user.
 * Used when naming the resultant workspaces.
 *
 * Assertions used to guard against any future changes that don't take
 * workspace naming into account.
 *
 * @returns the generated QString.
 */
QString ConvFit::backgroundString() const {
  switch (m_uiForm.cbBackground->currentIndex()) {
  case 0:
    return "FixF_s";
  case 1:
    return "FitF_s";
  case 2:
    return "FitL_s";
  default:
    return "";
  }
}

/**
 * Generates a string that defines the fitting minimizer based on the user
 * options.
 *
 * @return Minimizer as a string
 */
QString ConvFit::minimizerString(QString outputName) const {
  QString minimizer = "Levenberg-Marquardt";

  if (m_blnManager->value(m_properties["UseFABADA"])) {
    minimizer = "FABADA";

    int chainLength = static_cast<int>(
        m_dblManager->value(m_properties["FABADAChainLength"]));
    minimizer += ",ChainLength=" + QString::number(chainLength);

    double convergenceCriteria =
        m_dblManager->value(m_properties["FABADAConvergenceCriteria"]);
    minimizer += ",ConvergenceCriteria=" + QString::number(convergenceCriteria);

    double jumpAcceptanceRate =
        m_dblManager->value(m_properties["FABADAJumpAcceptanceRate"]);
    minimizer += ",JumpAcceptanceRate=" + QString::number(jumpAcceptanceRate);

    minimizer += ",PDF=" + outputName + "_PDF";

    if (m_blnManager->value(m_properties["OutputFABADAChain"]))
      minimizer += ",Chains=" + outputName + "_Chain";

    if (m_blnManager->value(m_properties["FABADASimAnnealingApplied"])) {
      minimizer += ",SimAnnealingApplied=1";
    } else {
      minimizer += ",SimAnnealingApplied=0";
    }
    double maximumTemperature =
        m_dblManager->value(m_properties["FABADAMaximumTemperature"]);
    minimizer += ",MaximumTemperature=" + QString::number(maximumTemperature);
    double refSteps =
        m_dblManager->value(m_properties["FABADANumRefrigerationSteps"]);
    minimizer += ",NumRefrigerationSteps=" + QString::number(refSteps);
    double simAnnealingIter =
        m_dblManager->value(m_properties["FABADASimAnnealingIterations"]);
    minimizer += ",SimAnnealingIterations=" + QString::number(simAnnealingIter);
    bool overexploration =
        m_blnManager->value(m_properties["FABADAOverexploration"]);
    minimizer += ",Overexploration=";
    minimizer += overexploration ? "1" : "0";

    double stepsBetweenValues =
        m_dblManager->value(m_properties["FABADAStepsBetweenValues"]);
    minimizer += ",StepsBetweenValues=" + QString::number(stepsBetweenValues);

    double inactiveConvCriterion =
        m_dblManager->value(m_properties["FABADAInactiveConvergenceCriterion"]);
    minimizer += ",InnactiveConvergenceCriterion=" +
                 QString::number(inactiveConvCriterion);

    double binsPDF = m_dblManager->value(m_properties["FABADANumberBinsPDF"]);
    minimizer += ",NumberBinsPDF=" + QString::number(binsPDF);
  }

  return minimizer;
}

/**
 * Changes property tree and plot appearance based on Fit Type
 * @param index A reference to the Fit Type (0-9)
 */
void ConvFit::typeSelection(int index) {

  auto hwhmRangeSelector = m_uiForm.ppPlot->getRangeSelector("ConvFitHWHM");

  if (index == 0) {
    hwhmRangeSelector->setVisible(false);
  } else if (index < 3) {
    hwhmRangeSelector->setVisible(true);
  } else {
    hwhmRangeSelector->setVisible(false);
    m_uiForm.ckPlotGuess->setChecked(false);
    m_blnManager->setValue(m_properties["UseDeltaFunc"], false);
  }

  // Disable Plot Guess and Use Delta Function for DiffSphere and
  // DiffRotDiscreteCircle
  m_uiForm.ckPlotGuess->setEnabled(index < 3 || index == 7);
  m_properties["UseDeltaFunc"]->setEnabled(index < 3 || index == 7);

  updatePlotOptions();
}

/**
 * Add/Remove sub property 'BGA1' from background based on Background type
 * @param index A reference to the Background type
 */
void ConvFit::bgTypeSelection(int index) {
  if (index == 2) {
    m_properties["LinearBackground"]->addSubProperty(m_properties["BGA1"]);
  } else {
    m_properties["LinearBackground"]->removeSubProperty(m_properties["BGA1"]);
  }
}

/**
 * Updates the plot in the GUI window
 */
void ConvFit::updatePlot() {
  using Mantid::Kernel::Exception::NotFoundError;

  auto inputWs = inputWorkspace();

  if (!inputWs) {
    g_log.error("No workspace loaded, cannot create preview plot.");
    return;
  }

  bool plotGuess = m_uiForm.ckPlotGuess->isChecked();
  m_uiForm.ckPlotGuess->setChecked(plotGuess);

  int specNo = m_uiForm.spPlotSpectrum->text().toInt();

  m_uiForm.ppPlot->clear();
  setPreviewPlotWorkspace(inputWs);
  m_uiForm.ppPlot->addSpectrum("Sample", inputWs, specNo);

  // Default FWHM to resolution of instrument
  double resolution = getInstrumentResolution(inputWorkspace());
  if (resolution > 0) {
    m_dblManager->setValue(m_properties["InstrumentResolution"], resolution);
    m_dblManager->setValue(m_properties["InstrumentResolution"], resolution);
  }

  // If there is a result workspace plot then plot it
  const auto baseGroupName = m_baseName.toStdString() + "_Workspaces";
  const auto singleGroupName =
      m_singleFitOutputName.toStdString() + "_Workspaces";

  if (AnalysisDataService::Instance().doesExist(baseGroupName)) {
    plotOutput(baseGroupName, specNo);
  } else if (AnalysisDataService::Instance().doesExist(singleGroupName)) {
    plotOutput(singleGroupName, specNo);
  }
}

void ConvFit::updatePlotRange() {

  try {
    const QPair<double, double> curveRange =
        m_uiForm.ppPlot->getCurveRange("Sample");
    const std::pair<double, double> range(curveRange.first, curveRange.second);
    m_uiForm.ppPlot->getRangeSelector("ConvFitRange")
        ->setRange(range.first, range.second);
    m_dblManager->setValue(m_properties["StartX"], range.first);
    m_dblManager->setValue(m_properties["EndX"], range.second);
  } catch (std::invalid_argument &exc) {
    showMessageBox(exc.what());
  }
}

/*
 * Plots the specified spectrum of the output group workspace witht the
 *specified name;
 * created from Convolution Fitting.
 *
 * @param outputWsName  The name of the output workspace whose data to plot.
 * @param specNo        The spectrum number to plot from the output workspace.
 */
void ConvFit::plotOutput(std::string const &outputWsName, int specNo) {
  WorkspaceGroup_sptr outputGroup =
      AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>(outputWsName);
  if (specNo - m_runMin >= static_cast<int>(outputGroup->size()))
    return;
  if ((specNo - m_runMin) >= 0) {
    MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(
        outputGroup->getItem(specNo - m_runMin));
    if (ws) {
      setPreviewPlotWorkspace(ws);
      m_uiForm.ppPlot->addSpectrum("Fit", ws, 1, Qt::red);
      m_uiForm.ppPlot->addSpectrum("Diff", ws, 2, Qt::blue);
      if (m_uiForm.ckPlotGuess->isChecked()) {
        m_uiForm.ppPlot->removeSpectrum("Guess");
        m_uiForm.ckPlotGuess->setChecked(false);
      }
    }
  }
}

void ConvFit::updateProperties(int specNo) {
  auto parameterNames = m_propertyToParameter.keys();

  for (auto &fitFunction :
       indexToFitFunctions(m_uiForm.cbFitType->currentIndex())) {
    updateProperties(specNo, fitFunction);
  }
}

void ConvFit::updateProperties(int specNo, const QString &fitFunction) {
  bool isTwoLorentzian = fitFunction == "Lorentzian 2";
  bool specOutOfBounds = specNo < m_runMin || m_runMax < specNo;

  for (auto &param : getFunctionParameters(fitFunction)) {
    auto propertyName = fitFunction + "." + param;

    if (!specOutOfBounds && m_propertyToParameter.contains(propertyName)) {
      auto paramName = m_propertyToParameter[propertyName];
      m_dblManager->setValue(m_properties[propertyName],
                             m_parameterValues[paramName][specNo]);
    } else {
      if (isTwoLorentzian) {
        m_dblManager->setValue(m_properties[propertyName],
                               m_defaultParams["default_" + param]);
      } else {
        m_dblManager->setValue(m_properties[propertyName],
                               m_defaultParams[param]);
      }
    }
  }
}

QVector<QString> ConvFit::indexToFitFunctions(const int &fitTypeIndex) {
  QVector<QString> fitFunctions = {};

  if (m_blnManager->value(m_properties["UseDeltaFunc"])) {
    fitFunctions.push_back("Delta Function");
  }

  if (fitTypeIndex == 1)
    fitFunctions.push_back("Lorentzian 1");
  else if (fitTypeIndex == 2) {
    fitFunctions.push_back("Lorentzian 1");
    fitFunctions.push_back("Lorentzian 2");
  } else if (fitTypeIndex == 3)
    fitFunctions.push_back("InelasticDiffSphere");
  else if (fitTypeIndex == 4)
    fitFunctions.push_back("InelasticDiffRotDiscreteCircle");
  else if (fitTypeIndex == 5)
    fitFunctions.push_back("ElasticDiffSphere");
  else if (fitTypeIndex == 6)
    fitFunctions.push_back("ElasticDiffRotDiscreteCircle");
  else if (fitTypeIndex == 7) {
    fitFunctions.push_back("StretchedExpFT");
  }
  return fitFunctions;
}

/**
 * Updates the guess for the plot
 */
void ConvFit::plotGuess() {
  m_uiForm.ppPlot->removeSpectrum("Guess");

  // Do nothing if there is not a sample and resolution
  if (!(m_uiForm.dsSampleInput->isValid() && m_uiForm.dsResInput->isValid() &&
        m_uiForm.ckPlotGuess->isChecked()))
    return;

  if (m_uiForm.cbFitType->currentIndex() > 2 &&
      m_uiForm.cbFitType->currentIndex() != 7) {
    return;
  }

  bool tieCentres = (m_uiForm.cbFitType->currentIndex() == 2);
  CompositeFunction_sptr function = createFunction(tieCentres);
  auto inputWs = inputWorkspace();

  if (!inputWs) {
    updatePlot();
    return;
  }

  const size_t binIndexLow =
      inputWs->binIndexOf(m_dblManager->value(m_properties["StartX"]));
  const size_t binIndexHigh =
      inputWs->binIndexOf(m_dblManager->value(m_properties["EndX"]));
  const size_t nData = binIndexHigh - binIndexLow;

  const auto &xPoints = inputWs->points(0);

  std::vector<double> dataX(nData);
  std::copy(&xPoints[binIndexLow], &xPoints[binIndexLow + nData],
            dataX.begin());

  FunctionDomain1DVector domain(dataX);
  FunctionValues outputData(domain);
  function->function(domain, outputData);

  std::vector<double> dataY(nData);
  for (size_t i = 0; i < nData; i++) {
    dataY[i] = outputData.getCalculated(i);
  }

  IAlgorithm_sptr createWsAlg =
      AlgorithmManager::Instance().create("CreateWorkspace");
  createWsAlg->initialize();
  createWsAlg->setChild(true);
  createWsAlg->setLogging(false);
  createWsAlg->setProperty("OutputWorkspace", "__GuessAnon");
  createWsAlg->setProperty("NSpec", 1);
  createWsAlg->setProperty("DataX", dataX);
  createWsAlg->setProperty("DataY", dataY);
  createWsAlg->execute();
  MatrixWorkspace_sptr guessWs = createWsAlg->getProperty("OutputWorkspace");

  m_uiForm.ppPlot->addSpectrum("Guess", guessWs, 0, Qt::green);
}

/**
 * Runs the single fit algorithm
 */
void ConvFit::singleFit() {
  // Validate tab before running a single fit
  if (!validate()) {
    return;
  }
  // disconnect signal for single fit
  disconnect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
             SLOT(singleFit(bool)));
  // ensure algorithm was successful
  m_uiForm.ckPlotGuess->setChecked(false);
  int specNo = m_uiForm.spPlotSpectrum->value();
  m_runMin = specNo;
  m_runMax = specNo;
  std::string specNoStr = m_uiForm.spPlotSpectrum->text().toStdString();

  m_fitFunctions = indexToFitFunctions(m_uiForm.cbFitType->currentIndex());
  auto cfs = sequentialFit(specNoStr, specNoStr, m_singleFitOutputName);

  // Connection to singleFitComplete SLOT (post algorithm completion)
  m_batchAlgoRunner->addAlgorithm(cfs);
  connect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
          SLOT(singleFitComplete(bool)));
  m_batchAlgoRunner->executeBatchAsync();
}

/**
 * Handle completion of the fit algorithm for single fit.
 *
 * @param error :: If the fit algorithm failed
 */
void ConvFit::singleFitComplete(bool error) {
  // Disconnect signal for single fit complete
  disconnect(m_batchAlgoRunner, SIGNAL(batchComplete(bool)), this,
             SLOT(singleFitComplete(bool)));
  algorithmComplete(error, m_singleFitOutputName);
}

/**
 * Handles the user entering a new minimum spectrum index.
 *
 * Prevents the user entering an overlapping spectra range.
 *
 * @param value Minimum spectrum index
 */
void ConvFit::specMinChanged(int value) {
  m_uiForm.spSpectraMax->setMinimum(value);
}

/**
 * Handles the user entering a new maximum spectrum index.
 *
 * Prevents the user entering an overlapping spectra range.
 *
 * @param value Maximum spectrum index
 */
void ConvFit::specMaxChanged(int value) {
  m_uiForm.spSpectraMin->setMaximum(value);
}

void ConvFit::minChanged(double val) {
  m_dblManager->setValue(m_properties["StartX"], val);
}

void ConvFit::maxChanged(double val) {
  m_dblManager->setValue(m_properties["EndX"], val);
}

void ConvFit::hwhmChanged(double val) {
  const double peakCentre =
      m_dblManager->value(m_properties["Lorentzian 1.PeakCentre"]);
  // Always want FWHM to display as positive.
  const double hwhm = std::fabs(val - peakCentre);
  // Update the property
  auto hwhmRangeSelector = m_uiForm.ppPlot->getRangeSelector("ConvFitHWHM");
  hwhmRangeSelector->blockSignals(true);
  QString propName = "Lorentzian 1.FWHM";
  if (m_uiForm.cbFitType->currentIndex() == 1) {
    propName = "One Lorentzian";
  }
  m_dblManager->setValue(m_properties[propName], hwhm * 2);
  hwhmRangeSelector->blockSignals(false);
}

void ConvFit::backgLevel(double val) {
  m_dblManager->setValue(m_properties["BGA0"], val);
}

void ConvFit::updateRS(QtProperty *prop, double val) {
  auto fitRangeSelector = m_uiForm.ppPlot->getRangeSelector("ConvFitRange");
  auto backRangeSelector =
      m_uiForm.ppPlot->getRangeSelector("ConvFitBackRange");

  if (prop == m_properties["StartX"]) {
    fitRangeSelector->setMinimum(val);
  } else if (prop == m_properties["EndX"]) {
    fitRangeSelector->setMaximum(val);
  } else if (prop == m_properties["BGA0"]) {
    backRangeSelector->setMinimum(val);
  } else if (prop == m_properties["InstrumentResolution"]) {
    hwhmUpdateRS(val);
  }
}

void ConvFit::hwhmUpdateRS(double val) {
  auto hwhmRangeSelector = m_uiForm.ppPlot->getRangeSelector("ConvFitHWHM");
  hwhmRangeSelector->setMinimum(-val / 2);
  hwhmRangeSelector->setMaximum(+val / 2);
}

void ConvFit::checkBoxUpdate(QtProperty *prop, bool checked) {
  UNUSED_ARG(checked);

  if (prop == m_properties["UseDeltaFunc"]) {
    updatePlotOptions();
    if (checked) {
      m_properties["Delta Function"]->addSubProperty(
          m_properties["Delta Function.Height"]);
      m_dblManager->setValue(m_properties["Delta Function.Height"], 1.0000);
      m_properties["Delta Function"]->addSubProperty(
          m_properties["Delta Function.Centre"]);
      m_dblManager->setValue(m_properties["Delta Function.Centre"], 0.0000);
    } else {
      m_properties["Delta Function"]->removeSubProperty(
          m_properties["Delta Function.Height"]);
      m_properties["Delta Function"]->removeSubProperty(
          m_properties["Delta Function.Centre"]);
    }
  } else if (prop == m_properties["UseFABADA"]) {
    if (checked) {
      // FABADA needs a much higher iteration limit
      m_dblManager->setValue(m_properties["MaxIterations"], 20000);
      showFABADA(m_blnManager->value(m_properties["FABADAAdvanced"]));
    } else {
      m_dblManager->setValue(m_properties["MaxIterations"], 500);
      hideFABADA();
    }
  } else if (prop == m_properties["FABADAAdvanced"]) {
    showFABADA(checked);
  }
}

/** Shows FABADA minimizer options in the property browser
 *
 * @param advanced :: true if advanced options should be shown, false otherwise
 */
void ConvFit::showFABADA(bool advanced) {

  m_properties["FABADA"]->addSubProperty(m_properties["OutputFABADAChain"]);
  m_properties["FABADA"]->addSubProperty(m_properties["FABADAChainLength"]);
  m_properties["FABADA"]->addSubProperty(
      m_properties["FABADAConvergenceCriteria"]);
  m_properties["FABADA"]->addSubProperty(
      m_properties["FABADAJumpAcceptanceRate"]);
  m_properties["FABADA"]->addSubProperty(m_properties["FABADAAdvanced"]);
  if (advanced) {
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADAStepsBetweenValues"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADAInactiveConvergenceCriterion"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADASimAnnealingApplied"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADAMaximumTemperature"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADANumRefrigerationSteps"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADASimAnnealingIterations"]);
    m_properties["FABADA"]->addSubProperty(
        m_properties["FABADAOverexploration"]);
    m_properties["FABADA"]->addSubProperty(m_properties["FABADANumberBinsPDF"]);
  } else {
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADAStepsBetweenValues"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADAInactiveConvergenceCriterion"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADASimAnnealingApplied"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADAMaximumTemperature"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADANumRefrigerationSteps"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADASimAnnealingIterations"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADAOverexploration"]);
    m_properties["FABADA"]->removeSubProperty(
        m_properties["FABADANumberBinsPDF"]);
  }
}

/** Hide FABADA minimizer options from the browser
 *
 */
void ConvFit::hideFABADA() {

  m_properties["FABADA"]->removeSubProperty(m_properties["OutputFABADAChain"]);
  m_properties["FABADA"]->removeSubProperty(m_properties["FABADAChainLength"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAConvergenceCriteria"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAJumpAcceptanceRate"]);
  m_properties["FABADA"]->removeSubProperty(m_properties["FABADAAdvanced"]);

  // Advanced options
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAStepsBetweenValues"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAInactiveConvergenceCriterion"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADASimAnnealingApplied"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAMaximumTemperature"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADANumRefrigerationSteps"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADASimAnnealingIterations"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADAOverexploration"]);
  m_properties["FABADA"]->removeSubProperty(
      m_properties["FABADANumberBinsPDF"]);
}

void ConvFit::fitContextMenu(const QPoint &) {
  QtBrowserItem *item(nullptr);

  item = m_cfTree->currentItem();

  if (!item)
    return;

  // is it a fit property ?
  QtProperty *prop = item->property();
  if (prop == m_properties["StartX"] || prop == m_properties["EndX"])
    return;

  // is it already fixed?
  bool fixed = prop->propertyManager() != m_dblManager;
  if (fixed && prop->propertyManager() != m_stringManager)
    return;

  // Create the menu
  QMenu *menu = new QMenu("ConvFit", m_cfTree);
  QAction *action;

  if (!fixed) {
    action = new QAction("Fix", m_parentWidget);
    connect(action, SIGNAL(triggered()), this, SLOT(fixItem()));
  } else {
    action = new QAction("Remove Fix", m_parentWidget);
    connect(action, SIGNAL(triggered()), this, SLOT(unFixItem()));
  }

  menu->addAction(action);

  // Show the menu
  menu->popup(QCursor::pos());
}

void ConvFit::fixItem() {
  QtBrowserItem *item = m_cfTree->currentItem();

  // Determine what the property is.
  QtProperty *prop = item->property();
  QtProperty *fixedProp = m_stringManager->addProperty(prop->propertyName());
  QtProperty *fprlbl = m_stringManager->addProperty("Fixed");
  fixedProp->addSubProperty(fprlbl);
  m_stringManager->setValue(fixedProp, prop->valueText());

  item->parent()->property()->addSubProperty(fixedProp);

  m_fixedProps[fixedProp] = prop;

  item->parent()->property()->removeSubProperty(prop);
}

void ConvFit::unFixItem() {
  QtBrowserItem *item = m_cfTree->currentItem();

  QtProperty *prop = item->property();
  if (prop->subProperties().empty()) {
    item = item->parent();
    prop = item->property();
  }

  item->parent()->property()->addSubProperty(m_fixedProps[prop]);
  item->parent()->property()->removeSubProperty(prop);
  m_fixedProps.remove(prop);
  QtProperty *proplbl = prop->subProperties()[0];
  delete proplbl;
  delete prop;
}

void ConvFit::showTieCheckbox(QString fitType) {
  m_uiForm.ckTieCentres->setVisible(fitType == "Two Lorentzians");
}

/**
 * Gets a list of parameters for a given fit function.
 * @return List of parameters
 */
QVector<QString> ConvFit::getFunctionParameters(QString functionName) const {
  IFunction_sptr func;
  bool isLorentzian = boost::starts_with(functionName, "Lorentzian");

  if (isLorentzian) {
    func = FunctionFactory::Instance().createFunction("Lorentzian");
  } else {
    func = FunctionFactory::Instance().createFunction(
        functionName.replace(" ", "").toStdString());
  }

  return IndirectTab::convertStdStringVector(func->getParameterNames());
}

/**
 * Handles a new fit function being selected.
 * @param functionName Name of new fit function
 */
void ConvFit::fitFunctionSelected(int fitTypeIndex) {
  // If resolution file has been entered update default FWHM to resolution
  if (m_uiForm.dsResInput->getCurrentDataName().compare("") != 0) {
    const auto res = getInstrumentResolution(inputWorkspace());
    m_defaultParams["FWHM"] = res;
    m_defaultParams["default_FWHM"] = res;
  }

  auto fitFunctions = indexToFitFunctions(fitTypeIndex);
  if (fitFunctions.isEmpty()) {
    return;
  }

  auto lastFunction = fitFunctions.last();

  // Remove previous parameters from tree
  m_cfTree->removeProperty(m_properties["FitFunction1"]);
  m_cfTree->removeProperty(m_properties["FitFunction2"]);

  m_uiForm.ckPlotGuess->setChecked(false);

  updatePlotOptions();

  // Two Lorentzians Fit
  if (lastFunction == "Lorentzian 2") {
    m_properties["FitFunction1"] = m_grpManager->addProperty("Lorentzian 1");
    m_cfTree->addProperty(m_properties["FitFunction1"]);
    m_properties["FitFunction2"] = m_grpManager->addProperty("Lorentzian 2");
    m_cfTree->addProperty(m_properties["FitFunction2"]);
  } else {
    m_properties["FitFunction1"] = m_grpManager->addProperty(lastFunction);
    m_cfTree->addProperty(m_properties["FitFunction1"]);
  }

  // If there are parameters in the list, add them
  addDefaultParametersToTree(fitFunctions);
}

/**
 * Adds all the parameters that are required for the fit function to the
 * parameter tree
 * @param fitFunction	:: The name of the fit function
 */
void ConvFit::addDefaultParametersToTree(const QVector<QString> &fitFunctions) {

  for (auto &fitFunction : fitFunctions) {
    addDefaultParametersToTree(fitFunction);
  }
}

void ConvFit::addDefaultParametersToTree(const QString &fitFunction) {
  auto paramNames = getFunctionParameters(fitFunction);
  bool isDelta = fitFunction == "Delta Function";
  bool isTwoLorentzian = !isDelta && fitFunction == "Lorentzian 2";

  for (auto &paramName : paramNames) {
    QString propertyName = fitFunction + "." + paramName;
    m_properties[propertyName] = m_dblManager->addProperty(paramName);

    if (isTwoLorentzian) {
      paramName = "default_" + paramName;
    }

    if (m_defaultParams.contains(paramName)) {
      m_dblManager->setValue(m_properties[propertyName],
                             m_defaultParams[paramName]);
    } else {
      m_dblManager->setValue(m_properties[propertyName], 0);
    }
    m_dblManager->setDecimals(m_properties[propertyName], NUM_DECIMALS);

    if (isTwoLorentzian) {
      m_properties["FitFunction2"]->addSubProperty(m_properties[propertyName]);
    } else if (!isDelta) {
      m_properties["FitFunction1"]->addSubProperty(m_properties[propertyName]);
    }
  }
}

/**
 * Populates the plot combobox
 */
void ConvFit::updatePlotOptions() {
  m_uiForm.cbPlotType->clear();

  const int fitType = m_uiForm.cbFitType->currentIndex();
  const auto fitFunctions = indexToFitFunctions(fitType);
  QStringList plotOptions;

  for (auto &fitFunction : fitFunctions) {
    plotOptions.append(getFunctionParameters(fitFunction).toList());
  }

  if (fitType != 0 || m_blnManager->value(m_properties["UseDeltaFunc"])) {
    plotOptions << "All";
  }
  m_uiForm.cbPlotType->addItems(plotOptions);
}

/**
 * Populates the default parameter map with the initial default values
 * @param map :: The default value QMap to populate
 * @return The QMap populated with default values
 */
QMap<QString, double>
ConvFit::createDefaultParamsMap(QMap<QString, double> map) {
  // If the parameters from a One Lorentzian fit are present
  if (map.contains("PeakCentre")) {
    map.remove("PeakCentre");
    map.remove("FWHM");
  }
  // Reset all parameters to default of 1
  map.insert("Amplitude", 1.0);
  map.insert("beta", 1.0);
  map.insert("Decay", 1.0);
  map.insert("Diffusion", 1.0);
  map.insert("height", 1.0); // Lower case in StretchedExp - this can be
                             // improved with a case insensitive check
  map.insert("Height", 1.0);
  map.insert("Intensity", 1.0);
  map.insert("Radius", 1.0);
  map.insert("tau", 1.0);
  map.insert("default_Amplitude", 1.0); // Used in the case of 2L fit
  return map;
}

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt
