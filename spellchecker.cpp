#include <QDir>
#include <QObject>
#include <QMessageBox>
#include <QApplication>

#include "spellchecker.h"

SpellChecker::SpellChecker()
{
	FMessageWidgets = NULL;
}

SpellChecker::~SpellChecker()
{

}

void SpellChecker::pluginInfo(IPluginInfo *APluginInfo)
{
	APluginInfo->name = tr("Spell Checker");
	APluginInfo->description = tr("Highlights words that may not be spelled correctly");
	APluginInfo->version = "0.0.3";
	APluginInfo->author = "Minnahmetov V.K.";
	APluginInfo->homePage = "http://code.google.com/p/vacuum-plugins/";
	APluginInfo->dependences.append(MESSAGEWIDGETS_UUID);
}

bool SpellChecker::initConnections(IPluginManager *APluginManager, int &AInitOrder)
{
	Q_UNUSED(AInitOrder);

	IPlugin *plugin = APluginManager->pluginInterface("IMessageWidgets").value(0);
	if (plugin) 
	{
		FMessageWidgets = qobject_cast<IMessageWidgets *>(plugin->instance());
		if (FMessageWidgets) 
		{
			connect(FMessageWidgets->instance(),SIGNAL(editWidgetCreated(IEditWidget *)),SLOT(onEditWidgetCreated(IEditWidget *)));
		}
	}
	return FMessageWidgets!=NULL;
}

void SpellChecker::appendHL(QTextDocument *ADocument) 
{
	SHPair hili;
	SpellHighlighter *spell = new SpellHighlighter(ADocument);
	hili.attachedTo = ADocument;
	hili.spellHighlighter = spell;
	FHighlighWidgets.append(hili);
	connect(ADocument,SIGNAL(destroyed(QObject *)),SLOT(onSpellDocumentDestroyed(QObject *)));	 
}

SpellHighlighter *SpellChecker::getSpellByDocument(QObject *ADocument) 
{
	SpellHighlighter *spell = NULL;
	for (int i=0; spell==NULL && i<FHighlighWidgets.count(); i++) 
	{
		if (ADocument == FHighlighWidgets.at(i).attachedTo) 
		{
			spell = FHighlighWidgets.at(i).spellHighlighter;
			FHighlighWidgets.removeAt(i);
		}
	}
	return spell;
}

void SpellChecker::onEditWidgetCreated(IEditWidget *AWidget)
{
	appendHL(AWidget->document());
}

void SpellChecker::onSpellDocumentDestroyed(QObject *ADocument) 
{
	delete getSpellByDocument(ADocument);
}

Q_EXPORT_PLUGIN2(plg_spellchecker, SpellChecker)
