/**
	File: BasicNetworking.cpp
	Author: Patrick Ferguson
	Created: 8/05/2015
	Description: Connects to the server and chats.
*/

#include "BasicNetworking.h"

#include <iostream>
#include <string>
#include <conio.h>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GameMessages.h"

BasicNetworkingApplication::BasicNetworkingApplication()
{

}

BasicNetworkingApplication::~BasicNetworkingApplication()
{

}

bool BasicNetworkingApplication::startup()
{
	//Setup the basic window
	//createWindow("Client Application", 1280, 720);

	std::string tempInput = "";
	std::cout << "Enter IP (default is " << m_ip << "): ";
	std::getline(std::cin, tempInput);
	if (strcmp(tempInput.c_str(), "") != 0)
	{
		m_ip = tempInput;
	}

	std::cout << "Enter port (default is " << m_port << "): ";
	std::getline(std::cin, tempInput);
	if (strcmp(tempInput.c_str(), "") != 0)
	{
		m_port = (unsigned short) strtoul(tempInput.c_str(), NULL, 0);
	}

	m_keepRunning = true;
	m_currMsg = "";
	HandleNetworkConnection();

	return true;
}

void BasicNetworkingApplication::shutdown()
{

}

bool BasicNetworkingApplication::update(float deltaTime)
{
	if (m_keepRunning)
	{
		HandleNetworkMessages();

		// Send messages
		SendClientInput();
	}
	return m_keepRunning;
}

void BasicNetworkingApplication::draw()
{

}

// @brief Initialise the connection
void BasicNetworkingApplication::HandleNetworkConnection()
{
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	InitialiseClientConnection();
}

void BasicNetworkingApplication::InitialiseClientConnection()
{
	// Create socket descriptor, this will be filled by the server
	RakNet::SocketDescriptor sd;

	// Connect the server, only one connection to this host
	m_pPeerInterface->Startup(32000, &sd, 1);

	AppendConsoleLine("Connecting to the server: " + m_ip + '\n');

	// Now attempt to connect
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(m_ip.c_str(), m_port, nullptr, 0);

	// See if the connection was a success
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		AppendConsoleLine("Unable to start connection, error number:" + res + '\n');
	}
}

