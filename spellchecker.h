#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <interfaces/ipluginmanager.h>
#include <interfaces/imessagewidgets.h>
#include <interfaces/imultiuserchat.h>

#include "spellhighlighter.h"

#define SPELLCHECKER_UUID "{0dc5fbd9-2dd4-4720-9c95-8c3393a577a5}"

struct SHPair
{
    QObject *attachedTo;
    SpellHighlighter *spellHighlighter;
};

class SpellChecker : 
        public QObject,
        public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    SpellChecker();
    ~SpellChecker();
    virtual QObject *instance() { return this; }
    virtual QUuid pluginUuid() const { return SPELLCHECKER_UUID; }
    virtual void pluginInfo(IPluginInfo *APluginInfo);
    virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
    virtual bool initObjects();
    virtual bool initSettings() { return true; }
    virtual bool startPlugin() { return true; }
protected:
    void appendHL(QTextDocument *ADocument, IMultiUserChat *AMultiUserChat);
    SpellHighlighter* getSpellByDocument(QObject *AEditor);
protected slots:
    void onEditWidgetCreated(IEditWidget* AWidget);
    void onSpellDocumentDestroyed(QObject* ADocument);
    void showContextMenu(const QPoint& pt);
    void repairWord();
    void setDict();
    void addWordToDict();
private:
    IMessageWidgets *FMessageWidgets;
    QTextEdit* FTextEdit;
    int FrepStart;
    int FrepLenght;
private:
    QList<SHPair> FHighlighWidgets;
    QMenu *FDictMenu;
    QMenu* suggestMenu(const QString &word);
    QMenu* dictMenu();
};

#endif // SPELLCHECKER_H
