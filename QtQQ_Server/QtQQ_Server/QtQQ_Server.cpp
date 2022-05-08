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
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败！"));
		close();
		return;
	}

	setDepNameMap();
	setStatusMap();
	setOnlineMap();
	initComboBoxData();

	m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//表格只读
	
	//初始化查询公司群所有员工信息
	m_employeeID = 0;
	m_depID = getCompDepID();
	m_compDepID = m_depID;

	updateTableData();

	//定时刷新数据
	m_timer = new QTimer(this);
	m_timer->setInterval(200);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &QtQQ_Server::onRefresh);

	initTcpSocket();
}

void QtQQ_Server::initComboBoxData()
{
	QString itemText;//组合框项的文本

	//获取公司总的部门数
	QSqlQueryModel queryDepModel;
	queryDepModel.setQuery("SELECT * FROM tab_department");
	int depCounts = queryDepModel.rowCount() - 1;//部门总数应减去公司群

	for (int i = 0; i < depCounts; i++) {
		itemText = ui.employeeDepBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//设置员工所属部门组合框的数据为相应的部门QQ号
		ui.employeeDepBox->setItemData(i, queryDepID.value(0).toInt());
	}

	//多一个公司群部门
	for (int i = 0; i < depCounts + 1; i++) {
		itemText = ui.departmentBox->itemText(i);

		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//设置部门组合框的数据为相应的部门QQ号
		ui.departmentBox->setItemData(i, queryDepID.value(0).toInt());
	}

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

void QtQQ_Server::setDepNameMap()
{
	m_depNameMap.insert(QStringLiteral("2001"), QStringLiteral("人事群"));
	m_depNameMap.insert(QStringLiteral("2002"), QStringLiteral("研发群"));
	m_depNameMap.insert(QStringLiteral("2003"), QStringLiteral("市场群"));
}

void QtQQ_Server::setStatusMap()
{
	m_statusMap.insert(QStringLiteral("1"), QStringLiteral("有效"));
	m_statusMap.insert(QStringLiteral("0"), QStringLiteral("已注销"));
}

void QtQQ_Server::setOnlineMap()
{
	m_onlineMap.insert(QStringLiteral("1"), QStringLiteral("离线"));
	m_onlineMap.insert(QStringLiteral("2"), QStringLiteral("在线"));
	m_onlineMap.insert(QStringLiteral("3"), QStringLiteral("隐身"));
}

int QtQQ_Server::getCompDepID()
{
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("公司群")));
	queryCompDepID.exec();
	queryCompDepID.first();

	int compDepID = queryCompDepID.value(0).toInt();
	return compDepID;
}

void QtQQ_Server::updateTableData(int depID, int employeeID)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID) {//查询部门
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID));
	}
	else if (employeeID) {//精确查找
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	}
	else {//查询所有
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}

	int rows = m_queryInfoModel.rowCount();//总行数（总记录数）
	int columns = m_queryInfoModel.columnCount();//总列数（总字段数）
	
	QModelIndex index;//模型索引
	
	//设置表格的行数、列数
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(columns);

	//设置表头
	QStringList headers;
	headers << QStringLiteral("部门")
			<< QStringLiteral("工号")
			<< QStringLiteral("员工姓名")
			<< QStringLiteral("员工签名")
			<< QStringLiteral("员工状态")
			<< QStringLiteral("员工头像")
			<< QStringLiteral("在线状态");
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	//设置列的宽
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			index = m_queryInfoModel.index(i, j);//行、列
			QString strData = m_queryInfoModel.data(index).toString();//获取i行j列的数据

			//获取字段名称
			QSqlRecord record = m_queryInfoModel.record(i);//当前行的记录
			QString strRecordName = record.fieldName(j);//列

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

	//检测员工QQ号是否输入
	if(!ui.queryIDLineEdit->text().length()){
		QMessageBox::information(this, 
			QString::fromLocal8Bit("提示"), 
			QString::fromLocal8Bit("请输入员工QQ号！"));
		ui.queryIDLineEdit->setFocus();
		return;
	}

	//获取用户输入的员工QQ号
	int employeeID = ui.queryIDLineEdit->text().toInt();

	QSqlQuery queryInfo(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	queryInfo.exec();
	if (!queryInfo.first()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请输入正确的员工QQ号！"));
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
