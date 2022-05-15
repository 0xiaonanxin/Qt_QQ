#include "ForgetPass.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QModelIndex>
#include <QSqlQueryModel>

ForgetPass::ForgetPass(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle(QString::fromLocal8Bit("找回密码"), ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ForgetPass");

	//将查询密码输入框设置为只读
	ui.editPass->setReadOnly(true);
}

void ForgetPass::on_btnFind_clicked() {
	QString account = ui.editEmployaccount->text();
	QSqlQuery sqlemployeeID(QString("SELECT code from tab_accounts WHERE employeeID=%1")
		.arg(account));
	QSqlQuery sqlaccount(QString("SELECT code from tab_accounts WHERE account='%1'")
		.arg(account));

	sqlemployeeID.exec();
	sqlaccount.exec();
	QString code;
	if (sqlemployeeID.first()) {
		code = sqlemployeeID.value(0).toString();
	}
	else if (sqlaccount.first()) {
		code = sqlaccount.value(0).toString();
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("您输入的账号或QQ号不存在！"));
		ui.editEmployaccount->clear();
		ui.editDepartID->clear();
		ui.editPass->clear();
		ui.editEmployaccount->setFocus();
		return;
	}

	QString departID = ui.editDepartID->text();
	QSqlQueryModel sqldepartmentID;
	sqldepartmentID.setQuery((QString("SELECT employeeID,employee_name from tab_employees WHERE departmentID=%1")
		.arg(departID)));

	QModelIndex idIndex, nameIndex;
	QString strId, strName;
	for (int i = 0; i < sqldepartmentID.rowCount(); i++) {
		idIndex = sqldepartmentID.index(i, 0);
		nameIndex = sqldepartmentID.index(i, 1);
		strId = sqldepartmentID.data(idIndex).toString();
		strName = sqldepartmentID.data(nameIndex).toString();

		if (account == strId || account == strName) {
			ui.editPass->setText(code);
			return;
		}
	}

	QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("您输入的账号或QQ号在该群不存在！"));
	ui.editEmployaccount->clear();
	ui.editDepartID->clear();
	ui.editPass->clear();
	ui.editEmployaccount->setFocus();
}

ForgetPass::~ForgetPass()
{
}
