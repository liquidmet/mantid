#ifndef MANTID_MDALGORITHMS_REFLECTOMETRYTRANSFORMP_H_
#define MANTID_MDALGORITHMS_REFLECTOMETRYTRANSFORMP_H_

#include <cmath>

#include "MantidDataObjects/CalculateReflectometry.h"
#include "MantidDataObjects/ReflectometryTransform.h"

namespace Mantid {
namespace MDAlgorithms {

/**
class CalculateReflectometryP: p-type transformation calculator
*/
class CalculateReflectometryP : public DataObjects::CalculateReflectometry {
private:
  double m_sin_theta_i;
  double m_sin_theta_f;

public:
  CalculateReflectometryP() : m_sin_theta_i(0.0), m_sin_theta_f(0.0) {}
  ~CalculateReflectometryP(){};

  void setThetaIncident(double thetaIncident) {
    m_sin_theta_i = sin(to_radians_factor * thetaIncident);
  }

  void setThetaFinal(double thetaFinal) {
    m_sin_theta_f = sin(to_radians_factor * thetaFinal);
  }

  double calculateDim0(double wavelength) const {
    double wavenumber = 2 * M_PI / wavelength;
    double ki = wavenumber * m_sin_theta_i;
    double kf = wavenumber * m_sin_theta_f;
    return ki + kf;
  }

  double calculateDim1(double wavelength) const {
    double wavenumber = 2 * M_PI / wavelength;
    double ki = wavenumber * m_sin_theta_i;
    double kf = wavenumber * m_sin_theta_f;
    return ki - kf;
  }
};

/** ReflectometryTransformP : Calculates workspace(s) of Pi and Pf based on the
  input workspace and incident theta angle.

  @date 2012-06-06

  Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport ReflectometryTransformP
    : public DataObjects::ReflectometryTransform {
public:
  ReflectometryTransformP(double pSumMin, double pSumMax, double pDiffMin,
                          double pDiffMax, double incidentTheta,
                          int numberOfBinsQx = 100, int numberOfBinsQz = 100);
  virtual ~ReflectometryTransformP();
  virtual Mantid::API::IMDEventWorkspace_sptr
  executeMD(Mantid::API::MatrixWorkspace_const_sptr inputWs,
            Mantid::API::BoxController_sptr boxController) const;
  /// Execute transformation
  virtual Mantid::API::MatrixWorkspace_sptr
  execute(Mantid::API::MatrixWorkspace_const_sptr inputWs) const;
};

} // namespace MDAlgorithms
} // namespace Mantid

#endif /* MANTID_MDALGORITHMS_REFLECTOMETRYTRANSFORMP_H_ */
