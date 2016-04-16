#include "ad.h"

void ad::main(std::string url)
{
	try {
		application::main(url);
	}
	catch(cppcms::cppcms_error const &e)
	{
		response().set_content_header("application/json");
		response().out() << "{\"status\":\"error\",\"message\":" << e.what() << ",\"data\":null}";
	}
}

void ad::list()
{
	std::pair<void *,size_t> post_data = request().raw_post_data();
	std::istringstream ss(std::string(reinterpret_cast<char const *>(post_data.first),post_data.second));
	std::tm now = booster::ptime::local_time(booster::ptime::now());
	std::string useragent = request().http_user_agent();
	cppcms::json::value request;
	std::string source_code, project_code;
	int phone_id;
	if(!request.load(ss, true)) {
		throw cppcms::cppcms_error("Invalid JSON");
	}

	if (request.type("phone_id") != cppcms::json::is_number) {
		throw cppcms::cppcms_error("Invalid phone_id");
	}

	phone_id = request.get<int>("phone_id");
	if(phone_id > 0) {
		cppdb::result phone = sql() << "SELECT name FROM phones WHERE id=?" << phone_id << cppdb::row;
		if(phone.empty()) {
			throw cppcms::cppcms_error("Invalid phone_id");
			return;
		}
	}

	if (request.type("source_code") != cppcms::json::is_string) {
		throw cppcms::cppcms_error("Invalid source_code");
	}
	source_code = request.get<std::string>("source_code");

	if (request.type("project_code") != cppcms::json::is_string) {
		throw cppcms::cppcms_error("Invalid project_code");
	}
	project_code = request.get<std::string>("project_code");
	

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
