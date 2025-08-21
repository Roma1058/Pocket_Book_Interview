#include "include/FileListModel.h"
#include <QDebug>
#include <QDirIterator>
#include <QThread>

FileItem::FileItem(const QString& name, const QString& path, qint64 size, const QString& extension)
    : m_name(name), m_path(path), m_size(size), m_extension(extension), m_status("Готово"), m_progress(0), m_progressText("")
{
}

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_files.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.size())
        return QVariant();

    const FileItem& item = m_files[index.row()];

    switch (role) {
    case NameRole:
        return item.name();
    case PathRole:
        return item.path();
    case SizeRole:
        return formatFileSize(item.size());
    case ExtensionRole:
        return item.extension();
    case StatusRole:
        return item.status();
    case ProgressRole:
        return item.progress();
    case ProgressTextRole:
        return item.progressText();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[SizeRole] = "size";
    roles[ExtensionRole] = "extension";
    roles[StatusRole] = "status";
    roles[ProgressRole] = "progress";
    roles[ProgressTextRole] = "progressText";
    return roles;
}

void FileListModel::loadDirectory(const QString& path)
{
    beginResetModel();
    m_files.clear();
    m_currentDirectory = path;

    QDir dir(path);
    if (dir.exists()) {
        scanDirectory(dir);
    }

    endResetModel();

    emit directoryChanged();
}

void FileListModel::loadDirectoryWitoutFileIndex(const int skipIndex, const QString &path)
{
    m_currentDirectory = path;

    if (skipIndex < 0 || skipIndex >= m_files.size()) {
        loadDirectory(m_currentDirectory);
        return;
    }

    FileItem preservedItem = m_files[skipIndex];

    QDir dir(m_currentDirectory);
    if (!dir.exists()) return;

    beginResetModel();
    m_files.clear();

    scanDirectory(dir);

    bool found = false;
    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files[i].path() == preservedItem.path()) {

            m_files[i] = preservedItem;
            found = true;
            break;
        }
    }

    if (!found) {
        m_files.append(preservedItem);
    }

    endResetModel();

    emit directoryChanged();
}

void FileListModel::scanDirectory(const QDir& dir)
{
    QDirIterator it(dir.absolutePath(), QDir::Files);

    int foundFiles = 0;
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        QString extension = fileInfo.suffix().toLower();

        if (extension == "bmp" || extension == "barch" || extension == "png") {
            FileItem item(fileInfo.fileName(), filePath, fileInfo.size(), extension);
            m_files.append(item);
            foundFiles++;
            qDebug() << "Додано файл:" << item.name();
        }
    }
}

void FileListModel::updateFileStatus(int index, const QString& status, int progress)
{
    qDebug() << "FileListModel::updateFileStatus викликано. Індекс:" << index << "Статус:" << status << "Прогрес:" << progress;

    if (index >= 0 && index < m_files.size()) {
        m_files[index].setStatus(status);

        if (progress != -1) {
            m_files[index].setProgress(progress);
        } else {

        }

        QModelIndex modelIndex = this->index(index, 0);
        
        emit dataChanged(modelIndex, modelIndex);
    } else {
        qWarning() << "Невірний індекс для оновлення статусу:" << index;
    }
}

QString FileListModel::getFilePath(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        return m_files[index].path();
    }
    return QString();
}

QString FileListModel::getFileExtension(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        return m_files[index].extension();
    }
    return QString();
}

bool FileListModel::isCompressibleFile(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        QString extension = m_files[index].extension();
        return extension == "bmp" || extension == "barch";
    }
    return false;
}

bool FileListModel::isReadOnlyFile(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        QString extension = m_files[index].extension();
        return extension == "png";  // PNG файли тільки для читання
    }
    return false;
}

QString FileListModel::getCurrentDirectoryPath() const
{
    QDir dir(m_currentDirectory);
    return dir.absolutePath();
}

QString FileListModel::formatFileSize(qint64 size) const
{
    if (size < 1024) {
        return QString::number(size) + " B";
    } else if (size < 1024 * 1024) {
        return QString::number(size / 1024.0, 'f', 1) + " KB";
    } else if (size < 1024 * 1024 * 1024) {
        return QString::number(size / (1024.0 * 1024.0), 'f', 1) + " MB";
    } else {
        return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 1) + " GB";
    }
}

void FileListModel::updateFileProgressOnly(int index, int progress)
{
    if (index >= 0 && index < m_files.size()) {
        m_files[index].setProgress(progress);

        QModelIndex modelIndex = this->index(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}


