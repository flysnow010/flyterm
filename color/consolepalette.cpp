#include "consolepalette.h"

ConsolePalette::ConsolePalette()
{
    for(int red = 0; red < 6; red++)
        for(int green = 0; green < 6; green++)
            for(int blue = 0; blue < 6; blue++)
    {
        int code = 16 + red * 36 + green * 6 + blue;
        QColor color(red ? (red * 40 + 55) : 0,
                     green ? (green * 40 + 55) : 0,
                     blue ? (blue * 40 + 55) : 0);
        addColor(toColorRole(code), ConsoleColor{ color, color });
    }

    for(int gray = 0; gray < 24; gray++)
    {
        int level = gray * 10 + 8;
        int code = 232 + gray;
         QColor color(level, level, level);
        addColor(toColorRole(code), ConsoleColor{ color, color });
    }
}

ConsolePalette::~ConsolePalette()
{
}

ConsoleColor ConsolePalette::color(ColorRole colorRole) const
{
    return colorMap[colorRole];
}

void ConsolePalette::addColor(ColorRole role, ConsoleColor const& color)
{
    colorMap[role] = color;
}

void ConsolePalette::setName(QString const& name)
{
    name_ = name;
}

ConsolePaletteManager::ConsolePaletteManager()
{
    palettes_ << ConsolePalette::Ptr(new Ambience)
        << ConsolePalette::Ptr(new XTerm)
        << ConsolePalette::Ptr(new Tango)
        << ConsolePalette::Ptr(new Solarized)
        << ConsolePalette::Ptr(new Ravt)
        << ConsolePalette::Ptr(new Linux)
        << ConsolePalette::Ptr(new GruvboxLight)
        << ConsolePalette::Ptr(new GruvboxDark);
}

ConsolePaletteManager* ConsolePaletteManager::Instance()
{
    static ConsolePaletteManager manager;
    return &manager;
}

void ConsolePaletteManager::addPalette(ConsolePalette::Ptr const& palette)
{
    palettes_ << palette;
}

int ConsolePaletteManager::size()
{
    return palettes_.size();
}

ConsolePalette::Ptr ConsolePaletteManager::palette(int index)
{
    return palettes_.at(index);
}

ConsolePalette::Ptr ConsolePaletteManager::findPalette(QString const& name)
{
    foreach(ConsolePalette::Ptr palette, palettes_)
    {
        if(palette->name() == name)
            return palette;
    }
    return ConsolePalette::Ptr();
}

Ambience::Ambience()
{
     setName("Ambience");
     addColor(Black, ConsoleColor{ QColor(46, 52, 54), QColor(85, 87, 83) });
     addColor(Red, ConsoleColor{ QColor(204, 0, 0), QColor(239, 41, 41) });
     addColor(Green, ConsoleColor{ QColor(78, 154, 6), QColor(138, 226, 52) });
     addColor(Yellow, ConsoleColor{ QColor(196, 160, 0), QColor(252, 233, 79) });
     addColor(Blue, ConsoleColor{ QColor(52, 101, 164), QColor(114, 159, 207) });
     addColor(Purple, ConsoleColor{ QColor(117, 80, 123), QColor(173, 127, 168) });
     addColor(Cyan, ConsoleColor{ QColor(6, 152, 154), QColor(52, 226, 226) });
     addColor(White, ConsoleColor{ QColor(211, 215, 207), QColor(238, 238, 236) });
     addColor(BrightBlack, ConsoleColor{ QColor(46, 52, 54).light(), QColor(85, 87, 83).light() });
     addColor(BrightRed, ConsoleColor{ QColor(204, 0, 0).light(), QColor(239, 41, 41).light() });
     addColor(BrightGreen, ConsoleColor{ QColor(78, 154, 6).light(), QColor(138, 226, 52).light() });
     addColor(BrightYellow, ConsoleColor{ QColor(196, 160, 0).light(), QColor(252, 233, 79).light() });
     addColor(BrightBlue, ConsoleColor{ QColor(52, 101, 164).light(), QColor(114, 159, 207).light() });
     addColor(BrightPurple, ConsoleColor{ QColor(117, 80, 123).light(), QColor(173, 127, 168).light() });
     addColor(BrightCyan, ConsoleColor{ QColor(6, 152, 154).light(), QColor(52, 226, 226).light() });
     addColor(BrightWhite, ConsoleColor{ QColor(211, 215, 207).light(), QColor(238, 238, 236).light() });
}

