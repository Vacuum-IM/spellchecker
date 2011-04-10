#ifndef SPELLHIGHLIGHTER_H
#define SPELLHIGHLIGHTER_H

#include <QString>
#include <QSyntaxHighlighter>

class SpellHighlighter : 
	public QSyntaxHighlighter
{
public:
	SpellHighlighter(QTextDocument *ADocument);
	virtual void highlightBlock(const QString &AText);
};

#endif //SPELLHIGHLIGHTER_H
