#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <definitions.h>

#include <interfaces/ipluginmanager.h>
#include <interfaces/imessagewidgets.h>

#include "spellhighlighter.h"
#include "spellbackend.h"

#define SPELLCHECKER_UUID "{0dc5fbd9-2dd4-4720-9c95-8c3393a577a5}"

struct SHPair 
{
	QObject *attachedTo;
	SpellHighlighter* spellHighlighter;
};

class SpellChecker : 
	public QObject,
	public IPlugin
{
	Q_OBJECT;
	Q_INTERFACES(IPlugin);
public:
	SpellChecker();
	~SpellChecker();
	virtual QObject *instance() { return this; }
	virtual QUuid pluginUuid() const { return SPELLCHECKER_UUID; }
	virtual void pluginInfo(IPluginInfo *APluginInfo);
	virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
	virtual bool initObjects() { return true; }
	virtual bool initSettings() { return true; }
	virtual bool startPlugin() { return true; }
protected:
	void appendHL(QTextDocument *ADocument);
	SpellHighlighter* getSpellByDocument(QObject *AEditor);
protected slots:
	void onEditWidgetCreated(IEditWidget* AWidget);
	void onSpellDocumentDestroyed(QObject* ADocument);
private:
	IMessageWidgets *FMessageWidgets;
private:
	QList<SHPair> FHighlighWidgets;
};

#endif // SPELLCHECKER_H
