#include <iostream>
#include <streambuf>
#include <thread>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include "GameMessages.h"

const unsigned short PORT = 5456;

RakNet::RakPeerInterface* pPeerInterface;

bool m_stayOpen;

std::thread m_netMessagePump;

// For spamming the users
std::thread m_pingThread;

// Forward definitions
void HandleNetworkMessages();
void SendClientPing(RakNet::RakPeerInterface* a_pPeerInterface);

int main(const char argc, const char* argv)
{
	pPeerInterface = nullptr;
	m_stayOpen = true;

	// Start the server
	std::cout << "Initialising server.." << std::endl;

	// Init the peer interface first
	pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	// Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd = RakNet::SocketDescriptor(PORT, 0);

	// Start the connection
	pPeerInterface->Startup(32, &sd, 1);
	pPeerInterface->SetMaximumIncomingConnections(32);

	m_netMessagePump = std::thread(HandleNetworkMessages);
	m_pingThread = std::thread(SendClientPing, pPeerInterface);

	m_pingThread.join();
	m_netMessagePump.join();
}

void HandleNetworkMessages()
{
	RakNet::Packet* pData = nullptr;

	while (m_stayOpen)
	{
		// Process all incoming packets
		for (pData = pPeerInterface->Receive();
			pData;
			pPeerInterface->DeallocatePacket(pData), pData = pPeerInterface->Receive())
		{
			// Process header
			switch (pData->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
					std::cout << "A connection is incoming." << std::endl;
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << "A client has disconnected." << std::endl;
					break;
				case ID_CONNECTION_LOST:
					std::cout << "A client lost the connection." << std::endl;
					break;
				case ID_CLIENT_TEXT_MESSAGE:
				{
					// Read the message
					RakNet::BitStream bsIn(pData->data, pData->length, false);
					// Ignore the header
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

					RakNet::RakString str;
					bsIn.Read(str);
					std::cout << str.C_String() << std::endl;
					break;
				}
				default:
					std::cout << "Recieved a message with unknown id: " << pData->data[0] << std::endl;
					break;
			}
		}
	}
}

void SendClientPing(RakNet::RakPeerInterface* a_pPeerInterface)
{
	// Begin mad hax
	while (true)
	{
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID) EGameMessages::ID_SERVER_TEXT_MESSAGE);
		bs.Write("Ping!");

		pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}