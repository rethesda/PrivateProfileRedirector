#pragma once
#include "Common.h"
#include <kxf/Serialization/INI.h>
#include <kxf/Core/IEncodingConverter.h>

namespace PPR
{
	template<class TChar>
	auto EncodingFrom(const kxf::String& str, kxf::IEncodingConverter& converter)
	{
		if constexpr(std::is_same_v<TChar, char>)
		{
			return converter.ToMultiByte(kxf::StringViewOf(str));
		}
		else if constexpr(std::is_same_v<TChar, wchar_t>)
		{
			return str.view();
		}
		else
		{
			static_assert(sizeof(TChar*) == 0, "invalid type");
		}
	}

	inline kxf::String EncodingTo(const char* str, kxf::IEncodingConverter& converter)
	{
		if (str)
		{
			return converter.ToWideChar(str);
		}
		return {};
	}
	inline kxf::String EncodingTo(const wchar_t* str, kxf::IEncodingConverter& converter)
	{
		if (str)
		{
			return str;
		}
		return {};
	}
}

namespace PPR
{
	template<class TChar>
	class ZSSTRZZ final
	{
		private:
			std::basic_string<TChar> m_Buffer;
			kxf::IEncodingConverter& m_EncodingConverter;
			size_t m_MaxSize = 0;

			size_t m_Count = 0;
			bool m_Truncated = false;

		private:
			bool TruncateIfNeeded()
			{
				if (m_Buffer.length() > m_MaxSize)
				{
					m_Buffer.resize(m_MaxSize);
					if (m_MaxSize >= 2)
					{
						m_Buffer[m_MaxSize - 2] = 0;
					}
					if (m_MaxSize >= 1)
					{
						m_Buffer[m_MaxSize - 1] = 0;
					}

					return true;
				}
				return false;
			}

		public:
			ZSSTRZZ(size_t maxSize, kxf::IEncodingConverter& encodingConverter)
				:m_EncodingConverter(encodingConverter), m_MaxSize(maxSize)
			{
				m_Buffer.reserve(m_MaxSize <= 8192 ? m_MaxSize : 0);
			}

		public:
			void OnItem(const kxf::String& item)
			{
				if (m_MaxSize != 0 && m_Buffer.size() < m_MaxSize)
				{
					m_Buffer.append(EncodingFrom<TChar>(item, m_EncodingConverter));
					m_Buffer.append(1, 0);

					m_Count++;
				}
			}
			void OnItem(const kxf::String& key, const kxf::String& value)
			{
				OnItem(key + '=' + value);
			}
			std::basic_string<TChar>& Finalize(bool* truncated = nullptr, size_t* count = nullptr)
			{
				if (m_MaxSize != 0)
				{
					m_Buffer.append(m_Count != 0 ? 1 : 2, 0);
					m_Truncated = TruncateIfNeeded();
				}
				else
				{
					m_Buffer.clear();
					m_Truncated = true;
				}
				kxf::Utility::SetIfNotNull(truncated, m_Truncated);
				kxf::Utility::SetIfNotNull(count, m_Count);

				return m_Buffer;
			}

			size_t GetCount() const noexcept
			{
				return m_Count;
			}
			bool WasTruncated() const noexcept
			{
				return m_Truncated;
			}
	};
}

namespace PPR
{
	class INIWrapper final
	{
		public:
			enum class Options: uint32_t
			{
				None = 0,
				WithBOM = 1u << 1
			};
			enum class Encoding
			{
				None = -1,
				Auto,

				UTF8,
				UTF16LE,
				UTF16BE,
				UTF32LE,
				UTF32BE
			};

			template<class TChar, class TFunc>
			requires(std::is_invocable_r_v<kxf::CallbackCommand, TFunc, std::basic_string<TChar>&, const kxf::String&>)
			static std::basic_string<TChar> CreateZSSTRZZ(TFunc&& func, const std::vector<kxf::String>& items, size_t maxSize, bool* truncated, size_t* count)
			{
				kxf::Utility::SetIfNotNull(count, 0);
				kxf::Utility::SetIfNotNull(truncated, false);

				if (maxSize == 0)
				{
					return {};
				}

				size_t addedCount = 0;
				std::basic_string<TChar> buffer;
				buffer.reserve(maxSize <= 8192 ? maxSize : 0);

				for (const kxf::String& item: items)
				{
					kxf::CallbackCommand result = std::invoke(func, buffer, item);
					if (result == kxf::CallbackCommand::Continue || result == kxf::CallbackCommand::Discard)
					{
						if (result == kxf::CallbackCommand::Continue)
						{
							addedCount++;
							buffer.append(1, 0);
						}

						if (buffer.length() >= maxSize)
						{
							break;
						}
					}
					else if (result == kxf::CallbackCommand::Terminate)
					{
						break;
					}
				}
				buffer.append(addedCount != 0 ? 1 : 2, 0);

				kxf::Utility::SetIfNotNull(count, addedCount);
				if (TruncateZSSTRZZ(buffer, maxSize))
				{
					kxf::Utility::SetIfNotNull(truncated, true);
				}
				return buffer;
			}
			
