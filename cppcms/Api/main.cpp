#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/mount_point.h>
#include <iostream>
#include <cstdlib>

#include "ad.h"
#include "setting.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
	try {
		cppcms::service srv(argc, argv);

		srv.applications_pool().mount(cppcms::applications_factory<ad>(), cppcms::mount_point("/api/ad"));
		srv.applications_pool().mount(cppcms::applications_factory<setting>(), cppcms::mount_point("/api/setting"));

		srv.run();
	} catch (std::exception const &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

