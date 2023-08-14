#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QDebug>
#include<QApplication>
#include<QCoreApplication>
#include<QFontDatabase>
#include<QStringList>
#include<QMap>
#include <windows.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <windows.h>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include<settingMainWindow.h>
#include "sceneBase.h"
#include<scenemanager.h>
#include<QThread>


#pragma comment(lib, "user32.lib")
#pragma execution_character_set("utf-8")


QMap<QString, QVector<QPushButton*>> g_scenMap;
QString g_sAppHandleName;
MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	init();
	setConnect();
	setWindowIcon(QIcon("Coin0001.png"));
    loadStyleSheet(":/rs/button.qss");
	setWindowTitle("场景应用");
    SceneManager::instance()->show();
	//setWindowFlags(Qt::Window | Qt::FramelessWindowHint);  // 隐藏状态栏 会导致对话框无法拖拽
	getAllappName();
	isPPT();
}


void MainWindow::getAllappName() {

	QString regPath = "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
	QSettings settings(regPath, QSettings::NativeFormat);

	QStringList appList = settings.childGroups();

	foreach(const QString & appName, appList) {
		QString appPath = regPath + "\\" + appName;
		QSettings appSettings(appPath, QSettings::NativeFormat);

		QString displayName = appSettings.value("DisplayName").toString();
		QString displayVersion = appSettings.value("DisplayVersion").toString();

		qDebug() << "Installed applications:" << displayName;

	}
}


void MainWindow::init()
{
	ui->setupUi(this);

	setting = new QSettings(QCoreApplication::applicationDirPath() + "/SceneConfig.ini", QSettings::IniFormat);
	//设置文件编码，配置文件中使用中文时，这是必须的，否则乱码
	setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
	//int fontId = QFontDatabase::addApplicationFont(parentPath + "/Alibaba-PuHuiTi-Regular.ttf");
	int fontId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + "/Alibaba-PuHuiTi-Regular.ttf");
	SceneManager::instance();//单例模式

	m_objNameMap["SceneArea_first"]= "m_scene1";
	m_objNameMap["SceneArea_second"] = "m_scene2";
	m_objNameMap["SceneArea_third"] = "m_scene3";
	m_objNameMap["SceneArea_four"] = "m_scene4";

	for (const auto& key : m_objNameMap.keys()) {
		QString widgetName = key;
		QString sceneButtonName = m_objNameMap.value(key);

		QWidget* sceneAreaWid = new QWidget;
		QVector<QPushButton*> scenButtonVec;
		sceneAreaWid->setObjectName(widgetName);
		ui->stackedWidget->addWidget(sceneAreaWid);
		ui->stackedWidget->setCurrentWidget(sceneAreaWid);
		setting->beginGroup(widgetName);
		for (int i = 0;i < 9;i++) {
			QString btnText = setting->value(QString("sceneBtnText%1").arg(i), "").toString();
			QString keySeq = setting->value(QString("scenekeySeq%1").arg(i), "").toString();
			QString btnImgPath = setting->value(QString("sceneBtn%1").arg(i), "").toString();

			QPushButton* sceneAreaBtn = new QPushButton(sceneAreaWid);
            btnImgPath.isEmpty()? sceneAreaBtn->setStyleSheet("QPushButton {background: #e4eff9;}"):
								  sceneAreaBtn->setStyleSheet("QPushButton { border-image: url(" + btnImgPath + ");background: #e4eff9;}");

            sceneAreaBtn->setFixedSize(110, 110);
			sceneAreaBtn->setText(btnText);
			sceneAreaBtn->move(16 + i % 3 * 110, 16 + i / 3 * 110);
			setButtonConnect(sceneAreaBtn, widgetName,i);//设置按钮连接
			scenButtonVec.push_back(sceneAreaBtn);
		}

        QPushButton* sceneBtn = this->findChild<QPushButton*>(sceneButtonName);
        if (sceneBtn) {
            connect(sceneBtn, &QPushButton::clicked, [=]() {
				ui->stackedWidget->setCurrentWidget(sceneAreaWid);
				qDebug() << widgetName;
			});
		}
		QString sceneAppBackg = setting->value(QString("appButtonBackg"), "").toString();
		QString sceneAppText = setting->value(QString("appTextEdit"), "").toString();
		sceneBtn->setText(sceneAppText);

		sceneAppBackg.isEmpty() ? sceneBtn->setStyleSheet("QPushButton {background: #1568FE;}") :
								  sceneBtn->setStyleSheet("QPushButton { border-image: url(" + sceneAppBackg + ");background: #e4eff9;}");

		QFile file(sceneAppBackg);
		if (!file.exists())
		{
			QString sImagePath = QCoreApplication::applicationDirPath();
			sImagePath.replace("debug","");
			sImagePath.replace("release", "");

			//if ("SceneArea_first" == widgetName)
			//	sImagePath += "/res/tray.png";
			//else if ("SceneArea_second" == widgetName)
			//	sImagePath += "/res/d.png";
			//else if ("SceneArea_third" == widgetName)
			//	sImagePath += "/res/WebQQAway20.png";
			//else if ("SceneArea_four" == widgetName)
			//	sImagePath += "/res/Coin0001.png";
			if ("SceneArea_first" == widgetName)
				sImagePath += "/res/ppt.jpg";
			else if ("SceneArea_second" == widgetName)
				sImagePath += "/res/Word.png";
			else if ("SceneArea_third" == widgetName)
				sImagePath += "/res/Excel.jpeg";
			else if ("SceneArea_four" == widgetName)
				sImagePath += "/res/title.ico";
			writeLog(sImagePath);
			sceneBtn->setStyleSheet("QPushButton { border-image: url(" + sImagePath + ");background: #e4eff9;}");
		}
		g_scenMap[widgetName] = scenButtonVec;
		setting->endGroup();
	}

}