		private:
			template<class TChar>
			static bool TruncateZSSTRZZ(std::basic_string<TChar>& buffer, size_t maxSize)
			{
				if (buffer.length() > maxSize)
				{
					buffer.resize(maxSize);
					if (maxSize >= 2)
					{
						buffer[maxSize - 2] = 0;
					}
					if (maxSize >= 1)
					{
						buffer[maxSize - 1] = 0;
					}

					return true;
				}
				return false;
			}

			template<class TChar>
			static std::basic_string<TChar> ToZSSTRZZ(const std::vector<kxf::String>& items, size_t maxSize, bool* truncated, size_t* count, kxf::IEncodingConverter& converter)
			{
				return CreateZSSTRZZ<TChar>([&](std::basic_string<TChar>& buffer, const kxf::String& item)
				{
					if (!item.empty())
					{
						buffer.append(EncodingFrom<TChar>(item, converter));
						return kxf::CallbackCommand::Continue;
					}
					return kxf::CallbackCommand::Discard;
				}, items, maxSize, truncated, count);
			}

		private:
			kxf::INIDocument m_INI;
			kxf::FlagSet<Options> m_Options;
			Encoding m_Encoding = Encoding::None;

		private:
			std::optional<kxf::String> ExtractValue(kxf::String value) const;

		public:
			INIWrapper() = default;

		public:
			bool IsEmpty() const noexcept
			{
				return m_INI.IsNull();
			}
			Encoding GetEncoding() const noexcept
			{
				return m_Encoding;
			}
			kxf::FlagSet<Options> GetOptions() const noexcept
			{
				return m_Options;
			}

			auto& Get() noexcept
			{
				return m_INI;
			}
			auto& Get() const noexcept
			{
				return m_INI;
			}

			bool Load(const kxf::FSPath& path, kxf::FlagSet<kxf::INIDocumentOption> options);
			bool Save(const kxf::FSPath& path, Encoding encoding = Encoding::None);

			std::optional<kxf::String> QueryValue(const kxf::String& section, const kxf::String& key) const
			{
				if (auto value = m_INI.QuerySectionAttribute(section, key))
				{
					return ExtractValue(std::move(*value));
				}
				return {};
			}
			kxf::String GetValue(const kxf::String& section, const kxf::String& key, kxf::String defaultValue = {}) const
			{
				auto value = QueryValue(section, key);
				return value ? std::move(*value) : std::move(defaultValue);
			}
			bool SetValue(const kxf::String& section, const kxf::String& key, const kxf::String& value, bool* sameData = nullptr)
			{
				if (sameData)
				{
					auto oldValue = QueryValue(section, key);
					if (!oldValue || *oldValue != value)
					{
						*sameData = false;
						return m_INI.SetSectionAttribute(section, key, value);
					}
					else
					{
						*sameData = true;
						return true;
					}
				}
				else
				{
					return m_INI.SetSectionAttribute(section, key, value);
				}
			}

			bool DeleteSection(const kxf::String& section)
			{
				return m_INI.RemoveSection(section);
			}
			bool DeleteKey(const kxf::String& section, const kxf::String& key)
			{
				return m_INI.RemoveSectionAttribute(section, key);
			}

		public:
			template<class TChar>
			std::basic_string<TChar> GetSectionNamesZSSTRZZ(kxf::IEncodingConverter& converter, size_t maxSize = kxf::String::npos, bool* truncated = nullptr, size_t* count = nullptr) const
			{
				ZSSTRZZ<TChar> zstr(maxSize, converter);
				m_INI.EnumSectionNames([&](kxf::String name)
				{
					zstr.OnItem(name);
				});
				return zstr.Finalize(truncated, count);
			}

			template<class TChar>
			std::basic_string<TChar> GetKeyNamesZSSTRZZ(kxf::IEncodingConverter& converter, const kxf::String& section, size_t maxSize = kxf::String::npos, bool* truncated = nullptr, size_t* count = nullptr) const
			{
				bool isMultikey = m_INI.GetOptions().Contains(kxf::INIDocumentOption::MultiKey);

				ZSSTRZZ<TChar> zstr(maxSize, converter);
				m_INI.EnumAttributeNames(section, [&](kxf::String name)
				{
					zstr.OnItem(name);
				}, !isMultikey);
				return zstr.Finalize(truncated, count);
			}
	};
}

namespace kxf
{
	kxf_FlagSet_Declare(PPR::INIWrapper::Options);
}
