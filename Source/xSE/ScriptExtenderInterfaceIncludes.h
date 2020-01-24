#pragma once

#if xSE_PLATFORM_SKSE

#include <common/IPrefix.h>
#include <skse/skse_version.h>
#include <skse/SafeWrite.h>
#include <skse/PluginAPI.h>
#include <skse/GameAPI.h>
#include <skse/CommandTable.h>

#pragma comment(lib, "skse/Release/skse.lib")
#pragma comment(lib, "skse/Release/loader_common.lib")
#pragma comment(lib, "common/Release VC9/common_vc9.lib")

#elif xSE_PLATFORM_SKSE64

#include <common/IPrefix.h>
#include <skse64_common/skse_version.h>
#include <skse64_common/Relocation.h>
#include <skse64_common/SafeWrite.h>
#include <skse64/PluginAPI.h>
#include <skse64/GameAPI.h>
#include <skse64/ObScript.h>

#pragma comment(lib, "skse64/x64/Release/skse64_1_5_97.lib")
#pragma comment(lib, "skse64/x64/Release/skse64_common.lib")
#pragma comment(lib, "skse64/x64/Release/skse64_loader_common.lib")
#pragma comment(lib, "skse64/x64_v142/Release/common_vc14.lib")

#elif xSE_PLATFORM_SKSEVR

#include <common/IPrefix.h>
#include <skse64_common/skse_version.h>
#include <skse64_common/Relocation.h>
#include <skse64_common/SafeWrite.h>
#include <skse64/PluginAPI.h>
#include <skse64/GameAPI.h>
#include <skse64/ObScript.h>

#pragma comment(lib, "skseVR/x64/Release/sksevr_1_4_15.lib")
#pragma comment(lib, "skseVR/x64/Release/skse64_common.lib")
#pragma comment(lib, "skseVR/x64/Release/skse64_loader_common.lib")
#pragma comment(lib, "skseVR/x64_v142/Release/common_vc14.lib")

#elif xSE_PLATFORM_F4SE

#include <common/IPrefix.h>
#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>
#include <f4se_common/SafeWrite.h>
#include <f4se/PluginAPI.h>
#include <f4se/GameAPI.h>
#include <f4se/ObScript.h>

#pragma comment(lib, "f4se/x64/Release/f4se_1_10_163.lib")
#pragma comment(lib, "f4se/x64/Release/f4se_common.lib")
#pragma comment(lib, "f4se/x64/Release/f4se_loader_common.lib")
#pragma comment(lib, "f4se/x64_vc11/Release/common_vc11.lib")

#elif xSE_PLATFORM_NVSE

#include <common/IPrefix.h>
#include <nvse/nvse_version.h>

// #include <nvse/PluginAPI.h>
// This include file crashes the compiler, so I just copy necessary definitions from there.
#include "ScriptExtenderInterfaceNVSE.h"

#pragma comment(lib, "nvse/Release/nvse.lib")
#pragma comment(lib, "nvse/Release/loader_common.lib")

#endif
