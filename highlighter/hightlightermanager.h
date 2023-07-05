#ifndef HIGHTLIGHTERMANAGER_H
#define HIGHTLIGHTERMANAGER_H
#include <QSyntaxHighlighter>
#include <QList>

struct HighLighterManager
{
    static HighLighterManager* Instance();

    struct HighLighter
    {
        QString name;
        QString text;
        QString showText() const
        {
            return QObject::tr(text.toStdString().c_str());
        }
    };

    void init();
    void add(HighLighter const& highLighter);
    int size();
    HighLighter const& highLighter(int index);
    QSyntaxHighlighter* create(QString const& name);

private:
    HighLighterManager(){}
    HighLighterManager(HighLighterManager const&);
    HighLighterManager & operator=(HighLighterManager const&);

private:
    QList<HighLighterManager::HighLighter> highLighters;
};

#define highLighterManager HighLighterManager::Instance()


#endif // HIGHTLIGHTERMANAGER_H
