#include <stdlib.h>
#include <iostream>
#include <signal.h>

#include <debug/steamnetworkingsockets.h>
#include <debug/isteamnetworkingutils.h>

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/minireflect.h>
#include <ctserialize_generated.h>

#include "../config.hpp"
#include "ctcursor_generated.h"

void steamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo );
void steamNetConnectionStatusChangedCallbackCursor( SteamNetConnectionStatusChangedCallback_t *pInfo );

static HSteamNetPollGroup pollGroup;
static HSteamNetPollGroup cursorPollGroup;
// TODO: Destroy connection in vector when user leaves
static std::vector<HSteamNetConnection> connections;
static std::vector<HSteamNetConnection> cursorConnections;


void handleSignal(int signum) {
    std::cout << "Caught signal " << signum << "\n";
    exit(EXIT_SUCCESS);
}

void sendMessageToAllWithException(uint8_t* data, uint32 size, HSteamNetPollGroup exception) {
	for (auto const& member : cursorConnections) {

        if (member == exception) {
            continue;
        }

        SteamNetworkingSockets()->SendMessageToConnection(member, data, size, k_nSteamNetworkingSend_Reliable, nullptr);
    }
}


void sendMessageToAllWithExceptionCursor(uint8_t* data, uint32 size, HSteamNetPollGroup exception) {
	for (auto const& member : connections) {

        if (member == exception) {
            continue;
        }

        SteamNetworkingSockets()->SendMessageToConnection(member, data, size, k_nSteamNetworkingSend_UnreliableNoDelay, nullptr);
    }
}



void pollMessages() {
    SteamNetworkingMessage_t* msgs[MAX_EDITOR_MESSAGES];
    int numMessages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(pollGroup, msgs, MAX_EDITOR_MESSAGES);
    
    if ( numMessages == 0 )
        return;

    if ( numMessages < 0 ) {
        std::cout << "Error checking for messages" << '\n';
        exit(EXIT_FAILURE);
    }

    for (int idxMsg = 0; idxMsg < numMessages; idxMsg++) {
		SteamNetworkingMessage_t* message = msgs[idxMsg];


        uint8_t* data = new uint8_t[message->GetSize()];
		memcpy(data, message->GetData(), message->GetSize());
        
		// data = static_cast<const uint8_t*>(message->GetData());


        // Delete the message after it's usage
        

		auto messageHeader = CTSerialize::GetMessageHeader(data);

        flatbuffers::Verifier verifier(data, message->GetSize());

		bool isVerified = CTSerialize::VerifyMessageHeaderBuffer(verifier);

        auto s = flatbuffers::FlatBufferToString(data, CTSerialize::MessageHeaderTypeTable());

        std::cout << "RecvMessage:" << s << '\n';

        if (messageHeader->body_type() == CTSerialize::MessageBody_RequestLevel && connections.size() == 1) {
            flatbuffers::FlatBufferBuilder builder;
            auto returnLvlString = CTSerialize::CreateReturnLevelStringDirect(builder, nullptr, "ignore");
            auto out = CTSerialize::CreateMessageHeader(builder, CTSerialize::MessageBody_ReturnLevelString, returnLvlString.Union());
            builder.Finish(out);
            SteamNetworkingSockets()->SendMessageToConnection(message->m_conn, builder.GetBufferPointer(), builder.GetSize(), k_nSteamNetworkingSend_Reliable, nullptr);        
        }

        sendMessageToAllWithException(data, message->GetSize(), message->m_conn);

		// TODO: Call release earlier
        message->Release();
        delete[] data;
    }
}

void pollCursorMessages() {
    SteamNetworkingMessage_t* msgs[MAX_CURSOR_MESSAGES];
    int numMessages = SteamNetworkingSockets()->ReceiveMessagesOnPollGroup(cursorPollGroup, msgs, MAX_CURSOR_MESSAGES);
    
    if ( numMessages == 0 )
        return;

    if ( numMessages < 0 ) {
        std::cout << "Error checking for messages (Cursor)" << '\n';
        exit(EXIT_FAILURE);
    }

    for (int idxMsg = 0; idxMsg < numMessages; idxMsg++) {
		SteamNetworkingMessage_t* message = msgs[idxMsg];


        uint8_t* data = new uint8_t[message->GetSize()];
		memcpy(data, message->GetData(), message->GetSize());

		auto messageHeader = CTSerialize::cursor::GetCursorUpdate(data);

        flatbuffers::Verifier verifier(data, message->GetSize());

		bool isVerified = CTSerialize::cursor::VerifyCursorUpdateBuffer(verifier);

        sendMessageToAllWithExceptionCursor(data, message->GetSize(), message->m_conn);

		// TODO: Call release earlier
        message->Release();
        delete[] data;
    }
}


void initEditorServer() {
    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
	serverLocalAddr.m_port = DEDICATED_EDITOR_PORT;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallback );
    auto listenSocket = SteamNetworkingSockets()->CreateListenSocketIP( serverLocalAddr, 1, &opt );

    if ( listenSocket == k_HSteamListenSocket_Invalid )
        std::cout << "ERROR: Failed to create a listen socket" << "\n";

    pollGroup = SteamNetworkingSockets()->CreatePollGroup();

    if ( pollGroup == k_HSteamNetPollGroup_Invalid )
        std::cout << "ERROR: Failed to create a poll group" << "\n";
}

