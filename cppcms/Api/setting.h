/* 
 * File:   setting.h
 * Author: zhuzx
 *
 * Created on April 15, 2016, 2:46 PM
 */

#ifndef SETTING_H
#define SETTING_H

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppdb/frontend.h>
#include <iostream>
#include <sys/types.h>

class setting : public cppcms::application {
public:
	setting(cppcms::service &srv) : cppcms::application(srv)
	{
		conn_str_ = settings().get<std::string>("api.connection");
		sql_.reset(new cppdb::session());

		dispatcher().assign("/info", &setting::info, this);
		mapper().assign("info", "/info");
	}
	void info();

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

#endif /* SETTING_H */

