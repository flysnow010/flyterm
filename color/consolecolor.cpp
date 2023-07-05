#include "consolecolor.h"
#include <QObject>
ConsoleColorManager::ConsoleColorManager()
{
    colorNames_ << "Light yellow Black";//浅黄背景黑字
    colors_ << ConsoleColor{ QColor(255, 255, 221), QColor(0, 0, 0) };
    colorNames_ << "White Black";//白底黑字
    colors_ << ConsoleColor{ QColor(255, 255, 255), QColor(0, 0, 0) };
    colorNames_ << "Black Gray";//黑底灰字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(170, 170, 170) };
    colorNames_ << "Black Green";//黑底绿字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(0, 255, 0) };
    colorNames_ << "Black White";//黑底白字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(255, 255, 255) };
    colorNames_ << "Black Orange";//黑底橙字
    colors_ << ConsoleColor{ QColor(0, 0, 0), QColor(229, 60, 0) };
    colorNames_ << "Ambience";//Ambience
    colors_ << ConsoleColor{ QColor(48, 10, 36), QColor(255, 255, 255) };
    colorNames_ << "Solarized light";
    colors_ << ConsoleColor{ QColor(253, 246, 227), QColor(101, 123, 131) };
    colorNames_ << "Solarized dark";
    colors_ << ConsoleColor{ QColor(0, 43, 54), QColor(131, 148, 150) };
    colorNames_ << "Gruvbox light";
    colors_ << ConsoleColor{ QColor(251, 241, 199), QColor(60, 56, 54) };
    colorNames_ << "Gruvbox dark";
    colors_ << ConsoleColor{ QColor(40, 40, 40), QColor(235, 219, 178) };
}

QStringList ConsoleColorManager::colorNames() const
{
    QStringList names;
    foreach(auto name, colorNames_)
        names << QObject::tr(name.toStdString().c_str());
    return names;
}

ConsoleColor ConsoleColorManager::color(int index) const
{
    if(index >= 0 & index < colors_.size())
        return colors_[index];
    return ConsoleColor();
}

ConsoleColorManager * ConsoleColorManager::Instance()
{
    static ConsoleColorManager manager;
    return &manager;
}
