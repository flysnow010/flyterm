#ifndef CONSOLEPALETTE_H
#define CONSOLEPALETTE_H
#include "consolecolor.h"
#include "consolerole.h"

#include <memory>
#include <QMap>

class ConsolePalette
{
public:
    ConsolePalette();
    virtual ~ConsolePalette();
    using Ptr = std::shared_ptr<ConsolePalette>;

    QString name() const { return name_; }

    ConsoleColor color(ColorRole colorRole) const;
protected:
    void addColor(ColorRole role, ConsoleColor const& color);
    void setName(QString const& name);
private:
    QMap<ColorRole, ConsoleColor> colorMap;
    QString name_;
};

class ConsolePaletteManager
{
public:
    ConsolePaletteManager(ConsolePaletteManager const&) = delete;
    void operator == (ConsolePaletteManager const&) = delete;

    static ConsolePaletteManager* Instance();

    void addPalette(ConsolePalette::Ptr const& palette);

    int size();
    ConsolePalette::Ptr palette(int index);
    ConsolePalette::Ptr findPalette(QString const& name);
private:
    ConsolePaletteManager();
    QList<ConsolePalette::Ptr> palettes_;
};

class Ambience : public ConsolePalette
{
public:
    Ambience();
};

class XTerm : public ConsolePalette
{
public:
    XTerm();
};

class Tango : public ConsolePalette
{
public:
    Tango();
};

class Solarized : public ConsolePalette
{
public:
    Solarized();
};

class Ravt : public ConsolePalette
{
public:
    Ravt();
};

class Linux : public ConsolePalette
{
public:
    Linux();
};

class GruvboxLight : public ConsolePalette
{
public:
    GruvboxLight();
};

class GruvboxDark : public ConsolePalette
{
public:
    GruvboxDark();
};

#endif // CONSOLEPALETTE_H
