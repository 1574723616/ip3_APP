
#include "sceneBase.h"
#include <QKeySequenceEdit>
#include"shortcutlineedit.h"
#include"hotkey_mgr.h"
#include <Windows.h>
#include<QDebug>




#pragma execution_character_set("utf-8")
#define SceneButtonNum 9
QMap<QString, HWND> g_WndMap;
SceneBase::SceneBase(QString scenneName,QWidget* parent)
    : QWidget(parent)
    ,m_scenneName(scenneName)
	, m_apply(new QPushButton("应用"))
    , m_pAppList(new QListWidget)
{
    init();
    setupUI();
    setConnect();
    FillList();
}

void SceneBase::init()
{
    m_apply->setObjectName("m_apply");

	m_setting = new QSettings(QCoreApplication::applicationDirPath() + "/SceneConfig.ini", QSettings::IniFormat);
	m_setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
}




void SceneBase::setupUI()
{
    //初始化动作
    buttonAction = new QAction("btnFirstAction", this);
    //初始化菜单
    buttonMenu = new QMenu(this);
    //动作添加到菜单
    buttonMenu->addAction(buttonAction);
    //给动作设置信号槽
    connect(buttonAction, &QAction::triggered, [=]()
        {
            qDebug() << "I'm btnFirstAction";
        });

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setSpacing(0);
    vlayout->setContentsMargins(20,20,0,0);
    m_setting->beginGroup(m_scenneName);
    for(int num=0;num< SceneButtonNum;num++){
        QPushButton* sceneAreaBtn = new QPushButton;
        sceneAreaBtn->setObjectName("sceneAreaBtn");

        QLineEdit* areaLineEdit = new QLineEdit;
        areaLineEdit->setObjectName("areaLineEdit");

		ShortCutLineEdit* ShortCutEdit = new ShortCutLineEdit;
		ShortCutEdit->setObjectName("ShortCutEdit");
        ShortCutEdit->installEventFilter(this);

        QPushButton* deleteBtnMes = new QPushButton;
        deleteBtnMes->setObjectName("deleteBtnMes");
        deleteBtnMes->setText("清空");

		QString btnText = m_setting->value(QString("sceneBtnText%1").arg(num), "").toString();
		QString keySeq = m_setting->value(QString("scenekeySeq%1").arg(num), "").toString();
		QString btnImgPath = m_setting->value(QString("sceneBtn%1").arg(num), "").toString();

		areaLineEdit->setText(btnText);
        ShortCutEdit->setText(keySeq);
		
		if (!btnImgPath.isEmpty())
			sceneAreaBtn->setStyleSheet("QPushButton { border-image: url(" + btnImgPath + ");background: #e4eff9;}");

        QHBoxLayout *AreaLayout = new QHBoxLayout(this);
        AreaLayout->setSpacing(0);
        AreaLayout->setContentsMargins(20,0,0,0);
        AreaLayout->addWidget(sceneAreaBtn);
        AreaLayout->addSpacing(30);
        AreaLayout->addWidget(new QLabel(QStringLiteral("背景文字 %1").arg(num + 1)));
        AreaLayout->addSpacing(10);
        AreaLayout->addWidget(areaLineEdit);
        AreaLayout->addSpacing(30);
        AreaLayout->addWidget(new QLabel(QStringLiteral("快捷键 %1").arg(num + 1)));
        AreaLayout->addSpacing(10);
        AreaLayout->addWidget(ShortCutEdit);
        AreaLayout->addSpacing(30);
        AreaLayout->addWidget(deleteBtnMes);
        AreaLayout->addStretch();

        vlayout->addLayout(AreaLayout);
        vlayout->addSpacing(15);

        connect(sceneAreaBtn, &QPushButton::clicked, [this, sceneAreaBtn, num]() {
			QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", QString(), "Image Files(*.png *.jpg *.bmp)");
			m_imgPathMap[num] = imagePath;
            if (!imagePath.isEmpty())sceneAreaBtn->setStyleSheet("QPushButton { border-image: url(" + imagePath + ");background: #e4eff9;}");
        });

        connect(deleteBtnMes, &QPushButton::clicked, [=]() {
            areaLineEdit->setText("");
            ShortCutEdit->setText("");
            sceneAreaBtn->setStyleSheet("");
            m_imgPathMap[num] = "";
            });

        m_imgPathMap[num] = m_setting->value(QString("sceneBtn%1").arg(num), "").toString();

        connect(this, &SceneBase::sig_applySetting,sceneAreaBtn,[this,areaLineEdit,num, ShortCutEdit]() {
			m_setting->beginGroup(m_scenneName);
			m_setting->setValue(QString("sceneBtnText%1").arg(num),areaLineEdit->text());
			m_setting->setValue(QString("scenekeySeq%1").arg(num), ShortCutEdit->text());

            //if(!m_imgPathMap[num].isEmpty())
			    m_setting->setValue(QString("sceneBtn%1").arg(num), m_imgPathMap[num]);

			m_setting->endGroup();
        });

        
    }
    int size = m_imgPathMap.size();
    //添加场景的自定义设置
    appNameEdit = new QLineEdit;
    appNameEdit->setObjectName("appNameEdit");

    appTextEdit = new QLineEdit;
    appTextEdit->setObjectName("appTextEdit");

    //QPushButton* app = new QPushButton("选择应用");
    //app->setObjectName("m_apply");

    QPushButton* appButtonBackg = new QPushButton;
    appButtonBackg->setObjectName("sceneAreaBtn");

    QString appNameText = m_setting->value(QString("appNameEdit"), "").toString();
    QString appText = m_setting->value(QString("appTextEdit"), "").toString();
    QString appBackg = m_setting->value(QString("appButtonBackg"), "").toString();

    appNameEdit->setText(appNameText);
    appTextEdit->setText(appText);
    m_appName = appNameText;

    if (!appBackg.isEmpty())
        appButtonBackg->setStyleSheet("QPushButton { border-image: url(" + appBackg + ");background: #e4eff9;}");
    appImagePath = appBackg;

    QHBoxLayout* sceneButtonLayout = new QHBoxLayout(this);
    sceneButtonLayout->setSpacing(10);
    //sceneButtonLayout->setContentsMargins(20, 0, 0, 0);
    //sceneButtonLayout->addWidget(app);
    //sceneButtonLayout->addSpacing(30);
    sceneButtonLayout->setContentsMargins(20, 0, 0, 0);
    //sceneButtonLayout->addWidget(m_pAppList);
    //sceneButtonLayout->addSpacing(30);
    sceneButtonLayout->addWidget(new QLabel(QStringLiteral("应用名称")));
    sceneButtonLayout->addSpacing(10);
    sceneButtonLayout->addWidget(appNameEdit);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(new QLabel(QStringLiteral("应用背景文字")));
    sceneButtonLayout->addSpacing(10);
    sceneButtonLayout->addWidget(appTextEdit);
    sceneButtonLayout->addSpacing(20);
    sceneButtonLayout->addWidget(new QLabel(QStringLiteral("应用背景")));
    sceneButtonLayout->addSpacing(10);
    sceneButtonLayout->addWidget(appButtonBackg);
    sceneButtonLayout->addStretch();


    vlayout->addLayout(sceneButtonLayout);
    vlayout->addSpacing(390);

    QHBoxLayout *applyLayout = new QHBoxLayout(this);
    applyLayout->setSpacing(0);
    applyLayout->setContentsMargins(0,0,0,0);
    applyLayout->addSpacing(550);
    applyLayout->addWidget(m_apply);

    vlayout->addLayout(applyLayout);
    vlayout->addSpacing(15);
    vlayout->addStretch();


    connect(this, &SceneBase::sig_applySetting, this, [=]() {
        m_setting->beginGroup(m_scenneName);
        m_setting->setValue(QString("appNameEdit"), appNameEdit->text());
        m_setting->setValue(QString("appTextEdit"), appTextEdit->text());
        m_setting->setValue(QString("appButtonBackg"), appImagePath);
        m_appName = appNameEdit->text();
        m_setting->endGroup();
        //qDebug() << m_appName;
        });

    connect(appButtonBackg, &QPushButton::clicked, [this, appButtonBackg]() {
        appImagePath = QFileDialog::getOpenFileName(this, "选择图片", QString(), "Image Files(*.png *.jpg *.bmp)");
        if (!appImagePath.isEmpty())appButtonBackg->setStyleSheet("QPushButton { border-image: url(" + appImagePath + ");background: #e4eff9;}");
        });

    /*connect(app, &QPushButton::clicked, [&]() {
        QString AppName = QFileDialog::getOpenFileName(this, "选择应用", QString(), " all EXE(*.exe*)");
        if (!AppName.isEmpty()) {
            QStringList list = AppName.split("/");
            m_appName = list.last();
            appNameEdit->setText(m_appName);
        }
    });*/

    m_setting->endGroup();

    loadStyleSheet();
}
void SceneBase::setConnect()
{
    connect(m_apply,&QPushButton::clicked,[&](){
        emit sig_applySetting();
    });
}

