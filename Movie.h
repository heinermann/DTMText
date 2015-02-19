// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include <string>
#include <set>
#include <map>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <sstream>

#include "CommonTypes.h"


namespace Movie
{


// GameCube Controller State
#pragma pack(push,1)
struct ControllerState
{
	bool Start:1, A:1, B:1, X:1, Y:1, Z:1; // Binary buttons, 6 bits
	bool DPadUp:1, DPadDown:1,             // Binary D-Pad buttons, 4 bits
	     DPadLeft:1, DPadRight:1;
	bool L:1, R:1;                         // Binary triggers, 2 bits
	bool disc:1;                           // Checks for disc being changed
	bool reset:1;                          // Console reset button
	bool reserved:2;                       // Reserved bits used for padding, 2 bits

	u8   TriggerL, TriggerR;               // Triggers, 16 bits
	u8   AnalogStickX, AnalogStickY;       // Main Stick, 16 bits
	u8   CStickX, CStickY;                 // Sub-Stick, 16 bits

	bool isDefault() const
	{
		return
			Start == false &&
			A == false &&
			B == false &&
			X == false &&
			Y == false &&
			Z == false &&
			DPadUp == false &&
			DPadDown == false &&
			DPadLeft == false &&
			DPadRight == false &&
			L == false &&
			R == false &&
			disc == false &&
			reset == false &&
			TriggerL == 0 &&
			TriggerR == 0 &&
			AnalogStickX == 128 &&
			AnalogStickY == 128 &&
			CStickX == 128 &&
			CStickY == 128;
	}

	void save(std::ostream & archive) const
	{
		std::set<std::string> buttons;
		std::map<std::string, unsigned> values;

		if (Start) buttons.insert("Start");
		if (A) buttons.insert("A");
		if (B) buttons.insert("B");
		if (X) buttons.insert("X");
		if (Y) buttons.insert("Y");
		if (Z) buttons.insert("Z");
		if (DPadUp) buttons.insert("DPadUp");
		if (DPadDown) buttons.insert("DPadDown");
		if (DPadLeft) buttons.insert("DPadLeft");
		if (DPadRight) buttons.insert("DPadRight");
		if (L) buttons.insert("L");
		if (R) buttons.insert("R");
		if (disc) buttons.insert("disc");
		if (reset) buttons.insert("reset");
		if (TriggerL > 0) values["TriggerL"] = TriggerL;
		if (TriggerR > 0) values["TriggerR"] = TriggerR;
		if (AnalogStickX != 128) values["AnalogX"] = AnalogStickX;
		if (AnalogStickY != 128) values["AnalogY"] = AnalogStickY;
		if (CStickX != 128) values["CStickX"] = CStickX;
		if (CStickY != 128) values["CStickY"] = CStickY;

		for (auto &s : buttons)
		{
			archive << s << " ";
		}

		for (auto &p : values)
		{
			archive << p.first << ":" << p.second << " ";
		}
	}

