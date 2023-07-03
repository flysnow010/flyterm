#include "consolecolor.h"
#include <QObject>
ConsoleColorManager::ConsoleColorManager()
{
    colorNames_ << QObject::tr("Light yellow Black");//浅黄背景黑字
    colors_ << ConsoleColor{ QColor(255, 255, 221), QColor(0, 0, 0) };
    colorNames_ << QObject::tr("White Black");//白底黑字
    colors_ << ConsoleColor{ QColor(255, 255, 255), QColor(0, 0, 0) };
    colorNames_ << QObject::tr("Black Gray");//黑底灰字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(170, 170, 170) };
    colorNames_ << QObject::tr("Black Green");//黑底绿字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(0, 255, 0) };
    colorNames_ << QObject::tr("Black White");//黑底白字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(255, 255, 255) };
    colorNames_ << QObject::tr("Black Orange");//黑底橙字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(229, 60, 0) };
    colorNames_ << QObject::tr("Ambience");//Ambience
    colors_ << ConsoleColor{ QColor(48, 10, 36), QColor(255, 255, 255) };
    colorNames_ << QObject::tr("Solarized light");
    colors_ << ConsoleColor{ QColor(253, 246, 227), QColor(101, 123, 131) };
    colorNames_ << QObject::tr("Solarized dark");
    colors_ << ConsoleColor{ QColor(0, 43, 54), QColor(131, 148, 150) };
    colorNames_ << QObject::tr("Gruvbox light");
    colors_ << ConsoleColor{ QColor(251, 241, 199), QColor(60, 56, 54) };
    colorNames_ << QObject::tr("Gruvbox dark");
    colors_ << ConsoleColor{ QColor(40, 40, 40), QColor(235, 219, 178) };
}

bool ConsoleColorManager::hasColor(QString const& name) const
{
    return colorNames_.contains(name);
}

ConsoleColor ConsoleColorManager::color(QString const& name) const
{
    int index = colorNames_.indexOf(name);
    if(index != -1)
        return colors_[index];
    return ConsoleColor();
}

ConsoleColorManager * ConsoleColorManager::Instance()
{
    static ConsoleColorManager manager;
    return &manager;
}
