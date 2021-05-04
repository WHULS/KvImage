#include "KvImage.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QString>
#include <Windows.h>
#include <QMessageBox>


#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void customMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& str)
{
	QString txt = str;
#else
void customMessageHandler(QtMsgType type, const char* msg)
{
	QString txt(msg);
#endif
	// ����־�ļ�
	QFile of("KvImage.log");
	of.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&of);
	ts.setCodec("UTF-8");  // ������������

	// ����ַ���
	QString typeStr;
	switch (type)
	{
	case QtDebugMsg:
		typeStr = "Debug";
		break;
	case QtWarningMsg:
		typeStr = "Warning";
		break;
	case QtCriticalMsg: // QtSystemMsg
		typeStr = "Critical";
		break;
	case QtFatalMsg:
		typeStr = "Fatal";
		break;
	case QtInfoMsg:
		typeStr = "Info";
		break;
	default:
		break;
	}

	QString logStr = QString("(%1) [%2] - %3 - %4 - in: %5:%6\n")
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // ʱ��
		.arg(typeStr)  // ��־����
		.arg(ctx.function)  // ����
		.arg(txt)      // ��Ϣ����
		.arg(ctx.file).arg(ctx.line);  // �ļ�������

	// ���������̨
	OutputDebugString(reinterpret_cast<const wchar_t*>(logStr.utf16()));

	// �����debug���ļ���
	if (type == QtDebugMsg)
	{
		return;
	}

	// ������ļ�
	ts << logStr;
	of.close();

	// �����Fatal����ʾ�Ի����˳�
	if (type == QtFatalMsg)
	{
		QMessageBox::warning(nullptr, "Warning", txt, QMessageBox::Button::Default, QMessageBox::Button::Cancel);
		exit(-1);
	}
}

int main(int argc, char* argv[])
{
	// ���Ƶ�����Ϣ
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	qInstallMessageHandler(customMessageHandler);
#else
	qInstallMsgHandler(customMessageHandler);
#endif

	QApplication a(argc, argv);

	qInfo() << "Start app";
	KvImage w;
	w.show();

	int exitCode = a.exec();
	qInfo() << "Exit app with code: " << exitCode;
	return exitCode;
}
