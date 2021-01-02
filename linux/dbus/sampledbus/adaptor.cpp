#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <dbus-c++/connection.h>
#include <unistd.h>
#include "adaptor.hpp"

#define VERSION  "1.0.0.0"

class Hello : public com::example::Hello_adaptor,
				public DBus::IntrospectableAdaptor,
				public DBus::ObjectAdaptor
{
public:
	Hello(DBus::Connection &connection):
		DBus::ObjectAdaptor(connection, "/com/example/Hello")
	{
	}

	virtual void SayHello(const std::string& name)
	{
		std::cout << "Hello '" << name << "'" << std::endl;
		for (int i = 0; i < 10; i++)
		{
			sleep(1);
		}
	}
	    
	virtual std::string getversion()
    {
        std::string version(VERSION);
        return version;
    }
};
DBus::BusDispatcher dispatcher;

int main()
{
	DBus::default_dispatcher = &dispatcher;
	DBus::Connection bus = DBus::Connection::SessionBus();

	bus.request_name("com.example.Hello");

	Hello hello(bus);

	dispatcher.enter();
	
	return 0;
}
