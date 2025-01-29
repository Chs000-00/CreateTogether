
bool isValidEnumRange(int n, int min, int max);


// Todo: This
// Returns an effectGameObject. Includes EnchancedGameObject.
matjson::Value getJsonFromEffectGameObject(EffectGameObject* obj);

Result<EffectGameObject*> getEffectGameObjectFromMatjson(matjson::Value);

// Todo: This
matjson::Value getJsonFromEnhancedGameObject(EnhancedGameObject* obj);

Result<EnhancedGameObject*> getEnhancedGameObjectFromMatjson(matjson::Value);