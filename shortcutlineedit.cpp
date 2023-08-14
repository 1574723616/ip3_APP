#include "shortcutlineedit.h"
#include<qdebug.h>
ShortCutLineEdit::ShortCutLineEdit(QWidget *parent )
    :QLineEdit(parent)
{
    setAttribute(Qt::WA_InputMethodEnabled, false) ;
	setContextMenuPolicy(Qt::NoContextMenu);
	connect(this, &QLineEdit::editingFinished,this, &ShortCutLineEdit::getSetting);
}


ShortCutLineEdit::~ShortCutLineEdit()
{

}

void ShortCutLineEdit::getSetting()
{
	setText(m_sKeyText);
}



void ShortCutLineEdit::keyPressEvent(QKeyEvent * keyevent)
{
	int uKey = keyevent->key();
	Qt::Key key = static_cast<Qt::Key>(uKey);
	if(key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt || key == Qt::Key_Meta)
	{
		return ;
	}

	QKeySequence keysq = QKeySequence(keyevent->modifiers()|keyevent->key());
    m_sKeyText = keysq.toString();
	setText(m_sKeyText);


    return ;
}


