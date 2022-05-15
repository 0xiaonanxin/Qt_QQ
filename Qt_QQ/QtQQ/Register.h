#pragma once

#include <QWidget>
#include "ui_Register.h"
#include "basicwindow.h"

class Register : public BasicWindow
{
	Q_OBJECT

public:
	Register(QWidget *parent = Q_NULLPTR);
	~Register();

private:
	bool eventFilter(QObject* obj, QEvent* event);

private slots:
	void on_addBtn_clicked();
	void on_selectPictureBtn_clicked();

private:
	Ui::Register ui;
	QString m_pixPath;	//Í·ÏñÂ·¾¶
};
