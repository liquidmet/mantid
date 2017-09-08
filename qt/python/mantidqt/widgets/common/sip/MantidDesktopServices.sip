// It would be nice to hide these namespaces but %HideNamespace is only available in sip >= 4.19
// For now we handle the renaming in the __init__.py of this module
namespace MantidQt {
namespace API {

class MantidDesktopServices
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
