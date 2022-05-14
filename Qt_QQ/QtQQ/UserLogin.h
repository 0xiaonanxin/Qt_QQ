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
	//�ж��Ƿ����ӳɹ�
	bool connectMySql();
	//��֤�˺�����
	bool veryfyAccountCode(bool &isAccountLogin,QString &strAccount);
	//���õ�¼���涯̬����
	void initbackground(const QSize& size);

	void paintEvent(QPaintEvent* event);

	//д�������ļ�
	void WriteInit(QString key, QString value);
	//��ȡ�����ļ�
	QString ReadInit(QString key);

	//��ʱ����˺�������ڵ��˺������ݿ����Ƿ����ҵ���Ӧ��ͷ��·��
	void refreshHeadLabel();
	void updateHeadLabel(QLabel* headLabel, QString path);

private:
	Ui::UserLogin ui;
	QTimer* m_timer;
	QMovie* m_movie;
	QString m_path = "./config.ini";
	QLabel* m_headLabel;
};
