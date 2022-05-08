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
	//��ӱ���
	void addEmotion(int emotionNum);

signals:
	//���鴰������
	void signalEmotionWindowHide();
	//���鱻���
	void signalEmotionItemClicked(int emotionNum);

private:
	void paintEvent(QPaintEvent* event) override;

private:
	Ui::EmotionWindow ui;
};
