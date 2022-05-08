#include "RootContatitem.h"
#include <QPainter>

RootContatitem::RootContatitem(bool hasArrow, QWidget *parent)
	: QLabel(parent)
	,m_rotation(0)
	,m_hasArrow(hasArrow)
{
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	//��ʼ�����Զ���
	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(50);	//���õ��ζ���ʱ��Ϊ50ms
	m_animation->setEasingCurve(QEasingCurve::InQuad);	//����������������
}

RootContatitem::~RootContatitem()
{
}

void RootContatitem::setText(const QString & title)
{
	m_titleText = title;
	update();
}

void RootContatitem::setExpanded(bool expand)
{
	if (expand) {
		//�������Զ����Ľ���ֵ���ü�ͷ������ת90��
		m_animation->setEndValue(90);
	}
	else {
		m_animation->setEndValue(0);
	}

	m_animation->start();
}

int RootContatitem::rotation()
{
	return m_rotation;
}

void RootContatitem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();
}

void RootContatitem::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	//TextAntialiasing �����
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	//��ͼƬƽ������
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();	//���滭������

	if (m_hasArrow) {
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");

		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);

		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);

		//����ϵƫ��(x����ƫ�ƣ�y����ƫ��)
		p.translate(pixmap.width() / 2, pixmap.height() / 2);
		p.rotate(m_rotation);	//��ת����ϵ(˳ʱ��)
		p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

		painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);
		painter.restore();	//�ָ���������
	}

	QLabel::paintEvent(event);
}
