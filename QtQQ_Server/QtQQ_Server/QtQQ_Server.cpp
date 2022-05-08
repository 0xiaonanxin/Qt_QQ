#include "QtQQ_Server.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlRecord>
#include <QAbstractItemView>
#include <QSqlQuery>

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

	setDepNameMap();
	setStatusMap();
	setOnlineMap();
	initComboBoxData();

	m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ֻ��
	
	//��ʼ����ѯ��˾Ⱥ����Ա����Ϣ
	m_employeeID = 0;
	m_depID = getCompDepID();
	m_compDepID = m_depID;

	updateTableData();

	//��ʱˢ������
	m_timer = new QTimer(this);
	m_timer->setInterval(200);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &QtQQ_Server::onRefresh);

	initTcpSocket();
}

void QtQQ_Server::initComboBoxData()
{
	QString itemText;//��Ͽ�����ı�

	//��ȡ��˾�ܵĲ�����
	QSqlQueryModel queryDepModel;
	queryDepModel.setQuery("SELECT * FROM tab_department");
	int depCounts = queryDepModel.rowCount() - 1;//��������Ӧ��ȥ��˾Ⱥ

	for (int i = 0; i < depCounts; i++) {
		itemText = ui.employeeDepBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//����Ա������������Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.employeeDepBox->setItemData(i, queryDepID.value(0).toInt());
	}

	//��һ����˾Ⱥ����
	for (int i = 0; i < depCounts + 1; i++) {
		itemText = ui.departmentBox->itemText(i);

		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//���ò�����Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.departmentBox->setItemData(i, queryDepID.value(0).toInt());
	}

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

void QtQQ_Server::setDepNameMap()
{
	m_depNameMap.insert(QStringLiteral("2001"), QStringLiteral("����Ⱥ"));
	m_depNameMap.insert(QStringLiteral("2002"), QStringLiteral("�з�Ⱥ"));
	m_depNameMap.insert(QStringLiteral("2003"), QStringLiteral("�г�Ⱥ"));
}

void QtQQ_Server::setStatusMap()
{
	m_statusMap.insert(QStringLiteral("1"), QStringLiteral("��Ч"));
	m_statusMap.insert(QStringLiteral("0"), QStringLiteral("��ע��"));
}

void QtQQ_Server::setOnlineMap()
{
	m_onlineMap.insert(QStringLiteral("1"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("2"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("3"), QStringLiteral("����"));
}

int QtQQ_Server::getCompDepID()
{
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryCompDepID.exec();
	queryCompDepID.first();

	int compDepID = queryCompDepID.value(0).toInt();
	return compDepID;
}

void QtQQ_Server::updateTableData(int depID, int employeeID)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID) {//��ѯ����
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID));
	}
	else if (employeeID) {//��ȷ����
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	}
	else {//��ѯ����
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}

	int rows = m_queryInfoModel.rowCount();//���������ܼ�¼����
	int columns = m_queryInfoModel.columnCount();//�����������ֶ�����
	
	QModelIndex index;//ģ������
	
	//���ñ�������������
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(columns);

	//���ñ�ͷ
	QStringList headers;
	headers << QStringLiteral("����")
			<< QStringLiteral("����")
			<< QStringLiteral("Ա������")
			<< QStringLiteral("Ա��ǩ��")
			<< QStringLiteral("Ա��״̬")
			<< QStringLiteral("Ա��ͷ��")
			<< QStringLiteral("����״̬");
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	//�����еĿ�
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			index = m_queryInfoModel.index(i, j);//�С���
			QString strData = m_queryInfoModel.data(index).toString();//��ȡi��j�е�����

			//��ȡ�ֶ�����
			QSqlRecord record = m_queryInfoModel.record(i);//��ǰ�еļ�¼
			QString strRecordName = record.fieldName(j);//��

			if (strRecordName == QLatin1String("departmentID")) {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_depNameMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("status")) {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_statusMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("online")) {
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_onlineMap.value(strData)));
				continue;
			}

			ui.tableWidget->setItem(i, j, new QTableWidgetItem(strData));
		}
	}
}

void QtQQ_Server::onRefresh()
{
	updateTableData(m_depID, m_employeeID);
}

void QtQQ_Server::on_queryDepartmentBtn_clicked()
{
	m_employeeID = 0;
	m_depID = ui.departmentBox->currentData().toInt();
}

void QtQQ_Server::on_queryIDBtn_clicked()
{
	m_depID = m_compDepID;

	//���Ա��QQ���Ƿ�����
	if(!ui.queryIDLineEdit->text().length()){
		QMessageBox::information(this, 
			QString::fromLocal8Bit("��ʾ"), 
			QString::fromLocal8Bit("������Ա��QQ�ţ�"));
		ui.queryIDLineEdit->setFocus();
		return;
	}

	//��ȡ�û������Ա��QQ��
	int employeeID = ui.queryIDLineEdit->text().toInt();

	QSqlQuery queryInfo(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	queryInfo.exec();
	if (!queryInfo.first()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��������ȷ��Ա��QQ�ţ�"));
		ui.queryIDLineEdit->clear();
		ui.queryIDLineEdit->setFocus();
		return;
	}
	else
	{
		m_employeeID = employeeID;
	}
}

void QtQQ_Server::onUDPbroadMsg(QByteArray& btData) {

}
