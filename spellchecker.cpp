#include <QDir>
#include <QObject>
#include <QMessageBox>
#include <QApplication>

#include <QDebug>

#include "spellbackend.h"
#include "spellchecker.h"
#include "definitions.h"

SpellChecker::SpellChecker() : FMessageWidgets(NULL), FCurrentTextEdit(NULL), FDictMenu(NULL)
{

}

SpellChecker::~SpellChecker()
{
    delete FDictMenu;
}

void SpellChecker::pluginInfo(IPluginInfo *APluginInfo)
{
    APluginInfo->name = tr("Spell Checker");
    APluginInfo->description = tr("Highlights words that may not be spelled correctly");
    APluginInfo->version = "0.0.7";
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
        Q_ASSERT(FMessageWidgets);
        if (FMessageWidgets)
        {
            connect(FMessageWidgets->instance(),SIGNAL(editWidgetCreated(IEditWidget *)),SLOT(onEditWidgetCreated(IEditWidget *)));
        }
    }
    return FMessageWidgets != NULL;
}

bool SpellChecker::initObjects()
{
    FDictMenu = dictMenu();

    return true;
}

void SpellChecker::appendHL(QTextDocument *ADocument, IMultiUserChat *AMultiUserChat)
{
    SHPair hili;
    hili.attachedTo = ADocument;
    hili.spellHighlighter = new SpellHighlighter(ADocument, AMultiUserChat);

    FHighlighWidgets.append(hili);

    connect(ADocument, SIGNAL(destroyed(QObject *)), this, SLOT(onSpellDocumentDestroyed(QObject *)));
}

SpellHighlighter *SpellChecker::getSpellByDocument(QObject *ADocument, int *index = NULL)
{
    SpellHighlighter *spell = NULL;
    int widgetCount = FHighlighWidgets.count();
    for (int i = 0; spell == NULL && i < widgetCount; i++)
    {
        if (ADocument == FHighlighWidgets.at(i).attachedTo)
        {
            spell = FHighlighWidgets.at(i).spellHighlighter;
            if (index)
            {
                *index = i;
            }
        }
    }
    return spell;
}

SpellHighlighter *SpellChecker::getSpellByDocumentAndRemove(QObject *ADocument)
{
    int i;
    SpellHighlighter *spell = getSpellByDocument(ADocument, &i);
    FHighlighWidgets.removeAt(i);

    return spell;
}

void SpellChecker::onEditWidgetCreated(IEditWidget *AWidget)
{
    IMultiUserChatWindow *window = NULL;
    QWidget *parent = AWidget->instance()->parentWidget();
    while (window == NULL && parent != NULL)
    {
        window = qobject_cast<IMultiUserChatWindow *>(parent);
        parent = parent->parentWidget();
    }

    appendHL(AWidget->document(), window ? window->multiUserChat() : NULL);

    QTextEdit *textEdit = AWidget->textEdit();
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
}

void SpellChecker::onSpellDocumentDestroyed(QObject *ADocument) 
{
    delete getSpellByDocumentAndRemove(ADocument);
}

QMenu* SpellChecker::suggestMenu(const QString &word)
{
    QList<QString> sgstions = SpellBackend::instance()->suggestions(word);
    QMenu *menu = new QMenu(tr("Suggestions"));

    for (QList<QString>::const_iterator sgstion = sgstions.begin(); sgstion != sgstions.end(); ++sgstion)
    {
        QAction *action = menu->addAction(*sgstion, this, SLOT(repairWord()));
        action->setProperty("word", *sgstion);
        action->setParent(menu);
    }

    return menu;
}

QMenu* SpellChecker::dictMenu()
{
    QMenu *menu = new QMenu(tr("Dictionary"));

    const QString actualLang = SpellBackend::instance()->actuallLang();
    const QList<QString> dicts = SpellBackend::instance()->dictionaries();
    QActionGroup *dictGroup = new QActionGroup(this);

    for (QList<QString>::const_iterator dict = dicts.begin(); dict != dicts.end(); dict++)
    {
        QAction *action = menu->addAction(*dict, this, SLOT(setDict()));
        action->setProperty("dictionary", *dict);
        action->setParent(menu);
        action->setCheckable(true);
        dictGroup->addAction(action);

        if (*dict == actualLang || actualLang.contains(*dict)) {
            action->setChecked(true);
        }
    }

    return menu;
}

void SpellChecker::showContextMenu(const QPoint &pt)
{
    FCurrentTextEdit = qobject_cast<QTextEdit *>(sender());
    Q_ASSERT(FCurrentTextEdit);

    QMenu *menu = FCurrentTextEdit->createStandardContextMenu();

    menu->addSeparator();
    menu->addMenu(FDictMenu);

    QTextCursor cursor = FCurrentTextEdit->cursorForPosition(pt);
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();
    QMenu *sugMenu = NULL;
    Q_ASSERT(!sugMenu);

    if (!word.isEmpty() && !SpellBackend::instance()->isCorrect(word)) {
        sugMenu = suggestMenu(word);

        if (!sugMenu->isEmpty()) {
          menu->addMenu(sugMenu);
        }

        QAction *action = menu->addAction(tr("Add to dictionary"), this, SLOT(addWordToDict()));
        action->setParent(menu);
    }

    menu->exec(FCurrentTextEdit->mapToGlobal(pt));

    if (sugMenu) {
        delete sugMenu;
    }
    delete menu;
}

void SpellChecker::repairWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = FCurrentTextEdit->textCursor();

    cursor.beginEditBlock();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.removeSelectedText();
    cursor.insertText(action->property("word").toString());
    cursor.endEditBlock();

    SpellHighlighter *spell = getSpellByDocument(FCurrentTextEdit->document());
    spell->rehighlightBlock(cursor.block());
}

void SpellChecker::setDict()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    const QString lang = action->property("dictionary").toString();
    SpellBackend::instance()->setLang(lang);

    SpellHighlighter *spell = getSpellByDocument(FCurrentTextEdit->document());
    spell->rehighlight();
}

void SpellChecker::addWordToDict()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);
    if (!action)
    {
        return;
    }

    QTextCursor cursor = FCurrentTextEdit->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString word = cursor.selectedText();

    SpellBackend::instance()->add(word);

    SpellHighlighter *spell = getSpellByDocument(FCurrentTextEdit->document());
    spell->rehighlightBlock(cursor.block());
}

Q_EXPORT_PLUGIN2(plg_spellchecker, SpellChecker)
