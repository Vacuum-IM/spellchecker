#ifndef HUNSPELLCHECKER_H
#define HUNSPELLCHECKER_H

#include <QList>
#include <QString>
#include <QTextCodec>

#include <hunspell/hunspell.hxx>
#include "spellbackend.h"

class HunspellChecker :
	public SpellBackend
{
public:
	HunspellChecker();
	~HunspellChecker();
	virtual QList<QString> suggestions(const QString &AWord);
	virtual bool isCorrect(const QString &AWord);
	virtual bool add(const QString &AWord);
	virtual bool available() const;
	virtual bool writable() const;
	virtual QList<QString> dictionaries();
	virtual void setLang(const QString &ALang);
	virtual QString actuallLang();
private:
	Hunspell *FHunSpell;
	QString dictPath;
	QTextCodec *codec;
	QString lang;
};

#endif
