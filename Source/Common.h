#pragma once
#define KXF_STATIC_LIBRARY 1

#include <kxf/pch.hpp>
#include <kxf/Core/FlagSet.h>
#include <kxf/Core/String.h>
#include <kxf/Core/Version.h>
#include <kxf/Log/ScopedLogger.h>
#include <kxf/Log/Categories.h>
#include <kxf/Utility/Common.h>
#include <kxf/Utility/ScopeGuard.h>

namespace PPR
{
	constexpr char ProjectName[] = "PrivateProfileRedirector";
	constexpr char ProjectAuthor[] = "Karandra";

	constexpr int VersionMajor = 0;
	constexpr int VersionMinor = 6;
	constexpr int VersionPatch = 3;
	constexpr int VersionRevision = 1;

	constexpr int MakeFullVersion(int major, int minor, int patch, int revision) noexcept
	{
		// 1.2.3.1		-> (1 * 1000) + (2 * 100) + (3 * 10) + (1 * 1) = 1231
		// 0.1			-> (0 * 1000) + (1 * 100) + (0 * 10) + (0 * 1) = 100
		return (major * 1000) + (minor * 100) + (patch * 10) + (revision * 1);
	}
	constexpr int VersionFull = MakeFullVersion(VersionMajor, VersionMinor, VersionPatch, VersionRevision);

	using kxf::operator|;
}

namespace PPR::LogCategory
{
	kxf_DefineLogCategory(xSEInterface);
	kxf_DefineLogCategory(ENBInterface);
}
