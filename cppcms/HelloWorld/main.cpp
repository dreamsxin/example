#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <iostream>
#include <cstdlib>

#include "hello.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
	try {
		cppcms::service srv(argc, argv);

		srv.applications_pool().mount(cppcms::applications_factory<hello>());

		srv.run();
	} catch (std::exception const &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

