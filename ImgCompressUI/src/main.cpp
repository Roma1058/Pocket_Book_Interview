#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "include/FileListModel.h"
#include "include/CompressionWorker.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("ImgCompress UI");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("UI для стискання зображень");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption directoryOption(QStringList() << "d" << "directory",
        "Шлях до директорії для сканування", "path");
    parser.addOption(directoryOption);

    parser.process(app);

    QString directoryPath = parser.value(directoryOption);
    if (directoryPath.isEmpty()) {
        directoryPath = QDir::currentPath();
    }

    QDir dir(directoryPath);
    if (!dir.exists()) {
        qWarning() << "Директорія не існує:" << directoryPath;
        directoryPath = QDir::currentPath();
    }

    QQmlApplicationEngine engine;

    qmlRegisterType<FileListModel>("ImgCompressUI", 1, 0, "FileListModel");
    qmlRegisterType<CompressionWorker>("ImgCompressUI", 1, 0, "CompressionWorker");

    engine.rootContext()->setContextProperty("initialDirectory", directoryPath);
    qDebug() << "initialDirectory передано у QML:" << directoryPath;

    engine.rootContext()->setContextProperty("globalInitialDirectory", directoryPath);

    const QUrl url(QStringLiteral("qrc:/src/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
