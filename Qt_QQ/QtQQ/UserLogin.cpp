#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMovie>
#include <QAction>
#include <QPainter>
#include <qmath.h>
#include <qdrawutil.h>
#include <QSettings>
#include <QTimer>
#include "ForgetPass.h"
#include "Register.h"

QString gLoginEmployeeID;//登陆者QQ号（员工号）

#define ACCOUNT_KEY "account"
#define PASSWORD_KEY "password"

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	m_headLabel = new QLabel(this);
	m_headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	m_headLabel->setFixedSize(68, 68);

	m_timer = new QTimer;
	m_timer->setInterval(200);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &UserLogin::refreshHeadLabel);

	//设置窗体的属性
	//Qt::WA_DeleteOnClose 当该小组件接受了关闭事件（见QWidget::closeEvent()），使Qt删除该小组件。
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	//setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");

	setAttribute(Qt::WA_TranslucentBackground);
	//setWindowFlags(Qt::FramelessWindowHint);
	
	QSize size(434, 158);
	initbackground(size);

	initControl();

	ui.editUserAccount->setText(ReadInit(ACCOUNT_KEY));
	if (ReadInit(PASSWORD_KEY) != "") {
		ui.editPassword->setText(ReadInit(PASSWORD_KEY));
	}
}

UserLogin::~UserLogin()
{
}

void UserLogin::on_forgetWordbtn_clicked()
{
	ForgetPass* forgetPass = new ForgetPass;
	forgetPass->show();
}

void UserLogin::on_registBtn_clicked()
{
	Register* regist = new Register;
	regist->show();
}

void UserLogin::initControl() {
	updateHeadLabel(m_headLabel, ":/Resources/MainWindow/app/logo.ico");
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//在账号输入框左侧放置图标
	QAction* UserAction = new QAction(ui.editUserAccount);
	UserAction->setIcon(QIcon(":/Resources/qtqq_images/QQ.png"));
	ui.editUserAccount->addAction(UserAction, QLineEdit::LeadingPosition);//表示action所在方位（左侧）
	//在密码输入框左侧放置图标
	QAction* PassAction = new QAction(ui.editPassword);
	PassAction->setIcon(QIcon(":/Resources/qtqq_images/lock.png"));
	ui.editPassword->addAction(PassAction, QLineEdit::LeadingPosition);//表示action所在方位（左侧）
												 
	if (!connectMySql()) { //连接数据库失败
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败！"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");	//数据库名称
	db.setHostName("localhost");//主机名
	db.setUserName("root");		//用户名
	db.setPassword("123456");	//密码
	db.setPort(3306);			//端口号

	if (db.open()) {
		return true;
	}
	else {
		return false;
	}
}

bool UserLogin::veryfyAccountCode(bool &isAccountLogin, QString &strAccount)
{
	QString strAccountInput = ui.editUserAccount->text();
	QString strCodeInput = ui.editPassword->text();

	//输入员工号(QQ号登录)
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = '%1'").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();

	//指向结果集第一条
	if (queryEmployeeID.first()) {
		//数据库中qq号对应的密码
		QString strCode = queryEmployeeID.value(0).toString();

		if (strCode == strCodeInput) {
			gLoginEmployeeID = strAccountInput;
			
			isAccountLogin = false;
			strAccount = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}

	//账号登录
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'")
					.arg(strAccountInput);
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();
	if (queryAccount.first()) {
		QString strCode = queryAccount.value(0).toString();
	
		if (strCode == strCodeInput) {
			gLoginEmployeeID = queryAccount.value(1).toString();
			
			isAccountLogin = true;
			strAccount = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

void UserLogin::initbackground(const QSize& size)
{
	//加载动态图
	m_movie = new QMovie(":/Resources/qtqq_images/back.gif");

	//设置动态图大小
	m_movie->setScaledSize(size);

	//添加动态图
	ui.backLabel->setMovie(m_movie);

	//开始动画
	m_movie->start();
}

void UserLogin::WriteInit(QString key, QString value)
{
	//创建配置文件操作对象
	QSettings* setting = new QSettings(m_path, QSettings::IniFormat);

	//将信息写入配置文件
	setting->beginGroup("config");
	setting->setValue(key, value);
	setting->endGroup();
	delete setting;
}

QString UserLogin::ReadInit(QString key)
{
	//创建配置文件操作对象
	QSettings* setting = new QSettings(m_path, QSettings::IniFormat);

	//读取配置信息
	QString value = setting->value(QString("config/%1").arg(key)).toString();
	delete setting;

	return value;
}

void UserLogin::refreshHeadLabel()
{
	QString account = ui.editUserAccount->text();
	QSqlQuery sqlIDHeadLabel(QString("SELECT picture from tab_employees WHERE employeeID=%1")
										.arg(account));
	QSqlQuery sqlNameHeadLabel(QString("SELECT picture from tab_employees WHERE employee_name='%1'")
										.arg(account));

	sqlIDHeadLabel.exec();
	sqlNameHeadLabel.exec();
	QString headLabelPath;
	if (sqlIDHeadLabel.first()) {
		headLabelPath = sqlIDHeadLabel.value(0).toString();
		updateHeadLabel(m_headLabel, headLabelPath);
	}else if (sqlNameHeadLabel.first()) {
		headLabelPath = sqlNameHeadLabel.value(0).toString();
		updateHeadLabel(m_headLabel, headLabelPath);
	}
}

void UserLogin::updateHeadLabel(QLabel * headLabel, QString path)
{
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(path), pix, headLabel->size()));
}

void UserLogin::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event);
	//定义一个画家
	QPainter painter(this);
	QPixmap pixmap(":/Resources/qtqq_images/yy.png");
	qDrawBorderPixmap(&painter, this->rect(), QMargins(0, 0, 0, 0), pixmap);
	QRect rect(this->rect().x() + 8, this->rect().y() + 8, this->rect().width() - 16, this->rect().height() - 16);
	painter.fillRect(rect, QColor(255, 255, 255));
}

void UserLogin::onLoginBtnClicked() {
	bool isAccountLogin;
	QString strAccount;//账号或QQ号
	QString strPassword = ui.editPassword->text();

	if (!veryfyAccountCode(isAccountLogin,strAccount)) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("您输入的账号或密码有误，请重新输入！"));
		ui.editUserAccount->clear();
		ui.editPassword->clear();
		ui.editUserAccount->setFocus();

		return;
	}

	if (ui.checkBox->isChecked()) {
		WriteInit(PASSWORD_KEY, strPassword);
	}
	else {
		WriteInit(PASSWORD_KEY, "");
	}
	WriteInit(ACCOUNT_KEY, strAccount);

	//更新登录状态为已登录
	QString strSqlOnline = QString("UPDATE tab_employees SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlOnline(strSqlOnline);
	sqlOnline.exec();

	close();
	CCMainWindow* mainwindow = new CCMainWindow(strAccount, isAccountLogin);
	mainwindow->show();
}