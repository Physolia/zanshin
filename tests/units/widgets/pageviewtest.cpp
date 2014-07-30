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

#include <QtTestGui>

#include <QAbstractItemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QStringListModel>
#include <QTreeView>

#include "widgets/itemdelegate.h"
#include "widgets/pageview.h"

Q_DECLARE_METATYPE(QAbstractItemModel*)

class SlotSpy : public QObject
{
    Q_OBJECT
public slots:
    void addTask(const QString &name)
    {
        taskNames << name;
    }

public:
    QStringList taskNames;
};

class PageViewTest : public QObject
{
    Q_OBJECT
private slots:
    void shouldHaveDefaultState()
    {
        Widgets::PageView page;

        auto centralView = page.findChild<QTreeView*>("centralView");
        QVERIFY(centralView);
        QVERIFY(centralView->isVisibleTo(&page));
        QVERIFY(!centralView->header()->isVisibleTo(&page));
        QVERIFY(qobject_cast<Widgets::ItemDelegate*>(centralView->itemDelegate()));
        QVERIFY(centralView->alternatingRowColors());

        QLineEdit *quickAddEdit = page.findChild<QLineEdit*>("quickAddEdit");
        QVERIFY(quickAddEdit);
        QVERIFY(quickAddEdit->isVisibleTo(&page));
        QVERIFY(quickAddEdit->text().isEmpty());
        QCOMPARE(quickAddEdit->placeholderText(), tr("Type and press enter to add an action"));
    }

    void shouldDisplayListFromPageModel()
    {
        // GIVEN
        QStringListModel model(QStringList() << "A" << "B" << "C" );

        QObject stubPageModel;
        stubPageModel.setProperty("centralListModel", QVariant::fromValue(static_cast<QAbstractItemModel*>(&model)));

        Widgets::PageView page;
        auto centralView = page.findChild<QTreeView*>("centralView");
        QVERIFY(centralView);
        QVERIFY(!centralView->model());

        // WHEN
        page.setModel(&stubPageModel);

        // THEN
        QCOMPARE(centralView->model(), &model);
    }

    void shouldCreateTasksWhenHittingReturn()
    {
        // GIVEN
        SlotSpy stubPageModel;
        Widgets::PageView page;
        page.setModel(&stubPageModel);
        auto quickAddEdit = page.findChild<QLineEdit*>("quickAddEdit");

        // WHEN
        QTest::keyClick(quickAddEdit, Qt::Key_Return); // Does nothing (edit empty)
        QTest::keyClicks(quickAddEdit, "Foo");
        QTest::keyClick(quickAddEdit, Qt::Key_Return);
        QTest::keyClick(quickAddEdit, Qt::Key_Return); // Does nothing (edit empty)
        QTest::keyClicks(quickAddEdit, "Bar");
        QTest::keyClick(quickAddEdit, Qt::Key_Return);
        QTest::keyClick(quickAddEdit, Qt::Key_Return); // Does nothing (edit empty)

        // THEN
        QCOMPARE(stubPageModel.taskNames, QStringList() << "Foo" << "Bar");
    }
};

QTEST_MAIN(PageViewTest)

#include "pageviewtest.moc"