#pragma once

#include "BasicWindow.h"
#include "ui_UserLogin.h"

class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();
private slots:
	void onLoginBtnClicked();

private:
	void initControl();
	//判断是否连接成功
	bool connectMySql();
	//验证账号密码
	bool veryfyAccountCode(bool &isAccountLogin,QString &strAccount);
	//设置登录界面动态背景
	void initbackground(const QSize& size);

	void paintEvent(QPaintEvent* event);

	//写入配置文件
	void WriteInit(QString key, QString value);
	//读取配置文件
	QString ReadInit(QString key);

	//定时检测账号输入框内的账号在数据库中是否能找到对应的头像路径
	void refreshHeadLabel();
	void updateHeadLabel(QLabel* headLabel, QString path);

private:
	Ui::UserLogin ui;
	QTimer* m_timer;
	QMovie* m_movie;
	QString m_path = "./config.ini";
	QLabel* m_headLabel;
};
