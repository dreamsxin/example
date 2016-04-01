#include <TreeFrogModel>
#include "blog.h"
#include "blogobject.h"

Blog::Blog()
    : TAbstractModel(), d(new BlogObject)
{
    d->id = 0;
    d->lock_revision = 0;
}

Blog::Blog(const Blog &other)
    : TAbstractModel(), d(new BlogObject(*other.d))
{ }

Blog::Blog(const BlogObject &object)
    : TAbstractModel(), d(new BlogObject(object))
{ }

Blog::~Blog()
{
    // If the reference count becomes 0,
    // the shared data object 'BlogObject' is deleted.
}

int Blog::id() const
{
    return d->id;
}

QString Blog::title() const
{
    return d->title;
}

void Blog::setTitle(const QString &title)
{
    d->title = title;
}

QString Blog::body() const
{
    return d->body;
}

void Blog::setBody(const QString &body)
{
    d->body = body;
}

QString Blog::createdAt() const
{
    return d->created_at;
}

QString Blog::updatedAt() const
{
    return d->updated_at;
}

int Blog::lockRevision() const
{
    return d->lock_revision;
}

Blog &Blog::operator=(const Blog &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Blog Blog::create(const QString &title, const QString &body)
{
    BlogObject obj;
    obj.title = title;
    obj.body = body;
    if (!obj.create()) {
        return Blog();
    }
    return Blog(obj);
}

Blog Blog::create(const QVariantMap &values)
{
    Blog model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Blog Blog::get(int id)
{
    TSqlORMapper<BlogObject> mapper;
    return Blog(mapper.findByPrimaryKey(id));
}

Blog Blog::get(int id, int lockRevision)
{
    TSqlORMapper<BlogObject> mapper;
    TCriteria cri;
    cri.add(BlogObject::Id, id);
    cri.add(BlogObject::LockRevision, lockRevision);
    return Blog(mapper.findFirst(cri));
}

int Blog::count()
{
    TSqlORMapper<BlogObject> mapper;
    return mapper.findCount();
}

QList<Blog> Blog::getAll()
{
    return tfGetModelListByCriteria<Blog, BlogObject>(TCriteria());
}

QJsonArray Blog::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<BlogObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<BlogObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Blog(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *Blog::modelData()
{
    return d.data();
}

const TModelObject *Blog::modelData() const
{
    return d.data();
}
