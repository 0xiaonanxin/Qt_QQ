#pragma once

#include "basicwindow.h"
#include "ui_ForgetPass.h"

class ForgetPass : public BasicWindow
{
	Q_OBJECT

public:
	ForgetPass(QWidget *parent = Q_NULLPTR);
	~ForgetPass();

private slots:
	void on_btnFind_clicked();

private:
	Ui::ForgetPass ui;
};
