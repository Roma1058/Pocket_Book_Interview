#include "include/FileListModel.h"
#include <QDebug>
#include <QDirIterator>

// Конструктор FileItem
FileItem::FileItem(const QString& name, const QString& path, qint64 size, const QString& extension)
    : m_name(name), m_path(path), m_size(size), m_extension(extension), m_status("Готово"), m_progress(0)
{
}

// Конструктор FileListModel
FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

// Кількість рядків у моделі
int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_files.size();
}

// Дані для відображення
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
    default:
        return QVariant();
    }
}

// Назви ролей для QML
QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PathRole] = "path";
    roles[SizeRole] = "size";
    roles[ExtensionRole] = "extension";
    roles[StatusRole] = "status";
    roles[ProgressRole] = "progress";
    return roles;
}

// Завантаження директорії
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
    
    qDebug() << "Завантажено" << m_files.size() << "файлів з директорії:" << path;
}

// Сканування директорії
void FileListModel::scanDirectory(const QDir& dir)
{
    QDirIterator it(dir.absolutePath(), QDir::Files, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        QString extension = fileInfo.suffix().toLower();
        
        // Показуємо тільки .bmp та .barch файли
        if (extension == "bmp" || extension == "barch") {
            FileItem item(fileInfo.fileName(), filePath, fileInfo.size(), extension);
            m_files.append(item);
        }
    }
}

// Оновлення статусу файлу
void FileListModel::updateFileStatus(int index, const QString& status, int progress)
{
    if (index >= 0 && index < m_files.size()) {
        m_files[index].setStatus(status);
        m_files[index].setProgress(progress);
        
        // Оновлюємо дані у моделі
        QModelIndex modelIndex = this->index(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

// Отримання шляху до файлу
QString FileListModel::getFilePath(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        return m_files[index].path();
    }
    return QString();
}

// Перевірка, чи можна стиснути файл
bool FileListModel::isCompressibleFile(int index) const
{
    if (index >= 0 && index < m_files.size()) {
        QString extension = m_files[index].extension();
        return extension == "bmp" || extension == "barch";
    }
    return false;
}

// Форматування розміру файлу
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