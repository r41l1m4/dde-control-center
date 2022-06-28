#include "pluginmanagerv20.h"
#include "moduleinterface.h"
#include "frameproxyinterface.h"
#include "adapterv20tov23module.h"

#include <QDir>
#include <QLibrary>
#include <QElapsedTimer>
#include <QPluginLoader>
const QString &PluginDirectory = QStringLiteral("/usr/lib/dde-control-center/modules");

DCC_USE_NAMESPACE
using namespace dccV20;

PluginManagerV20::PluginManagerV20()
    : m_rootModule(nullptr)
{
}

PluginManagerV20::~PluginManagerV20()
{
}

QStringList PluginManagerV20::pluginPath()
{
    QStringList paths;
    QDir moduleDir(PluginDirectory);
    if (!moduleDir.exists()) {
        qWarning() << "plugin directory not exists";
        return paths;
    }
    auto moduleList = moduleDir.entryInfoList();
    for (auto i : moduleList) {
        QString path = i.absoluteFilePath();
        if (!QLibrary::isLibrary(path))
            continue;
        paths.append(path);
    }
    return paths;
}

void PluginManagerV20::loadPlugin(QString path, dccV20::FrameProxyInterface *frameProxy)
{
    qDebug() << "loading module: " << path;
    QElapsedTimer et;
    et.start();
    QPluginLoader loader(path);
    const QJsonObject &meta = loader.metaData().value("MetaData").toObject();

    QObject *instance = loader.instance();
    if (!instance) {
        qDebug() << loader.errorString();
        return;
    }

    auto *module = qobject_cast<ModuleInterface *>(instance);
    if (!module) {
        return;
    }
    qDebug() << "load plugin Name: " << module->name() << module->displayName();
    qDebug() << "load this plugin using time: " << et.elapsed() << "ms";
    m_modules.append(new AdapterV20toV23Module(module));
}
