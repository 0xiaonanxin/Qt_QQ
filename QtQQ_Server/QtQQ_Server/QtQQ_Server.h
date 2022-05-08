#pragma once

#include <QtWidgets/QDialog>
#include <QSqlQueryModel>
#include <QTimer>
#include "ui_QtQQ_Server.h"
#include "TcpServer.h"

class QtQQ_Server : public QDialog
{
    Q_OBJECT

public:
    QtQQ_Server(QWidget *parent = Q_NULLPTR);
	
private:
	void initComboBoxData();//��ʼ����Ͽ������
	void initTcpSocket();
	bool connectMySql();
	void setDepNameMap();
	void setStatusMap();
	void setOnlineMap();
	int getCompDepID();//��ȡ��˾ȺQQ��
	void updateTableData(int depID = 0, int employeeID = 0);

private slots:
	void onUDPbroadMsg(QByteArray& btData);
	void onRefresh();
	//����ȺQQ�Ų���Ա��������ź���ۺ����Զ�����
	void on_queryDepartmentBtn_clicked();
	//����Ա��QQ��ɸѡ
	void on_queryIDBtn_clicked();

private:
    Ui::QtQQ_ServerClass ui;

	QTimer* m_timer;//��ʱˢ������
	int m_compDepID;	//��˾ȺQQ��
	int m_depID;		//����QQ��
	int m_employeeID;	//Ա��QQ��
	QMap<QString, QString> m_statusMap;	//״̬
	QMap<QString, QString> m_depNameMap;//��������
	QMap<QString, QString> m_onlineMap;	//����״̬
	QSqlQueryModel m_queryInfoModel;//��ѯ����Ա������Ϣģ��

	TcpServer* m_tcpServer;
};
