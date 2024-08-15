#include "kdirectorytableview.h"
#include <QContextMenuEvent>
#include <QMenu>

KDirectoryTableView::KDirectoryTableView(QWidget* parent)
    : QTableView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &KDirectoryTableView::showContextMenu);
}

void KDirectoryTableView::contextMenuEvent(QContextMenuEvent* event)
{
    showContextMenu(event->pos());
}

void KDirectoryTableView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        emit editRequested(index);
    }
    //QTableView::mouseDoubleClickEvent(event);
}

void KDirectoryTableView::showContextMenu(const QPoint& pos)
{
    QModelIndex index = indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu(this);
    QAction* pauseAction = contextMenu.addAction(QString::fromLocal8Bit("��ͣ����"));
    QAction* resumeAction = contextMenu.addAction(QString::fromLocal8Bit("�ָ�����"));
    QAction* addAction = contextMenu.addAction(QString::fromLocal8Bit("����"));
    QAction* editAction = contextMenu.addAction(QString::fromLocal8Bit("�༭"));
    QAction* deleteAction = contextMenu.addAction(QString::fromLocal8Bit("ɾ��"));
    QString path = model()->data(index).toString();

    connect(pauseAction, &QAction::triggered, this, [this, index]() { emit pauseMonitoring(index); });
    connect(resumeAction, &QAction::triggered, this, [this, index]() { emit resumeMonitoring(index); });
    connect(addAction, &QAction::triggered, this, &KDirectoryTableView::addDirectory);
    connect(editAction, &QAction::triggered, this, [this, index]() { emit editRequested(index); });
    connect(deleteAction, &QAction::triggered, this, [this, index]() { emit deleteRequested(index); });

    contextMenu.exec(mapToGlobal(pos));
}
