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
	setTitleBarTitle(QString::fromLocal8Bit("ע��"), ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("Register");

	ui.employeeDepBox->installEventFilter(this);
}

Register::~Register()
{
}

//��������б���¼����ˣ���ֹ��������б�ʱ�����ƶ�
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
	//���Ա������������
	QString strName = ui.nameLineEdit->text();
	if (!strName.size()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���������������"));
		ui.nameLineEdit->setFocus();
		return;
	}

	//���Ա��ѡ��ͷ��
	if (!m_pixPath.size()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���������ͷ��·����"));
		return;
	}

	//���ݿ�����µ�Ա������
	//��ȡԱ��QQ��
	QSqlQuery maxEmployeeID("SELECT MAX(employeeID) FROM tab_employees");
	maxEmployeeID.exec();
	maxEmployeeID.first();

	int employeeID = maxEmployeeID.value(0).toInt() + 1;

	//Ա������QQ��
	int depID = ui.employeeDepBox->currentData().toInt();

	//ͼƬ·����ʽ����Ϊxxx\xxx\xxx.png��"/"�滻Ϊ"\"
	m_pixPath.replace("/", "\\\\");

	QSqlQuery insertSql(QString("INSERT INTO tab_employees(departmentID,employeeID,employee_name,picture) \
								VALUES('%1', '%2', '%3', '%4')")
		.arg(depID)
		.arg(employeeID)
		.arg(strName)
		.arg(m_pixPath));

	insertSql.exec();
	QMessageBox::information(this,
		QString::fromLocal8Bit("��ʾ"),
		QString::fromLocal8Bit("ע��ɹ���"));
	m_pixPath = "";
	ui.nameLineEdit->clear();
	ui.headLabel->setText(QString::fromLocal8Bit("   Ա������   "));
}

void Register::on_selectPictureBtn_clicked()
{
	//��ȡѡ���ͷ��·��
	m_pixPath = QFileDialog::getOpenFileName(
		this,
		QString::fromLocal8Bit("ѡ��ͷ��"),
		".",
		"*.png;;*.jpg"
	);

	if (!m_pixPath.size()) {
		return;
	}

	//��ͷ����ʾ����ǩ
	QPixmap pixmap;
	pixmap.load(m_pixPath);

	qreal widthRadio = (qreal)ui.headLabel->width() / (qreal)pixmap.width();
	qreal heightRadio = (qreal)ui.headLabel->height() / (qreal)pixmap.height();

	QSize size(pixmap.width() * widthRadio, pixmap.height() * heightRadio);
	ui.headLabel->setPixmap(pixmap.scaled(size));
}

