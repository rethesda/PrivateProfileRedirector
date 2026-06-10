#include "stdafx.h"
#include "AppConfigLoader.h"

namespace PPR
{
	AppConfigLoader::AppConfigLoader(std::shared_ptr<kxf::IInputStream> stream)
	{
		m_Config.SetOptions(kxf::INIDocumentOption::MultiKey);

		if (stream && m_Config.LoadDocument(*stream))
		{
			m_General = m_Config.QuerySection("General");
			m_Redirector = m_Config.QuerySection("Redirector");
			m_XSEInterface = m_Config.QuerySection("XSE");
			m_ENBInterface = m_Config.QuerySection("ENB");
		}
	}
}
