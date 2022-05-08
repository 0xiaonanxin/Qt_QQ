#pragma once

#include <QPixmap>
#include <QSize>
#include <QProxyStyle>

//改变系统默认的部件风格
class CustomProxyStyle :public QProxyStyle
{
public:
	CustomProxyStyle(QObject* parent)
	{
		setParent(parent);
	}

	//重写父类方法
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = 0)const
	{
		if (PE_FrameFocusRect == element)
		{
			//去掉windows中部件默认的边框或虚线框，部件获取焦点时直接返回，不进行绘制边框
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

class CommonUtils
{
public:
	CommonUtils();
public:
	//获取圆头像
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));
	//给指定部件加载样式表
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
	//设置默认颜色
	static void setDefaultSkinColor(const QColor& color);
	//获取默认颜色
	static QColor getDefaultSkinColor();
};