void MainWindow::setButtonConnect(QPushButton*sceneButton,QString scenName,int num) {
	connect(sceneButton, &QPushButton::clicked, [=]() {
		setting->beginGroup(scenName);
		QString keySeq = setting->value(QString("scenekeySeq%1").arg(num), "").toString();
		QString appName = setting->value(QString("appNameEdit"), "").toString();
		QStringList value1 = appName.split('.');
		QString value = value1.at(0);
		setting->endGroup();
		isApp(value);
		CustomizeShortcuts(keySeq);
		qDebug() << "按下" << scenName << keySeq;
		});
}
void MainWindow::setConnect()
{
	initSystemTray();

	auto func = [&]() {
		for (auto mapIt = g_scenMap.begin(); mapIt != g_scenMap.end(); ++mapIt) {
			areaObjName = mapIt.key();
			QVector<QPushButton*> sceneButton= mapIt.value();
			SceneType type = static_cast<SceneType>(areaObjName.toInt());
			setting->beginGroup(areaObjName);

			for (int i = 0; i < sceneButton.size(); i++) {
				QString btnText = setting->value(QString("sceneBtnText%1").arg(i), "").toString();
				QString keySeq = setting->value(QString("scenekeySeq%1").arg(i), "").toString();
				QString btnImgPath = setting->value(QString("sceneBtn%1").arg(i), "").toString();

				sceneButton[i]->setText(btnText);
				btnImgPath.isEmpty()? sceneButton[i]->setStyleSheet("QPushButton {background: #e4eff9;}"):
									  sceneButton[i]->setStyleSheet("QPushButton { border-image: url(" + btnImgPath + ");background: #e4eff9;}");
			}

			QString sceneAppBackg = setting->value(QString("appButtonBackg"), "").toString();
			QString sceneAppText = setting->value(QString("appTextEdit"), "").toString();
			
			auto it = m_objNameMap.find(areaObjName);
			QPushButton* sceneBtn = this->findChild<QPushButton*>(it.value());
			sceneBtn->setText(sceneAppText);

			sceneAppBackg.isEmpty() ? sceneBtn->setStyleSheet("QPushButton {background: #1568FE;}") :
									  sceneBtn->setStyleSheet("QPushButton { border-image: url(" + sceneAppBackg + ");background: #e4eff9;}");
			QFile file(sceneAppBackg);
			if (!file.exists())
			{
				QString sImagePath = QCoreApplication::applicationDirPath();
				sImagePath.replace("debug", "");
				sImagePath.replace("release", "");

				if ("SceneArea_first" == areaObjName)
					sImagePath += "/res/ppt.jpg";
				else if ("SceneArea_second" == areaObjName)
					sImagePath += "/res/Word.png";
				else if ("SceneArea_third" == areaObjName)
					sImagePath += "/res/Excel.jpeg";
				else if ("SceneArea_four" == areaObjName)
					sImagePath += "/res/title.ico";
				writeLog(sImagePath);
				sceneBtn->setStyleSheet("QPushButton { border-image: url(" + sImagePath + ");background: #e4eff9;}");
			}
			setting->endGroup();
		}
	};


	connect(SceneManager::instance()->at(SceneType::SceneArea_first), &SceneBase::sig_applySetting, func);
	connect(SceneManager::instance()->at(SceneType::SceneArea_second), &SceneBase::sig_applySetting, func);
	connect(SceneManager::instance()->at(SceneType::SceneArea_third), &SceneBase::sig_applySetting, func);
	connect(SceneManager::instance()->at(SceneType::SceneArea_four), &SceneBase::sig_applySetting, func);

	connect(m_openSetting, &QAction::triggered, []() {
		SceneManager::instance()->move(1000, 300);
		SceneManager::instance()->show();
	});
	
}