XTerm::XTerm()
{
    setName("XTerm");
    addColor(Black, ConsoleColor{ QColor(0, 0, 0), QColor(127, 127, 127) });
    addColor(Red, ConsoleColor{ QColor(205, 0, 0), QColor(255, 0, 0) });
    addColor(Green, ConsoleColor{ QColor(0, 205, 0), QColor(0, 255, 0) });
    addColor(Yellow, ConsoleColor{ QColor(205, 205, 0), QColor(255, 255, 0) });
    addColor(Blue, ConsoleColor{ QColor(92, 92, 255), QColor(27, 186, 233) });
    addColor(Purple, ConsoleColor{ QColor(205, 0, 205), QColor(255, 0, 255) });
    addColor(Cyan, ConsoleColor{ QColor(0, 205, 205), QColor(142, 221, 244) });
    addColor(White, ConsoleColor{ QColor(229, 229, 229), QColor(255, 255, 255) });
    addColor(BrightBlack, ConsoleColor{ QColor(0, 0, 0).light(), QColor(127, 127, 127).light() });
    addColor(BrightRed, ConsoleColor{ QColor(205, 0, 0).light(), QColor(255, 0, 0).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(0, 205, 0).light(), QColor(0, 255, 0).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(205, 205, 0).light(), QColor(255, 255, 0).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(92, 92, 255).light(), QColor(27, 186, 233).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(205, 0, 205).light(), QColor(255, 0, 255).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(0, 205, 205).light(), QColor(0, 255, 255).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(229, 229, 229).light(), QColor(255, 255, 255).light() });
}

Tango::Tango()
{
    setName("Tango");
    addColor(Black, ConsoleColor{ QColor(0, 0, 0), QColor(85, 87, 83) });
    addColor(Red, ConsoleColor{ QColor(204, 0, 0), QColor(239, 41, 41) });
    addColor(Green, ConsoleColor{ QColor(78, 154, 6), QColor(138, 226, 52) });
    addColor(Yellow, ConsoleColor{ QColor(196, 160, 0), QColor(252, 233, 79) });
    addColor(Blue, ConsoleColor{ QColor(52, 101, 164), QColor(114, 159, 207) });
    addColor(Purple, ConsoleColor{ QColor(117, 80, 123), QColor(173, 127, 168) });
    addColor(Cyan, ConsoleColor{ QColor(6, 152, 154), QColor(52, 226, 226) });
    addColor(White, ConsoleColor{ QColor(221, 215, 207), QColor(238, 238, 236) });
    addColor(BrightBlack, ConsoleColor{ QColor(0, 0, 0).light(), QColor(85, 87, 83).light() });
    addColor(BrightRed, ConsoleColor{ QColor(204, 0, 0).light(), QColor(239, 41, 41).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(78, 154, 6).light(), QColor(138, 226, 52).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(196, 160, 0).light(), QColor(252, 233, 79).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(52, 101, 164).light(), QColor(114, 159, 207).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(117, 80, 123).light(), QColor(173, 127, 168).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(6, 152, 154).light(), QColor(52, 226, 226).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(221, 215, 207).light(), QColor(238, 238, 236).light() });

}

