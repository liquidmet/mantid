#ifndef MANTID_MANTIDWIDGETS_DATAPROCESSORTOSTDSTRINGMAP_H
#define MANTID_MANTIDWIDGETS_DATAPROCESSORTOSTDSTRINGMAP_H
#include "MantidQtMantidWidgets/WidgetDllOption.h"
#include <QString>
#include <map>
#include <string>
/**

This file defines functions for converting a QString -> QString map to a
std::string -> std::string map.

Copyright &copy; 2011-16 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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

File change history is stored at: <https://github.com/mantidproject/mantid>.
Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
namespace MantidQt {
namespace MantidWidgets {
std::map<std::string, std::string> EXPORT_OPT_MANTIDQT_MANTIDWIDGETS
toStdStringMap(std::map<QString, QString> const &inMap);
}
}
#endif // MANTID_MANTIDWIDGETS_DATAPROCESSORTOSTDSTRINGMAP_H
