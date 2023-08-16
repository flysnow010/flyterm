#ifndef CONSOLECODEC_H
#define CONSOLECODEC_H
#include <QString>
#include <QList>

struct ConsoleCodec
{
    QString name;
    QString codec;
};

class ConsoleCodecManager
{
public:
    ConsoleCodecManager(ConsoleCodecManager const&) = delete;
    void operator == (ConsoleCodecManager const&) = delete;

    static ConsoleCodecManager* Instance();

    QString codecToName(QString const& codec) const;
    QList<ConsoleCodec> const& codecs() const { return consoleCodecs_; }
private:
    ConsoleCodecManager();
    QList<ConsoleCodec> consoleCodecs_;
};

#endif // CONSOLECODEC_H
