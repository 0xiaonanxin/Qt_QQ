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

private:
	Ui::UserLogin ui;
};