void MainWindow::initSystemTray()
{
	// 创建一个QSystemTrayIcon对象
	trayIcon = new QSystemTrayIcon(QIcon("Coin0001.png"), nullptr);

	// 创建一个右键菜单
	QMenu* menu = new QMenu;
	m_openSetting = new QAction("Settings");
	menu->addAction(m_openSetting);
	trayIcon->setContextMenu(menu);
	trayIcon->show();
}

void MainWindow::loadStyleSheet(QString path)
{
	QFile file(path);
	file.open(QFile::ReadOnly);
    setStyleSheet(file.readAll());
	file.close();
}

MainWindow::~MainWindow()
{
	delete trayIcon;
	delete ui;
}


// 获取PPT窗口句柄的函数
HWND MainWindow::getPowerPointWindowHandle(QString sName)
{
	g_sAppHandleName = sName;
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
            if (title.contains(g_sAppHandleName))
            {
                *reinterpret_cast<HWND*>(param) = wnd;  // 将窗口句柄保存到参数中
                qDebug() << "find windows:" << title;
               // return FALSE;  // 返回FALSE以停止枚举
            }

            return TRUE;  // 继续枚举其他窗口
        }, reinterpret_cast<LPARAM>(&hwnd));

	return hwnd;
}
void MainWindow::isApp(QString appName)
{
	HWND pptHandle = getPowerPointWindowHandle(appName);
	//	HWND pptHandle = ::FindWindow(L"PPTFrameClass", NULL);
	if (pptHandle != NULL) {
		// 将PPT窗口设为前台窗口并获取焦点
		SetForegroundWindow(pptHandle);
	}
}
void MainWindow::isPPT() {
	HWND pptHandle = getPowerPointWindowHandle("微信"); //PowerPoint
//	HWND pptHandle = ::FindWindow(L"PPTFrameClass", NULL);
	if (pptHandle != NULL) {
		// 将PPT窗口设为前台窗口并获取焦点
		SetForegroundWindow(pptHandle);
	}
}

void MainWindow::isWord() {
	HWND WordHandle = getPowerPointWindowHandle("Word");
	if (WordHandle != NULL) {
		// 将PPT窗口设为前台窗口并获取焦点
		SetForegroundWindow(WordHandle);
	}
}

void MainWindow::isExcel() {
	HWND ExcelHandle = getPowerPointWindowHandle("Excel");
	if (ExcelHandle != NULL) {
		// 将PPT窗口设为前台窗口并获取焦点
		SetForegroundWindow(ExcelHandle);
	}
}

