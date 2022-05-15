#include "Register.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>

Register::Register(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle(QString::fromLocal8Bit("注册"), ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("Register");

	ui.employeeDepBox->installEventFilter(this);
}

Register::~Register()
{
}

//添加下拉列表的事件过滤，防止点击下拉列表时窗口移动
bool Register::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == ui.employeeDepBox) {
		if (event->type() == QEvent::MouseMove) {
			return true;
		}
	}

	return BasicWindow::eventFilter(obj, event);
}

void Register::on_addBtn_clicked()
{
	//检测员工姓名的输入
	QString strName = ui.nameLineEdit->text();
	if (!strName.size()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请输入你的姓名！"));
		ui.nameLineEdit->setFocus();
		return;
	}

	//检测员工选择头像
	if (!m_pixPath.size()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请输入你的头像路径！"));
		return;
	}

	//数据库插入新的员工数据
	//获取员工QQ号
	QSqlQuery maxEmployeeID("SELECT MAX(employeeID) FROM tab_employees");
	maxEmployeeID.exec();
	maxEmployeeID.first();

	int employeeID = maxEmployeeID.value(0).toInt() + 1;

	//员工部门QQ号
	int depID = ui.employeeDepBox->currentData().toInt();

	//图片路径格式设置为xxx\xxx\xxx.png，"/"替换为"\"
	m_pixPath.replace("/", "\\\\");

	QSqlQuery insertSql(QString("INSERT INTO tab_employees(departmentID,employeeID,employee_name,picture) \
								VALUES('%1', '%2', '%3', '%4')")
		.arg(depID)
		.arg(employeeID)
		.arg(strName)
		.arg(m_pixPath));

	insertSql.exec();
	QMessageBox::information(this,
		QString::fromLocal8Bit("提示"),
		QString::fromLocal8Bit("注册成功！"));
	m_pixPath = "";
	ui.nameLineEdit->clear();
	ui.headLabel->setText(QString::fromLocal8Bit("   员工寸照   "));
}

void Register::on_selectPictureBtn_clicked()
{
	//获取选择的头像路径
	m_pixPath = QFileDialog::getOpenFileName(
		this,
		QString::fromLocal8Bit("选择头像"),
		".",
		"*.png;;*.jpg"
	);

	if (!m_pixPath.size()) {
		return;
	}

	//将头像显示到标签
	QPixmap pixmap;
	pixmap.load(m_pixPath);

	qreal widthRadio = (qreal)ui.headLabel->width() / (qreal)pixmap.width();
	qreal heightRadio = (qreal)ui.headLabel->height() / (qreal)pixmap.height();

	QSize size(pixmap.width() * widthRadio, pixmap.height() * heightRadio);
	ui.headLabel->setPixmap(pixmap.scaled(size));
}