Solarized::Solarized()
{
    setName("Solarized");
    addColor(Black, ConsoleColor{ QColor(7, 54, 66), QColor(0, 43, 54) });
    addColor(Red, ConsoleColor{ QColor(220, 50, 47), QColor(203, 75, 22) });
    addColor(Green, ConsoleColor{ QColor(133, 153, 0), QColor(88, 110, 117) });
    addColor(Yellow, ConsoleColor{ QColor(181, 137, 0), QColor(101, 123, 131) });
    addColor(Blue, ConsoleColor{ QColor(38, 139, 210), QColor(131, 148, 150) });
    addColor(Purple, ConsoleColor{ QColor(211, 54, 130), QColor(108, 113, 196) });
    addColor(Cyan, ConsoleColor{ QColor(42, 161, 152), QColor(147, 161, 161) });
    addColor(White, ConsoleColor{ QColor(238, 232, 213), QColor(253, 246, 227) });
    addColor(BrightBlack, ConsoleColor{ QColor(7, 54, 66).light(), QColor(0, 43, 54).light() });
    addColor(BrightRed, ConsoleColor{ QColor(220, 50, 47).light(), QColor(203, 75, 22).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(133, 153, 0).light(), QColor(88, 110, 117).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(181, 137, 0).light(), QColor(101, 123, 131).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(38, 139, 210).light(), QColor(131, 148, 150).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(211, 54, 130).light(), QColor(108, 113, 196).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(42, 161, 152).light(), QColor(147, 161, 161).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(238, 232, 213).light(), QColor(253, 246, 227).light() });
}

Ravt::Ravt()
{
    setName("Ravt");
    addColor(Black, ConsoleColor{ QColor(0, 0, 0), QColor(64, 64, 64) });
    addColor(Red, ConsoleColor{ QColor(205, 0, 0), QColor(255, 0, 0) });
    addColor(Green, ConsoleColor{ QColor(0, 205, 0), QColor(0, 255, 0) });
    addColor(Yellow, ConsoleColor{ QColor(205, 205, 0), QColor(255, 255, 0) });
    addColor(Blue, ConsoleColor{ QColor(0, 0, 205), QColor(0, 0, 255) });
    addColor(Purple, ConsoleColor{ QColor(205, 0, 205), QColor(255, 0, 255) });
    addColor(Cyan, ConsoleColor{ QColor(0, 205, 205), QColor(0, 255, 255) });
    addColor(White, ConsoleColor{ QColor(250, 235, 215), QColor(255, 255, 255) });
    addColor(BrightBlack, ConsoleColor{ QColor(0, 0, 0).light(), QColor(64, 64, 64).light() });
    addColor(BrightRed, ConsoleColor{ QColor(205, 0, 0).light(), QColor(255, 0, 0).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(0, 205, 0).light(), QColor(0, 255, 0).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(205, 205, 0).light(), QColor(255, 255, 0).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(0, 0, 205).light(), QColor(0, 0, 255).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(205, 0, 205).light(), QColor(255, 0, 255).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(0, 205, 205).light(), QColor(0, 255, 255).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(250, 235, 215).light(), QColor(255, 255, 255).light() });
}

Linux::Linux()
{
    setName("Linux");
    addColor(Black, ConsoleColor{ QColor(0, 0, 0), QColor(85, 85, 85) });
    addColor(Red, ConsoleColor{ QColor(170, 0, 0), QColor(255, 85, 85) });
    addColor(Green, ConsoleColor{ QColor(0, 170, 0), QColor(85, 255, 85) });
    addColor(Yellow, ConsoleColor{ QColor(170, 85, 0), QColor(255, 255, 85) });
    addColor(Blue, ConsoleColor{ QColor(0, 0, 170), QColor(85, 85, 255) });
    addColor(Purple, ConsoleColor{ QColor(170, 0, 170), QColor(255, 85, 255) });
    addColor(Cyan, ConsoleColor{ QColor(0, 170, 170), QColor(85, 255, 255) });
    addColor(White, ConsoleColor{ QColor(170, 170, 170), QColor(255, 255, 255) });
    addColor(BrightBlack, ConsoleColor{ QColor(0, 0, 0).light(), QColor(85, 85, 85).light() });
    addColor(BrightRed, ConsoleColor{ QColor(170, 0, 0).light(), QColor(255, 85, 85).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(0, 170, 0).light(), QColor(85, 255, 85).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(170, 85, 0).light(), QColor(255, 255, 85).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(0, 0, 170).light(), QColor(85, 85, 255).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(170, 0, 170).light(), QColor(255, 85, 255).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(0, 170, 170).light(), QColor(85, 255, 255).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(170, 170, 170).light(), QColor(255, 255, 255).light() });
}

