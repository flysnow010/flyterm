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
    White,
    Color016 = 16,
    Color017,
    Color018,
    Color019,
    Color020,
    Color021,
    Color022,
    Color023,
    Color024,
    Color025,
    Color026,
    Color027,
    Color028,
    Color029,
    Color030,
    Color031,
    Color032,
    Color033,
    Color034,
    Color035,
    Color036,
    Color037,
    Color038,
    Color039,
    Color040,
    Color041,
    Color042,
    Color043,
    Color044,
    Color045,
    Color046,
    Color047,
    Color048,
    Color049,
    Color050,
    Color051,
    Color052,
    Color053,
    Color054,
    Color055,
    Color056,
    Color057,
    Color058,
    Color059,
    Color060,
    Color061,
    Color062,
    Color063,
    Color064,
    Color065,
    Color066,
    Color067,
    Color068,
    Color069,
    Color070,
    Color071,
    Color072,
    Color073,
    Color074,
    Color075,
    Color076,
    Color077,
    Color078,
    Color079,
    Color080,
    Color081,
    Color082,
    Color083,
    Color084,
    Color085,
    Color086,
    Color087,
    Color088,
    Color089,
    Color090,
    Color091,
    Color092,
    Color093,
    Color094,
    Color095,
    Color096,
    Color097,
    Color098,
    Color099,
    Color100,
    Color101,
    Color102,
    Color103,
    Color104,
    Color105,
    Color106,
    Color107,
    Color108,
    Color109,
    Color110,
    Color111,
    Color112,
    Color113,
    Color114,
    Color115,
    Color116,
    Color117,
    Color118,
    Color119,
    Color120,
    Color121,
    Color122,
    Color123,
    Color124,
    Color125,
    Color126,
    Color127,
    Color128,
    Color129,
    Color130,
    Color131,
    Color132,
    Color133,
    Color134,
    Color135,
    Color136,
    Color137,
    Color138,
    Color139,
    Color140,
    Color141,
    Color142,
    Color143,
    Color144,
    Color145,
    Color146,
    Color147,
    Color148,
    Color149,
    Color150,
    Color151,
    Color152,
    Color153,
    Color154,
    Color155,
    Color156,
    Color157,
    Color158,
    Color159,
    Color160,
    Color161,
    Color162,
    Color163,
    Color164,
    Color165,
    Color166,
    Color167,
    Color168,
    Color169,
    Color170,
    Color171,
    Color172,
    Color173,
    Color174,
    Color175,
    Color176,
    Color177,
    Color178,
    Color179,
    Color180,
    Color181,
    Color182,
    Color183,
    Color184,
    Color185,
    Color186,
    Color187,
    Color188,
    Color189,
    Color190,
    Color191,
    Color192,
    Color193,
    Color194,
    Color195,
    Color196,
    Color197,
    Color198,
    Color199,
    Color200,
    Color201,
    Color202,
    Color203,
    Color204,
    Color205,
    Color206,
    Color207,
    Color208,
    Color209,
    Color210,
    Color211,
    Color212,
    Color213,
    Color214,
    Color215,
    Color216,
    Color217,
    Color218,
    Color219,
    Color220,
    Color221,
    Color222,
    Color223,
    Color224,
    Color225,
    Color226,
    Color227,
    Color228,
    Color229,
    Color230,
    Color231,
    Color232,
    Color233,
    Color234,
    Color235,
    Color236,
    Color237,
    Color238,
    Color239,
    Color240,
    Color241,
    Color242,
    Color243,
    Color244,
    Color245,
    Color246,
    Color247,
    Color248,
    Color249,
    Color250,
    Color251,
    Color252,
    Color253,
    Color254,
    Color255
};

inline ColorRole toColorRole(int role)
{
    return static_cast<ColorRole>(role);
}
struct ConsolRole
{
    ColorRole back = NullRole;
    ColorRole fore = NullRole;
    bool isReverse = false;

    bool operator == (ConsolRole const& r) const
    {
        return this->back == r.back
                && this->fore == r.fore
                && this->isReverse == r.isReverse;
    }
    bool operator != (ConsolRole const& r) const
    {
        return !(*this == r);
    }
};
#endif // CONSOLEROLE_H
