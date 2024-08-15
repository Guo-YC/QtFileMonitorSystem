#ifndef __KDIRECTORYTABLEVIEW_H_
#define __KDIRECTORYTABLEVIEW_H_

#include <QTableView>

class KDirectoryTableView : public QTableView
{
    Q_OBJECT

public:
    explicit KDirectoryTableView(QWidget* parent = nullptr);
    ~KDirectoryTableView() = default;
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void showContextMenu(const QPoint& pos);

signals:
    void pauseMonitoring(const QModelIndex& index);
    void resumeMonitoring(const QModelIndex& index);
    void addDirectory();
    void editDirectory(const QString& path);
    void editRequested(const QModelIndex& index);
    void deleteRequested(const QModelIndex& index);
};

#endif // __KDIRECTORYTABLEVIEW_H_