void MainWindow::sendGlobalCtrlMKeyPressEvent(char key, int nCtrl, int nShift)
{
	// 模拟按下Ctrl键
	INPUT ctrlKeyDown;
	ZeroMemory(&ctrlKeyDown, sizeof(INPUT));
	ctrlKeyDown.type = INPUT_KEYBOARD;
	ctrlKeyDown.ki.wVk = nCtrl;
	// 模拟按下Shift键
	INPUT shiftKeyDown;
	ZeroMemory(&shiftKeyDown, sizeof(INPUT));
	shiftKeyDown.type = INPUT_KEYBOARD;
	shiftKeyDown.ki.wVk = nShift;
	// 模拟按下M键
	INPUT mKeyDown;
	ZeroMemory(&mKeyDown, sizeof(INPUT));
	mKeyDown.type = INPUT_KEYBOARD;
	mKeyDown.ki.wVk = key;

	// 模拟释放Ctrl和M键
	INPUT keyRelease;
	ZeroMemory(&keyRelease, sizeof(INPUT));
	keyRelease.type = INPUT_KEYBOARD;
	keyRelease.ki.dwFlags = KEYEVENTF_KEYUP;

	// 将按键操作写入输入缓冲区
	INPUT inputEvents[4] = { ctrlKeyDown, shiftKeyDown,mKeyDown, keyRelease };
	SendInput(4, inputEvents, sizeof(INPUT));

	// 可选：给操作系统一些时间来处理按键事件
	Sleep(100);

	// 模拟释放Ctrl键
	INPUT ctrlKeyUp;
	ZeroMemory(&ctrlKeyUp, sizeof(INPUT));
	ctrlKeyUp.type = INPUT_KEYBOARD;
	ctrlKeyUp.ki.wVk = nCtrl;
	ctrlKeyUp.ki.dwFlags = KEYEVENTF_KEYUP;
	// 模拟释放shift键
	INPUT shiftKeyUp;
	ZeroMemory(&shiftKeyUp, sizeof(INPUT));
	shiftKeyUp.type = INPUT_KEYBOARD;
	shiftKeyUp.ki.wVk = nShift;
	shiftKeyUp.ki.dwFlags = KEYEVENTF_KEYUP;
	// 模拟释放M键
	INPUT mKeyUp;
	ZeroMemory(&mKeyUp, sizeof(INPUT));
	mKeyUp.type = INPUT_KEYBOARD;
	mKeyUp.ki.wVk = key;
	mKeyUp.ki.dwFlags = KEYEVENTF_KEYUP;

	// 将按键释放操作写入输入缓冲区
	INPUT keyReleaseEvents[3] = { ctrlKeyUp, shiftKeyUp, mKeyUp };
	SendInput(3, keyReleaseEvents, sizeof(INPUT));
}
void MainWindow::sendGlobalCtrlMKeyPressEvent(char key, int Type)
{

	// 模拟按下Ctrl键
	INPUT ctrlKeyDown;
	ZeroMemory(&ctrlKeyDown, sizeof(INPUT));
	ctrlKeyDown.type = INPUT_KEYBOARD;
	ctrlKeyDown.ki.wVk = Type;

	// 模拟按下M键
	INPUT mKeyDown;
	ZeroMemory(&mKeyDown, sizeof(INPUT));
	mKeyDown.type = INPUT_KEYBOARD;
	mKeyDown.ki.wVk = key;

	// 模拟释放Ctrl和M键
	INPUT keyRelease;
	ZeroMemory(&keyRelease, sizeof(INPUT));
	keyRelease.type = INPUT_KEYBOARD;
	keyRelease.ki.dwFlags = KEYEVENTF_KEYUP;

	// 将按键操作写入输入缓冲区
	INPUT inputEvents[3] = { ctrlKeyDown, mKeyDown, keyRelease };
	SendInput(3, inputEvents, sizeof(INPUT));

	// 可选：给操作系统一些时间来处理按键事件
	Sleep(100);

	// 模拟释放Ctrl键
	INPUT ctrlKeyUp;
	ZeroMemory(&ctrlKeyUp, sizeof(INPUT));
	ctrlKeyUp.type = INPUT_KEYBOARD;
	ctrlKeyUp.ki.wVk = Type;
	ctrlKeyUp.ki.dwFlags = KEYEVENTF_KEYUP;

	// 模拟释放M键
	INPUT mKeyUp;
	ZeroMemory(&mKeyUp, sizeof(INPUT));
	mKeyUp.type = INPUT_KEYBOARD;
	mKeyUp.ki.wVk = key;
	mKeyUp.ki.dwFlags = KEYEVENTF_KEYUP;

	// 将按键释放操作写入输入缓冲区
	INPUT keyReleaseEvents[2] = { ctrlKeyUp, mKeyUp };
	SendInput(2, keyReleaseEvents, sizeof(INPUT));
}

