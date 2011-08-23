#ifndef MANTID_CRYSTAL_FIND_UB_USING_INDEXED_PEAKS_H_
#define MANTID_CRYSTAL_FIND_UB_USING_INDEXED_PEAKS_H_
    
#include "MantidKernel/System.h"
#include "MantidAPI/Algorithm.h" 
#include "MantidKernel/Logger.h"

namespace Mantid
{
namespace Crystal
{
/** FindUB_UsingIndexedPeaks : Algorithm to calculate a UB matrix,
    given a list of peaks that have already been indexed by some means.
    
    @author Dennis Mikkelson(adapted from Andrei Savici's CalculateUMatrix)
    @date   2011-08-17

    Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory & 
                     NScD Oak Ridge National Laboratory

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

    File change history is stored at: 
    <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
  */
  class DLLExport FindUB_UsingIndexedPeaks : public API::Algorithm
  {
  public:
    FindUB_UsingIndexedPeaks();
    ~FindUB_UsingIndexedPeaks();
    
    /// Algorithm's name for identification 
    virtual const std::string name() const 
            { return "FindUB_UsingIndexedPeaks";};

    /// Algorithm's version for identification 
    virtual int version() const 
            { return 1;};

    /// Algorithm's category for identification
    virtual const std::string category() const 
            { return "Crystal";}
    
  private:

    /// Sets documentation strings for this algorithm
    virtual void initDocs();

    /// Initialise the properties
    void init();

    /// Run the algorithm
    void exec();

    /// Static reference to the logger class
        static Kernel::Logger& g_log;
  };


} // namespace Crystal
} // namespace Mantid

#endif  /* MANTID_CRYSTAL_FIND_UB_USING_INDEXED_PEAKS */
