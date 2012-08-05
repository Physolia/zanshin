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


#ifndef ABSTRACTPIMITEM_H
#define ABSTRACTPIMITEM_H

#include <Akonadi/Item>
#include <KDateTime>
#include <QStringList>

class KJob;

namespace Akonadi {
    class Monitor;
    class Session;
}

struct PimItemTreeNode;

struct PimItemTreeNode {
    PimItemTreeNode(const QByteArray &uid, const QString &name = QString(), const QList<PimItemTreeNode> &parentNodes = QList<PimItemTreeNode>());
    QByteArray uid;
    QString name;
    QList<PimItemTreeNode> parentNodes;
};

struct PimItemRelation
{
  enum Type {
    Invalid,
    Project,
    Context,
    Topic
  };
  
  PimItemRelation(Type type, const QList<PimItemTreeNode> &parentNodes);
  PimItemRelation();
  QList<PimItemTreeNode> parentNodes;

  //     QDateTime timestamp; //for merging

  Type type;
};

PimItemRelation relationFromXML(const QByteArray &xml);
QString relationToXML(const PimItemRelation &rel);


/**
 * A wrapper around akonadi item
 *
 * There are two subclasses for Notes and Incidences
 *
 * The default strategy is, to avoid loading the payload if possible (by using available attributes),
 * and loading the whole payload only as needed. This does not happen automatically though, if you want to access
 * some values which are only in the payload, fetch it first via fetchPayload.
 *
 */
class AbstractPimItem : public QObject
{
    Q_OBJECT

public:
    AbstractPimItem(QObject *parent = 0);
    AbstractPimItem(const Akonadi::Item &, QObject *parent = 0);
    /**
     * Copy Constructor used to create a new Item from with the same content as another item
     *
     * Copies only the fields which are accessible in all item types:
     * -title
     * -text
     *
     * creation date and last modified date are not copied
     */
    AbstractPimItem(AbstractPimItem &item, QObject* parent = 0);
    virtual ~AbstractPimItem();

    enum ItemType {
        None = 0,
        Unknown = 1,
        Note = 2,
        //Calendar Items
        Event = 4,
        Todo = 8,
        Journal = 16,
        Incidence = Event | Todo | Journal, //All calendar Items
        All = Note | Event | Todo | Journal
    };
    Q_DECLARE_FLAGS(ItemTypes, ItemType)

    /**
     * FIXME this works only if the mimetype of the akonadi item has been saved already
     */
    static ItemType itemType(const Akonadi::Item &);
    virtual ItemType itemType() = 0;

    virtual QString mimeType() = 0 ;
    static QString mimeType(ItemType);
    //Returns a list of all supported mimetypes
    static QStringList mimeTypes();

    virtual QString getUid();
    virtual void setText(const QString &, bool isRich = false);
    virtual QString getText();
    virtual void setTitle(const QString &, bool isRich = false);
    virtual QString getTitle();
    virtual void setCreationDate(const KDateTime &);
    virtual KDateTime getCreationDate();
    virtual KDateTime getLastModifiedDate();
    virtual QString getIconName() = 0;
    /**
     * Note: last modified
     * Todo: todo due date
     * Event: start date
     */
    virtual KDateTime getPrimaryDate() = 0;

    enum ItemStatus {
        Complete = 1,
        NotComplete,
        Now,
        Later,
        Attention
    };

    /**
     * Note: Always Later
     * Todo: set by user (priority)
     * Event: In Future/ Today/ Passed
     */
    virtual ItemStatus getStatus() const = 0;

    virtual void setRelations(const QList<PimItemRelation> &) = 0;
    virtual QList<PimItemRelation> getRelations() = 0;
    virtual void setCategories(const QStringList &);
    virtual QStringList getCategories();

    /**
     * this will fetch the payload if not already fetched,
     * and emit payloadFetchComplete on completion
     */
    void fetchPayload(bool blocking = false);
    bool payloadFetched();
    virtual bool hasValidPayload() = 0;

    const Akonadi::Item &getItem() const;

    /**
     * store the item, and update our copy afterwards
     *
     * This does not emit changed() for this AbstractPimItem,
     * but if there are other AbstractPimItem refering to the same akonadi item,
     * changed will be emitted there
     * 
     * If the item is not yet created in the akonadi store (invalid id), this will just update the payload of the item, but not save it to the akonadi store.
     *
     * If subsequent writes are needed, the monitor must be enabled to keep our copy up to date, otherwise there will be conflicts.
     */
    void saveItem();

    /**
     * enable monitor for the given item, so changed() is emitted,
     * always do this if an item is used over a period of time, and you plan to save it back,
     * since it could always change in the background (modified by another application, etc.)
     *
     * This will also enable the removed signal
     */
    void enableMonitor();

    bool textIsRich();
    bool titleIsRich();

    enum ChangedPart {
        Title = 0x01,
        Text = 0x02,
        CreationDate = 0x04,
        LastModifiedDate = 0x08,
        Payload = 0x10, //The payload of the akonadi item has changed in any way
        AllParts = Title|Text|CreationDate|LastModifiedDate|Payload
    };
    Q_DECLARE_FLAGS(ChangedParts, ChangedPart)
    
signals:
    void payloadFetchComplete();
    /**
     * emitted if the akonadi item was changed from somwhere else than this instance of AbstractPimItem
     */
    void changed(AbstractPimItem::ChangedParts);
    /**
     * emitted after item was removed from storage
     */
    void removed();

protected:
    QString m_uid;
    QString m_text;
    QString m_title;
    KDateTime m_creationDate;

    /**
     * sync data to akonadi item
     */
    virtual void commitData() = 0;
    /**
     * sync data from akonadi item
     */
    virtual void fetchData() = 0;

    Akonadi::Item m_item;

    bool m_dataFetched;
    bool m_textIsRich; //if content is rich
    bool m_titleIsRich;

private slots:
    void itemFetchDone(KJob *job);

    /**
     * Update our copy, after a an external modification (update local variables) and emti changed signal
     */
    void updateItem(const Akonadi::Item &, const QSet<QByteArray>&);
    /**
     * update item after akonadi item was modified from this instance (local variables are already up to date)
     */
    //void itemModified(const Akonadi::Item &);
    void modifyDone( KJob *job );

private:
    Q_DISABLE_COPY(AbstractPimItem);

    Akonadi::Monitor *m_monitor;
    bool m_itemOutdated;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractPimItem::ItemTypes)
Q_DECLARE_METATYPE(AbstractPimItem::ItemTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractPimItem::ChangedParts)
Q_DECLARE_METATYPE(AbstractPimItem::ChangedParts)

#endif // ABSTRACTPIMITEM_H
