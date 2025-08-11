#pragma once

#include "SharedHighLevelHeaders.hpp"

Result<uint8_t> recvCreateObjects(const CTSerialize::CreateObjects* msg);

Result<uint8_t> recvDeleteObjects(const CTSerialize::DeleteObjects* msg);

Result<uint8_t> recvMoveObjects(const CTSerialize::MoveObjects* msg);

Result<uint8_t> recvLevelSettingChange(const CTSerialize::LevelSettingChange* msg);

Result<uint8_t> recvRotateObjects(const CTSerialize::RotateObjects* msg);

Result<uint8_t> recvPasteObjects(const CTSerialize::PasteObjects* msg);

Result<uint8_t> recvModifyObjects(const CTSerialize::ModifyObjects* msg);

Result<uint8_t> recvChangeDefaultColor(const CTSerialize::ChangeDefaultColor* msg);

Result<uint8_t> recvRequestLevel(const CTSerialize::RequestLevel* msg);

Result<uint8_t> recvReturnLevelString(const CTSerialize::ReturnLevelString* msg);

Result<uint8_t> recvUpdateSong(const CTSerialize::UpdateSong* msg);

Result<uint8_t> recvChangeArt(const CTSerialize::ChangeArt* msg);

Result<uint8_t> recvSpeedChange(const CTSerialize::SpeedChange* msg);

Result<uint8_t> recvGameModeChange(const CTSerialize::GameModeChange* msg);

