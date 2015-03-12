/* 
 * File:   main.cpp
 * Author: Dreamszhu
 *
 * Created on March 12, 2015, 8:17 AM
 */

#include <cstdlib>
#include <iostream>
#include <pqxx/pqxx>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
	pqxx::connection c("dbname=eotu host=192.168.1.108 user=eotu password=wx:)123456");
	pqxx::work txn(c);

	pqxx::result r = txn.exec(
			"SELECT id, name "
			"FROM apks "
			); // "WHERE name =" + txn.quote(argv[1])

	if (r.size() != 1) {
		std::cerr
				<< "found " << r.size() << std::endl;
		return 1;
	}

	std::cout << "Updating apk #" << r[0][0].as<int>() << r[0][1].as<string>() << std::endl;

	txn.exec(
			"UPDATE apks "
			"SET version = version + 1 "); // "WHERE id = " + txn.quote(employee_id)

	txn.commit();
	return 0;
}

