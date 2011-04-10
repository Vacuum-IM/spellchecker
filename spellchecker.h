#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <vector>

#include <definitions.h>

#include <interfaces/ipluginmanager.h>
#include <interfaces/imessagewidgets.h>
#include <interfaces/imultiuserchat.h>

#include "spellhighlighter.h"
#include "spellbackend.h"

#define SPELLPLUGIN_UUID "{0dc5fbd9-2dd4-4720-9c95-8c3393a577a5}"

struct SHPair {
  QObject* attachedTo;
  SpellHighlighter* spellHighlighter;
};

class SpellPlugin : 
public QObject,
  public IPlugin
{
  Q_OBJECT;
  Q_INTERFACES(IPlugin);
public:
  SpellPlugin();
  ~SpellPlugin();
  virtual QObject *instance() { return this; }
  virtual QUuid pluginUuid() const { return SPELLPLUGIN_UUID; }
  virtual void pluginInfo(IPluginInfo *APluginInfo);
  virtual bool initConnections(IPluginManager *APluginManager, int &AInitOrder);
  virtual bool initObjects();
  virtual bool initSettings() { return true; }
  virtual bool startPlugin() { return true; }
public slots:
  void addCheckSpelling (IMessageWindow* mWindow);
  void addCheckSpelling (IChatWindow* mWindow);
  void addCheckSpelling (IMultiUserChatWindow* mWindow);
  void deleteCheckSpelling (QObject* document);
private:
  std::vector<SHPair> highlighWidgets;
  IMessageWidgets* FMessageWidgets;
  IMultiUserChatPlugin* FMultiUserChatPlugin;
  SpellHighlighter* getSpellByDocument(QObject* editor);
  void appendHL(QObject* mDocument);
};

#endif // SPELLCHECKER_H