	void load(std::istream & archive)
	{
		std::set<std::string> buttons;
		std::map<std::string, unsigned> values;

		std::string unparsed, s;
		std::getline(archive, unparsed);
		std::istringstream ss(unparsed);
		while (ss >> s)
		{
			size_t split = s.find_last_of(':');
			if (split == std::string::npos)
			{
				buttons.insert(s);
			}
			else
			{
				std::string buttonStr = s.substr(0, split);
				unsigned buttonValue = std::stoul(s.substr(split+1));
				values[buttonStr] = buttonValue;
			}
		}

		Start = buttons.count("Start") != 0;
		A = buttons.count("A") != 0;
		B = buttons.count("B") != 0;
		X = buttons.count("X") != 0;
		Y = buttons.count("Y") != 0;
		Z = buttons.count("Z") != 0;
		L = buttons.count("L") != 0;
		R = buttons.count("R") != 0;
		DPadUp = buttons.count("DPadUp") != 0;
		DPadDown = buttons.count("DPadDown") != 0;
		DPadLeft = buttons.count("DPadLeft") != 0;
		DPadRight = buttons.count("DPadRight") != 0;
		disc = buttons.count("disc") != 0;
		reset = buttons.count("reset") != 0;

		TriggerL = values["TriggerL"];
		TriggerR = values["TriggerR"];

		AnalogStickX = AnalogStickY = CStickX = CStickY = 128;
		{
			auto it = values.find("AnalogX");
			if (it != values.end())
			{
				AnalogStickX = it->second;
			}
		}
		{
			auto it = values.find("AnalogY");
			if (it != values.end())
			{
				AnalogStickY = it->second;
			}
		}
		{
			auto it = values.find("CStickX");
			if (it != values.end()) CStickX = it->second;
		}
		{
			auto it = values.find("CStickY");
			if (it != values.end()) CStickY = it->second;
		}

		reserved = 0;
	}
};
static_assert(sizeof(ControllerState) == 8, "ControllerState should be 8 bytes");
#pragma pack(pop)

template <class T>
inline std::string arrToStrImp(const T *arr, unsigned N)
{
	std::string result(arr, arr+N);
	while(!result.empty() && result.back() == '\0')
	{
		result.pop_back();
	}
	return result;
}
#define arrToStr(T) arrToStrImp(T, std::extent<decltype(T)>::value)

template <class T>
inline std::string arrToHexStrImp(const T *arr, unsigned N)
{
	std::ostringstream ss;
	for (unsigned i = 0; i < N; ++i)
		ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(arr[i]);
	return ss.str();
}
#define arrToHexStr(T) arrToHexStrImp(T, std::extent<decltype(T)>::value)

template <class T>
inline void hexStrToArrImp(std::string const &s, T *arr, unsigned N)
{
	for (unsigned i = 0; i < s.size() && i/2 < N; i += 2)
	{
		unsigned value = std::stoul(s.substr(i,2), 0, 16);
		arr[i/2] = static_cast<T>(value);
	}
}
#define hexStrToArr(S,A) hexStrToArrImp(S, A, std::extent<decltype(A)>::value)

std::ostream &operator <<(std::ostream & os, u8 const & v)
{
	return os << static_cast<unsigned>(v);
}
std::istream &operator >>(std::istream & is, u8 & v)
{
	unsigned value;
	is >> value;
	v = static_cast<u8>(value);
	return is;
}

template <class T>
void getHeaderLine(std::istream &is, T & v)
{
	std::string s;
	getline(is, s);
	std::istringstream ss(s);
	ss >> s >> std::boolalpha >> v;
}

template <>
void getHeaderLine(std::istream &is, std::string & v)
{
	std::string s;
	getline(is, s);
	
	size_t pos = s.find_first_of(':');
	if (pos != std::string::npos)
	{
		if (s.size() > pos+1 && s[pos+1] == ' ') ++pos;
		v = s.substr(pos+1);
	}
	else
	{
		v = s;
	}
}


#pragma pack(push,1)
struct DTMHeader
{
	u8 filetype[4];         // Unique Identifier (always "DTM"0x1A)

	u8 gameID[6];           // The Game ID
	bool bWii;              // Wii game

	u8  numControllers;     // The number of connected controllers (1-4)

	bool bFromSaveState;    // false indicates that the recording started from bootup, true for savestate
	u64 frameCount;         // Number of frames in the recording
	u64 inputCount;         // Number of input frames in recording
	u64 lagCount;           // Number of lag frames in the recording
	u64 uniqueID;           // (not implemented) A Unique ID comprised of: md5(time + Game ID)
	u32 numRerecords;       // Number of rerecords/'cuts' of this TAS
	u8  author[32];         // Author's name (encoded in UTF-8)

	u8  videoBackend[16];   // UTF-8 representation of the video backend
	u8  audioEmulator[16];  // UTF-8 representation of the audio emulator
	u8  md5[16];            // MD5 of game iso

	u64 recordingStartTime; // seconds since 1970 that recording started (used for RTC)

	bool bSaveConfig;       // Loads the settings below on startup if true
	bool bSkipIdle;
	bool bDualCore;
	bool bProgressive;
	bool bDSPHLE;
	bool bFastDiscSpeed;
	u8   CPUCore;           // 0 = interpreter, 1 = JIT, 2 = JITIL
	bool bEFBAccessEnable;
	bool bEFBCopyEnable;
	bool bCopyEFBToTexture;
	bool bEFBCopyCacheEnable;
	bool bEFBEmulateFormatChanges;
	bool bUseXFB;
	bool bUseRealXFB;
	u8   memcards;
	bool bClearSave;        // Create a new memory card when playing back a movie if true
	u8   bongos;
	bool bSyncGPU;
	bool bNetPlay;
	u8   reserved[13];      // Padding for any new config options
	u8   discChange[40];    // Name of iso file to switch to, for two disc games.
	u8   revision[20];      // Git hash
	u32  DSPiromHash;
	u32  DSPcoefHash;
	u64  tickCount;	        // Number of ticks in the recording
	u8   reserved2[11];     // Make heading 256 bytes, just because we can

