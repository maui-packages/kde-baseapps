/*
 *   Copyright © 2008 Fredrik Höglund <fredrik@kde.org>
 *   Copyright © 2008 Rafael Fernández López <ereslibre@kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QPersistentModelIndex>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QStyleOption>
#include <QPointer>
#include <QBasicTimer>

#include <KActionCollection>
#include <KMimeType>

#include <plasma/containment.h>
#include "ui_folderviewFilterConfig.h"
#include "ui_folderviewDisplayConfig.h"
#include "ui_folderviewLocationConfig.h"

class KDirModel;
class KFileItemDelegate;
class KNewMenu;
class QItemSelectionModel;
class ProxyModel;

namespace Plasma
{
    class ScrollBar;
}

struct ViewItem
{
    ViewItem() : rect(QRect()), layouted(false) {}
    QRect rect;
    bool layouted;
};

class FolderView : public Plasma::Containment
{
    Q_OBJECT

public:
    FolderView(QObject *parent, const QVariantList &args);
    ~FolderView();

    void init();
    void saveState(KConfigGroup &config) const;
    void createAnimationFrames();
    void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);
    void setPath(const QString&);
    QRect visualRect(const QModelIndex &index);

protected:
    void createConfigurationInterface(KConfigDialog *parent);
    QList<QAction*> contextualActions();

    QPointF mapToViewport(const QPointF &point) const;
    QPointF mapFromViewport(const QPointF &point) const;
    QRectF mapToViewport(const QRectF &point) const;
    QRectF mapFromViewport(const QRectF &point) const;

private slots:
    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsRemoved(const QModelIndex &parent, int first, int last);
    void modelReset();
    void layoutChanged();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void configAccepted();
    void customFolderToggled(bool checked);
    void fontSettingsChanged();
    void iconSettingsChanged(int group);
    void themeChanged();
    void aboutToShowCreateNew();
    void clipboardDataChanged();
    void scrollBarValueChanged(int);
    void chkCustomIconSizeToggled(bool checked);

    // These slots are for KonqPopupMenu
    void copy();
    void cut();
    void paste();
    void pasteTo();
    void refreshIcons();
    void renameSelectedIcon();
    void moveToTrash(Qt::MouseButtons, Qt::KeyboardModifiers);
    void deleteSelectedIcons();
    void undoTextChanged(const QString &text);
    void toggleIconsLocked(bool locked);
    void toggleAlignToGrid(bool align);
    void toggleDirectoriesFirst(bool enable);
    void sortingChanged(QAction *action);

    void listingCompleted();
    void listingCanceled();

    void commitData(QWidget *editor);
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

    void filterChanged(int index);
    void selectUnselectAll();

private:
    void setUrl(const KUrl &url);
    void createActions();
    void updateSortActionsState();
    KUrl::List selectedUrls() const;
    void showContextMenu(QWidget *widget, const QPoint &pos, const QModelIndexList &indexes);
    int columnsForWidth(qreal width) const;
    int rowsForHeight(qreal height) const;
    QPoint nextGridPosition(const QPoint &prevPos, const QSize &gridSize, const QRect &contentRect) const;
    QPoint findNextEmptyPosition(const QPoint &prevPos, const QSize &gridSize, const QRect &contentRect) const;
    void layoutItems();
    void alignIconsToGrid();
    bool doLayoutSanityCheck();
    void saveIconPositions() const;
    void loadIconPositions();
    void updateScrollBar();
    void updateScrollBarGeometry();
    QRect scrollBackbufferContents();
    void markAreaDirty(const QRect &rect);
    void markAreaDirty(const QRectF &rect) { markAreaDirty(rect.toAlignedRect()); }
    void markEverythingDirty();
    void updateTextShadows(const QColor &textColor);
    QModelIndex indexAt(const QPointF &point);
    QSize iconSize() const;
    QSize gridSize() const;
    QStyleOptionViewItemV4 viewOptions() const;
    void timerEvent(QTimerEvent *event);
    void startDrag(const QPointF &pos, QWidget *widget);
    void constraintsEvent(Plasma::Constraints constraints);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    KFileItemDelegate *m_delegate;
    KDirModel *m_dirModel;
    ProxyModel *m_model;
    Plasma::ScrollBar *m_scrollBar;
    QPixmap m_pixmap;
    QPixmap m_topFadeTile;
    QPixmap m_bottomFadeTile;
    QRegion m_dirtyRegion;
    QItemSelectionModel *m_selectionModel;
    KUrl m_url;
    QString m_titleText;
    int m_titleHeight;
    int m_lastScrollValue;
    bool m_viewScrolled;
    int m_filterType;
    QString m_filterFiles;
    QStringList m_filterFilesMimeList;
    QFont m_font;
    QPointer<KNewMenu> m_newMenu;
    KActionCollection m_actionCollection;
    QActionGroup *m_sortingGroup;
    QVector<ViewItem> m_items;
    QHash<QString, QPoint> m_savedPositions;
    int m_columns;
    int m_rows;
    int m_validRows;
    int m_sortColumn;
    bool m_layoutBroken;
    bool m_needPostLayoutPass;
    bool m_initialListing;
    bool m_positionsLoaded;
    QPersistentModelIndex m_hoveredIndex;
    QPersistentModelIndex m_pressedIndex;
    QPersistentModelIndex m_editorIndex;
    QRect m_rubberBand;
    QRectF m_viewportRect;
    QPointF m_buttonDownPos;
    QTime m_pressTime;
    Ui::folderviewFilterConfig uiFilter;
    Ui::folderviewDisplayConfig uiDisplay;
    Ui::folderviewLocationConfig uiLocation;
    bool m_doubleClick;
    bool m_dragInProgress;
    bool m_iconsLocked;
    bool m_alignToGrid;
    bool m_sortDirsFirst;
    QString m_customLabel;
    int m_customIconSize;
    QListView::Flow m_flow;
    QPoint m_lastDeletedPos;
    QPoint m_currentLayoutPos;
    int m_animFrame;
    QPixmap m_animFrames;
    QBasicTimer m_delayedSaveTimer;
    QBasicTimer m_delayedCacheClearTimer;
    QBasicTimer m_delayedLayoutTimer;
    QBasicTimer m_animTimer;
};



// ---------------------------------------------------------------------------



class MimeModel : public QStringListModel
{
public:
    MimeModel(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
    KMimeType::List m_mimetypes;
    QMap<KMimeType*, Qt::CheckState> m_state;
};



// ---------------------------------------------------------------------------



class ProxyMimeModel : public QSortFilterProxyModel
{
Q_OBJECT

public:
    ProxyMimeModel(QObject *parent = 0);

    virtual void setSourceModel(QAbstractItemModel *sourceModel);

public slots:
    void setFilter(const QString &filter);

protected:
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString m_filter;
};

K_EXPORT_PLASMA_APPLET(folderview, FolderView)

#endif