// 模拟全局按下Ctrl+F12组合键(上面的组合键不能实现f1-f12)
void MainWindow::simulateCtrlF12Keypress()
{
	INPUT input[4];
	ZeroMemory(input, sizeof(input));

	// 按下Ctrl键
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = VK_CONTROL;

	// 按下F12键
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = VK_F12;

	// 松开F12键
	input[2].type = INPUT_KEYBOARD;
	input[2].ki.wVk = VK_F12;
	input[2].ki.dwFlags = KEYEVENTF_KEYUP;

	// 松开Ctrl键
	input[3].type = INPUT_KEYBOARD;
	input[3].ki.wVk = VK_CONTROL;
	input[3].ki.dwFlags = KEYEVENTF_KEYUP;

	// 发送输入事件
	SendInput(4, input, sizeof(INPUT));
}

void MainWindow::CustomizeShortcuts(QString value) {
	QStringList list = value.split("+");
	if (list.size() == 2) {
		QString list2 = list.at(1); // 获取数组中索引为1的元素
		QStringList list3 = list2.split(",");
		QString list4 = list3.at(0);
		QByteArray byteArray = list4.toUtf8(); // 转换为 QByteArray
		const char* charArray = byteArray.data(); // 获取 char 数组指针
		char value1 = *charArray;
		qDebug() << charArray;
		QString sControlVal = list.at(0);//拿第一个值看看是VK_CONTROL还是VK_MENU
		if (sControlVal == "Alt")
		{
			sendGlobalCtrlMKeyPressEvent(value1, VK_MENU);// 0x12
			//sendGlobalCtrlMKeyPressEvent(value1, 18);
		}
		else
		{
			sendGlobalCtrlMKeyPressEvent(value1, VK_CONTROL);
		}

	}
	if (list.size() == 3) {
		QString list2 = list.at(2); // 获取数组中索引为2的元素
		QStringList list3 = list2.split(",");
		QString list4 = list3.at(0);
		QByteArray byteArray = list4.toUtf8(); // 转换为 QByteArray
		const char* charArray = byteArray.data(); // 获取 char 数组指针
		char value1 = *charArray;
		qDebug() << charArray;
		//sendGlobalCtrlMKeyPressEvent(value1, VK_CONTROL, VK_SHIFT);//CTRL+SHIFT+M
		//sendGlobalCtrlMKeyPressEvent(value1, VK_CONTROL, VK_MENU);//CTRL+ALT+M
		QString sControlVal = list.at(1);//拿第一个值看看是VK_CONTROL还是VK_MENU
		if (sControlVal == "Shift")
		{
			sendGlobalCtrlMKeyPressEvent(value1, VK_CONTROL, VK_SHIFT);//CTRL+SHIFT+M
		}
		else
		{
			sendGlobalCtrlMKeyPressEvent(value1, VK_CONTROL, VK_MENU);//CTRL+ALT+M
		}
	}
}


// 写txt文件
void MainWindow::writeLog(QString str)
{
	// 文件位置
	QFile file("log.txt");

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		return;
	}
	// 文件流
	QTextStream stream(&file);
	// 输入内容
	stream << str + "\n";
	file.close();
}

