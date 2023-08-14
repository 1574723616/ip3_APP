#ifndef SETTINGMAINWINDOW_H
#define SETTINGMAINWINDOW_H

#include <QMainWindow>
#include<qvector.h>
#include<qlineedit.h>
#include <QFile>
#include <qsettings.h> 
#include <QTextCodec> 
#include<QPushButton>
namespace Ui {
class settingMainWindow;
}

class settingMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit settingMainWindow(QWidget *parent = nullptr);
    ~settingMainWindow();
	
	void WriteLine(QString sFilePath, QVector<QString> VecStr);

	void ReadLine(QString sFilePath, QVector<QString>& VecStr);

	void initButton(int iBtn, int iSence);

	void SceneChanged(QPushButton * btnScene);

	void init();
    void ConfigSetting();
    QSettings* setting;
signals:
    void ButtonValueChange(int iScene);

private:
    Ui::settingMainWindow *ui;
    QVector<QLineEdit*> lineVec;
public:
    QVector<QString> SceneVec1;
	//场景位置 默认场景1
	int m_iScene = 1;
};

#endif // SETTINGMAINWINDOW_H
