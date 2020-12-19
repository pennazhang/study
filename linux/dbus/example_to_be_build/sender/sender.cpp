#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

int send_a_signal(char * sigvalue)
{
	DBusError err;
	DBusConnection *connection;
	DBusMessage *msg;
	DBusMessageIter arg;
	dbus_uint32_t serial = 0;
	int ret;
	
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
		return (-1);		
	}
	
	/* 2. Allocate a well-known name for the connection with D-Bus */
#if 1
	ret = dbus_bus_request_name(connection, "test.singal.source", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if (dbus_error_is_set(&err))
	{
		fprintf(stderr, "Name Err: %s\n", err.message);
		dbus_error_free(&err);
	}
	
	if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	{
		return (-1);
	}
#endif		

	/* 3. Create a Message */
	msg = dbus_message_new_signal("/test/signal/Object", "test.signal.Type", "Test");
	if (msg == NULL)
	{
		fprintf(stderr, "Message is NULL");
		return (-1);
	}
	
	/* 4. Fill the message */
	dbus_message_iter_init_append(msg, &arg);
	if (!dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &sigvalue))
	{
		fprintf(stderr, "Out of Memory");
		return (-1);
	}
	
	/* 5. Send the message */
	if (!dbus_connection_send(connection, msg, &serial))
	{
		fprintf(stderr, "Out of Memory");
		return (-1);
	}
	
	dbus_connection_flush(connection);
	printf("Signal Send");
	
	/* 6. Release Memory */
	dbus_message_unref(msg);
	return (0);
}

int main(int argc, char **argv)
{
	send_a_signal("Hello, world!");
	return (0);
}

	
	
	
	
	
	
	
	
	
	
	