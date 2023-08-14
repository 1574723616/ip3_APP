#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"QPushButton"
#include<QFile>
#include <QSystemTrayIcon>
#include<QVector>
#include <qsettings.h> 
#include<QAbstractNativeEventFilter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum sceneArea1Btn{
        BTN1_PLAY = 0,
        BTN2_PLAY,
        BTN3_PLAY,
        BTN4_PLAY,
        BTN5_PLAY,
        BTN6_PLAY,
        BTN7_PLAY,
        BTN8_PLAY,
        BTN9_PLAY,
    };
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void CustomizeShortcuts(QString value);
    HWND getPowerPointWindowHandle(QString sName);
    void sendGlobalCtrlMKeyPressEvent(char key, int Type);
    void sendGlobalCtrlMKeyPressEvent(char key, int nCtrl, int nShift);
    void simulateCtrlF12Keypress();

    void isPPT();
    void isWord();
    void isExcel();
    void isApp(QString appName);


    void getAllappName();
    void initSystemTray();
    void writeLog(QString str);
    QString appBackg;
    QString appText;
    QString areaObjName;

    //QVector<QPushButton*> scenButtonVec;

private:
    void init();
    void setConnect();
	void loadStyleSheet(QString path);
    void setButtonConnect(QPushButton* sceneButton, QString scenName, int num);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* trayIcon;
	QSettings* setting;
	QAction* m_openSetting;
	int m_iScene = 1;
    QMap<QString, QString> m_objNameMap;
 
};
#endif // MAINWINDOW_H
