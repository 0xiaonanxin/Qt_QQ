#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include <QListWidget>
#include <QPoint>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QUdpSocket>
#include <QSqlQuery>
#include "WindowManager.h"
#include "TalkWindowItem.h"
#include "ReceiveFile.h"

QString gFileName;	//�ļ�����
QString gFileData;	//�ļ�����
extern QString gLoginEmployeeID;
const int gUdpPort = 6666;

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();

	QFile file("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size()) {
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);
		if (!createJSFile(employeesIDList)) {
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("����js�ļ�����ʧ�ܣ�"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow * talkWindow, TalkWindowItem * talkWindowItem,const QString uid/*, GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);

	//�ж���Ⱥ�Ļ��ǵ���
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1")
									.arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();

	if (rows == 0)//����
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
								.arg(uid);
		sqlDepModel.setQuery(strQuery);
	}

	QModelIndex index;
	index = sqlDepModel.index(0, 0);//0��0��

	QImage img;
	img.load(sqlDepModel.data(index).toString());

	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem,&TalkWindowItem::signalCloseClicked,
		[talkWindowItem, talkWindow, aItem, this]() {
		m_talkwindowItemMap.remove(aItem);
		talkWindow->close();
		ui.listWidget->takeItem(ui.listWidget->row(aItem));
		delete talkWindowItem;
		ui.rightStackedWidget->removeWidget(talkWindow);
		if (ui.rightStackedWidget->count() < 1)
			close();
	});
}

void TalkWindowShell::setCurrentWidget(QWidget * widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkwindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("����-���촰��"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();	//���ر��鴰��

	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);	//���������óߴ�

	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));

}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 port = gUdpPort; port < gUdpPort + 200; ++port) {
		if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress))
			break;
	}

	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getEmployeesID(QStringList& employeesList)
{
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	
	//����ģ�͵�������(Ա��������)
	int employeesNum = queryModel.rowCount();
	QModelIndex index;
	for (int i = 0; i < employeesNum; i++) {
		index = queryModel.index(i, 0);//�У���
		employeesList << queryModel.data(index).toString();
	}
}

bool TalkWindowShell::createJSFile(QStringList & employeeList)
{
	//��ȡtxt�ļ�����
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;

	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��ȡ msgtmpl.txt ʧ�ܣ�"));
		return false;
	}

	//�滻��external0��appendHtml0�����Լ�����Ϣʹ�ã�
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		//���¿�ֵ
		QString strSourceInitNull = "var external = null;";
		
		//���³�ʼ��
		QString strSourceInit = "external = channel.objects.external;";
		
		//����newWebChannel
		QString strSourceNew =
			"new QWebChannel(qt.webChannelTransport,\
				function(channel) {\
					external = channel.objects.external;\
				}\
			 );\
			";
		//����׷��recvHtml���ű�����˫�����޷�ֱ�ӽ��и�ֵ�����ö��ļ���ʽ
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("��ȡ recvHtml.txt ʧ�ܣ�"));
			return false;
		}

		//�����滻��Ľű�
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < employeeList.length(); i++) {
			//�༭�滻��Ŀ�ֵ
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//�༭�滻��ĳ�ʼֵ
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//�༭�滻���newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			//�༭�滻��� recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeeList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
		}

		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return true;
	}
	else {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("д msgtmpl.js ʧ�ܣ�"));
		return false;
	}
}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);

	if (msgType == 1) {//�ı���Ϣ
		msgTextEdit.document()->toHtml();
	}
	else if(msgType == 0)//������Ϣ
	{
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;

		for (int i = 0; i < emotionNum; i++) {
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	QString html = msgTextEdit.document()->toHtml();

	//�ı�html���û���������������
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ")
				, QString::fromLocal8Bit("�ļ�msgFont.txt�����ڣ�"));
			return;
		}

		if (!html.contains(fontHtml)) {
			html.replace(strMsg, fontHtml);
		}
	}

	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderEmployeeID));
}

//�ı����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ���� + ���ݳ��� + ����
//�������ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ���� + ������� + images + ����
//msgType 0������Ϣ 1�ı���Ϣ 2�ļ���Ϣ
void TalkWindowShell::updateSendTcpMsg(QString & strData, int & msgType, QString filename)
{
	//��ȡ��ǰ����촰��
	TalkWindow* curTalkWindnow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindnow->getTalkId();

	QString strGroupFlag;
	QString strSend;
	if (talkId.length() == 4) {//ȺQQ�ĳ���
		strGroupFlag = "1";
	}
	else {
		strGroupFlag = "0";
	}

	int nstrDataLength = strData.length();
	int dataLength = QString::number(nstrDataLength).length();
	QString strdataLength;

	if (msgType == 1) {//�����ı���Ϣ
		//�ı���Ϣ�ĳ���Լ��Ϊ5λ
		if (dataLength == 1) {
			strdataLength = "0000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 2) {
			strdataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3) {
			strdataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4) {
			strdataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5) {
			strdataLength = QString::number(nstrDataLength);
		}
		else {
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("����������ݳ��ȣ�"));
		}

		//�ı����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ���� + ���ݳ��� + ����
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0) {//������Ϣ
		//�������ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ���� + ������� + images + ����
		strSend = strGroupFlag + gLoginEmployeeID + talkId
			+ "0" + strData;
	}
	else if (msgType == 2) {//�ļ���Ϣ
		//�ļ����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ��(ȺQQ��) 
		//+ ��Ϣ����(2) + �ļ����� + "bytes" + �ļ����� + "data_begin" + �ļ�����
		
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());

		strSend = strGroupFlag + gLoginEmployeeID + talkId
			+ "2" + strLength + "bytes" + filename + "data_begin" + strData;
	}

	QByteArray dataBt;
	dataBt.resize(strSend.length());
	dataBt = strSend.toUtf8();
	m_tcpClientSocket->write(dataBt);
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//����ǰ�ؼ������λ��ת��Ϊ��Ļ�ľ���λ��

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item) {
	QWidget* talkwindowWidget = m_talkwindowItemMap.find(item).value();
	ui.rightStackedWidget->setCurrentWidget(talkwindowWidget);
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum) {
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (curTalkWindow) {
		curTalkWindow->addEmotionImage(emotionNum);
	}
}

/********************************************************************************************************************************************
	���ݰ���ʽ��
	�ı����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ� + ��Ϣ���ͣ�1��+ ���ݳ��� + ����
	�������ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ� + ��Ϣ���ͣ�0��+ ������� + image + ��������
	�ļ����ݰ���ʽ��Ⱥ�ı�־ + ����ϢԱ��QQ�� + ����ϢԱ��QQ�ţ�ȺQQ�ţ� + ��Ϣ���ͣ�2��+ �ļ��ֽ��� + bytes + �ļ��� + data_begin + �ļ�����

	Ⱥ�ı�־ռ1λ��0��ʾ���ģ�1��ʾȺ��
	��Ϣ����ռ1λ��0��ʾ������Ϣ��1��ʾ�ı���Ϣ��2��ʾ�ļ���Ϣ

	QQ��ռ5λ��ȺQQ��ռ4λ�����ݳ���ռ5λ����������ռ3λ
	��ע�⣺��Ⱥ�ı�־Ϊ1ʱ�������ݰ�û������ϢԱ��QQ�ţ���������ϢȺQQ��
			��Ⱥ�ı�־Ϊ0ʱ�������ݰ�û������ϢȺQQ�ţ���������ϢԱ��QQ�ţ�

	Ⱥ���ı���Ϣ�磺1100012001100005Hello   ��ʾQQ10001��Ⱥ2001�����ı���Ϣ��������5������ΪHello
	����ͼƬ��Ϣ�磺0100011000201images060	��ʾQQ10001��QQ10002����1������60.png
	Ⱥ���ļ���Ϣ�磺11000520002bytestest.txtdata_beginhelloworld
									        ��ʾQQ10005��Ⱥ2000�����ļ���Ϣ���ļ���text.txt���ļ����ݳ���10��������helloworld
	
	Ⱥ���ļ���Ϣ������1 10001 2001 1 00005 Hello
	����ͼƬ��Ϣ������0 10001 10002 0 1 images 060
	Ⱥ���ļ���Ϣ������1 10005 2000 2 10 bytes test.txt data_begin helloworld
********************************************************************************************************************************************/
void TalkWindowShell::processPendingData()
{
	//�ж϶˿����Ƿ���δ���������
	while (m_udpReceiver->hasPendingDatagrams()) {
		const static int groupFlagWidth = 1;	//Ⱥ�ı�־ռλ
		const static int groupWidth = 4;		//ȺQQ�ſ��
		const static int employeeWidth = 5;		//Ա��QQ�ſ��
		const static int msgTypeWidth = 1;		//��Ϣ���Ϳ��
		const static int msgLengthWidth = 5;	//�ı���Ϣ����
		const static int pictureWidth = 3;		//����ͼƬ�Ŀ��

		//��ȡudp����
		QByteArray btData;
		//pendingDatagramSize ���ص�һ���������UDP���ݱ��Ĵ�С
		btData.resize(m_udpReceiver->pendingDatagramSize());
		//��ȡ���ݰ�����btData
		m_udpReceiver->readDatagram(btData.data(), btData.size());

		QString strData = btData.data();
		QString strWindowID;//���촰��ID��Ⱥ������Ⱥ�ţ���������Ա��QQ��
		QString strSendEmployeeID, strReceiveEmployeeID;//���ͼ����ն˵�QQ��
		QString strMsg;	//����
		
		int msgLen;	//���ݳ���
		int MsgType;//��������

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);

		//�Լ�������Ϣ��������
		if (strSendEmployeeID == gLoginEmployeeID) {
			return;
		}

		if (btData[0] == '1') {//Ⱥ��
			//ȺQQ��
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);
			
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1') {//�ı���Ϣ
				MsgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth
					+ groupWidth + msgTypeWidth, msgLengthWidth).toInt();
				strMsg = strData.mid(groupFlagWidth + employeeWidth
					+ groupWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cMsgType == '0') {//������Ϣ
				MsgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.right(strData.length() - posImages - QString("images").length());
			}
			else if (cMsgType == '2') {//�ļ���Ϣ
				MsgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");

				//�ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gFileName = fileName;

				//�ļ�����
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gFileData = strMsg;

				//����employeeID��ȡ����������
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
												.arg(employeeID));
				querySenderName.exec();

				if (querySenderName.first()) {
					sender = querySenderName.value(0).toString();
				}

				//�����ļ��ĺ�������������
				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ���ܣ�");
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		else {//����
			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
			strWindowID = strSendEmployeeID;

			//���Ƿ����ҵ���Ϣ��������
			if (strReceiveEmployeeID != gLoginEmployeeID) {
				return;
			}

			//��ȡ��Ϣ������
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1') {//�ı���Ϣ
				MsgType = 1;

				//�ı���Ϣ�ĳ���
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth, msgLengthWidth).toInt();

				//�ı���Ϣ
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cMsgType == '0') {//������Ϣ
				MsgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);
			}
			else if (cMsgType == '2') {//�ļ���Ϣ
				MsgType = 2;

				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				//�ļ�����
				QString filename = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gFileName = filename;

				//�ļ�����
				strMsg = strData.mid(posData_begin + data_beginWidth);
				gFileData = strMsg;

				//����employeeID��ȡ����������
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1")
					.arg(employeeID));
				querySenderName.exec();

				if (querySenderName.first()) {
					sender = querySenderName.value(0).toString();
				}

				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
				});
				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ���ܣ�");
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}

		//�����촰����Ϊ���
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget) {//���촰�ڴ���
			this->setCurrentWidget(widget);

			//ͬ��������������б�
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(true);
		}
		else {//���촰�ڲ�����
			return;
		}

		//�ļ���Ϣ��������
		if (MsgType != 2) {
			int sendEmployeeID = strSendEmployeeID.toInt();
			handleReceivedMsg(sendEmployeeID, MsgType, strMsg);
		}
	}
}
