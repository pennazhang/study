#include <json/json.h>
#include "commonInterface.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include "utility.h"
#include "helloSetting.h"

using namespace std;

CommonInterface::CommonInterface(DBus::Connection &connection, const char *ObjectPath)
: DBus::ObjectAdaptor(connection, ObjectPath)
{
}

typedef int commonInterfaceFunctionPtr(const std::string& cmdArg, std::string& outputInfo);

std::string CommonInterface::get(const std::vector< std::string >& keyList)
{
    Json::Value obj(Json::objectValue);
	int i = 0;
	for(auto& it : keyList)
    {
        auto serverNumber = to_string(i);
        i++;
        std::string server = "server"+serverNumber;
        obj[it] = server;
		cout << "get "<< it << " = " << server << endl;
    }
	return (obj.toStyledString());
}

std::map<std::string, commonInterfaceFunctionPtr *> s_setMap = 
{
	{std::string("ExecuteIRCommand"), sendIR },
	{std::string("irCommandHoldoff"), setCommandHoldOff },
	{std::string("irRepeatHoldoff"), setRepeatHoldOff },
	{std::string("irCommandID"), setIRCommandID },
	{std::string("irCommands"), setIRCommands },
	{std::string("irEnablePassThrough"), enablePassThrough },
	{std::string("irPassThroughPeerIP"), setPeerIPAddress },
};

std::string CommonInterface::set(const std::string& jsonPair)
{
	Json::Value jsonValue;
	std::string outputInfo;
	int result;

	if (stringToJson(jsonPair, jsonValue) == STATUS_OK)
	{
//		dumpJsonValue(jsonValue);
		
		Json::Value::Members members = jsonValue.getMemberNames();
		for (auto command : members) 
    	{
			std::string value = jsonValue.operator[](command).toStyledString();
			logInfo("DBus Set: %s = %s", command.c_str(), value.c_str());
			
			for (const auto &item: s_setMap)
			{
				if (item.first == command)
				{
					commonInterfaceFunctionPtr *pFunc = item.second;
					result = (*pFunc)(value, outputInfo);
					return (resultInfo(result, outputInfo.c_str()));
				}
			}
		} 
	}
	return (resultInfo(STATUS_ERROR, "Invalid param"));
}

std::map<std::string, commonInterfaceFunctionPtr *> s_commandMap = 
{
//	{std::string("sendirraw"), sendIRRaw },
//	{std::string("sendir"), sendIR },
//	{std::string("testIR"), testIR },
//	{std::string("irrepeat"), repeatIR },
//	{std::string("irstop"), stopIR },
};

std::string CommonInterface::daemonControl(const std::string& cmd, const std::string& jsonParam)
{
	Json::Value obj;
	int result;
	std::string outputInfo, cmdArg;

	if (getStringFromJsonString(jsonParam, "parameter", cmdArg) == STATUS_ERROR)
	{
		return (resultInfo(STATUS_ERROR, "No parameter"));
	}
	else
	{
		trim(cmdArg, "\" \t\n\r\f\v");
		logInfo("cmd = %s, cmdArg = %s", cmd.c_str(), cmdArg.c_str());
	}

    for (const auto &item: s_commandMap)
    {
		if (item.first == cmd)
		{
			commonInterfaceFunctionPtr *pFunc = item.second;
			result = (*pFunc)(cmdArg, outputInfo);
			return (resultInfo(result, outputInfo.c_str()));
		}
	}
	return (resultInfo(STATUS_ERROR, "Invalid command!"));
}
