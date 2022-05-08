#pragma once

#include <QObject>

class NotifyManager : public QObject
{
	Q_OBJECT

public:
	NotifyManager();
	~NotifyManager();

signals:
	void signalSkinChanged(const QColor& color);

public:
	//获取当前操作的实例
	static NotifyManager* getInstance();

	//通知其他窗口改变主题颜色
	void notifyOtherWindowChangeSkin(const QColor& color);

private:
	//实例
	static NotifyManager* instance;
};
