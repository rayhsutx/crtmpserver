/*
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application/clientapplicationmanager.h"

#ifdef HAS_PROTOCOL_CLI
#include "cliappprotocolhandler.h"
#include "adminapplication.h"
using namespace app_admin;

CLIAppProtocolHandler::CLIAppProtocolHandler(Variant &configuration)
: BaseCLIAppProtocolHandler(configuration) {

}

CLIAppProtocolHandler::~CLIAppProtocolHandler() {
}

bool CLIAppProtocolHandler::ProcessMessage(BaseProtocol *pFrom, Variant &message) {
	INFO("message:\n %s", STR(message.ToString()));
	if (!message.HasKey("command"))
		return SendFail(pFrom, "no command");

	bool hasParameters = message["parameters"] == V_MAP;
	string command = message["command"];
	Variant params = message["parameters"];

	if (command == "add")
	{
		if (!hasParameters)
			return SendFail(pFrom, "no parameters");

		if (params.HasKey("uri") == false)
			return SendFail(pFrom, "no URI");
		else if (params.HasKey("streamName") == false)
			return SendFail(pFrom, "no stream name");

		string uri = params["uri"];
		string streamName = params["streamName"];
		if (uri.size() == 0 || streamName.size() == 0)
			return SendFail(pFrom, "invalid URI or stream name");

		BaseClientApplication* application = ClientApplicationManager::FindAppByName("flvplayback");
		if (application != NULL)
		{
			Variant streamConfigs;
			streamConfigs.IsArray(false);
			streamConfigs["uri"] = uri;
			streamConfigs["localStreamName"] = streamName;
			streamConfigs["forceTcp"] = true;

			if (application->PullExternalStream(streamConfigs))
			{
				INFO("added external stream %s", STR(uri));
				return SendSuccess(pFrom, "OK", message);
			}
			else
			{
				string desc;
				desc += "failed to add stream";
				desc += STR(streamName);
				desc += "=";
				desc += STR(uri);
				return SendFail(pFrom, desc);
			}
		}
	}
	else if (command == "exit")
	{
		pFrom->EnqueueForDelete();
		return SendSuccess(pFrom, "OK", message);
	}

	return SendFail(pFrom, "Unknown command");
}
#endif	/* HAS_PROTOCOL_CLI */
