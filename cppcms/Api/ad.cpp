#include "ad.h"

void ad::main(std::string url)
{
	application::main(url);
}

void ad::list()
{
	cppdb::result res = sql() << "SELECT id, title, description, icon, pkgname, payout FROM ads LIMIT 10";

	response().out() << "<html>\n<body>\n<h1>list</h1>\n<ul>\n";
	while(res.next()) {
		int id;
		double payout;
		std::string title, description, icon, pkgname;
		res >> id >> title >> description >> icon >> pkgname >> payout;
		response().out() << "<li>" << id << title << description << icon << pkgname << payout << "</li>\n";
	}

	response().out() <<
		"</url>\n</body>\n"
		"</html>\n";
}

void ad::welcome()
{
	response().out() <<
		"<html>\n"
		"<body>\n"
		"  <h1>welcome</h1>\n"
		"</body>\n"
		"</html>\n";
}

void ad::hello()
{
	response().out() <<
		"<html>\n"
		"<body>\n"
		"  <h1>hello</h1>\n"
		"</body>\n"
		"</html>\n";
}
