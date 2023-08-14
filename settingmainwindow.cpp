#include "settingmainwindow.h"
#include "ui_settingmainwindow.h"
#include <QDebug>
#include<QFileDialog>
#include<QPushButton>
#include<QPixmap>


QMap<QWidget*, QVector<QString>> g_scenMap;
settingMainWindow::settingMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settingMainWindow)
{
    ui->setupUi(this);
	ConfigSetting();
    init();
}
void settingMainWindow::SceneChanged(QPushButton * btnScene)
{
	connect(btnScene, &QPushButton::clicked, [=]() {
		if (btnScene->objectName() == "m_scene1")
			m_iScene = 1;
		else if (btnScene->objectName() == "m_scene2")
			m_iScene = 2;
		else if (btnScene->objectName() == "m_scene3")
			m_iScene = 3;
		else if (btnScene->objectName() == "m_scene4")
			m_iScene = 4;
		for (int i = 0; i < 9; i++) {
			initButton(i, m_iScene);
		}
		emit ButtonValueChange(m_iScene);
	});
}
class sceen_first;
void settingMainWindow::init()
{
	resize(1000, 500);
	SceneChanged(ui->m_scene1);
	SceneChanged(ui->m_scene2);
	SceneChanged(ui->m_scene3);
	SceneChanged(ui->m_scene4);
	for (int i = 0;i < 9;i++) {
		QPushButton* sceneAreaBtn = new QPushButton(this);
		sceneAreaBtn->setStyleSheet("");
		sceneAreaBtn->setStyleSheet("background:#6deb7c;border: 1px solid #ebebeb;border-radius: 5px;");
		QString objectName = QString("sceneAreaBtn_%1").arg(i);
		sceneAreaBtn->setObjectName(QString("sceneAreaBtn_%1").arg(i));
		sceneAreaBtn->setParent(this);
		sceneAreaBtn->setFixedSize(100, 100);
		sceneAreaBtn->move(110 + i % 3 * 100, 16 + i / 3 * 100);
		connect(sceneAreaBtn, &QPushButton::clicked, [=]() {
			QString imagePath = QFileDialog::getOpenFileName(this, "选择图片", QString(), "Image Files(*.png *.jpg *.bmp)");
			QPixmap pixmap(imagePath);
			if (!imagePath.isEmpty())sceneAreaBtn->setStyleSheet("QPushButton { border-image: url(" + imagePath + ");}");
			if (!imagePath.isEmpty())setting->setValue(QString("button/m_iScene%1_BtnImage_%2").arg(m_iScene).arg(i), imagePath);
			emit ButtonValueChange(m_iScene);
			});
		//initButton(i, m_iScene);
		//读文件给按钮文字赋值
		QString btnNam = QString("button/m_iScene%1_sceneAreaBtn_%2").arg(m_iScene).arg(i);
		QString name = setting->value(btnNam).toString();
		qDebug() << name;
		sceneAreaBtn->setText(name);
		//读文件给图片赋值 imagePath是图片的绝对路径
		btnNam = QString("button/m_iScene%1_BtnImage_%2").arg(m_iScene).arg(i);
		QString imagePath = setting->value(btnNam).toString();
		qDebug() << imagePath;
		if (!imagePath.isEmpty())sceneAreaBtn->setStyleSheet("QPushButton { border-image: url(" + imagePath + ");}");
	}

	lineVec.push_back(ui->lineEdit);
	lineVec.push_back(ui->lineEdit_2);
	lineVec.push_back(ui->lineEdit_3);
	lineVec.push_back(ui->lineEdit_4);
	lineVec.push_back(ui->lineEdit_5);
	lineVec.push_back(ui->lineEdit_6);
	lineVec.push_back(ui->lineEdit_7);
	lineVec.push_back(ui->lineEdit_8);
	lineVec.push_back(ui->lineEdit_9);

	for (int i = 0;i < lineVec.size();i++) {
		QString btnNam = QString("button/m_iScene%1_sceneAreaBtn_%2").arg(m_iScene).arg(i);
		QString name = setting->value(btnNam).toString();
		lineVec[i]->setText(name);
	}
		
	connect(ui->pushButton, &QPushButton::clicked, [&]() {
		for (int i = 0;i < 9;i++) {
			QString lineText = lineVec[i]->text();
			qDebug() << lineText;
			QPushButton* tmpBtn = this->findChild<QPushButton*>(QString("sceneAreaBtn_%1").arg(i));
			if (tmpBtn) tmpBtn->setText(lineText);
			setting->setValue(QString("button/m_iScene%1_sceneAreaBtn_%2").arg(m_iScene).arg(i), lineText);
			
		}
		emit ButtonValueChange(m_iScene);
		});
}


void settingMainWindow::ConfigSetting()
{
	//文件路径+文件名
	QString fileName = QCoreApplication::applicationDirPath() + "/Config.ini";
	//创建配置目标，输入文件路径，文件格式
	setting = new QSettings(fileName, QSettings::IniFormat);
	//设置文件编码，配置文件中使用中文时，这是必须的，否则乱码
	setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
}

settingMainWindow::~settingMainWindow()
{
    delete ui;
}
void settingMainWindow::WriteLine(QString sFilePath, QVector<QString> VecStr)
{

	QFile file("要写的文件路径");
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream stream(&file);
		for (QString write : VecStr)
		{
			stream.seek(file.size());
			stream << write << "\n";
		}
		
		file.close();
	}
}
void settingMainWindow::ReadLine(QString sFilePath, QVector<QString>& VecStr)
{

	QFile file(sFilePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!file.atEnd())
		{
			QByteArray line = file.readLine();
			QString str(line);
			VecStr.push_back(line);
			qDebug() << str;
		}
		file.close();

	}
}

void settingMainWindow::initButton( int iBtn, int iScene)
{
	//读文件给按钮文字赋值
	QString qs = QString("button/m_iScene%1_sceneAreaBtn_%2").arg(1).arg(iBtn);
	//(QString("sceneAreaBtn_%1").arg(i));
	QPushButton* button = this->findChild<QPushButton*>(QString("sceneAreaBtn_%1").arg(iBtn));
	QString btnNam = QString("button/m_iScene%1_sceneAreaBtn_%2").arg(iScene).arg(iBtn);
	QString name = setting->value(btnNam).toString();
	if (button) button->setText(name);

	//读文件给图片赋值 imagePath是图片的绝对路径
	btnNam = QString("button/m_iScene%1_BtnImage_%2").arg(iScene).arg(iBtn);
	QString imagePath = setting->value(btnNam).toString();
	qDebug() << imagePath;
	
	if (!imagePath.isEmpty())
		button->setStyleSheet("QPushButton { border-image: url(" + imagePath + ");}");
	else
		button->setStyleSheet("background:#6deb7c;border: 1px solid #ebebeb;border-radius: 5px;");


	for (int i = 0; i < lineVec.size(); i++) {
		QString btnNam = QString("button/m_iScene%1_sceneAreaBtn_%2").arg(m_iScene).arg(i);
		QString name = setting->value(btnNam).toString();
		lineVec[i]->setText(name);
	}
}