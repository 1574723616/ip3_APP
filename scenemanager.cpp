#include "scenemanager.h"
#include <QDebug>

SceneManager::SceneManager(QWidget *parent)
    : QTabWidget(parent)
    ,m_SceneBase(new SceneBase("SceneArea_first"))
    ,m_SceneBase1(new SceneBase("SceneArea_second"))
    ,m_SceneBase2(new SceneBase("SceneArea_third"))
    ,m_SceneBase3(new SceneBase("SceneArea_four"))
{ 
    saveAndInserInfo();
    setTabPosition(QTabWidget::West);
    setWindowIcon(QIcon("Coin0001.png"));
    setWindowTitle("场景设置");
    tabBar()->setStyle(new CustomTabStyle);
    //resize(930,600);
    setFixedSize(960, 800);
}

void SceneManager::saveAndInserInfo()
{
    m_sceneMap[SceneArea_first]=m_SceneBase;
    m_sceneMap[SceneArea_second] = m_SceneBase1;
    m_sceneMap[SceneArea_third] = m_SceneBase2;
    m_sceneMap[SceneArea_four] = m_SceneBase3;

    
    insertTab(0,m_SceneBase, m_SceneBase->GetSceneAppName());
    insertTab(1,m_SceneBase1, m_SceneBase1->GetSceneAppName());
    insertTab(2,m_SceneBase2, m_SceneBase2->GetSceneAppName());
    insertTab(3,m_SceneBase3, m_SceneBase3->GetSceneAppName());

    connect(m_SceneBase, &SceneBase::sig_applySetting, [=]() {
        setTabText(0, m_SceneBase->GetSceneAppName());
        });
    connect(m_SceneBase1, &SceneBase::sig_applySetting, [=]() {
        setTabText(1, m_SceneBase1->GetSceneAppName());
        });
    connect(m_SceneBase2, &SceneBase::sig_applySetting, [=]() {
        setTabText(2, m_SceneBase2->GetSceneAppName());
        });
    connect(m_SceneBase3, &SceneBase::sig_applySetting, [=]() {
        setTabText(3, m_SceneBase3->GetSceneAppName());
        });
      
}
void SceneManager::loadStyleSheet(QString path)
{
	QFile file(path);
	file.open(QFile::ReadOnly);
	QString MainQss = file.readAll();
	setStyleSheet(MainQss);
	file.close();
}

