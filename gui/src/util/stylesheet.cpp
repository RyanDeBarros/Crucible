#include <crucible/gui/util/stylesheet.hpp>

#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>

template<>
struct std::hash<std::pair<QString, QString>>
{
    size_t operator()(const std::pair<QString, QString>& pair) const
    {
        return std::hash<QString>{}(pair.first) ^ std::hash<QString>{}(pair.second);
    }
};

namespace crucible::gui
{

static std::unordered_map<QString, QString> cached_static_stylesheets;
static std::unordered_map<std::pair<QString, QString>, QString> cached_variable_stylesheets;

static QString loadStylesheet(const QString& stylesheet_path)
{
    auto it = cached_static_stylesheets.find(stylesheet_path);
    if (it != cached_static_stylesheets.end())
        return it->second;

    QFile stylesheet_file(stylesheet_path);

    if (!stylesheet_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    return cached_static_stylesheets.emplace(
        stylesheet_path,
        QString::fromUtf8(stylesheet_file.readAll())).first->second;
}

static QString loadStylesheet(const QString& stylesheet_path, const QString& variables_path)
{
    auto pair = std::make_pair(stylesheet_path, variables_path);
    auto it = cached_variable_stylesheets.find(pair);
    if (it != cached_variable_stylesheets.end())
        return it->second;

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

    return cached_variable_stylesheets.emplace(pair, stylesheet).first->second;
}

QString loadStylesheet(Stylesheet stylesheet)
{
    switch (stylesheet)
    {
    case Stylesheet::Dark:
        return loadStylesheet(
            QStringLiteral(":/style/dark.qss"),
            QStringLiteral(":/style/dark.vars"));

    case Stylesheet::ToggleIcon:
        return loadStylesheet(QStringLiteral(":/style/toggle_icon.qss"));

    default:
        return {};
    }
}

} // crucible::gui
