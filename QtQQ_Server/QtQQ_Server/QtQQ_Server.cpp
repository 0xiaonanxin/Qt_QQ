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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�������ݿ�ʧ�ܣ�"));
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

	//�յ�tcp�ͻ��˷�������Ϣ�����udp�㲥
	connect(m_tcpServer, &TcpServer::signalTcpMsgComes,
		this, &QtQQ_Server::onUDPbroadMsg);
}

bool QtQQ_Server::connectMySql()
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

int QtQQ_Server::getCompDepID()
{
	return 0;
}

void QtQQ_Server::updateTableData(int depID = 0, int employeeID = 0)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID) {//���ǹ�˾Ⱥ
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID));
	}
	else if (depID == m_compDepID) {//�ǹ�˾Ⱥ
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}
}

void QtQQ_Server::onUDPbroadMsg(QByteArray& btData) {

}
