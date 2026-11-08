#pragma once

class QString;

namespace crucible::gui
{

extern QString loadStylesheet(const QString& stylesheet_path);
extern QString loadStylesheet(const QString& stylesheet_path, const QString& variables_path);

} // crucible::gui
