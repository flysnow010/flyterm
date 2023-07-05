#include "hightlightermanager.h"
#include "defaulthightlighter.h"
#include "errorhighlighter.h"

HighLighterManager* HighLighterManager::Instance()
{
    static HighLighterManager theManger;
    return &theManger;
}

void HighLighterManager::init()
{
    add(HighLighter{ "default", QObject::tr("Default") });
    add(HighLighter{ "errorHighLighter", QObject::tr("Error/Warning/Sucess") });

}

QSyntaxHighlighter* HighLighterManager::create(QString const& name)
{
    if(name == "default")
        return new DefaultHightLighter();
    else if(name == "errorHighLighter")
        return new ErrorHighlighter();
    return nullptr;
}

void HighLighterManager::add(HighLighter const& highLighter)
{
    highLighters << highLighter;
}

int HighLighterManager::size()
{
    return highLighters.size();
}

HighLighterManager::HighLighter const& HighLighterManager::highLighter(int index)
{
    return highLighters.at(index);
}


