#pragma once

#include <QWidget>
#include "ui_ReceiveFile.h"
#include "basicwindow.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = Q_NULLPTR);
	~ReceiveFile();
	void setMsg(QString& msgLabel);

signals:
	void refuseFile();

private slots:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();

private:
	Ui::ReceiveFile ui;
};
