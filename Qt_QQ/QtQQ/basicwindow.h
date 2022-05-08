#pragma once

#include <QDialog>
#include "titlebar.h"

class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = nullptr);
	virtual ~BasicWindow();

public:
	//加载样式表
	void loadStyleSheet(const QString& sheetName);

	//获取圆头像
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));

private:
	//初始化窗口背景
	void initBackGroundColor();

protected:
	//绘图事件
	void paintEvent(QPaintEvent*);
	//鼠标按下事件
	void mousePressEvent(QMouseEvent* event);
	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent* event);
	//鼠标松开事件
	void mouseReleaseEvent(QMouseEvent*);

protected:
	//初始化标题栏按钮类型
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);
	//初始化标题栏标题
	void setTitleBarTitle(const QString& title, const QString& icon = "");

public slots:
	//显示关闭
	void onShowClose(bool);
	//显示最小化
	void onShowMin(bool);
	//窗口隐藏
	void onShowHide(bool);
	//窗口为常态
	void onShowNormal(bool);
	//退出
	void onShowQuit(bool);
	//窗体皮肤改变
	void onSignalSkinChanged(const QColor& color);


	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	//鼠标按下时的鼠标位置
	QPoint m_mousePoint;
	//鼠标是否按下
	bool m_mousePressed;
	//背景色
	QColor m_colorBackGround;
	//样式文件名称
	QString m_styleName;

	//标题栏
	TitleBar* _titleBar;
};
