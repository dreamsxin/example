#ifndef AD_H
#define AD_H

#include <cppcms/json.h>
#include <cppcms/cppcms_error.h>
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppdb/frontend.h>
#include <iostream>
#include <sys/types.h>
#include <booster/posix_time.h>

class ad : public cppcms::application {
public:

	ad(cppcms::service &srv) : cppcms::application(srv)
	{
		conn_str_ = settings().get<std::string>("api.connection");
		sql_.reset(new cppdb::session());

		dispatcher().assign("/list", &ad::list, this);
		mapper().assign("list", "/list");

		dispatcher().assign("/welcome", &ad::welcome, this);
		mapper().assign("welcome", "/welcome");

		dispatcher().assign("/hello", &ad::hello, this);
		mapper().assign("hello", "/hello");

		dispatcher().assign("", &ad::hello, this);
		mapper().assign("");
  
		mapper().root("/api/ad");  
	}
	virtual void main(std::string url);
	void list();
	void welcome();
	void hello();

protected:
	cppdb::session &sql()
	{
		if(!sql_->is_open())
			sql_->open(conn_str_);
		return *sql_;
	}

private:
		std::unique_ptr<cppdb::session> sql_;
		std::string conn_str_;
};

#endif /* AD_H */

