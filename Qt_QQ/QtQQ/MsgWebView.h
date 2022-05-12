#pragma once

#include <QWebEngineView>
#include <QDomNode>

class MsgHtmlObj :public QObject 
{
	Q_OBJECT
	//��̬���� ���˷�������Ϣ
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
	//��̬���� �ҷ�����Ϣ
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)
	
public:
	//msgLPicPath ����Ϣ�����˵�ͷ��·��
	MsgHtmlObj(QObject* parent,QString msgLPicPath = "");

signals:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();//��ʼ��������ҳ
	//�����Ҵ��ڷֱ���г�ʼ��
	QString getMsgTmplHtml(const QString& code);

private:
	QString m_msgLPicPath; //����Ϣ�����˵�ͷ��·��
	QString m_msgLHtmlTmpl;//���˷�������Ϣ
	QString m_msgRHtmlTmpl;//�ҷ�����Ϣ
};

class MsgWebPage :public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {};

protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

//QWebEngineView��ͼ������ʾ��ҳ����
class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();
	void appendMsg(const QString& html, QString strObj = "0");//׷����Ϣ

private:
	QList<QStringList> parseHtml(const QString& html);
	
	//Qt������DOM�ڵ㣨���ԡ�˵�����ı��ȣ�������ʹ��QDomNode���б�ʾ
	QList<QStringList> parseDocNode(const QDomNode& node);//�����ڵ�

signals:
	void signalSendMag(QString& strData, int& magType, QString sFile = "");

private:
	MsgHtmlObj* m_msgHtmlObj;
	QWebChannel* m_channel;
};
