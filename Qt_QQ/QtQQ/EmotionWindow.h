#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = Q_NULLPTR);
	~EmotionWindow();

private:
	void initControl();

private slots:
	//添加表情
	void addEmotion(int emotionNum);

signals:
	//表情窗口隐藏
	void signalEmotionWindowHide();
	//表情被点击
	void signalEmotionItemClicked(int emotionNum);

private:
	void paintEvent(QPaintEvent* event) override;

private:
	Ui::EmotionWindow ui;
};
