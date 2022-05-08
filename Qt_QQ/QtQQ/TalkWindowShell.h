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
	COMPANY = 0,		//公司群
	PERSONALGROUP,		//人事部
	DEVELOPMENTGROUP,	//研发部
	MARKETGROUP,		//市场部
	PTOP				//同事之间单独聊天
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//添加新的聊天窗口
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString uid/*, GroupType grouptype*/);
	
	//设置当前聊天窗口
	void setCurrentWidget(QWidget* widget);

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

private:
	void initControl();		//初始化控件
	void initTcpSocket();	//初始化TCP
	void initUdpSocket();	//初始化UDP
	void getEmployeesID(QStringList& employeesList);	//获取所有员工QQ号
	bool createJSFile(QStringList& employeeList);

public slots:
	//表情按钮点击后执行的槽函数
	void onEmotionBtnClicked(bool);
	//客户端发送Tcp数据（数据，数据类型，文件）
	void updateSendTcpMsg(QString& strData, int& msgType, QString filename = "");

private slots:
	//左侧列表点击后执行的槽函数
	void onTalkWindowItemClicked(QListWidgetItem* item);
	//表情被选中
	void onEmotionItemClicked(int emotionNum);
	void processPendingData();//处理UDP广播收到的数据

private:
	Ui::TalkWindowClass ui;
	//打开的聊天窗口
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;
	//表情窗口
	EmotionWindow* m_emotionWindow;

private:
	QTcpSocket* m_tcpClientSocket;	//TCP客户端
	QUdpSocket* m_udpReceiver;		//udp接收端
};
