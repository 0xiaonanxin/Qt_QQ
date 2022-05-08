#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

enum ButtonType		//用枚举值来指定按钮类型
{
	MIN_BUTTON = 0,		//最小化及关闭按钮
	MIN_MAX_BUTTON,		//最小化、最大化及关闭按钮
	ONLY_CLOSE_BUTTON	//只有关闭按钮
};

//自定义标题栏
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	//构造方法
	TitleBar(QWidget *parent = nullptr);
	//析构函数
	~TitleBar();

	//自定义方法，用来设置标题栏图标，参数为 QString 类型的文件路径
	void setTitleIcon(const QString& filePath);			

	//自定义方法，用来设置标题栏内容，参数为 QString 类型的内容
	void setTitleContent(const QString& titleContent);	

	//自定义方法，用来设置标题栏的宽度，参数为 int 类型的宽度
	void setTitleWidth(int width);			

	//自定义方法，用来设置标题栏按钮的类型，参数为 ButtonType 类型的按钮类型
	void setButtonType(ButtonType buttonType);		


	//自定义方法，保存窗口最大化前窗口的位置及大小
	void saveRestoreInfo(const QPoint& point, const QSize& size);

	//自定义方法，获取窗口最大化前窗口的位置及大小
	void getRestoreInfo(QPoint& point, QSize& size);

private:
	//自定义私有方法，对绘图事件进行处理
	void paintEvent(QPaintEvent* event);

	//自定义私有方法，对鼠标双击事件进行处理
	void mouseDoubleClickEvent(QMouseEvent* event);

	//自定义私有方法，对鼠标按下事件进行处理
	void mousePressEvent(QMouseEvent* event);

	//自定义私有方法，对鼠标移动事件进行处理
	void mouseMoveEvent(QMouseEvent* event);

	//自定义私有方法，对鼠标松开事件进行处理
	void mouseReleaseEvent(QMouseEvent* event);


	//自定义私有方法，初始化控件
	void initControl();

	//自定义私有方法，初始化信号与槽的连接
	void initConnections();

	//自定义私有方法，加载样式表
	void loadStyleSheet(const QString& sheetName);

	
signals:
	//当标题栏最小化按钮被点击时触发的信号
	void signalButtonMinClicked();

	//当标题栏最大化还原按钮被点击时触发的按钮
	void signalButtonRestoreClicked();

	//当标题栏最大化按钮被点击时触发的信号
	void signalButtonMaxClicked();

	//当标题栏关闭按钮被点击时触发的信号
	void signalButtonCloseClicked();


private slots:
	//最小化按钮响应的槽
	void onButtonMinClicked();

	//最大化还原按钮响应的槽
	void onButtonRestoreClicked();

	//最大化按钮响应的槽
	void onButtonMaxClicked();

	//关闭按钮响应的槽
	void onButtonCloseClicked();


private:
	//标题栏的图标
	QLabel* m_pIcon;	

	//标题栏内容
	QLabel* m_pTitleContent;

	//标题栏最小化按钮
	QPushButton* m_pButtonMin;

	//标题栏最大化还原按钮
	QPushButton* m_pButtonRestore;

	//标题栏最大化按钮
	QPushButton* m_pButtonMax;

	//标题栏关闭按钮
	QPushButton* m_pButtonClose;


	//最大化还原按钮变量（用于保存窗体位置）
	QPoint m_restorePos;

	//最大化还原按钮变量（用于保存窗体大小）
	QSize m_restoreSize;

	//移动窗口的变量，判断标题栏是否被点击
	bool m_isPressed;

	//移动窗口的变量，保存窗口开始移动时的位置
	QPoint m_startMovePos;

	//标题栏的内容
	QString m_titleContent;

	//标题栏右上角按钮类型
	ButtonType m_buttonType;
};
