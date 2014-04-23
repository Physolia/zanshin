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

#include <QtTest>

#include <Akonadi/Collection>
#include <Akonadi/Item>

#include <mockitopp/mockitopp.hpp>
#include "testlib/akonadimocks.h"

#include "akonadi/akonaditaskrepository.h"
#include "akonadi/akonadiserializerinterface.h"
#include "akonadi/akonadistorageinterface.h"

using namespace mockitopp;

class AkonadiTaskRepositoryTest : public QObject
{
    Q_OBJECT
private slots:
    void shouldCreateNewItemsOnSave()
    {
        // GIVEN

        // A default collection for saving
        Akonadi::Collection col(42);

        // A task and its corresponding item not existing in storage yet
        Akonadi::Item item(42);
        Domain::Task::Ptr task(new Domain::Task);

        // A mock create job
        auto itemCreateJob = new MockAkonadiJob(this);

        // Storage mock returning the create job
        mock_object<Akonadi::StorageInterface> storageMock;
        storageMock(&Akonadi::StorageInterface::defaultTaskCollection).when().thenReturn(col);
        storageMock(&Akonadi::StorageInterface::createItem).when(item, col)
                                                           .thenReturn(itemCreateJob);

        // Serializer mock returning the item for the task
        mock_object<Akonadi::SerializerInterface> serializerMock;
        serializerMock(&Akonadi::SerializerInterface::createItemFromTask).when(task).thenReturn(item);

        // WHEN
        QScopedPointer<Akonadi::TaskRepository> repository(new Akonadi::TaskRepository(&storageMock.getInstance(),
                                                                                       &serializerMock.getInstance()));
        repository->save(task)->exec();

        // THEN
        QVERIFY(serializerMock(&Akonadi::SerializerInterface::createItemFromTask).when(task).exactly(1));
        QVERIFY(storageMock(&Akonadi::StorageInterface::defaultTaskCollection).when().exactly(1));
        QVERIFY(storageMock(&Akonadi::StorageInterface::createItem).when(item, col).exactly(1));
    }

    void shouldUpdateExistingItemsOnSave()
    {
        // GIVEN

        // A default collection for saving
        Akonadi::Collection col(42);

        // A task and its corresponding item already existing in storage
        Akonadi::Item item(42);
        Domain::Task::Ptr task(new Domain::Task);
        task->setProperty("itemId", item.id());

        // A mock create job
        auto itemModifyJob = new MockAkonadiJob(this);

        // Storage mock returning the create job
        mock_object<Akonadi::StorageInterface> storageMock;
        storageMock(&Akonadi::StorageInterface::updateItem).when(item)
                                                           .thenReturn(itemModifyJob);

        // Serializer mock returning the item for the task
        mock_object<Akonadi::SerializerInterface> serializerMock;
        serializerMock(&Akonadi::SerializerInterface::createItemFromTask).when(task).thenReturn(item);

        // WHEN
        QScopedPointer<Akonadi::TaskRepository> repository(new Akonadi::TaskRepository(&storageMock.getInstance(),
                                                                                       &serializerMock.getInstance()));
        repository->save(task)->exec();

        // THEN
        QVERIFY(serializerMock(&Akonadi::SerializerInterface::createItemFromTask).when(task).exactly(1));
        QVERIFY(storageMock(&Akonadi::StorageInterface::updateItem).when(item).exactly(1));
    }
};

QTEST_MAIN(AkonadiTaskRepositoryTest)

#include "akonaditaskrepositorytest.moc"