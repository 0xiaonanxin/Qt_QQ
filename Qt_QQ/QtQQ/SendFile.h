#pragma once

#include <QWidget>
#include "ui_SendFile.h"
#include "basicwindow.h"

class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = Q_NULLPTR);
	~SendFile();

signals://发送文件发射的信号
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	//打开文件按钮点击
	void on_openBtn_clicked();
	//发送文件按钮点击
	void on_sendBtn_clicked();

private:
	Ui::SendFile ui;
	//用户选择的文件路径
	QString m_filePath;
};
