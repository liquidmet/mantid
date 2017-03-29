//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAlgorithms/EstimateMuonAsymmetryFromCounts.h"
#include "MantidAlgorithms/MuonAsymmetryHelper.h"

#include "MantidAPI/IFunction.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/Workspace_fwd.h"
#include "MantidAPI/WorkspaceFactory.h"

#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/PhysicalConstants.h"

#include <cmath>
#include <numeric>
#include <vector>

namespace Mantid {
namespace Algorithms {

using namespace Kernel;
using API::Progress;
using std::size_t;

// Register the class into the algorithm factory
DECLARE_ALGORITHM(EstimateMuonAsymmetryFromCounts)

/** Initialisation method. Declares properties to be used in algorithm.
 *
 */
void EstimateMuonAsymmetryFromCounts::init() {
  declareProperty(make_unique<API::WorkspaceProperty<API::MatrixWorkspace>>(
                      "InputWorkspace", "", Direction::Input),
                  "The name of the input 2D workspace.");
  declareProperty(make_unique<API::WorkspaceProperty<API::MatrixWorkspace>>(
                      "OutputWorkspace", "", Direction::Output),
                  "The name of the output 2D workspace.");
  std::vector<int> empty;
  declareProperty(
      Kernel::make_unique<Kernel::ArrayProperty<int>>("Spectra", empty),
      "The workspace indices to remove the exponential decay from.");
  declareProperty(
      "StartX", 0.1,
      "The lower limit for calculating the asymmetry (an X value).");
  declareProperty(
      "EndX", 15.0,
      "The upper limit for calculating the asymmetry  (an X value).");
  declareProperty(
      "Normalization Constant", Direction::Output,
      "The normalization constant used for calculating the asymmetry.");

}

/*
* Validate the input parameters
* @returns map with keys corresponding to properties with errors and values
* containing the error messages.
*/
std::map<std::string, std::string>
EstimateMuonAsymmetryFromCounts::validateInputs() {
  // create the map
  std::map<std::string, std::string> validationOutput;
  // check start and end times
  double startX = getProperty("StartX");
  double endX = getProperty("EndX");
  if (startX > endX) {
    validationOutput["StartX"] = "Start time is after the end time.";
  } else if (startX == endX) {
    validationOutput["StartX"] = "Start and end times are equal, there is no "
                                 "data to apply the algorithm to.";
  }
  return validationOutput;
}

/** Executes the algorithm
 *
 */
void EstimateMuonAsymmetryFromCounts::exec() {
  std::vector<int> spectra = getProperty("Spectra");

  // Get original workspace
  API::MatrixWorkspace_const_sptr inputWS = getProperty("InputWorkspace");
  auto numSpectra = inputWS->getNumberHistograms();
  // Create output workspace with same dimensions as input
  API::MatrixWorkspace_sptr outputWS = getProperty("OutputWorkspace");
  if (inputWS != outputWS) {
    outputWS = API::WorkspaceFactory::Instance().create(inputWS);
  }
  double startX = getProperty("StartX");
  double endX = getProperty("EndX");
  const Mantid::API::Run &run = inputWS->run();
  const double numGoodFrames = std::stod(run.getProperty("goodfrm")->value());

  // Share the X values
  for (size_t i = 0; i < static_cast<size_t>(numSpectra); ++i) {
    outputWS->setSharedX(i, inputWS->sharedX(i));
  }

  // No spectra specified = process all spectra
  if (spectra.empty()) {
    spectra = std::vector<int>(numSpectra);
    std::iota(spectra.begin(), spectra.end(), 0);
  }

  Progress prog(this, 0.0, 1.0, numSpectra + spectra.size());
  if (inputWS != outputWS) {

    // Copy all the Y and E data
    PARALLEL_FOR_IF(Kernel::threadSafe(*inputWS, *outputWS))
    for (int64_t i = 0; i < int64_t(numSpectra); ++i) {
      PARALLEL_START_INTERUPT_REGION
      const auto index = static_cast<size_t>(i);
      outputWS->setSharedY(index, inputWS->sharedY(index));
      outputWS->setSharedE(index, inputWS->sharedE(index));
      prog.report();
      PARALLEL_END_INTERUPT_REGION
    }
    PARALLEL_CHECK_INTERUPT_REGION
  }

  // Do the specified spectra only
  int specLength = static_cast<int>(spectra.size());
  PARALLEL_FOR_IF(Kernel::threadSafe(*inputWS, *outputWS))
  for (int i = 0; i < specLength; ++i) {
    PARALLEL_START_INTERUPT_REGION
    const auto specNum = static_cast<size_t>(spectra[i]);
    if (spectra[i] > static_cast<int>(numSpectra)) {
      g_log.error("The spectral index " + std::to_string(spectra[i]) +
                  " is greater than the number of spectra!");
      throw std::invalid_argument("The spectral index " +
                                  std::to_string(spectra[i]) +
                                  " is greater than the number of spectra!");
    }
    // Calculate the normalised counts
    const double normConst = estimateNormalisationConst(
        inputWS->histogram(specNum), numGoodFrames, startX, endX);
    // Calculate the asymmetry
    outputWS->setHistogram(
        specNum, normaliseCounts(inputWS->histogram(specNum), numGoodFrames));
    outputWS->mutableY(specNum) /= normConst;
    outputWS->mutableY(specNum) -= 1.0;
    outputWS->mutableE(specNum) /= normConst;
    setProperty("Normalization Constant", normConst);
 
    prog.report();
    PARALLEL_END_INTERUPT_REGION
  }
  PARALLEL_CHECK_INTERUPT_REGION

  // Update Y axis units
  outputWS->setYUnit("Asymmetry");

  setProperty("OutputWorkspace", outputWS);
}
} // namespace Algorithm
} // namespace Mantid
