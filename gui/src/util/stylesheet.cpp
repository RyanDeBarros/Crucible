#include <crucible/gui/util/stylesheet.hpp>

#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>

namespace crucible::gui
{

static std::unordered_map<StyleSheet, std::unique_ptr<QString>> cached_stylesheets;

static QString loadStyleSheet(const QString& stylesheet_path)
{
    QFile stylesheet_file(stylesheet_path);

    if (!stylesheet_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    return QString::fromUtf8(stylesheet_file.readAll());
}

static QString loadStyleSheet(const QString& stylesheet_path, const QString& variables_path)
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

static QString loadStyleSheetImpl(StyleSheet stylesheet)
{
    switch (stylesheet)
    {
    case StyleSheet::MainWindow:
        return loadStyleSheet(
            QStringLiteral(":/style/main_window.qss"),
            QStringLiteral(":/style/dark.vars"));

    case StyleSheet::Separator:
        return loadStyleSheet(
            QStringLiteral(":/style/separator.qss"),
            QStringLiteral(":/style/dark.vars"));

    case StyleSheet::ToggleIcon:
        return loadStyleSheet(
            QStringLiteral(":/style/toggle_icon.qss"),
            QStringLiteral(":/style/dark.vars"));

    default:
        return {};
    }
}

const QString& loadStyleSheet(StyleSheet stylesheet)
{
    auto it = cached_stylesheets.find(stylesheet);
    if (it != cached_stylesheets.end())
        return *it->second;
    else
    {
        return *cached_stylesheets.emplace(
            stylesheet,
            std::make_unique<QString>(loadStyleSheetImpl(stylesheet))).first->second;
    }
}

} // crucible::gui
