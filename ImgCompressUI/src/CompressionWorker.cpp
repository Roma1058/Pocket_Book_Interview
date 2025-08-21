#include "include/CompressionWorker.h"
#include "ImgCompressLib.h"
#include "RawImageData.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QBuffer>
#include <QDateTime>

CompressionWorker::CompressionWorker(QObject *parent)
    : QObject(parent), m_operation(Compress), m_fileIndex(-1), m_compressLib(nullptr), m_workerThread(nullptr)
{
    m_compressLib = new ImgCompressLib();
}

CompressionWorker::~CompressionWorker()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
        m_workerThread = nullptr;
    }

    if (m_compressLib) {
        delete m_compressLib;
        m_compressLib = nullptr;
    }
}

void CompressionWorker::setOperation(Operation op, const QString& inputPath, const QString& outputPath, int fileIndex)
{
    m_operation = op;
    m_inputPath = inputPath;
    m_outputPath = outputPath;
    m_fileIndex = fileIndex;

    startWork();
}

void CompressionWorker::startWork()
{
    m_workerThread = new QThread();

    connect(m_workerThread, &QThread::started, this, &CompressionWorker::doWork);
    connect(this, &CompressionWorker::operationCompleted, m_workerThread, &QThread::quit);

    m_workerThread->start();
}

void CompressionWorker::doWork()
{
    bool success = false;
    QString message;

    emit statusChanged(m_operation == Compress ? "Кодується" : "Розкодовується");

    try {
        if (m_operation == Compress) {
            success = compressFile();
        } else {
            success = decompressFile();
        }

        if (success) {
            message = "Операція завершена успішно";
        } else {
            message = "Помилка при виконанні операції";
        }
    } catch (const std::exception& e) {
        message = QString("Виняток: %1").arg(e.what());
        success = false;
    } catch (...) {
        message = "Невідома помилка";
        success = false;
    }

    emit operationCompleted(success, message);
}

bool CompressionWorker::compressFile()
{
    QFileInfo inputFile(m_inputPath);
    if (!inputFile.exists()) {
        qWarning() << "Вхідний файл не існує:" << m_inputPath;
        return false;
    }

    QFileInfo outputFile(m_outputPath);
    QDir outputDir = outputFile.dir();
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    qDebug() << "Кодуємо файл:" << m_inputPath;

    try {
        QFile inputFile(m_inputPath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Не вдалося відкрити вхідний файл:" << m_inputPath;
            return false;
        }

        RawImageData imageData;

        QDataStream stream(&inputFile);
        stream.setByteOrder(QDataStream::LittleEndian);

        if (stream.status() != QDataStream::Ok) {
            qWarning() << "Помилка створення потоку для читання BMP";
            return false;
        }

        quint16 signature;
        quint32 fileSize;
        quint16 reserved1, reserved2;
        quint32 dataOffset;

        stream >> signature >> fileSize >> reserved1 >> reserved2 >> dataOffset;

        if (signature != 0x4D42) { // 'BM'
            qWarning() << "Не валідний BMP файл (підпис:" << QString::number(signature, 16) << ")";
            return false;
        }

        quint32 headerSize;
        stream >> headerSize;

        if (headerSize < 40) {
            qWarning() << "Непідтримуваний BMP формат (header size:" << headerSize << ")";
            return false;
        }

        qint32 width, height;
        stream >> width >> height;

        quint16 planes, bitsPerPixel;
        stream >> planes >> bitsPerPixel;

        if (planes != 1 || bitsPerPixel != 8) {
            qWarning() << "Підтримуються тільки 8-bit grayscale BMP файли";
            return false;
        }

         quint32 compression, imageSize;
         stream >> compression >> imageSize;

        if (compression != 0) {
            qWarning() << "Підтримуються тільки нестиснуті BMP файли";
            return false;
        }

        imageData.width = abs(width);
        imageData.height = abs(height);

        qDebug() << "BMP розміри:" << imageData.width << "x" << imageData.height;
        qDebug() << "Data offset:" << dataOffset << "Image size:" << imageSize;

        inputFile.seek(0);
        QByteArray bmpHeader = inputFile.read(dataOffset);

        QByteArray imageBytes = inputFile.readAll();

        if (imageBytes.size() < imageData.width * imageData.height) {
            qWarning() << "Недостатньо даних зображення";
            return false;
        }

        imageData.data = new unsigned char[imageData.width * imageData.height];
        memcpy(imageData.data, imageBytes.data(), imageData.width * imageData.height);


        QString tempPath = m_outputPath + ".tmp";

        ImgCompressLib compressor;

        int lastProgress = -1;
        std::function<void(int)> progressCallback = [this, &lastProgress](int progress){
            if (progress != lastProgress) {
                lastProgress = progress;
                // QTextStream(stdout) << "Прогрес: " << progress << "%" << "\n";
                // fflush(stdout);
            }
        };

        int result = compressor.encodeToFile(imageData, tempPath.toStdString(), progressCallback);

        if (result != 0) {
            qWarning() << "Помилка стискання файлу. Код помилки:" << result;
            return false;
        }

        qDebug() << "Стискання завершено. Результат:" << result;
        qDebug() << "Розмір зображення:" << imageData.width << "x" << imageData.height;

        QFile finalFile(m_outputPath);
        if (!finalFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Не вдалося створити фінальний файл:" << m_outputPath;
            return false;
        }

        QDataStream finalStream(&finalFile);
        finalStream.setByteOrder(QDataStream::LittleEndian);

        finalStream << quint32(0x48435241);  // 'ARCH' magic
        finalStream << quint32(bmpHeader.size());  // розмір заголовка BMP
        finalStream.writeRawData(bmpHeader.data(), bmpHeader.size());

        QFile tempFile(tempPath);
        if (tempFile.open(QIODevice::ReadOnly)) {
            QByteArray compressedData = tempFile.readAll();
            finalStream.writeRawData(compressedData.data(), compressedData.size());
            tempFile.close();
            tempFile.remove();
        }

        finalFile.close();

        delete[] imageData.data;
        imageData.data = nullptr;

        qDebug() << "Стискання завершено успішно, створено файл:" << m_outputPath;

        if (m_fileIndex >= 0) {
            emit progressTextUpdated(m_fileIndex, "");
        }

        return true;

    } catch (const std::exception& e) {
        qWarning() << "Виняток при кодуванні:" << e.what();

        if (m_fileIndex >= 0) {
            emit progressTextUpdated(m_fileIndex, "");
        }

        return false;
    }
}

