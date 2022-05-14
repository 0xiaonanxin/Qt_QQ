#include "titlebar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>


//定义宏，按钮的高度为 27
#define BUTTON_HEIGHT 27
//定义宏，按钮宽度为 27
#define BUTTON_WIDTH 27
//定义宏，标题栏高度为 27
#define TITLE_HEIGHT 40

//构造函数
TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
	,m_isPressed(false)
	,m_buttonType(MIN_MAX_BUTTON)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

//析构函数
TitleBar::~TitleBar()
{
}

//初始化控件
void TitleBar::initControl()
{
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);

	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonMax = new QPushButton(this);
	m_pButtonClose = new QPushButton(this);

	//setFixedSize 对按钮指定固定大小
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	//setObjectName 设置对象名
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	//设置标题栏布局
	QHBoxLayout* mylayout = new QHBoxLayout(this);

	//将控件添加到布局上
	mylayout->addWidget(m_pIcon);
	mylayout->addWidget(m_pTitleContent);

	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	//设置布局周围使用的左、上、右和下边距
	mylayout->setContentsMargins(5, 6, 8, 0);
	//设置控件之间的间距
	mylayout->setSpacing(0);

	//将标题水平方向设置为自适应，垂直方向设置为大小固定
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	//将标题栏的最小和最大高度都设置为h，而不改变宽度
	setFixedHeight(TITLE_HEIGHT);
	//产生一个无边界的窗口
	setWindowFlags(Qt::FramelessWindowHint);
}

//初始化信号与槽的连接
void TitleBar::initConnections() {
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

//设置标题栏的图标
void TitleBar::setTitleIcon(const QString & filePath)
{
	//将图片的路径传给titleIcon
	QPixmap titleIcon(filePath);

	//把图标大小设置为titleIcon的大小
	m_pIcon->setFixedSize(titleIcon.size());
	//设置图标
	m_pIcon->setPixmap(titleIcon);
}

//设置标题栏内容
void TitleBar::setTitleContent(const QString & titleContent)
{
	//通过m_pTitleContent设置文本
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;
}

//设置标题栏宽度
void TitleBar::setTitleWidth(int width)
{
	//设置固定宽度
	setFixedWidth(width);
}

//设置标题栏按钮类型
void TitleBar::setButtonType(ButtonType buttonType)
{
	m_buttonType = buttonType;

	switch (buttonType)
	{
		case MIN_BUTTON:
		{
			//只有最小化按钮，所以把其他按钮设置为不可见
			m_pButtonRestore->setVisible(false);
			m_pButtonMax->setVisible(false);
			break;
		}
		case MIN_MAX_BUTTON:
		{
			//只有最小化和最大化按钮，所以把其他按钮设置为不可见
			m_pButtonRestore->setVisible(false);
			break;
		}
		case ONLY_CLOSE_BUTTON:
		{
			//只有关闭按钮，所以把其他三个按钮设置为不可见
			m_pButtonRestore->setVisible(false);
			m_pButtonMax->setVisible(false);
			m_pButtonMin->setVisible(false);
			break;
		}
		default:
		{
			break;
		}
	}
}

//保存窗口最大化前窗口的位置及大小
void TitleBar::saveRestoreInfo(const QPoint & point, const QSize & size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

//获取窗口最大化前窗口的位置及大小
void TitleBar::getRestoreInfo(QPoint & point, QSize & size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

//绘制标题栏
void TitleBar::paintEvent(QPaintEvent * event)
{
	//设置背景色，先设置一个画师
	QPainter painter(this);

	//再设置一个绘图路径，用来绘制背景
	QPainterPath pathBack;

	//设置绘图路径的填充规则
	pathBack.setFillRule(Qt::WindingFill);

	//为绘图路径添加一个圆角的矩形,
	//QRect构建一个以(x, y)为左上角，给定宽度和高度的矩形,斜率为3
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);

	//SmoothPixmapTransform 使用平滑的像素图转换算法（如双线性）
	//如果on为true，则在画师上设置给定的渲染提示；否则清除渲染提示。
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);


	//当窗口最大化或还原之后，窗口宽度改变，标题栏相应做出改变
	//parentWidget() 返回父部件
	if (width() != parentWidget()->width()) 
	{
		//重新设置标题栏的宽度
		setFixedWidth(parentWidget()->width());
	}

	QWidget::paintEvent(event);
}

//鼠标双击响应事件，主要实现双击标题栏最大化，最小化的操作
void TitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
	//只有存在最大化、最小化按钮时操作才有效
	if (m_buttonType == MIN_MAX_BUTTON) {
		//判断最大化按钮是否可见
		if (m_pButtonMax->isVisible()) {
			onButtonMaxClicked();
		}
		else {
			onButtonRestoreClicked();
		}
	}

	return QWidget::mouseDoubleClickEvent(event);
}

//通过鼠标按下、鼠标移动、鼠标释放事件实现拖动标题栏达到移动窗口效果
void TitleBar::mousePressEvent(QMouseEvent * event)
{
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		//在窗口最大化时禁止拖动窗口
		if (m_pButtonMax->isVisible()) 
		{
			m_isPressed = true;
			//globalPos() 返回事件发生的时候鼠标所在的全局位置
			m_startMovePos = event->globalPos();
		}
	}
	else
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent * event)
{
	//判断 m_isPressed 是 true
	if (m_isPressed)
	{
		//获取窗口移动的长度
		QPoint movePoint = event->globalPos() - m_startMovePos;
		//获取父部件的位置
		QPoint widgetPos = parentWidget()->pos();
		//将 m_startMovePos 进行更新
		m_startMovePos = event->globalPos();

		//进行移动，比如部件的x值加上移动长度的x值
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}

	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent * event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

//加载样式表
void TitleBar::loadStyleSheet(const QString & sheetName)
{
	//根据路径读取样式表文件
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) 
	{
		//将读取到的样式表进行保存
		QString styleSheet = this->styleSheet();
		//读取文件内所有数据，QLatin1String 可以保存字符串的地址和长度
		styleSheet += QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
	}

	file.close();
}

//最小化按钮响应的槽
void TitleBar::onButtonMinClicked() {
	emit signalButtonMinClicked();
}

//最大化还原按钮响应的槽
void TitleBar::onButtonRestoreClicked()
{
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

//最大化按钮响应的槽
void TitleBar::onButtonMaxClicked()
{
	m_pButtonMax->setVisible(false);
	m_pButtonRestore->setVisible(true);
	emit signalButtonMaxClicked();
}

//关闭按钮响应的槽
void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}
