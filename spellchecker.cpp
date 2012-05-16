#include <QDir>
#include <QObject>
#include <QMessageBox>
#include <QApplication>

#include "spellbackend.h"

#include "definitions.h"

#include "spellchecker.h"

SpellChecker::SpellChecker()
{
    FMessageWidgets = NULL;
    FDictMenu = dictMenu();
}

SpellChecker::~SpellChecker()
{
    delete FDictMenu;
}

void SpellChecker::pluginInfo(IPluginInfo *APluginInfo)
{
    APluginInfo->name = tr("Spell Checker");
    APluginInfo->description = tr("Highlights words that may not be spelled correctly");
    APluginInfo->version = "0.0.6";
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

void SpellChecker::appendHL(QTextDocument *ADocument, IMultiUserChat *AMultiUserChat)
{
    SHPair hili;
    SpellHighlighter *spell = new SpellHighlighter(ADocument, AMultiUserChat);
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
    IMultiUserChatWindow *window = NULL;
    QWidget *parent = AWidget->instance()->parentWidget();
    while (window == NULL && parent != NULL)
    {
        window = qobject_cast<IMultiUserChatWindow *>(parent);
        parent = parent->parentWidget();
    }

    appendHL(AWidget->document(), window ? window->multiUserChat() : NULL);

    FTextEdit=AWidget->textEdit();
    AWidget->instance()->contextMenuPolicy();
    FTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(FTextEdit,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint &)));
}

void SpellChecker::onSpellDocumentDestroyed(QObject *ADocument) 
{
    delete getSpellByDocument(ADocument);
}

QMenu* SpellChecker::suggestMenu(const QString &word){
    QList<QString> sgstions = SpellBackend::instance()->suggestions(word);
    QMenu *menu = new QMenu(tr("Suggestions"));
    QAction *action;

    for (QList<QString>::iterator sgstion = sgstions.begin(); sgstion != sgstions.end(); sgstion++)
    {
        action = menu->addAction(*sgstion, this, SLOT(repairWord()));
        action->setProperty("word", *sgstion);
        action->setParent(menu);
    }

    return menu;
}

QMenu* SpellChecker::dictMenu() {
    QList<QString> dicts = SpellBackend::instance()->dictionaries();
    QMenu *menu = new QMenu(tr("Dictionary"));
    QString actualLang = SpellBackend::instance()->actuallLang();
    QAction *action;

    for (QList<QString>::iterator dict = dicts.begin(); dict != dicts.end(); dict++)
    {
        action = menu->addAction(*dict, this, SLOT(setDict()));
        action->setProperty("dictionary", *dict);
        action->setParent(menu);
        action->setCheckable(true);

        if (*dict == actualLang) {
            action->setChecked(true);
        }
    }

    return menu;
}

void SpellChecker::showContextMenu(const QPoint& pt)
{
    FTextEdit = qobject_cast<QTextEdit *>(sender());
    QTextCursor cursor = FTextEdit->cursorForPosition(pt);
    QMenu *menu = FTextEdit->createStandardContextMenu();
    QMenu *sugMenu = 0;

    //get word
    static const QRegExp expression("\\b\\w+\\b");
    QString AText = FTextEdit->toPlainText();

    int index = AText.indexOf(expression);
    while (index >= 0)
    {
        int length = expression.matchedLength();
        FrepLenght = -1;

        if (cursor.position() >= index && cursor.position() <= index + length) {
            AText.remove(index + length, AText.length());
            AText.remove(0, index);
            FrepLenght = length;
            break;
        }

        index = AText.indexOf(expression, index + length);
    }

    FrepStart = index;
    menu->addSeparator();
    menu->addMenu(FDictMenu);

    if (!AText.isEmpty() && FrepLenght > 0 && !SpellBackend::instance()->isCorrect(AText)) {
        sugMenu = suggestMenu(AText);

        if (!sugMenu->isEmpty()) {
          menu->addMenu(sugMenu);
        }

        QAction *action = menu->addAction(tr("Add to dictionary"),this,SLOT(addWordToDict()));
        action->setProperty("word", AText);
        action->setParent(menu);
    }

    menu->exec(FTextEdit->mapToGlobal(pt));

    if (sugMenu) {
        delete sugMenu;
    }
    delete menu;
}

void SpellChecker::repairWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;
    QString word = action->property("word").toString();
    QString AText=FTextEdit->toPlainText();

    AText.replace(FrepStart, FrepLenght, word);
    QTextCursor cursor= FTextEdit->textCursor();
    FTextEdit->setText(AText);
    FTextEdit->setTextCursor(cursor);
}

void SpellChecker::setDict()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (!action)
    {
        return;
    }

    QString lang = action->property("dictionary").toString();
    SpellBackend::instance()->setLang(lang);
    FTextEdit->setText(FTextEdit->toPlainText());
}

void SpellChecker::addWordToDict()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
    {
        return;
    }
    QString word = action->property("word").toString();
    SpellBackend::instance()->add(word);

    FTextEdit->setText(FTextEdit->toPlainText());
}

Q_EXPORT_PLUGIN2(plg_spellchecker, SpellChecker)

