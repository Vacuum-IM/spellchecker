#include "spellhighlighter.h"
#include "spellchecker.h"
#include "spellbackend.h"

SpellHighlighter::SpellHighlighter(QTextDocument *ADocument, IMultiUserChat *AMultiUserChat)
    : QSyntaxHighlighter(ADocument),
      FMultiUserChat(AMultiUserChat)
{

}

void SpellHighlighter::highlightBlock(const QString &AText)
{
	QTextCharFormat tcf;
	tcf.setUnderlineColor(Qt::red);
	tcf.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

	// Match words (minimally)
	QRegExp expression("\\b\\w+\\b");

	QRegExp number("\\b\\d+\\b");

	// Iterate through all words
	int index = AText.indexOf(expression);
	while (index >= 0) 
	{                
		int length = expression.matchedLength();
                if (!expression.cap().contains(number) && !isUserNickName(FMultiUserChat, expression.cap()))
                {
		        if (!SpellBackend::instance()->isCorrect(expression.cap()))
                        {
			        setFormat(index, length, tcf);
                        }
                }
		index = AText.indexOf(expression, index + length);
	}
}

bool SpellHighlighter::isUserNickName(IMultiUserChat *AMultiUserChat, const QString &AText)
{
        if (AMultiUserChat != NULL)
        {
                QList<IMultiUser *> list = AMultiUserChat->allUsers();
                for (int i = 0; i < list.count(); ++i)
                {
                        if (AText == list.at(i)->nickName())
                        {
                                return true;
                        }
                }
        }

        return false;
}