	void save(std::ostream & archive) const
	{
		std::string gameID = arrToStr(this->gameID);
		std::string author = arrToStr(this->author);
		std::string videoBackend = arrToStr(this->videoBackend);
		std::string audioEmulator = arrToStr(this->audioEmulator);
		std::string md5 = arrToHexStr(this->md5);
		std::string discChange = arrToStr(this->discChange);
		std::string revision = arrToHexStr(this->revision);

#define writeO(V) archive << #V ": " << V << "\n";
		writeO(gameID);
		writeO(bWii);
		writeO(numControllers);
		writeO(bFromSaveState);
		writeO(frameCount);
		writeO(inputCount);
		writeO(lagCount);
		writeO(uniqueID);
		writeO(numRerecords);
		writeO(author);
		writeO(videoBackend);
		writeO(audioEmulator);
		writeO(md5);
		writeO(recordingStartTime);
		writeO(bSaveConfig);
		writeO(bSkipIdle);
		writeO(bDualCore);
		writeO(bProgressive);
		writeO(bDSPHLE);
		writeO(bFastDiscSpeed);
		writeO(CPUCore);
		writeO(bEFBAccessEnable);
		writeO(bEFBCopyEnable);
		writeO(bCopyEFBToTexture);
		writeO(bEFBCopyCacheEnable);
		writeO(bEFBEmulateFormatChanges);
		writeO(bUseXFB);
		writeO(bUseRealXFB);
		writeO(memcards);
		writeO(bClearSave);
		writeO(bongos);
		writeO(bSyncGPU);
		writeO(bNetPlay);
		writeO(discChange);
		writeO(revision);
		writeO(DSPiromHash);
		writeO(DSPcoefHash);
		writeO(tickCount);
#undef writeO
	}

	void load(std::istream & archive)
	{
		std::string gameID, author, videoBackend, audioEmulator, md5, discChange, revision;
		std::memset(this, 0, sizeof(*this));
		std::strncpy(reinterpret_cast<char*>(filetype), "DTM\x1A", sizeof(filetype));

		std::string s;
#define readI(V) getHeaderLine(archive, V);
		readI(gameID);
		readI(bWii);
		readI(numControllers);
		readI(bFromSaveState);
		readI(frameCount);
		readI(inputCount);
		readI(lagCount);
		readI(uniqueID);
		readI(numRerecords);
		readI(author);
		readI(videoBackend);
		readI(audioEmulator);
		readI(md5);
		readI(recordingStartTime);
		readI(bSaveConfig);
		readI(bSkipIdle);
		readI(bDualCore);
		readI(bProgressive);
		readI(bDSPHLE);
		readI(bFastDiscSpeed);
		readI(CPUCore);
		readI(bEFBAccessEnable);
		readI(bEFBCopyEnable);
		readI(bCopyEFBToTexture);
		readI(bEFBCopyCacheEnable);
		readI(bEFBEmulateFormatChanges);
		readI(bUseXFB);
		readI(bUseRealXFB);
		readI(memcards);
		readI(bClearSave);
		readI(bongos);
		readI(bSyncGPU);
		readI(bNetPlay);
		readI(discChange);
		readI(revision);
		readI(DSPiromHash);
		readI(DSPcoefHash);
		readI(tickCount);
#undef readI

		std::strncpy(reinterpret_cast<char*>(this->gameID), gameID.c_str(), sizeof(this->gameID));
		std::strncpy(reinterpret_cast<char*>(this->author), author.c_str(), sizeof(this->author));
		std::strncpy(reinterpret_cast<char*>(this->videoBackend), videoBackend.c_str(), sizeof(this->videoBackend));
		std::strncpy(reinterpret_cast<char*>(this->audioEmulator), audioEmulator.c_str(), sizeof(this->audioEmulator));
		hexStrToArr(md5, this->md5);
		std::strncpy(reinterpret_cast<char*>(this->discChange), discChange.c_str(), sizeof(this->discChange));
		hexStrToArr(revision, this->revision);
	}
};
static_assert(sizeof(DTMHeader) == 256, "DTMHeader should be 256 bytes");

#pragma pack(pop)

}

