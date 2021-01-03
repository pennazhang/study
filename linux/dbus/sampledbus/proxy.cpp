#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include "proxy.hpp"
class Hello : public com::example::Hello_proxy,
				public DBus::IntrospectableProxy,
				public DBus::ObjectProxy
{
public:
	Hello(DBus::Connection &connection, const char *path, const char *name):
	DBus::ObjectProxy(connection, path, name)
	{
	}
};

DBus::BusDispatcher dispatcher;

int main(int argc, char** argv)
{
	DBus::default_dispatcher = &dispatcher;
	DBus::Connection bus = DBus::Connection::SessionBus();

	Hello hello(bus, "/com/example/Hello", "com.example.Hello");

	std::cout << "Adaptor version: " << hello.getversion() << endl << endl;
	
	if(argc>=2)
		hello.SayHello(argv[1]);
	else
		hello.SayHello("world");	
		
	printf("Returned...\n");
	
	return 0;
}
