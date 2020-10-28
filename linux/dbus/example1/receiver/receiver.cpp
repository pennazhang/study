#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

void listen_signal()
{
	DBusError err;
	DBusConnection *connection;
	DBusMessage *msg;
	DBusMessageIter arg;
	int ret;
	char *sigvalue;
	
	/* 1. Create Connection with D-Bus */
	dbus_error_init(&err);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err))
	{
		fprintf(stderr, "ConnectionErr : %s\n", err.message);
		dbus_error_free(&err);
	}
	
	if (connection == NULL)
	{
		return;		
	}
	
	/* 2. Allocate a well-known name for the connection with D-Bus */
#if 1
	ret = dbus_bus_request_name(connection, "test.singal.dest", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if (dbus_error_is_set(&err))
	{
		fprintf(stderr, "Name Err: %s\n", err.message);
		dbus_error_free(&err);
	}
	
	if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	{
		return;
	}
#endif		

	/* 3. Notify the D-Bus daemon to listen to the signal: test.signal.Type:  */
	dbus_bus_add_match(connection, "type='signal', interface='test.signal.Type'", &err);
	dbus_connection_flush(connection);
	
	if (dbus_error_is_set(&err))
	{
		fprintf(stderr, "Match Err: %s\n", err.message);
		dbus_error_free(&err);
	}
	
	/* 4. Try to get the signal from the connection once per second. */
	while (1)
	{
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection);
		if (msg == NULL)
		{
			sleep(1);
			continue;
		}
		
		if (dbus_message_is_signal(msg, "test.signal.Type", "Test"))
		{
			if (!dbus_message_iter_init(msg, &arg))
			{
				fprintf(stderr, "Message has no param!");
			}
			else if (dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING)
			{
				fprintf(stderr, "Param is not string");
			}
			else
			{
				dbus_message_iter_get_basic(&arg, &sigvalue);
			}
			printf("Recieve Signal Value : %s\n", sigvalue);
		}
		dbus_message_unref(msg);
	}
}

int main(int argc, char **argv)
{
	printf("---------- Start Listen Signal ----------\n");
	listen_signal();
	return (0);
}
