//File by Samuel Cole.

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GameMessages.h"

void HandleNetworkMessages(RakNet::RakPeerInterface* a_pPeerInterface)
{
	RakNet::Packet* packet = nullptr;

	std::vector<std::string> history;

	while (true)
	{
		for (packet = a_pPeerInterface->Receive(); packet; a_pPeerInterface->DeallocatePacket(packet), packet = a_pPeerInterface->Receive())
		{
			RakNet::BitStream bs;
			RakNet::RakString str;
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A connection is incoming.\n";

				//Send chat dump.
				if (history.size() > 0)
				{
					bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_DUMP);
					int i;
					if ((int)history.size() - 22 > 0) 
					{
						i = (int)history.size() - 22; 
					}
					else 
					{ 
						i = 0; 
					}
					bs.Write((unsigned int)history.size() - i);
					for (i; i < history.size(); ++i)
					{
						bs.Write(history[i].c_str());
					}
					a_pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				}

				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				break;
			case ID_CLIENT_TEXT_MESSAGE:
				std::cout << "Pong!" << std::endl;
				break;
			case ID_CLIENT_CHAT_MESSAGE:

				//Read in chat message...
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(str);

				if ((history.size() > 0 && (std::string(packet->systemAddress.ToString()) + std::string(": ") + std::string(str.C_String())).c_str() != history[history.size() - 1]) || history.size() == 0)
				{
					//...and send it out to everyone but the sender
					bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CHAT_MESSAGE);
					bs.Write((std::string(packet->systemAddress.ToString()) + std::string(": ") + std::string(str.C_String())).c_str());

					std::cout << std::string(packet->systemAddress.ToString()) + std::string(": ") + std::string(str.C_String()) << std::endl;

					a_pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

					history.push_back((std::string(packet->systemAddress.ToString()) + std::string(": ") + std::string(str.C_String())).c_str());
				}

				break;
			default:
				std::cout << "Received a message with an unknown id: " << packet->data[0] << std::endl;
				break;
			}
		}

	}
}

void SendClientPing(RakNet::RakPeerInterface* pPeerInterface)
{
	while (true)
	{
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
		bs.Write("Ping!");

		pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}

int main()
{
	const unsigned int PORT = 5456;
	RakNet::RakPeerInterface* pPeerInterface = nullptr;

	//Start up the server, and start listening to clients
	std::cout << "Starting up the server..." << std::endl;

	//Initialize the Raknet peer interface first
	pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	//Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//Now call startup - max of 32 connections, on the assigned port
	pPeerInterface->Startup(32, &sd, 1);
	pPeerInterface->SetMaximumIncomingConnections(32);

	std::thread pingThread(SendClientPing, pPeerInterface);

	HandleNetworkMessages(pPeerInterface);

	return 0;
}