#include <QPlatformIntegrationPlugin>
#include "emboxvcintegration.h"
#include <qstringlist.h>

QT_BEGIN_NAMESPACE

class QEmboxVCIntegrationPlugin : public QPlatformIntegrationPlugin
{
public:
    QStringList keys() const;
    QPlatformIntegration *create(const QString&, const QStringList&);
};

QStringList QEmboxVCIntegrationPlugin::keys() const
{
    QStringList list;
    list << "EmboxVC";
    return list;
}

QPlatformIntegration* QEmboxVCIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    if (system.toLower() == "emboxvc")
        return new QEmboxVCIntegration;

    return 0;
}

Q_EXPORT_PLUGIN2(emboxvc, QEmboxVCIntegrationPlugin)

QT_END_NAMESPACE
