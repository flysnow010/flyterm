#include "consolecodec.h"

/*
 *
    Big5*
    Big5-HKSCS*
    CP949
    EUC-JP*
    EUC-KR*
    GB18030*
    HP-ROMAN8
    IBM 850
    IBM 866
    IBM 874
    ISO 2022-JP
    ISO 8859-1 to 10*
    ISO 8859-13 to 16*
    Iscii-Bng, Dev, Gjr, Knd, Mlm, Ori, Pnj, Tlg, and Tml
    KOI8-R *
    KOI8-U *
    Macintosh
    Shift-JIS *
    TIS-620
    TSCII
    UTF-8 *
    UTF-16 *
    UTF-16BE
    UTF-16LE *
    UTF-32 *
    UTF-32BE *
    UTF-32LE *
    Windows-1250 to 1258 *
*/

ConsoleCodecManager::ConsoleCodecManager()
{
    consoleCodecs_
        << ConsoleCodec{"UTF-8", "UTF-8"}
        << ConsoleCodec{"UTF-16LE", "UTF-16LE"}
        << ConsoleCodec{"UTF-16BE", "UTF-16BE"}
        << ConsoleCodec{"UTF-32LE", "UTF-32LE"}
        << ConsoleCodec{"UTF-32BE", "UTF-32BE"}
        << ConsoleCodec{"Wester(Windows-1252)", "Windows-1252"}
        << ConsoleCodec{"Wester(ISO 8859-1)", "ISO 8859-1"}
        << ConsoleCodec{"Wester(ISO 8859-3)", "ISO 8859-3"}
        << ConsoleCodec{"Wester(ISO 8859-15)", "ISO 8859-15"}
        << ConsoleCodec{"Arabic(Windows-1256)", "Windows-1256"}
        << ConsoleCodec{"Arabic(ISO 8859-6)", "ISO 8859-6"}
        << ConsoleCodec{"Baltic(Windows-1257)", "Windows-1257"}
        << ConsoleCodec{"Baltic(ISO 8859-4)", "ISO 8859-4"}
        << ConsoleCodec{"Celtic(ISO 8859-14)", "ISO 8859-14"}
        << ConsoleCodec{"Central European(Windows-1250)", "Windows-1250"}
        << ConsoleCodec{"Central European(ISO 8859-2)", "ISO 8859-2"}
        << ConsoleCodec{"Cyrilic(Windows-1251)", "Windows-1251"}
        << ConsoleCodec{"Cyrilic(ISO 8859-5)", "ISO 8859-5"}
        << ConsoleCodec{"Cyrilic(KOI8-R)", "KOI8-R"}
        << ConsoleCodec{"Cyrilic(KOI8-U)", "KOI8-U"}
        << ConsoleCodec{"Estonian(ISO 8859-13)", "ISO 8859-13"}
        << ConsoleCodec{"Greek(Windows-1253)", "Windows-1253"}
        << ConsoleCodec{"Greek(ISO 8859-7)", "ISO 8859-7"}
        << ConsoleCodec{"Hebrew(Windows-1255)", "Windows-1255"}
        << ConsoleCodec{"Hebrew(ISO 8859-8)", "ISO 8859-8"}
        << ConsoleCodec{"Hebrew(ISO 8859-10)", "ISO 8859-10"}
        << ConsoleCodec{"Romania(ISO 8859-16)", "ISO 8859-16"}
        << ConsoleCodec{"Turkish(Windows-1254)", "Windows-1254"}
        << ConsoleCodec{"Turkish(ISO 8859-9)", "ISO 8859-9"}
        << ConsoleCodec{"Vietnameese(Windows-1258)", "Windows-1258"}
        << ConsoleCodec{"Simplified Chinese(GB18030)", "GB18030"}
        << ConsoleCodec{"Fraditional Chinese(Big5)", "Big5"}
        << ConsoleCodec{"Fraditional Chinese(Big5-HKSCS)", "Big5-HKSCS"}
        << ConsoleCodec{"Japanese(Shift-JIS)", "Shift-JIS"}
        << ConsoleCodec{"Japanese(EUC-JP)", "EUC-JP"}
        << ConsoleCodec{"Korean(EUC-KR)", "EUC-KR"}
    ;
}

QString ConsoleCodecManager::codecToName(QString const& codec) const
{
    foreach(auto consoleCodec, consoleCodecs_)
    {
        if(consoleCodec.codec == codec)
            return consoleCodec.name;
    }
    return QString();
}

ConsoleCodecManager* ConsoleCodecManager::Instance()
{
    static ConsoleCodecManager manager;
    return &manager;
}
