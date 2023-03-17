#ifndef CONSOLEROLE_H
#define CONSOLEROLE_H

enum ColorRole
{
    NullRole,
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Purple,
    SkyBlue,
    White
};

struct ConsolRole
{
    ColorRole back = NullRole;
    ColorRole fore = NullRole;

    bool operator == (ConsolRole const& r) const
    {
        return this->back == r.back && this->fore == r.fore;
    }
    bool operator != (ConsolRole const& r) const
    {
        return !(*this == r);
    }
};
#endif // CONSOLEROLE_H
