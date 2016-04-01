#include "hello.h"

void hello::main(std::string /*url*/)
{
	response().out() <<
		"<html>\n"
		"<body>\n"
		"  <h1>Hello World</h1>\n"
		"</body>\n"
		"</html>\n";
}
