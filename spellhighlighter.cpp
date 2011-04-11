#include "spellhighlighter.h"

#include "spellbackend.h"

SpellHighlighter::SpellHighlighter(QTextDocument *ADocument) : QSyntaxHighlighter(ADocument)
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
		if (!expression.cap().contains(number))
		        if (!SpellBackend::instance()->isCorrect(expression.cap()))
			        setFormat(index, length, tcf);
		index = AText.indexOf(expression, index + length);
	}
}