void initCursorServer() {
    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
	serverLocalAddr.m_port = DEDICATED_CURSOR_PORT;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr( k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)steamNetConnectionStatusChangedCallbackCursor );
    auto listenSocket = SteamNetworkingSockets()->CreateListenSocketIP( serverLocalAddr, 1, &opt );

    if ( listenSocket == k_HSteamListenSocket_Invalid )
        std::cout << "ERROR: Failed to create a listen socket (cursor)" << "\n";

    cursorPollGroup = SteamNetworkingSockets()->CreatePollGroup();

    if ( cursorPollGroup == k_HSteamNetPollGroup_Invalid )
        std::cout << "ERROR: Failed to create a poll group (cursor)" << "\n";
}

void initServer() {
    initEditorServer();
    initCursorServer();
}

void initLib() {
    SteamDatagramErrMsg errMsg;
    if ( !GameNetworkingSockets_Init( nullptr, errMsg ) ) {
        std::cout << "ERROR: GameNetworkingSockets_Init failed: " << errMsg;
        exit(EXIT_FAILURE);
    }
    SteamNetworkingUtils()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_LogLevel_Message, k_ESteamNetworkingSocketsDebugOutputType_Debug);
}

int main(int argc, char *argv[]) {

    std::cout << "MOD_LOBBY_ID == " MOD_LOBBY_ID "\n";
    std::cout << "MOD_VERSION == " MOD_VERSION "\n";
    std::cout << "DEDICATED_EDITOR_PORT == " << DEDICATED_EDITOR_PORT << '\n';
    std::cout << "Hosting debug server on 127.0.0.1:" << DEDICATED_EDITOR_PORT << '\n';

    initLib();
    initServer();
    
    signal(SIGINT, handleSignal);  

    while (true) {
        pollMessages();
        SteamNetworkingSockets()->RunCallbacks();
    }
}


void steamNetConnectionStatusChangedCallback( SteamNetConnectionStatusChangedCallback_t *pInfo ) {

        // std::cout << "Accepted message request from someone " << '\n'; // << pCallback->m_identityRemote.GetIPv4()

		// What's the state of the connection?
		switch ( pInfo->m_info.m_eState ) {
			case k_ESteamNetworkingConnectionState_None:
				// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
				break;

			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {

                if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting ) {
                    std::cout << "A networking issue occured; " << pInfo->m_info.m_szEndDebug << '\n';
                }

                else if ( pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally ) {
                    std::cout << "A local problem occured; " << pInfo->m_info.m_szEndDebug << '\n';
                    // _beep(700, 100);
                }
                else {
                    std::cout << "Peer has decided to quit; " << pInfo->m_info.m_szEndDebug << '\n';
                }
            
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

                // _beep(1400, 100);

				// A client is attempting to connect
				// Try to accept the connection.
				if ( SteamNetworkingSockets()->AcceptConnection( pInfo->m_hConn ) != k_EResultOK ) {
                    
					// This could fail.  If the remote host tried to connect, but then
					// disconnected, the connection may already be half closed.  Just
					// destroy whatever we have on our side.
                    std::cout << "Can't accept connection.  (It was already closed?)" << '\n';
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					break;
				}

				// Assign the poll group
				if ( !SteamNetworkingSockets()->SetConnectionPollGroup( pInfo->m_hConn, pollGroup ) ) {
                    std::cout << "Failed to set poll group?" << '\n';
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					break;
				}

                connections.push_back(pInfo->m_hConn);

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


void steamNetConnectionStatusChangedCallbackCursor( SteamNetConnectionStatusChangedCallback_t *pInfo ) {

        // std::cout << "Accepted message request from someone " << '\n'; // << pCallback->m_identityRemote.GetIPv4()

		// What's the state of the connection?
		switch ( pInfo->m_info.m_eState ) {
			case k_ESteamNetworkingConnectionState_None:
				// NOTE: We will get callbacks here when we destroy connections.  You can ignore these.
				break;

			case k_ESteamNetworkingConnectionState_ClosedByPeer:
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {

                if ( pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting ) {
                    std::cout << "A networking issue occured; (Cursor) " << pInfo->m_info.m_szEndDebug << '\n';
                }

                else if ( pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally ) {
                    std::cout << "A local problem occured; (Cursor) " << pInfo->m_info.m_szEndDebug << '\n';
                    // _beep(700, 100);
                }
                else {
                    std::cout << "Peer has decided to quit; (Cursor) " << pInfo->m_info.m_szEndDebug << '\n';
                }
            
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

				std::cout <<  "(Cursor) Connection request from " << pInfo->m_info.m_szConnectionDescription << '\n';

                // _beep(1400, 100);

				// A client is attempting to connect
				// Try to accept the connection.
				if ( SteamNetworkingSockets()->AcceptConnection( pInfo->m_hConn ) != k_EResultOK ) {
                    
					// This could fail.  If the remote host tried to connect, but then
					// disconnected, the connection may already be half closed.  Just
					// destroy whatever we have on our side.
                    std::cout << "Can't accept connection.  (It was already closed?) (Cursor)" << '\n';
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					break;
				}

				// Assign the poll group
				if ( !SteamNetworkingSockets()->SetConnectionPollGroup( pInfo->m_hConn, pollGroup ) ) {
                    std::cout << "Failed to set poll group? (Cursor)" << '\n';
					SteamNetworkingSockets()->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
					break;
				}

                cursorConnections.push_back(pInfo->m_hConn);

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