
#ifndef SCENEBASE_H
#define SCENEBASE_H


#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QFile>
#include <QCoreApplication>
#include<QDebug>
#include<QSettings>
#include<QTextCodec>
#include<QFileDialog>
#include<QObject>
#include<QVariant>
#include<QMap>

#include<QAction>
#include<QMenu>
#include <windows.h>
#include<QListWidget>

class SceneBase: public QWidget
{
    Q_OBJECT
public:
    SceneBase(QString scenneName,QWidget *parent = nullptr);
	QString m_scenneName;
    QString appImagePath;
    QString GetSceneAppName(){ return m_appName; };
    void SetAppName(QString appName) { m_appName = appName; };

protected:
    void init();
    void setupUI();
    void setConnect();
    void loadStyleSheet();
    bool eventFilter(QObject* obj, QEvent* event);
    HWND getPowerPointWindowHandle();
    void FillList();
    void onItemDoubleClicked(QListWidgetItem* item);
signals:
    void sig_applySetting();

private:
	QSettings* m_setting;
    QString m_appName;
    QLineEdit* appNameEdit;
    QLineEdit* appTextEdit;
    QPushButton* m_apply;
	QMap<int,QString> m_imgPathMap;

    QAction* buttonAction;
    QMenu* buttonMenu;
    QListWidget* m_pAppList;

};

#endif // SCENEBASE_H
