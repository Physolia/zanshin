/* This file is part of Zanshin

   Copyright 2015 Kevin Ottens <ervin@kde.org>

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

#include "akonadifakedata.h"

#include <algorithm>

using namespace Testlib;

Akonadi::Collection::List AkonadiFakeData::collections() const
{
    return m_collections.values();
}

Akonadi::Collection::List AkonadiFakeData::childCollections(Akonadi::Collection::Id parentId) const
{
    if (!m_childCollections.contains(parentId))
        return {};

    const auto ids = m_childCollections.value(parentId);
    auto result = Akonadi::Collection::List();
    std::transform(std::begin(ids), std::end(ids),
                   std::back_inserter(result),
                   [this] (Akonadi::Collection::Id id) {
                       Q_ASSERT(m_collections.contains(id));
                       return m_collections.value(id);
                   });
    return result;
}

Akonadi::Collection AkonadiFakeData::collection(Akonadi::Collection::Id id) const
{
    if (!m_collections.contains(id))
        return Akonadi::Collection();

    return m_collections.value(id);
}

void AkonadiFakeData::createCollection(const Akonadi::Collection &collection)
{
    Q_ASSERT(!m_collections.contains(collection.id()));
    m_collections[collection.id()] = collection;

    const auto parent = collection.parentCollection();
    const auto parentId = parent.isValid() ? parent.id()
                                           : Akonadi::Collection::root().id();

    m_childCollections[parentId] << collection.id();
}
