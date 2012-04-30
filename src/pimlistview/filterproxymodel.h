/* This file is part of Zanshin Todo.

   Copyright 2011 Christian Mollekopf <chrigi_1@fastmail.fm>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.
*/

#ifndef FILTERPROXYMODEL_H
#define FILTERPROXYMODEL_H

#include <QString>
#include <QSortFilterProxyModel>
#include <QModelIndex>
#include "pimitemmodel.h"

namespace Akonadi {
class ItemSearchJob;
}

/**
 * Filtering of items
 */
class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit FilterProxyModel(QObject *parent = 0);
    virtual ~FilterProxyModel();

    void setFilterString(const QString &);
    
    virtual void setSourceModel(QAbstractItemModel* sourceModel);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QString m_filterString;
    class SearchFilterCache *m_cache;
};

#endif // NOTESORTFILTERPROXYMODEL_H

