#pragma once

#include <thread>
#include <vector>

#include "BaseApplication.h"

namespace RakNet
{
	class RakPeerInterface;
}

class BasicNetworkingApplication : public BaseApplication
{
public:
	BasicNetworkingApplication();
	~BasicNetworkingApplication();

	virtual bool startup();

	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

	void AppendConsoleLine(std::string a_line);
private:
	RakNet::RakPeerInterface* m_pPeerInterface;

	std::string m_ip = "10.17.21.218";//"10.17.15.91";//"127.0.0.1";
	unsigned short m_port = 5456;
	const unsigned int MAX_MESSAGE_HISTORY = 22;
	const unsigned int MAX_MSG_LENGTH = 61;
	const char COLOUR_CHAR = '~';

	// If the server goes a few seconds without responding, reconnect.
	const float MAX_HEARTBEAT_TIME = 5.0f;

	//const long m_lastPingTime

	bool m_keepRunning;

	std::string m_currMsg;
	std::vector<std::string> m_consoleLines;

	std::vector<std::thread> m_pingThreads;

	// @brief Initialise the connection
	void HandleNetworkConnection();
	void InitialiseClientConnection();

	// @brief Handle incoming packets
	void HandleNetworkMessages();

	// @brief Hax the server
	void SendServerPing(int a_slot);

	void SendClientInput();

	void DrawLines();

	// @brief Attempts to reconnect on a few second cool-down.
	void AttemptReconnect();

	void CheckPing(const char* a_str);

	// @brief Hax the server advanced
	void DDOSServer(int a_slot);

	bool TestCommand(const char* a_str);

	// @brief Parses the command-prompt colour codes in a text and prints it coloured.
	void PrintColouredLine(std::string a_line);
};