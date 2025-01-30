
bool isValidEnumRange(int n, int min, int max);


template <typename T> 
inline void setJsonObjectFromProperty(matjson::Value json, T* val, const char* str);

// TODO: Uhh this I guess
template <typename T> 
matjson::Value getJsonFromGameObject(T* obj);

template <typename T> 
matjson::Value setGameObjectFromJson(T* obj);