// @brief Handle incoming packets
void BasicNetworkingApplication::HandleNetworkMessages()
{
	RakNet::Packet* pData;

	// Process all incoming packets
	for (pData = m_pPeerInterface->Receive();
		pData;
		m_pPeerInterface->DeallocatePacket(pData), pData = m_pPeerInterface->Receive())
	{
		// Read the message
		RakNet::BitStream bsIn(pData->data, pData->length, false);

		// Process header
		switch (pData->data[0])
		{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				AppendConsoleLine("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				AppendConsoleLine("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				AppendConsoleLine("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				AppendConsoleLine("Our connection request has been accepted.\n");

				/*for (int i = 0; i < 1024; ++i)
				{
					m_pingThreads.push_back(std::thread(&BasicNetworkingApplication::SendServerPing, this, i));
				}*/
				//m_pingThreads.push_back(std::thread(&BasicNetworkingApplication::SendServerPing, this, 0));
				break;
			}
			case ID_CONNECTION_ATTEMPT_FAILED:
				AppendConsoleLine("Failed to connect to the server!\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				AppendConsoleLine("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				AppendConsoleLine("We have been disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
				BasicNetworkingApplication::AttemptReconnect();
				AppendConsoleLine("Conection lost.\n");
				break;
			case ID_SERVER_TEXT_MESSAGE:
			// Recieved a random server message
			{
				// Ignore the header
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				RakNet::RakString str;
				bsIn.Read(str);
				CheckPing(str.C_String());
				break;
			}
			case ID_SERVER_CHAT_MESSAGE:
			{
				// Ignore the header
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// Beep as a notification
				std::cout << '\a';

				// Print out the chat message
				RakNet::RakString str;
				bsIn.Read(str);
				AppendConsoleLine(std::string(str.C_String()) + '\n');
				break;
			}
			case ID_CHAT_DUMP:
			{
				// Process each line

				// Ignore the header
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				// Grab the number of messages
				unsigned int numLines = 0;
				bsIn.Read(numLines);

				// Grab each message
				if (numLines > 0)
				{
					AppendConsoleLine("Receiving chat dump, number of lines : " + std::to_string(numLines) + '\n');
					RakNet::RakString tempMsg;
					for (unsigned int i = 0; i < numLines; ++i)
					{
						bsIn.Read(tempMsg);
						AppendConsoleLine(std::string(tempMsg.C_String()) + '\n');
					}
				}
				else
				{
					AppendConsoleLine("Recieved empty chat dump!\n");
				}

				RakNet::RakString str;
				bsIn.Read(str);
				AppendConsoleLine(std::string(str.C_String()) + '\n');
				break;
			}
			default:
				AppendConsoleLine("Recieved a message with unknown id: " + pData->data[0] + '\n');
				break;
		}
	}
}

void BasicNetworkingApplication::SendServerPing(int a_slot)
{
	// Begin mad hax
	while (true)
	{
		RakNet::BitStream bs;
		std::string str;
		bs.Write((RakNet::MessageID) EGameMessages::ID_CLIENT_TEXT_MESSAGE);
		str = "Ping! " + std::string(m_pPeerInterface->GetLocalIP(0));
		bs.Write(str.c_str());

		m_pPeerInterface->Send(&bs, LOW_PRIORITY, UNRELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		std::this_thread::sleep_for(std::chrono::seconds(1));

		//std::cout << "Ping: " << m_pPeerInterface->GetAveragePing() << std::endl;
	}
}

void BasicNetworkingApplication::SendClientInput()
{
	// Check if a key was pressed
	if (_kbhit())
	{
		// Get which key was pressed
		int ch = _getch();
		if (ch != 0)
		{
			// Check for enter
			if (ch != '\r')
			{
				if (ch != '\b')
				{
					// Only add characters if not at maximum
					if (m_currMsg.size() <= MAX_MSG_LENGTH)
					{
						// Append the string to the message
						m_currMsg += ch;
					}
				}
				else
				{
					if (m_currMsg.size() > 0)
					{
						// Backspace was pressed
						m_currMsg.pop_back();
					}
				}

				// Update the text output
				DrawLines();
			}
			else
			{
				// Check for exit
				if (strcmp(m_currMsg.c_str(), "q") == 0)
				{
					m_keepRunning = false;
				}
				else if (strlen(m_currMsg.c_str()) > 0)
				{
					// Print the message locally
					AppendConsoleLine("Me: " + m_currMsg + "\n");

					// Send the message
					bool commandFound = TestCommand(m_currMsg.c_str());
					if (!commandFound)
					{
						RakNet::BitStream bs;
						bs.Write((RakNet::MessageID) EGameMessages::ID_CLIENT_CHAT_MESSAGE);
						bs.Write(m_currMsg.c_str());

						m_pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}

					// Clear message
					m_currMsg = "";

					// Update the text output
					DrawLines();
				}
			}
		}
	}
}

void BasicNetworkingApplication::AppendConsoleLine(std::string a_line)
{
	m_consoleLines.push_back(a_line);

	// Get rid of old messages
	if (m_consoleLines.size() > MAX_MESSAGE_HISTORY)
	{
		// Remove the element from the start
		std::reverse(m_consoleLines.begin(), m_consoleLines.end());
		m_consoleLines.pop_back();
		std::reverse(m_consoleLines.begin(), m_consoleLines.end());
	}

	DrawLines();
}

void BasicNetworkingApplication::DrawLines()
{
	// Clear the screen
	system("cls");

	for (auto& line : m_consoleLines)
	{
		std::cout << line;
	}

	// Line for spacing
	std::cout << std::endl;

	// Output the user input
	std::cout << "Msg (q to quit): " << m_currMsg << std::endl;
}

// @brief Attempts to reconnect on a few second cool-down.
void BasicNetworkingApplication::AttemptReconnect()
{
	AppendConsoleLine("Attempting to reconnect.\n");

	InitialiseClientConnection();
}

void BasicNetworkingApplication::CheckPing(const char* a_str)
{
	if (strcmp(a_str, "Ping!") == 0)
	{
		//m_
	}
}

bool BasicNetworkingApplication::TestCommand(const char* a_str)
{
	bool commandRun = false;
	if (strcmp(a_str, "ddos") == 0)
	{
		// Commence hax
		m_pingThreads.push_back(std::thread(&BasicNetworkingApplication::DDOSServer, this, 0));
		commandRun = true;
	}

	return commandRun;
}

// @brief Hax the server advanced
void BasicNetworkingApplication::DDOSServer(int a_slot)
{
	for (unsigned int i = 0; i < MAX_MESSAGE_HISTORY; ++i)
	{
		// Send the message
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID) EGameMessages::ID_CLIENT_CHAT_MESSAGE);
		bs.Write("Get DDOSd, better call in ASIO!");

		// Print the message locally
		AppendConsoleLine("Me: DDOS!\n");

		m_pPeerInterface->Send(&bs, HIGH_PRIORITY, UNRELIABLE, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// @brief Parses the command-prompt colour codes in a text and prints it coloured.
void BasicNetworkingApplication::PrintColouredLine(std::string a_line)
{
	// DOESN'T SEEM TO WORK

	size_t findPos = a_line.find(COLOUR_CHAR);
	if (findPos != std::string::npos && findPos < a_line.length() - 2)
	{
		// Colour codes, parse
		std::string colCode = "";
		do
		{
			// Parse the colour codes
			colCode = a_line.substr(findPos + 1, 2);
			system(("Color " + colCode).c_str());

			size_t newFindPos = a_line.find(COLOUR_CHAR, findPos + 1);
			std::cout << a_line;
		} while (findPos != std::string::npos && findPos < a_line.length() - 2);

		// Revert the colours to default
		system("Color 00");
	}
	else
	{
		// No colour codes, just print the message
		std::cout << a_line;
	}
}