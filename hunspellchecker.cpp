#include <QDir>
#include <QLocale>
#include <QCoreApplication>

#include "hunspellchecker.h"

HunspellChecker::HunspellChecker()
{
	FHunSpell = NULL;

#ifdef Q_WS_WIN
	dictPath = QString("%1/hunspell").arg(QCoreApplication::applicationDirPath()).toUtf8().constData();
#elif defined (Q_WS_X11)
	dictPath = "/usr/share/hunspell/";
#elif defined (Q_WS_MAC)
    dictPath = QString("%1/Library/Spelling").arg(QDir::homePath());
#endif

	lang = QLocale().name().toUtf8().constData();
	loadHunspell(lang);
}

HunspellChecker::~HunspellChecker()
{
	if (FHunSpell)
		delete FHunSpell;
}

bool HunspellChecker::isCorrect(const QString &AWord)
{

	if(FHunSpell)
	{
		QByteArray encodedString;
		encodedString = codec->fromUnicode(AWord);
		return FHunSpell->spell(encodedString.data());
	}
	return true;
}

QList<QString> HunspellChecker::suggestions(const QString &AWord)
{
	QList<QString> words;
	if(FHunSpell)
	{
		char **sugglist;
		QByteArray encodedString;
		encodedString = codec->fromUnicode(AWord);
		int count = FHunSpell->suggest(&sugglist, encodedString.data());
		for(int i = 0; i < count; ++i)
			words << (codec ? codec->toUnicode(sugglist[i]) : QString::fromUtf8(sugglist[i]));
		FHunSpell->free_list(&sugglist, count);
		return words;
	}
	return words;
}

bool HunspellChecker::add(const QString &AWord)
{
	bool result = false;
	if (FHunSpell)
	{
		FHunSpell->add(AWord.toUtf8().constData());
		result = true;
	}
	return result;
}

bool HunspellChecker::available() const
{
	return (FHunSpell != NULL);
}

bool HunspellChecker::writable() const
{
	return false;
}

QList< QString > HunspellChecker::dictionaries()
{
	QStringList dict;
	QDir dir(dictPath);
	if(dir.exists()) {
		QStringList lstDic = dir.entryList(QStringList("*.dic"), QDir::Files);
		foreach(QString tmp, lstDic) {
			if (tmp.startsWith("hyph_"))
				continue;
			if (tmp.startsWith("th_"))
				continue;
			if (tmp.endsWith(".dic"))
				tmp = tmp.mid(0, tmp.length() - 4);
			dict << tmp;
		}
	}
	return dict;
}

void HunspellChecker::setLang(const QString &ALang)
{
	if(FHunSpell)
		delete FHunSpell;
	lang = ALang;
	loadHunspell(lang);
}

void HunspellChecker::loadHunspell(const QString &ALang)
{
	QString dic = QString("%1/%2.dic").arg(dictPath).arg(ALang);
	if (QFileInfo(dic).exists())
	{
		FHunSpell = new Hunspell(QString("%1/%2.aff").arg(dictPath).arg(ALang).toUtf8().constData(), dic.toUtf8().constData());
		codec = QTextCodec::codecForName(FHunSpell->get_dic_encoding());
	}
}

QString HunspellChecker::actuallLang()
{
	return lang;
}
