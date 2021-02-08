#include "propsgs-server.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

static const char *PROPS_SERVER_NAME = "org.freedesktop.DBus.Examples.Properties";
static const char *PROPS_SERVER_PATH = "/org/freedesktop/DBus/Examples/Properties";

PropsServer::PropsServer(DBus::Connection &connection)
: DBus::ObjectAdaptor(connection, PROPS_SERVER_PATH)
{
	Version = 1;
	Message = "default message";
}

void PropsServer::on_set_property
	(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	std::cout << "on_set_property called here, property = " << property << std::endl;
//	return;
	
	if (property == "Message")
	{
#if 0		
		std::cout << "'Message' has been changed\n";

		std::string msg = value;
		std::cout << "call MessageChanged begin...\n";
		this->MessageChanged(msg);
		std::cout << "call MessageChanged end...\n";
		sleep(5);
#endif
		std::string msg = value;
		cout << "value = " << msg << endl;
	}
	if (property == "Data")
	{
//		std::cout << "'Data' has been changed\n";

//		string data = value;
		double data = value;
		cout << "value = " << data << endl;
//		this->DataChanged(data);
	}
}

DBus::BusDispatcher dispatcher;

void niam(int sig)
{
	dispatcher.leave();
}

int main()
{
	signal(SIGTERM, niam);
	signal(SIGINT, niam);

	DBus::default_dispatcher = &dispatcher;

	DBus::Connection conn = DBus::Connection::SessionBus();
	conn.request_name(PROPS_SERVER_NAME);

	PropsServer server(conn);

	dispatcher.enter();

	return 0;
}
