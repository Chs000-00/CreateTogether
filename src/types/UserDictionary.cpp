// UserDictionary::UserDictionary() {

// }

UserDictionary* UserDictionary::fromUserIdentity(SteamNetworkingIdentity identity) {
    UserDictionary res = new UserDictionary;
    res->identity = identity;
    return res;
}

// Creates a 
// UserDictinary does not get auto destroyed!
UserDictionary* UserDictionary::create(CSteamID id) {
    UserDictionary res = new UserDictionary;
    SteamNetworkingIdentity member;
    member.SetSteamID(id);
    res->identity = member;
    return res;
}

void UserDictionary::sendDataToUser(std::string msg) {
    this->sendDataToUser(msg.c_str());
}

void UserDictionary::sendDataToUser(const char* msg) {
    SteamNetworkingMessages()->SendMessageToUser(member, msg, static_cast<uint32>(strlen(msg)), k_nSteamNetworkingSend_Reliable, 0);
}