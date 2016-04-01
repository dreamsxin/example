#ifndef BLOGCONTROLLER_H
#define BLOGCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT BlogController : public ApplicationController
{
    Q_OBJECT
public:
    BlogController() { }
    BlogController(const BlogController &other);

public slots:
    void index();
    void show(const QString &pk);
    void entry();
    void create();
    void edit(const QString &pk);
    void save(const QString &pk);
    void remove(const QString &pk);

private:
    void renderEntry(const QVariantMap &blog = QVariantMap());
    void renderEdit(const QVariantMap &blog = QVariantMap());
};

T_DECLARE_CONTROLLER(BlogController, blogcontroller)

#endif // BLOGCONTROLLER_H
