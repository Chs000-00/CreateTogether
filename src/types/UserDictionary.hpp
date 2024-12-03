class UserDictionary {
public:
    static UserDictionary* create(CSteamID id);
    static UserDictionary* fromUserIdentity(SteamNetworkingIdentity identity);
    SteamNetworkingIdentity identity;
    CCDictionary* UserDictionary = CCDictionary::create();
    void sendDataToUser(std::string msg);
    void sendDataToUser(const char* msg);
private:
    // UserDictionary();
    // UserDictionary(CSteamID id);
}