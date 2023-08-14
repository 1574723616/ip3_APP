#include "settingdialog.h"
#include "ui_mainwindow.h"

SettingDialog::SettingDialog(QWidget *parent) 
    : QMainWindow(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
