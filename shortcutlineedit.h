#ifndef SHORTCUT_LINEEDIT_H
#define SHORTCUT_LINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QStringList>
#include <QKeyEvent>
class ShortCutLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    ShortCutLineEdit(QWidget *parent =0);
    ~ShortCutLineEdit();
	void getSetting();
	int CheckHotKeyState(const QKeySequence& keysq = QKeySequence(0));

signals:
	void sigError(QString);
protected:
    void keyPressEvent(QKeyEvent * event);
	QString m_sKeyText;
private:


};

#endif // SHORTCUT_LINEEDIT_H
