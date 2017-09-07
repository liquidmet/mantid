namespace MantidQt {
namespace API {


class MantidDesktopServices /PyName=DesktopServices/
{
%TypeHeaderCode
#include "MantidQtWidgets/Common/MantidDesktopServices.h"
%End
public:
  static bool openUrl(const QUrl &url);
  static void setUrlHandler(const QString &scheme, QObject *receiver,
                            const char *method);
  static void unsetUrlHandler(const QString &scheme);
  static QString storageLocation(QDesktopServices::StandardLocation type);
  static QString displayName(QDesktopServices::StandardLocation type);
};

};
};
