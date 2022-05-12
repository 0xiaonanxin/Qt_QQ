#include "basicwindow.h"
#include "CommonUtils.h"
#include "NotifyManager.h"
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QSqlQuery>

extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent)
{
	//窗口背景颜色初始化，颜色从配置文件里读取
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	//设置窗口风格
	//FramelessWindowHint 产生一个无边界的窗口。用户不能通过窗口系统移动或调整无边界窗口的大小
	setWindowFlag(Qt::FramelessWindowHint);

	//设置该小组件的attribute属性
	/*表示小部件应该有一个半透明的背景，也就是说，小部件的任何非不透明区域将是半透明的，
	因为小部件将有一个alpha通道。设置这个标志会导致WA_NoSystemBackground被设置。在Windows上，
	该小组件还需要设置Qt::FramelessWindowHint窗口标志。这个标志是由小组件的作者设置或清除的。*/
	//setAttribute(Qt::WA_TranslucentBackground,true);

	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));

}

BasicWindow::~BasicWindow()
{
}

//窗体皮肤改变
void BasicWindow::onSignalSkinChanged(const QColor & color)
{
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}

//设置标题栏标题
void BasicWindow::setTitleBarTitle(const QString & title, const QString & icon)
{
	_titleBar->setTitleIcon(icon);
	_titleBar->setTitleContent(title);
}

//初始化标题栏
void BasicWindow::initTitleBar(ButtonType buttontype)
{
	_titleBar = new TitleBar(this);
	_titleBar->setButtonType(buttontype);
	//将_titleBar移动到(0,0)
	_titleBar->move(0, 0);

	//连接信号与槽
	connect(_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

//加载样式表
void BasicWindow::loadStyleSheet(const QString & sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());

		//获取用户当前的皮肤RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		qsstyleSheet += QString("QWidget[titleskin = true]\
								{background-color:rgb(%1,%2,%3);\
								border-top-left-radius:4px;}\
								QWidget[bottomskin = true]\
								{border-top:1px solid rgba(%1,%2,%3,100);\
								background-color:rgba(%1,%2,%3,50);\
								border-bottom-left-radius:4px;\
								border-bottom-right-radius:4px;}")
								.arg(r).arg(g).arg(b);
		//设置样式
		setStyleSheet(qsstyleSheet);
	}

	file.close();
}

//初始化窗口背景
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//子类化部件时，需要重写绘图事件设置背景
void BasicWindow::paintEvent(QPaintEvent *event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}

//头像转圆头像
QPixmap BasicWindow::getRoundImage(const QPixmap & src, QPixmap & mask, QSize masksize)
{
	if (masksize == QSize(0, 0)) {
		masksize = mask.size();
	}
	else 
	{
		//对图像进行缩放
		//KeepAspectRatio 尺寸被缩放为在给定的矩形内尽可能大的矩形，保留了长宽比
		//SmoothTransformation 对图像进行平滑处理
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//保存转换后的图像
	//Format_ARGB32_Premultiplied 图像使用预乘的32位ARGB格式（0xAARRGGBB）进行存储
	QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);

	//设置一个画家，用resultImage画画
	QPainter painter(&resultImage);

	//设置图片叠加模式
	//CompositionMode_Source 输出是源像素。(这意味着一个基本的复制操作，当源像素是不透明的时候，与SourceOver相同）
	painter.setCompositionMode(QPainter::CompositionMode_Source);

	//用指定的画笔填充给定的矩形
	//rect 返回图像的矩形格式（0, 0, width(), height()）
	//transparent 一个透明的黑色数值（例如，QColor(0, 0, 0, 0)）
	painter.fillRect(resultImage.rect(), Qt::transparent);

	//CompositionMode_SourceOver 这是默认模式。源点的alpha被用来混合目标点上面的像素
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	//将mask的矩形部分源绘制到绘画设备的给定目标中
	painter.drawPixmap(0, 0, mask);

	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

	//将src的矩形部分源绘制到绘画设备的给定目标中
	painter.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//结束绘画，绘画时使用的任何资源都被释放
	painter.end();

	//将给定的图像转换为 QPixmap 格式,并返回结果
	return QPixmap::fromImage(resultImage);
}

//关闭
void BasicWindow::onShowClose(bool) {
	close();
}

//最小化
void BasicWindow::onShowMin(bool) {
	//显示最小化的小组件，作为一个图标。调用这个函数只影响到窗口。
	showMinimized();
}

//隐藏
void BasicWindow::onShowHide(bool) {
	//隐藏小组件。这个函数等同于setVisible(false)。
	hide();
}

//正常
void BasicWindow::onShowNormal(bool) {
	//显示小组件和它的子小组件。
	show();
	//同时将该窗口设置为可活动的窗口
	activateWindow();
}

//退出
void BasicWindow::onShowQuit(bool) {
	//更新登录状态为离线
	QString strSqlOnline = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlOnline(strSqlOnline);
	sqlOnline.exec();

	//告诉应用程序以返回代码0（成功）退出。相当于调用QCoreApplication::exit(0)。
	QApplication::quit();
}

//鼠标移动事件
void BasicWindow::mouseMoveEvent(QMouseEvent * e)
{
	//判断当前鼠标时按下的，并且鼠标按下的是左键
	//buttons() 返回一个位域，用于检查可能伴随着鼠标事件的鼠标按钮。
	if (m_mousePressed = true && (e->buttons() && Qt::LeftButton)) {
		//e->globalPos() 事件发生时的全局坐标，相对于屏幕左上角(0,0)
		move(e->globalPos() - m_mousePoint);

		//设置事件对象的接受标志，相当于调用setAccepted（true）。设置接受参数表明事件接收器想要这个事件。不需要的事件可能会被传播到父部件。
		e->accept();
	}
}

//鼠标按下事件
void BasicWindow::mousePressEvent(QMouseEvent* e) {
	if (e->buttons() && Qt::LeftButton) {			//判断鼠标按下的是否为左键
		m_mousePressed = true;						//把 是否按下变量 设置为true

		//pos() 事件发生时相对于窗口左上角(0,0)的偏移
		m_mousePoint = e->globalPos()-pos();		//更新m_mousePoint的值

		e->accept();
	}
}

//鼠标松开事件
void BasicWindow::mouseReleaseEvent(QMouseEvent* e) {
	m_mousePressed = false;
}

void BasicWindow::onButtonMinClicked() {
	//判断当前窗体风格有没有工具栏风格
	if (Qt::Tool == (windowFlags() & Qt::Tool)) {
		hide();
	}
	else {
		//显示最小化的小组件，作为一个图标。调用这个函数只影响到窗口。
		showMinimized();
	}
}

void BasicWindow::onButtonRestoreClicked() {
	//最大化之前窗口位置
	QPoint windowPos;
	//最大化之前窗口大小
	QSize windowSize;
	//获取窗体位置及大小的信息
	_titleBar->getRestoreInfo(windowPos, windowSize);
	//设置一个矩形
	setGeometry(QRect(windowPos, windowSize));
}

void BasicWindow::onButtonMaxClicked() {
	//保存当前窗口的宽度，高度
	_titleBar->saveRestoreInfo(pos(),QSize(width(),height()));
	//获取桌面应用程序的矩形信息
	QRect desktopRect = QApplication::desktop()->availableGeometry();

	//设置最大化后的矩形信息
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);

	//设置一个矩形
	setGeometry(factRect);
}

void BasicWindow::onButtonCloseClicked() {
	close();
}

