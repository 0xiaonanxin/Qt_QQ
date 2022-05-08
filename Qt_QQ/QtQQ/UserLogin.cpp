#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

QString gLoginEmployeeID;//��½��QQ�ţ�Ա���ţ�

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	//���ô��������
	//Qt::WA_DeleteOnClose ����С��������˹ر��¼�����QWidget::closeEvent()����ʹQtɾ����С�����
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initControl() {
	QLabel* headlabel = new QLabel(this);
	headlabel->setFixedSize(68, 68);
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));
	headlabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//�������ݿ�ʧ��
	if (!connectMySql()) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�������ݿ�ʧ�ܣ�"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");	//���ݿ�����
	db.setHostName("localhost");//������
	db.setUserName("root");		//�û���
	db.setPassword("123456");	//����
	db.setPort(3306);			//�˿ں�

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

	//����Ա����(QQ�ŵ�¼)
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = '%1'").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();

	//ָ��������һ��
	if (queryEmployeeID.first()) {
		//���ݿ���qq�Ŷ�Ӧ������
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

	//�˺ŵ�¼
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

void UserLogin::onLoginBtnClicked() {
	bool isAccountLogin;
	QString strAccount;//�˺Ż�QQ��

	if (!veryfyAccountCode(isAccountLogin,strAccount)) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��������˺Ż������������������룡"));
		ui.editUserAccount->clear();
		ui.editPassword->clear();
		ui.editUserAccount->setFocus();

		return;
	}

	close();
	CCMainWindow* mainwindow = new CCMainWindow(strAccount, isAccountLogin);
	mainwindow->show();
}