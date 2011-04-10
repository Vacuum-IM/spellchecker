#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QObject>

#include "spellplugin.h"

SpellPlugin::SpellPlugin()
{
  FMessageWidgets = NULL;
  FMultiUserChatPlugin = NULL;
}

SpellPlugin::~SpellPlugin()
{
}

void SpellPlugin::pluginInfo(IPluginInfo *APluginInfo)
{
  APluginInfo->name = tr("Spell-checker Plugin");
  APluginInfo->description = tr("Highlights words that may not be spelled correctly");
  APluginInfo->version = "0.0.1";
  APluginInfo->author = "Minnahmetov V.K.";
  APluginInfo->homePage = "not yet needed";
  APluginInfo->dependences.append(MESSAGEWIDGETS_UUID);
  APluginInfo->dependences.append(MULTIUSERCHAT_UUID);
}

bool SpellPlugin::initConnections(IPluginManager *APluginManager, int &AInitOrder)
{
  Q_UNUSED(AInitOrder);

  IPlugin *plugin0 = APluginManager->pluginInterface("IMessageWidgets").value(0);
  if (plugin0) {
      FMessageWidgets = qobject_cast<IMessageWidgets *>(plugin0->instance());
    }
  IPlugin *plugin1 = APluginManager->pluginInterface("IMultiUserChatPlugin").value(0);
  if (plugin1) {
      FMultiUserChatPlugin = qobject_cast<IMultiUserChatPlugin *>(plugin1->instance());
    }
  return (FMessageWidgets != NULL) || (FMultiUserChatPlugin != NULL);
}

bool SpellPlugin::initObjects()
{
  if (FMessageWidgets) {
    connect(FMessageWidgets->instance(), 
	    SIGNAL(messageWindowCreated(IMessageWindow*)), 
	    this,
	    SLOT(addCheckSpelling(IMessageWindow*))
	    );
    connect(FMessageWidgets->instance(), 
	    SIGNAL(chatWindowCreated(IChatWindow*)), 
	    this,
	    SLOT(addCheckSpelling(IChatWindow*))
	    );
  }
  if (FMultiUserChatPlugin) {
    connect(FMultiUserChatPlugin->instance(), 
	    SIGNAL(multiChatWindowCreated(IMultiUserChatWindow*)), 
	    this,
	    SLOT(addCheckSpelling(IMultiUserChatWindow*))
	    );
  }
  return true;
}

void SpellPlugin::deleteCheckSpelling (QObject* document) {
  SpellHighlighter* spell = getSpellByDocument(document);
  if (spell) {
    delete spell;
  }
}

void SpellPlugin::appendHL (QObject* mDocument) {
  SHPair hili;
  QTextDocument* docobj = qobject_cast<QTextDocument* >(mDocument);
  SpellHighlighter* spell = new SpellHighlighter(docobj);
  hili.attachedTo = mDocument;
  hili.spellHighlighter = spell;
  highlighWidgets.resize(highlighWidgets.size() + 1);
  highlighWidgets[highlighWidgets.size() - 1] = hili;
  connect(mDocument,
	  SIGNAL(destroyed(QObject*)),
	  this,
	  SLOT(deleteCheckSpelling(QObject*)));	 
}

SpellHighlighter* SpellPlugin::getSpellByDocument(QObject* document) {
  SpellHighlighter* spell;
  for (int i = 0; i < highlighWidgets.size(); i++) {
    if (document == highlighWidgets[i].attachedTo) {
      spell = highlighWidgets[i].spellHighlighter;
      highlighWidgets.erase(highlighWidgets.begin() + i);
      return spell;
    }
  }
  return NULL;
}

void SpellPlugin::addCheckSpelling (IChatWindow* mWindow)
{
  appendHL(mWindow->editWidget()->document());
}

void SpellPlugin::addCheckSpelling (IMultiUserChatWindow* mWindow)
{
  appendHL(mWindow->editWidget()->document());
}

void SpellPlugin::addCheckSpelling (IMessageWindow* mWindow)
{
  appendHL(mWindow->editWidget()->document());
}


Q_EXPORT_PLUGIN2(plg_spellplugin, SpellPlugin)
