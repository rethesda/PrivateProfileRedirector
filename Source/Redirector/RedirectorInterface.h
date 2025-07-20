#pragma once
#include "Common.h"
#include "CommonWinAPI.h"
#include "AppModule.h"
#include "INIWrapper.h"
#include "AppConfigLoader.h"
#include "ConfigObject.h"
#include <kxf/Core/IEncodingConverter.h>
#include <kxf/FileSystem/NativeFileSystem.h>
#include <kxf/Threading/ReadWriteLock.h>
#include <kxf/System/CFunctionHook.h>
#include <kxf/Utility/String.h>

namespace PPR
{
	class DLLApplication;
}

namespace PPR
{
	class RedirectorInterface final: public AppModule
	{
		friend struct PrivateProfileT;

		public:
			static RedirectorInterface& GetInstance();

		private:
			kxf::FlagSet<RedirectorOption> m_Options;
			kxf::Utility::SetIC<kxf::String> m_Exclusions;
			std::unique_ptr<kxf::IEncodingConverter> m_EncodingConverter;
			int m_SaveOnWriteBuffer = 0;

			mutable kxf::ReadWriteLock m_INIMapLock;
			kxf::Utility::UnorderedMapIC<kxf::String, std::unique_ptr<ConfigObject>> m_INIMap;
			std::atomic<size_t> m_TotalWriteCount = 0;

			// Hooks
			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileStringA)> m_GetStringA;
			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileStringW)> m_GetStringW;

			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileIntA)> m_GetIntA;
			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileIntW)> m_GetIntW;

			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileSectionNamesA)> m_GetSectionNamesA;
			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileSectionNamesW)> m_GetSectionNamesW;

			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileSectionA)> m_GetSectionA;
			kxf::CFunctionTypedHook<decltype(::GetPrivateProfileSectionW)> m_GetSectionW;

			kxf::CFunctionTypedHook<decltype(::WritePrivateProfileStringA)> m_WriteStringA;
			kxf::CFunctionTypedHook<decltype(::WritePrivateProfileStringW)> m_WriteStringW;

		private:
			void LoadConfig(DLLApplication& app, const AppConfigLoader& config);

			void InitHooks();
			void RestoreHooks();
			void SetupIntegrations(DLLApplication& app);

		public:
			RedirectorInterface(DLLApplication& app, const AppConfigLoader& config);
			~RedirectorInterface();

		public:
			// AppModule
			void OnInit(DLLApplication& app) override;
			void OnExit(DLLApplication& app) override;
			
			// RedirectorInterface
			kxf::IEncodingConverter& GetEncodingConverter() const noexcept
			{
				return *m_EncodingConverter;
			}
			std::optional<size_t> GetSaveOnWriteBuffer() const noexcept
			{
				if (m_SaveOnWriteBuffer > 0)
				{
					return m_SaveOnWriteBuffer;
				}
				return {};
			}
			bool IsOptionEnabled(RedirectorOption option) const noexcept
			{
				return m_Options.Contains(option);
			}

			ConfigObject& GetOrLoadFile(const kxf::String& filePath);
			size_t SaveChangedFiles(const wchar_t* message);
			size_t OnFileWrite(ConfigObject& configObject) noexcept;
			size_t RefreshINI();
	};
}