bool CompressionWorker::decompressFile()
{
    QFileInfo inputFile(m_inputPath);
    if (!inputFile.exists()) {
        qWarning() << "Вхідний файл не існує:" << m_inputPath;
        return false;
    }

    QFileInfo outputFile(m_outputPath);
    QDir outputDir = outputFile.dir();
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    try {
        RawImageData imageData;

        QFile barchFile(m_inputPath);
        if (!barchFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Не вдалося відкрити BARCH файл:" << m_inputPath;
            return false;
        }

        QDataStream barchStream(&barchFile);
        barchStream.setByteOrder(QDataStream::LittleEndian);

        quint32 magic, headerSize;
        barchStream >> magic >> headerSize;

        if (magic != 0x48435241) { // 'ARCH'
            qWarning() << "Не валідний BARCH файл (magic:" << QString::number(magic, 16) << ")";
            barchFile.close();
            return false;
        }

        QByteArray bmpHeader = barchFile.read(headerSize);

        QString tempPath = m_inputPath + ".tmp";
        QFile tempFile(tempPath);
        if (!tempFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Не вдалося створити тимчасовий файл:" << tempPath;
            barchFile.close();
            return false;
        }

        QByteArray compressedData = barchFile.readAll();
        tempFile.write(compressedData);
        tempFile.close();
        barchFile.close();

        qDebug() << "Стиснуті дані розмір:" << compressedData.size() << "байт";

        ImgCompressLib decompressor;

        int lastProgress = -1;
        std::function<void(int)> progressCallback = [this, &lastProgress](int progress){
            if (progress != lastProgress) {
                lastProgress = progress;
                // QTextStream(stdout) << "Прогрес: " << progress << "%" << "\n";
                // fflush(stdout);
            }
        };

        int result = decompressor.decodeFromFile(imageData, tempPath.toStdString(), progressCallback);

        QFile::remove(tempPath);

        if (result != 0) {
            qWarning() << "Помилка розтискання файлу. Код помилки:" << result;
            return false;
        }

        qDebug() << "Розтискання завершено. Розмір зображення:" << imageData.width << "x" << imageData.height;

        QFile outputFile(m_outputPath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Не вдалося створити вихідний файл:" << m_outputPath;
            return false;
        }

        outputFile.write(bmpHeader);

        QByteArray imageDataBytes(reinterpret_cast<const char*>(imageData.data),
                                  imageData.width * imageData.height);
        outputFile.write(imageDataBytes);

        outputFile.close();

        if (m_fileIndex >= 0) {
            emit progressTextUpdated(m_fileIndex, "");
        }

        return true;

    } catch (const std::exception& e) {
        qWarning() << "Виняток при розтисканні:" << e.what();

        if (m_fileIndex >= 0) {
            emit progressTextUpdated(m_fileIndex, "");
        }

        return false;
    }
}
