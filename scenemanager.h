#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include<sceneBase.h>
#include<singleton.h>
#include<QTabWidget>
#include<QMap>
#include "sceneBase.h"
#include"custtomStyle.hpp"


class SceneManager : public QTabWidget,public Singleton<SceneManager>
{
    Q_OBJECT
public:
    explicit SceneManager(QWidget *parent = nullptr);
	SceneBase* at(SceneType scenTp) { return  m_sceneMap[scenTp]; };
    
    QSettings* m_setting;
    QString appText1;
    QString appText2;
    QString appText3;
    QString appText4;
signals:

public slots:

private:
    void loadStyleSheet(QString path);
    void saveAndInserInfo();
 

private:
	QMap<SceneType,SceneBase*> m_sceneMap;
    SceneBase* m_SceneBase;
    SceneBase* m_SceneBase1;
    SceneBase* m_SceneBase2;
    SceneBase* m_SceneBase3;

};

#endif // SCENEMANAGER_H
