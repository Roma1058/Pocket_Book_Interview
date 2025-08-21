#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QFileInfo>
#include <QDir>

class FileItem {
public:
    FileItem(const QString& name, const QString& path, qint64 size, const QString& extension);
    
    QString name() const { return m_name; }
    QString path() const { return m_path; }
    qint64 size() const { return m_size; }
    QString extension() const { return m_extension; }
    QString status() const { return m_status; }
    int progress() const { return m_progress; }
    QString progressText() const { return m_progressText; }
    
    void setStatus(const QString& status) { m_status = status; }
    void setProgress(int progress) { m_progress = progress; }
    void setProgressText(const QString& progressText) { m_progressText = progressText; }
    
private:
    QString m_name;
    QString m_path;
    qint64 m_size;
    QString m_extension;
    QString m_status;
    int m_progress;
    QString m_progressText;
};

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        SizeRole,
        ExtensionRole,
        StatusRole,
        ProgressRole,
        ProgressTextRole
    };

    explicit FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void loadDirectory(const QString& path);
    Q_INVOKABLE void loadDirectoryWitoutFileIndex(const int index, const QString& path);
    Q_INVOKABLE void updateFileStatus(int index, const QString& status, int progress = 0);
    Q_INVOKABLE void updateFileProgressOnly(int index, int progress);
    Q_INVOKABLE QString getFilePath(int index) const;
    Q_INVOKABLE QString getFileExtension(int index) const;
    Q_INVOKABLE bool isCompressibleFile(int index) const;
    Q_INVOKABLE bool isReadOnlyFile(int index) const;
    Q_INVOKABLE QString getCurrentDirectoryPath() const;

signals:
    void directoryChanged();
    void progressTextUpdated(int index, const QString& progressText);

private:
    QList<FileItem> m_files;
    QString m_currentDirectory;
    
    void scanDirectory(const QDir& dir);
    QString formatFileSize(qint64 size) const;
};

#endif // FILELISTMODEL_H