void SceneBase::loadStyleSheet()
{
    QFile file(":/rs/SceneBase.qss");
    file.open(QFile::ReadOnly);
    setStyleSheet(file.readAll());
    file.close();

}



bool SceneBase::eventFilter(QObject* obj, QEvent* event)
{
	 if (obj->objectName() == "ShortCutEdit" && event->type() == QEvent::MouseButtonRelease){
		 ShortCutLineEdit* tmpShortCutEdit = static_cast<ShortCutLineEdit*>(obj);

		 if (tmpShortCutEdit->isEnabled())
			 tmpShortCutEdit->selectAll();

	}
	return QWidget::eventFilter(obj, event);
}


void SceneBase::FillList()
{
    getPowerPointWindowHandle();
    m_pAppList->setParent(this);
    m_pAppList->setFixedSize(740, 150);
    m_pAppList->move(40, 580);
    for (auto mapIt = g_WndMap.begin(); mapIt != g_WndMap.end(); ++mapIt) {
        QString sTitle = mapIt.key();
        QListWidgetItem* pItem = new QListWidgetItem(sTitle);
        m_pAppList->addItem(pItem);
        connect(m_pAppList, &QListWidget::itemDoubleClicked, this, &SceneBase::onItemDoubleClicked);
    }
}

void SceneBase::onItemDoubleClicked(QListWidgetItem* item)
{
    // 获取列表框中的元素文本
    m_appName = item->text();
    appNameEdit->setText(item->text());
}

// 获取PPT窗口句柄的函数
HWND SceneBase::getPowerPointWindowHandle()
{
    g_WndMap.clear();
    HWND hwnd = NULL;  // 窗口句柄
    // 遍历所有顶级窗口，查找包含"PPT"字样的窗口标题

    EnumWindows([](HWND wnd, LPARAM param) -> BOOL
        {
            constexpr int MAX_WINDOW_TITLE_LENGTH = 256;
            TCHAR windowTitle[MAX_WINDOW_TITLE_LENGTH];

            // 获取窗口标题
            GetWindowText(wnd, windowTitle, MAX_WINDOW_TITLE_LENGTH);

            // 判断是否是PPT窗口（可以根据实际情况调整判断条件）
            QString title = QString::fromWCharArray(windowTitle);
            if (!title.isEmpty())
                g_WndMap[title] = wnd;
            return TRUE;  // 继续枚举其他窗口
        }, reinterpret_cast<LPARAM>(&hwnd));

    return hwnd;
}
