#ifndef CONSOLECOLOR_H
#define CONSOLECOLOR_H
#include <QColor>
#include <QString>

struct ConsoleColor
{
    QColor back;
    QColor fore;
};

class ConsoleColorManager
{
public:
    static ConsoleColorManager * Instance();

    QStringList colorNames() const { return colorNames_; }
    bool hasColor(QString const& name) const;
    ConsoleColor color(QString const& name) const;
private:
    ConsoleColorManager();
    QStringList colorNames_;
    QList<ConsoleColor> colors_;
};

#endif // CONSOLECOLOR_H
