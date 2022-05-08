#include "QtQQ_Server.h"
#include <QSqlDatabase>
#include <QMessageBox>

const int tcpPort = 8888;

QtQQ_Server::QtQQ_Server(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

	if (!connectMySql()) {
		QMessageBox::warning(NULL,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败！"));
		close();
		return;
	}

	m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	updateTableData();


	initTcpSocket();
}

void QtQQ_Server::initTcpSocket()
{
	m_tcpServer = new TcpServer(tcpPort);
	m_tcpServer->run();

	//收到tcp客户端发来的信息后进行udp广播
	connect(m_tcpServer, &TcpServer::signalTcpMsgComes,
		this, &QtQQ_Server::onUDPbroadMsg);
}

bool QtQQ_Server::connectMySql()
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

int QtQQ_Server::getCompDepID()
{
	return 0;
}

void QtQQ_Server::updateTableData(int depID = 0, int employeeID = 0)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID) {//不是公司群
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID));
	}
	else if (depID == m_compDepID) {//是公司群
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}
}

void QtQQ_Server::onUDPbroadMsg(QByteArray& btData) {

}
