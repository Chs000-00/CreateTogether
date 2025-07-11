#include <stdlib.h>
#include <iostream>
#include <signal.h>

#include <debug/steamnetworkingsockets.h>
#include <debug/isteamnetworkingutils.h>

#include "../config.hpp"

void steamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo );

static HSteamNetPollGroup pollGroup;

void handleSignal(int signum) {
    std::cout << "Caught signal " << signum << "\n";
    exit(EXIT_SUCCESS);
}

void pollMessages() {
    SteamNetworkingMessage_t* msgs[128];
    int numMessages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(pollGroup, msgs, MAX_MESSAGES);
    
    if ( numMessages == 0 )
        return;

    if ( numMessages < 0 )
        std::cout << "Error checking for messages" << '\n';
        exit(EXIT_FAILURE);

    for (int idxMsg = 0; idxMsg < numMessages; idxMsg++) {
		SteamNetworkingMessage_t* message = msgs[idxMsg];
    }
}

void initServer() {
    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
	serverLocalAddr.m_port = DEDICATED_PORT;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallback );
    auto listenSocket = SteamNetworkingSockets()->CreateListenSocketIP( serverLocalAddr, 1, &opt );

    if ( listenSocket == k_HSteamListenSocket_Invalid )
        std::cout << "ERROR: Failed to create a listen socket" << "\n";

    pollGroup = SteamNetworkingSockets()->CreatePollGroup();

    if ( pollGroup == k_HSteamNetPollGroup_Invalid )
        std::cout << "ERROR: Failed to create a poll group" << "\n";

}

void initLib() {
    SteamDatagramErrMsg errMsg;
    if ( !GameNetworkingSockets_Init( nullptr, errMsg ) ) {
        std::cout << "ERROR: GameNetworkingSockets_Init failed: " << errMsg;
        exit(EXIT_FAILURE);
    }
    SteamNetworkingUtils()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_LogLevel_Message, k_ESteamNetworkingSocketsDebugOutputType_Msg);
}

int main(int argc, char *argv[]) {
    
    std::cout << "MOD_LOBBY_ID == " MOD_LOBBY_ID "\n";
    std::cout << "MOD_VERSION == " MOD_VERSION "\n";
    std::cout << "DEDICATED_PORT == " << DEDICATED_PORT << '\n';
    std::cout << "Hosting debug server on 127.0.0.1:" << DEDICATED_PORT << '\n';

    initLib();
    initServer();
    
    signal(SIGINT, handleSignal);  

    while (true) {
        pollMessages();
    }
}


void steamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo ) {

        std::cout << "Accepted message request from... someone " << '\n'; // << pCallback->m_identityRemote.GetIPv4()
        _beep(1000, 100);

		// What's the state of the connection?
		switch ( pInfo->m_info.m_eState )
		{
			case k_ESteamNetworkingConnectionState_None:
				// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
				break;

			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			{

				// Clean up the connection.  This is important!
				// The connection is "closed" in the network sense, but
				// it has not been destroyed.  We must close it on our end, too
				// to finish up.  The reason information do not matter in this case,
				// and we cannot linger because it's already closed on the other end,
				// so we just pass 0's.
				SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
				break;
			}

			case k_ESteamNetworkingConnectionState_Connecting:
			{
				// This must be a new connection
				// assert( m_mapClients.find( pInfo->m_hConn ) == m_mapClients.end() );

				std::cout <<  "Connection request from " << pInfo->m_info.m_szConnectionDescription << '\n';

				// A client is attempting to connect
				// Try to accept the connection.
				if ( SteamNetworkingSockets()->AcceptConnection( pInfo->m_hConn ) != k_EResultOK ) {
                    
					// This could fail.  If the remote host tried to connect, but then
					// disconnected, the connection may already be half closed.  Just
					// destroy whatever we have on our side.
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					std::cout << "Can't accept connection.  (It was already closed?)" << '\n';
					break;
				}

				// Assign the poll group
				if ( !SteamNetworkingSockets()->SetConnectionPollGroup( pInfo->m_hConn, pollGroup ) ) {
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					std::cout << "Failed to set poll group?" << '\n';
					break;
				}

				break;
			}

			case k_ESteamNetworkingConnectionState_Connected:
				// We will get a callback immediately after accepting the connection.
				// Since we are the server, we can ignore this, it's not news to us.
				break;

			default:
				// Silences -Wswitch
				break;
		}
	}