GruvboxLight::GruvboxLight()
{
    setName("Gruvbox light");
    addColor(Black, ConsoleColor{ QColor(251, 241, 199), QColor(146, 131, 116) });
    addColor(Red, ConsoleColor{ QColor(204, 36, 29), QColor(157, 0, 6) });
    addColor(Green, ConsoleColor{ QColor(152, 151, 26), QColor(121, 116, 14) });
    addColor(Yellow, ConsoleColor{ QColor(215, 153, 33), QColor(181, 118, 20) });
    addColor(Blue, ConsoleColor{ QColor(69, 133, 136), QColor(7, 102, 120) });
    addColor(Purple, ConsoleColor{ QColor(177, 98, 134), QColor(143, 63, 113) });
    addColor(Cyan, ConsoleColor{ QColor(104, 157, 106), QColor(66, 123, 88) });
    addColor(White, ConsoleColor{ QColor(124, 111, 100), QColor(60, 56, 54) });
    addColor(BrightBlack, ConsoleColor{ QColor(251, 241, 199).light(), QColor(146, 131, 116).light() });
    addColor(BrightRed, ConsoleColor{ QColor(204, 36, 29).light(), QColor(157, 0, 6).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(152, 151, 26).light(), QColor(121, 116, 14).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(215, 153, 33).light(), QColor(181, 118, 20).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(69, 133, 136).light(), QColor(7, 102, 120).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(177, 98, 134).light(), QColor(143, 63, 113).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(104, 157, 106).light(), QColor(66, 123, 88).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(124, 111, 100).light(), QColor(60, 56, 54).light() });
}

GruvboxDark::GruvboxDark()
{
    setName("Gruvbox dark");
    addColor(Black, ConsoleColor{ QColor(40, 40, 40), QColor(146, 131, 116) });
    addColor(Red, ConsoleColor{ QColor(204, 36, 29), QColor(251, 73, 52) });
    addColor(Green, ConsoleColor{ QColor(152, 151, 26), QColor(184, 187, 38) });
    addColor(Yellow, ConsoleColor{ QColor(215, 153, 33), QColor(250, 189, 47) });
    addColor(Blue, ConsoleColor{ QColor(69, 133, 136), QColor(131, 165, 152) });
    addColor(Purple, ConsoleColor{ QColor(177, 98, 134), QColor(211, 134, 155) });
    addColor(Cyan, ConsoleColor{ QColor(104, 157, 106), QColor(142, 192, 124) });
    addColor(White, ConsoleColor{ QColor(168, 153, 132), QColor(235, 219, 178) });
    addColor(BrightBlack, ConsoleColor{ QColor(40, 40, 40).light(), QColor(146, 131, 116).light() });
    addColor(BrightRed, ConsoleColor{ QColor(204, 36, 29).light(), QColor(251, 73, 52).light() });
    addColor(BrightGreen, ConsoleColor{ QColor(152, 151, 26).light(), QColor(184, 187, 38).light() });
    addColor(BrightYellow, ConsoleColor{ QColor(215, 153, 33).light(), QColor(250, 189, 47).light() });
    addColor(BrightBlue, ConsoleColor{ QColor(69, 133, 136).light(), QColor(131, 165, 152).light() });
    addColor(BrightPurple, ConsoleColor{ QColor(177, 98, 134).light(), QColor(211, 134, 155).light() });
    addColor(BrightCyan, ConsoleColor{ QColor(104, 157, 106).light(), QColor(142, 192, 124).light() });
    addColor(BrightWhite, ConsoleColor{ QColor(168, 153, 132).light(), QColor(235, 219, 178).light() });
}

