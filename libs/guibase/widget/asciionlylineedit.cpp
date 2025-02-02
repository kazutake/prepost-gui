#include "asciionlylineedit.h"

#include <QMessageBox>
#include <QTextCodec>

AsciiOnlyLineEdit::AsciiOnlyLineEdit(QWidget* parent) :
	QLineEdit(parent)
{
	m_errorMessage = tr("String has to consist of only English characters.");
}

void AsciiOnlyLineEdit::clearIfNotAscii(bool withoutMessage)
{
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	if (asciiCodec->canEncode(text())) {
		// no problem
		return;
	}
	clear();

	if (! withoutMessage){
		QMessageBox::warning(this, tr("Warning"), m_errorMessage);
	}
}

void AsciiOnlyLineEdit::setErrorMessage(const QString& msg)
{
	m_errorMessage = msg;
}

QString AsciiOnlyLineEdit::text() const
{
	return QLineEdit::text();
}

void AsciiOnlyLineEdit::setText(const QString& text)
{
	setText(text, false);
}

void AsciiOnlyLineEdit::setText(const QString& text, bool warnIfNotAscii)
{
	QLineEdit::setText(text);
	clearIfNotAscii(! warnIfNotAscii);
}

void AsciiOnlyLineEdit::focusOutEvent(QFocusEvent* e)
{
	clearIfNotAscii();
	QLineEdit::focusOutEvent(e);
}
