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

signals://�����ļ�������ź�
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	//���ļ���ť���
	void on_openBtn_clicked();
	//�����ļ���ť���
	void on_sendBtn_clicked();

private:
	Ui::SendFile ui;
	//�û�ѡ����ļ�·��
	QString m_filePath;
};
