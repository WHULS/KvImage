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
	// 打开日志文件
	QFile of("KvImage.log");
	of.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&of);
	ts.setCodec("UTF-8");  // 处理中文乱码

	// 输出字符串
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
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // 时间
		.arg(typeStr)  // 日志类型
		.arg(ctx.function)  // 函数
		.arg(txt)      // 消息内容
		.arg(ctx.file).arg(ctx.line);  // 文件和行数

	// 输出到控制台
	OutputDebugString(reinterpret_cast<const wchar_t*>(logStr.utf16()));

	// 不输出debug到文件中
	if (type == QtDebugMsg)
	{
		return;
	}

	// 输出到文件
	ts << logStr;
	of.close();

	// 如果是Fatal，显示对话框并退出
	if (type == QtFatalMsg)
	{
		QMessageBox::warning(nullptr, "Warning", txt, QMessageBox::Button::Default, QMessageBox::Button::Cancel);
		exit(-1);
	}
}

int main(int argc, char* argv[])
{
	// 控制调试信息
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
