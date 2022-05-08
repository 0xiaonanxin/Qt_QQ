#pragma once

#include <QWidget>
#include "ui_TalkWindowShell.h"
#include "basicwindow.h"
#include <QMap>
#include <QTcpSocket>
#include <QUdpSocket>

class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gtcpPort = 8888;

enum GroupType {
	COMPANY = 0,		//��˾Ⱥ
	PERSONALGROUP,		//���²�
	DEVELOPMENTGROUP,	//�з���
	MARKETGROUP,		//�г���
	PTOP				//ͬ��֮�䵥������
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//����µ����촰��
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString uid/*, GroupType grouptype*/);
	
	//���õ�ǰ���촰��
	void setCurrentWidget(QWidget* widget);

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

private:
	void initControl();		//��ʼ���ؼ�
	void initTcpSocket();	//��ʼ��TCP
	void initUdpSocket();	//��ʼ��UDP
	void getEmployeesID(QStringList& employeesList);	//��ȡ����Ա��QQ��
	bool createJSFile(QStringList& employeeList);

public slots:
	//���鰴ť�����ִ�еĲۺ���
	void onEmotionBtnClicked(bool);
	//�ͻ��˷���Tcp���ݣ����ݣ��������ͣ��ļ���
	void updateSendTcpMsg(QString& strData, int& msgType, QString filename = "");

private slots:
	//����б�����ִ�еĲۺ���
	void onTalkWindowItemClicked(QListWidgetItem* item);
	//���鱻ѡ��
	void onEmotionItemClicked(int emotionNum);
	void processPendingData();//����UDP�㲥�յ�������

private:
	Ui::TalkWindowClass ui;
	//�򿪵����촰��
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;
	//���鴰��
	EmotionWindow* m_emotionWindow;

private:
	QTcpSocket* m_tcpClientSocket;	//TCP�ͻ���
	QUdpSocket* m_udpReceiver;		//udp���ն�
};
