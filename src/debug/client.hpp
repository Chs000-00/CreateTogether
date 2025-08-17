// This file is used to make the code more bearable to read

#include "debug/isteamnetworkingsockets.h"
#include "../networking/NetManager.hpp"

inline void steamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t* pInfo ) {
	auto netManager = NetManager::get();
	switch ( pInfo->m_info.m_eState ) {
		case k_ESteamNetworkingConnectionState_None:
			// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
			break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
			if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting ) {
				log::warn("A networking issue occured; {}", pInfo->m_info.m_szEndDebug);
			}

			else if ( pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally ) {
				log::warn("Connection lost; {}", pInfo->m_info.m_szEndDebug);
			}
			else {
				log::info("Host has left the confines of this world; {}", pInfo->m_info.m_szEndDebug);
			}
			// Clean up the connection.  This is important!
			// The connection is "closed" in the network sense, but
			// it has not been destroyed.  We must close it on our end, too
			// to finish up.  The reason information do not matter in this case,
			// and we cannot linger because it's already closed on the other end,
			// so we just pass 0's.
			SteamNetworkingSockets()->CloseConnection(netManager->connection, 0, nullptr, false);
			netManager->leaveLobby();
		}
		case k_ESteamNetworkingConnectionState_Connecting:
			// We will get this callback when we start connecting.
			// We can ignore this.
			break;

		case k_ESteamNetworkingConnectionState_Connected:
			log::info("Sucessfully connected to server.");
			break;

		default:
			// Silences -Wswitch
			break;
	}
}