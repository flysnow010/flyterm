#include "consolecodec.h"

/*
 *
    Big5
    Big5-HKSCS
    CP949
    EUC-JP
    EUC-KR
    GB18030
    HP-ROMAN8
    IBM 850
    IBM 866
    IBM 874
    ISO 2022-JP
    ISO 8859-1 to 10
    ISO 8859-13 to 16
    Iscii-Bng, Dev, Gjr, Knd, Mlm, Ori, Pnj, Tlg, and Tml
    KOI8-R
    KOI8-U
    Macintosh
    Shift-JIS
    TIS-620
    TSCII
    UTF-8
    UTF-16
    UTF-16BE
    UTF-16LE
    UTF-32
    UTF-32BE
    UTF-32LE
    Windows-1250 to 1258
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
        << ConsoleCodec{"Simplified Chinese(GB18030)", "GB18030"}
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
