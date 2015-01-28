#include <boost/multi_index_container.hpp> 
#include <boost/multi_index/hashed_index.hpp> 
#include <boost/multi_index/member.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream> 
#include <string> 

struct person {
	std::string name;
	int age;

	person(const std::string &n, int a)
	: name(n), age(a) {
	}
};

typedef boost::multi_index::multi_index_container<
person,
boost::multi_index::indexed_by<
boost::multi_index::hashed_non_unique<
boost::multi_index::member<
person, std::string, &person::name
>
>,
boost::multi_index::hashed_non_unique<
boost::multi_index::member<
person, int, &person::age
>
>
>
> person_multi;

void set_age(person &p) {
	p.age = 25;
}

int main() {
	person_multi persons;

	persons.insert(person("Boris", 31));
	persons.insert(person("Boris", 32));
	persons.insert(person("Anton", 35));
	persons.insert(person("Caesar", 25));
	persons.insert(person("Test", 25));

	std::cout << "Boris count " << persons.count("Boris") << std::endl;

	person_multi::iterator it = persons.find("Boris");
	persons.modify(it, set_age);

	person_multi::iterator start;
	person_multi::iterator end;

	boost::tie(start, end) = persons.equal_range("Boris");

	while (start != end) {
		std::cout << start->name << std::endl;
		start++;
	}
	std::cout << "-std=c++11 use range" << std::endl;
	auto range = persons.equal_range("Boris");
	for (auto const& p : boost::make_iterator_range(range.first, range.second))
		std::cout << p.name << "\n";

	std::cout << "Anton count " << persons.count("Anton") << std::endl;

	boost::tie(start, end) = persons.equal_range("Anton");

	while (start != end) {
		std::cout << start->name << std::endl;
		start++;
	}

	const person_multi::nth_index<1>::type &age_index = persons.get<1>();

	std::cout << "Age 25 count " << age_index.count(25) << std::endl;

	person_multi::nth_index<1>::type::iterator startage;
	person_multi::nth_index<1>::type::iterator endage;

	boost::tie(startage, endage) = age_index.equal_range(25);

	while (startage != endage) {
		std::cout << startage->name << std::endl;
		startage++;
	}
	std::cout << "-std=c++11 use range" << std::endl;
	auto& index = persons.get<1>();
	auto range2 = index.equal_range(25);
	for (auto const& p : boost::make_iterator_range(range2.first, range2.second)) {
		std::cout << p.name << "\n";
	}
} 