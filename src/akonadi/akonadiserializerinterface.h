/* This file is part of Zanshin

   Copyright 2014 Kevin Ottens <ervin@kde.org>

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

#ifndef AKONADI_SERIALIZERINTERFACE_H
#define AKONADI_SERIALIZERINTERFACE_H

#include "domain/task.h"
#include "domain/note.h"

namespace Akonadi {

class Item;

class SerializerInterface
{
public:
    SerializerInterface();
    virtual ~SerializerInterface();

    virtual Domain::Task::Ptr createTaskFromItem(Akonadi::Item item) = 0;
    virtual void updateTaskFromItem(Domain::Task::Ptr task, Akonadi::Item item) = 0;
    virtual Akonadi::Item createItemFromTask(Domain::Task::Ptr task) = 0;
    virtual bool isTaskChild(Domain::Task::Ptr task, Akonadi::Item item) = 0;
    virtual QString relatedUidFromItem(Akonadi::Item item) = 0;

    virtual Domain::Note::Ptr createNoteFromItem(Akonadi::Item item) = 0;
    virtual void updateNoteFromItem(Domain::Note::Ptr note, Akonadi::Item item) = 0;
};

}

#endif // AKONADI_SERIALIZERINTERFACE_H