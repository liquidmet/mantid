#ifndef MANTID_DATAOBJECTS_WORKSPACE1D_H_
#define MANTID_DATAOBJECTS_WORKSPACE1D_H_

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidDataObjects/Histogram1D.h"

namespace Mantid
{

namespace Kernel
{
  class Logger;
}

namespace DataObjects
{
/** Concrete workspace implementation. Data is a Histogram1D
    @author Laurent C Chapon, ISIS, RAL
    @date 26/09/2007

    Copyright &copy; 2007-9 STFC Rutherford Appleton Laboratory

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

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>.
    Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport Workspace1D : public API::MatrixWorkspace, public Histogram1D
{

public:
  /// Typedef for the workspace_iterator to use with a Workspace1D
  typedef API::workspace_iterator<API::LocatedDataRef, Workspace1D> iterator;
  /// Typedef for the const workspace_iterator to use with a Workspace1D
  typedef API::workspace_iterator<const API::LocatedDataRef, const Workspace1D> const_iterator;

  /**
  	Gets the name of the workspace type
  	\return Standard string name
  */
  virtual const std::string id() const {return "Workspace1D";}

  Workspace1D();
  virtual ~Workspace1D();

  //section required for iteration
  ///Returns the number of single indexable items in the workspace
  virtual int size() const;
  ///Returns the size of each block of data returned by the dataX accessors
  virtual int blocksize() const;

  const int getNumberHistograms() const { return 1; }

  //inheritance redirections
  // Reimplemented these Histogram1D methods simply to hide the other ones
  /// Sets the x data.
  void setX(const RCtype& X) { Histogram1D::setX(X); }
  /// Sets the data.
  void setData(const RCtype& Y) { Histogram1D::setData(Y); }
  /// Sets the data and errors
  void setData(const RCtype& Y, const RCtype& E) { Histogram1D::setData(Y,E); }  
  /// Sets the x data
  void setX(const RCtype::ptr_type& X) { Histogram1D::setX(X); }
  /// Sets the data and errors
  void setData(const RCtype::ptr_type& Y, const RCtype::ptr_type& E) { Histogram1D::setData(Y,E); }  
  
  ///Returns the x data
  virtual MantidVec& dataX(int const index) { return Histogram1D::dataX(); }
  ///Returns the y data
  virtual MantidVec& dataY(int const index) { return Histogram1D::dataY(); }
  ///Returns the error data
  virtual MantidVec& dataE(int const index) { return Histogram1D::dataE(); }
  /// Returns the x data const
  virtual const MantidVec& dataX(int const index) const {return dataX();}
  /// Returns the y data const
  virtual const MantidVec& dataY(int const index) const {return dataY();}
  /// Returns the error const
  virtual const MantidVec& dataE(int const index) const {return dataE();}

  /// Returns a pointer to the x data
  virtual Kernel::cow_ptr<MantidVec> refX(const int index) const {return Histogram1D::ptrX(); }
  /// Set the specified X array to point to the given existing array
  virtual void setX(const int index, const Kernel::cow_ptr<MantidVec>& X) { setX(X); }

  ///Returns non-const vector of the x data
  virtual MantidVec& dataX() { return Histogram1D::dataX(); }
  ///Returns non-const vector of the y data
  virtual MantidVec& dataY() { return Histogram1D::dataY(); }
  ///Returns non-const vector of the error data
  virtual MantidVec& dataE() { return Histogram1D::dataE(); }
  /// Returns the x data const
  virtual const MantidVec& dataX() const { return Histogram1D::dataX(); }
  /// Returns the y data const
  virtual const MantidVec& dataY() const { return Histogram1D::dataY(); }
  /// Returns the error data const
  virtual const MantidVec& dataE() const { return Histogram1D::dataE(); }

private:
  /// Private copy constructor. NO COPY ALLOWED
  Workspace1D(const Workspace1D&);
  /// Private copy assignment operator. NO ASSIGNMENT ALLOWED
  Workspace1D& operator=(const Workspace1D&);

  // allocates space in a new workspace
  virtual void init(const int &NVectors, const int &XLength, const int &YLength);

  /// Static reference to the logger class
  static Kernel::Logger &g_log;
};

///shared pointer to the Workspace1D class
typedef boost::shared_ptr<Workspace1D> Workspace1D_sptr;
///shared pointer to the Workspace1D class (const version)
typedef boost::shared_ptr<const Workspace1D> Workspace1D_const_sptr;

} // namespace DataObjects
} // namespace Mantid

#endif /*MANTID_DATAOBJECTS_WORKSPACE1D_H_*/
