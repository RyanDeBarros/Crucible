#include <crucible/gui/stylesheet.hpp>

#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>

namespace crucible::gui
{

// TODO move to util/ and rename to loadStylesheet(). Cache stylesheets since they might be used multiple times. In fact, pass enums instead of paths.

QString load_stylesheet(const QString& stylesheet_path)
{
    QFile stylesheet_file(stylesheet_path);

    if (!stylesheet_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    return QString::fromUtf8(stylesheet_file.readAll());
}

QString load_stylesheet(const QString& stylesheet_path, const QString& variables_path)
{
    QFile stylesheet_file(stylesheet_path);
    QFile variables_file(variables_path);

    if (!variables_file.open(QIODevice::ReadOnly | QIODevice::Text) ||
        !stylesheet_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return {};
    }

    QMap<QString, QString> variables;
    QTextStream variables_stream(&variables_file);

    while (!variables_stream.atEnd())
    {
        auto line = variables_stream.readLine().trimmed();

        if (line.isEmpty() || line.startsWith('#'))
            continue;

        auto const separator = line.indexOf('=');

        if (separator < 0)
            continue;

        auto const key = line.left(separator).trimmed();
        auto const value = line.mid(separator + 1).trimmed();

        variables.insert(key, value);
    }

    auto stylesheet = QString::fromUtf8(stylesheet_file.readAll());

    for (auto it = variables.cbegin(); it != variables.cend(); ++it)
        stylesheet.replace(QString("{{%1}}").arg(it.key()), it.value());

    return stylesheet;
}

} // crucible::gui
