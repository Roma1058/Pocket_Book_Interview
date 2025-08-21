#ifndef COMPRESSIONWORKER_H
#define COMPRESSIONWORKER_H

#include <QThread>
#include <QString>
#include <QObject>

class CompressionWorker : public QThread
{
    Q_OBJECT

public:
    enum Operation {
        Compress,
        Decompress
    };

    explicit CompressionWorker(QObject *parent = nullptr);
    
    void setOperation(Operation op, const QString& inputPath, const QString& outputPath);

signals:
    void progressUpdated(int progress);
    void operationCompleted(bool success, const QString& message);
    void statusChanged(const QString& status);

protected:
    void run() override;

private:
    Operation m_operation;
    QString m_inputPath;
    QString m_outputPath;
    
    bool compressFile();
    bool decompressFile();
    void updateProgress(int progress);
};

#endif // COMPRESSIONWORKER_H 