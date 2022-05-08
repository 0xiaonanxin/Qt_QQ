#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>

CommonUtils::CommonUtils()
{
}

QPixmap CommonUtils::getRoundImage(const QPixmap & src, QPixmap & mask, QSize masksize)
{
	if (masksize == QSize(0, 0))
	{
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

	//将src(原图)的矩形部分源绘制到绘画设备的给定目标中
	painter.drawPixmap(0, 0, src.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	//结束绘画
	painter.end();

	//将给定的图像转换为 QPixmap 格式,并返回结果
	return QPixmap::fromImage(resultImage);
}

void CommonUtils::loadStyleSheet(QWidget * widget, const QString & sheetName)
{
	QFile file(":/Resources/QSS" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}
	file.close();
}

void CommonUtils::setDefaultSkinColor(const QColor & color)
{
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	QSettings settings(path, QSettings::IniFormat);		//对配置文件进行读写操作
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());
}

QColor CommonUtils::getDefaultSkinColor()
{
	QColor color;
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	if (!QFile::exists(path))	//当文件不存在时
		setDefaultSkinColor(QColor(22, 154, 218));

	QSettings settings(path, QSettings::IniFormat);
	color.setRed(settings.value("DefaultSkin/red").toInt());
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}