#ifndef HELLO_H
#define HELLO_H

#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>

class hello : public cppcms::application {
public:

	hello(cppcms::service &srv) :
	cppcms::application(srv)
	{
	}
	virtual void main(std::string url);
};

#endif /* HELLO_H */

