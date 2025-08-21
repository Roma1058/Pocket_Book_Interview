#ifndef COMPRESSIONWORKER_H
#define COMPRESSIONWORKER_H

#include <QObject>
#include <QString>
#include <QThread>

class ImgCompressLib;

class CompressionWorker : public QObject
{
    Q_OBJECT

public:
    enum Operation {
        Compress,
        Decompress
    };
    Q_ENUM(Operation)

    explicit CompressionWorker(QObject *parent = nullptr);
    ~CompressionWorker();
    
    Q_INVOKABLE void setOperation(Operation op, const QString& inputPath, const QString& outputPath, int fileIndex = -1);

signals:
    void operationCompleted(bool success, const QString& message);
    void statusChanged(const QString& status);
    void progressTextUpdated(int index, const QString& progressText);
    void progressUpdated(int progress);

public slots:
    void doWork();
    void startWork();

private:
    Operation m_operation;
    QString m_inputPath;
    QString m_outputPath;
    int m_fileIndex;
    
    ImgCompressLib* m_compressLib;
    
    QThread* m_workerThread;
    
    bool compressFile();
    bool decompressFile();
};

#endif // COMPRESSIONWORKER_H
