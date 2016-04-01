#include <QtCore>
#include <TreeFrogView>
#include "blog.h" 
#include "applicationhelper.h"

class T_VIEW_EXPORT blog_showView : public TActionView
{
  Q_OBJECT
public:
  blog_showView() : TActionView() { }
  blog_showView(const blog_showView &) : TActionView() { }
  QString toString();
};

QString blog_showView::toString()
{
  responsebody.reserve(1611);
  responsebody += QLatin1String("<!DOCTYPE html>\n");
    tfetch(Blog, blog);
  responsebody += QLatin1String("<html>\n<head>\n  <meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />\n  <title>");
  responsebody += THttpUtility::htmlEscape(controller()->name() + ": " + controller()->activeAction());
  responsebody += QLatin1String("</title>\n</head>\n<body>\n<p style=\"color: red\">");
  tehex(error);
  responsebody += QLatin1String("</p>\n<p style=\"color: green\">");
  tehex(notice);
  responsebody += QLatin1String("</p>\n\n<h1>Showing Blog</h1>\n<dt>ID</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.id());
  responsebody += QLatin1String("</dd><br />\n<dt>Title</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.title());
  responsebody += QLatin1String("</dd><br />\n<dt>Body</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.body());
  responsebody += QLatin1String("</dd><br />\n<dt>Created at</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.createdAt());
  responsebody += QLatin1String("</dd><br />\n<dt>Updated at</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.updatedAt());
  responsebody += QLatin1String("</dd><br />\n<dt>Lock Revision</dt><dd>");
  responsebody += THttpUtility::htmlEscape(blog.lockRevision());
  responsebody += QLatin1String("</dd><br />\n\n");
  responsebody += QVariant(linkTo("Edit", urla("edit", blog.id()))).toString();
  responsebody += QLatin1String(" |\n");
  responsebody += QVariant(linkTo("Back", urla("index"))).toString();
  responsebody += QLatin1String("\n\n</body>\n</html>\n");

  return responsebody;
}

Q_DECLARE_METATYPE(blog_showView)
T_REGISTER_VIEW(blog_showView)

#include "blog_showView.moc"
