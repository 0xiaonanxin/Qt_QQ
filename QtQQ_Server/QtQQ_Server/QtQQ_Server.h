#pragma once

#include <QtWidgets/QDialog>
#include <QSqlQueryModel>
#include "ui_QtQQ_Server.h"
#include "TcpServer.h"

class QtQQ_Server : public QDialog
{
    Q_OBJECT

public:
    QtQQ_Server(QWidget *parent = Q_NULLPTR);
	
private:
	void initTcpSocket();
	bool connectMySql();
	int getCompDepID();//获取公司群QQ号
	void updateTableData(int depID = 0, int employeeID = 0);

private slots:
	void onUDPbroadMsg(QByteArray& btData);

private:
    Ui::QtQQ_ServerClass ui;

	int m_compDepID;	//公司群QQ号
	QSqlQueryModel m_queryInfoModel;//查询所有员工的信息模型

	TcpServer* m_tcpServer;
};
