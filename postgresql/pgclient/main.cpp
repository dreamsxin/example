/* 
 * File:   main.cpp
 * Author: Dreamszhu
 *
 * Created on March 12, 2015, 8:17 AM
 */

#include <cstdlib>
#include <iostream>
#include <pqxx/pqxx>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;

pqxx::result get_all_apks(pqxx::transaction_base &txn) {
	return txn.exec("SELECT id, name, system, version FROM apks");
}

void add_apk(pqxx::transaction_base &txn, std::string name, std::string system, int version) {
	txn.exec(
			"INSERT INTO apks(name, system, version) "
			"VALUES (" +
			txn.quote(name) + ", " +
			txn.quote(system) + ", " +
			txn.quote(version) +
			")");
}

/*
 * 
 */
int main(int argc, char** argv) {
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("config.ini", pt);

	pqxx::connection c(
			"dbname=" + pt.get<std::string>("database.dbname") +
			" host=" + pt.get<std::string>("database.host") +
			" user=" + pt.get<std::string>("database.user") +
			" password=" + pt.get<std::string>("database.password")
			);
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

	try {
		pqxx::result apks = get_all_apks(txn);

		std::cout
				<< "ID" << "\t"
				<< "Name" << "\t"
				<< "SYSTEM" << "\t"
				<< "VERSION." << std::endl;

		for (
				pqxx::result::const_iterator row = apks.begin();
				row != apks.end();
				++row
				) {
			std::cout
					<< row["id"].as<int>() << "\t"
					<< row["name"].as<std::string>() << "\t"
					<< row["system"].as<std::string>() << "\t"
					<< row["version"].as<int>()
					<< std::endl;
		}

		if (!apks.empty()) {
			std::cout
					<< "That first employee was:" << std::endl
					<< apks[0]["id"].c_str() << "\t"
					<< apks[0]["name"].c_str() << "\t"
					<< apks[0]["system"].c_str() << "\t"
					<< apks[0]["version"].c_str()
					<< std::endl;
		}


		txn.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}

