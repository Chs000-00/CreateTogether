# CreateTogether
This is where he makes a mod.

<img src="logo.png" width="150" alt="the mod's very epic logo" />

Yes I know this code is gross-looking, but feel free to contribute with a pr.


## Getting started
We recommend heading over to [the getting started section on our docs](https://docs.geode-sdk.org/getting-started/) for useful info on what to do next.

## Build instructions
For more info, see [our docs](https://docs.geode-sdk.org/getting-started/create-mod#build)

This project requires steamworks sdk! Install `steam_api64.dll` and `steam_api64.lib` into the `lib/` folder!  

```sh
# Assuming you have the Geode CLI set up already
geode build
```
## Local testing (If you have no friends)
1. Enable NO_STEAMWORKS & COMPILE_TEST_SERVER in CmakeLists
2. Recompile
3. Open test server (`build/.../server.exe`) (note you may be required to provide dlls)
4. The game now supports connecting to 127.0.0.1, to do so:
5. Click the LobbyList button to join (note you have to provide dlls into the games exe location)
6. Profit

# Resources
* [Geode SDK Documentation](https://docs.geode-sdk.org/)
* [Steamworks SDK Documentation](https://partner.steamgames.com/doc/gettingstarted)
* [Geode SDK Source Code](https://github.com/geode-sdk/geode/)
* [Geode CLI](https://github.com/geode-sdk/cli)
* [Bindings](https://github.com/geode-sdk/bindings/)
* [Dev Tools](https://github.com/geode-sdk/DevTools)
