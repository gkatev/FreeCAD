/***************************************************************************
 *   Copyright (c) 2015 Eivind Kvedalen (eivind@kvedalen.name)             *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This file is based on the Qt spreadsheet example code.                *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef SPREADSHEETDELEGATE_H
#define SPREADSHEETDELEGATE_H

#include <QStyledItemDelegate>
#include <QAbstractItemView>

namespace Spreadsheet {
class Sheet;
}

namespace SpreadsheetGui {

class SheetTableView;

class SpreadsheetDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SpreadsheetDelegate(Spreadsheet::Sheet * sheet, QWidget *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const override;

    void setEditTrigger(QAbstractItemView::EditTrigger trigger);

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

Q_SIGNALS:
    void finishedWithKey(int key, Qt::KeyboardModifiers modifiers);

private Q_SLOTS:
    void commitAndCloseEditor();
    void onEditorFinishedWithKey(int key, Qt::KeyboardModifiers modifiers);

private:
    Spreadsheet::Sheet * sheet;
    bool committing = false;
    mutable bool updating = false;
    mutable bool syncCombo = false;
    mutable QWidget *lastEditor = nullptr;
    QAbstractItemView::EditTrigger editTrigger = QAbstractItemView::NoEditTriggers;

    friend class SheetTableView;
};

}

#endif // SPREADSHEETDELEGATE_H
