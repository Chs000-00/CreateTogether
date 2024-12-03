inline void sendDataToMember(SteamNetworkingIdentity member, const char* msg){
    SteamNetworkingMessages()->SendMessageToUser(member, msg, static_cast<uint32>(strlen(msg)), k_nSteamNetworkingSend_Reliable, 0);
}

inline void sendDataToMember(SteamNetworkingIdentity member,  std::string msg) {
    sendDataToMember(member, msg.c_str())
}