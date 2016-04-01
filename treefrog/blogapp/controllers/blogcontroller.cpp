#include "blogcontroller.h"
#include "blog.h"


BlogController::BlogController(const BlogController &)
    : ApplicationController()
{ }

void BlogController::index()
{
    QList<Blog> blogList = Blog::getAll();
    texport(blogList);
    render();
}

void BlogController::show(const QString &pk)
{
    auto blog = Blog::get(pk.toInt());
    texport(blog);
    render();
}

void BlogController::entry()
{
    renderEntry();
}

void BlogController::create()
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto form = httpRequest().formItems("blog");
    auto blog = Blog::create(form);
    if (!blog.isNull()) {
        QString notice = "Created successfully.";
        tflash(notice);
        redirect(urla("show", blog.id()));
    } else {
        QString error = "Failed to create.";
        texport(error);
        renderEntry(form);
    }
}

void BlogController::renderEntry(const QVariantMap &blog)
{
    texport(blog);
    render("entry");
}

void BlogController::edit(const QString &pk)
{
    auto blog = Blog::get(pk.toInt());
    if (!blog.isNull()) {
        session().insert("blog_lockRevision", blog.lockRevision());
        renderEdit(blog.toVariantMap());
    } else {
        redirect(urla("entry"));
    }
}

void BlogController::save(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    QString error;
    int rev = session().value("blog_lockRevision").toInt();
    auto blog = Blog::get(pk.toInt(), rev);
    if (blog.isNull()) {
        error = "Original data not found. It may have been updated/removed by another transaction.";
        tflash(error);
        redirect(urla("edit", pk));
        return;
    }

    auto form = httpRequest().formItems("blog");
    blog.setProperties(form);
    if (blog.save()) {
        QString notice = "Updated successfully.";
        tflash(notice);
        redirect(urla("show", pk));
    } else {
        error = "Failed to update.";
        texport(error);
        renderEdit(form);
    }
}

void BlogController::renderEdit(const QVariantMap &blog)
{
    texport(blog);
    render("edit");
}

void BlogController::remove(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto blog = Blog::get(pk.toInt());
    blog.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_REGISTER_CONTROLLER(blogcontroller)
