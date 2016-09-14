#include "GPUPerfAPICounterLoader.h"
#include <string.h>


bool GPUPerfAPICounterLoader::LoadPerfAPICounterDll(const gtString& strDLLPath)
{
    if (!m_bGPAPerfAPICounterLoaded)
    {

        // Set DLL Path
        std::string strGPACounterDllName;

#ifdef USE_DEBUG_GPA
#define DEBUG_DLL_SUFFIX "-d"
#else
#define DEBUG_DLL_SUFFIX ""
#endif

        strGPACounterDllName = LIB_PREFIX "GPUPerfAPICounters" BITNESS DEBUG_DLL_SUFFIX GDT_BUILD_SUFFIX LIB_SUFFIX;

        std::wstring strGPACounterDllNameUnicode;
        StringUtils::Utf8StringToWideString(strGPACounterDllName, strGPACounterDllNameUnicode);

        gtString strGPACounterDll = strDLLPath;
        strGPACounterDll.append(strGPACounterDllNameUnicode.c_str());


        std::string utf8DllPath;
        StringUtils::WideStringToUtf8String(strGPACounterDll.asCharArray(), utf8DllPath);
        // Load counter access DLL
        m_DllModuleHandle = OSUtils::Instance()->GenericLoadLibrary(utf8DllPath);

        if (m_DllModuleHandle != NULL)
        {
            m_pGetAvailableCountersByGen = reinterpret_cast<GPA_GetAvailableCountersByGenerationProc>(OSUtils::Instance()->GetSymbolAddr(m_DllModuleHandle, "GPA_GetAvailableCountersByGeneration"));
            SpAssertRet(m_pGetAvailableCountersByGen != NULL) false;
            m_pGetAvailableCountersForDevice = reinterpret_cast<GPA_GetAvailableCountersForDeviceProc>(OSUtils::Instance()->GetSymbolAddr(m_DllModuleHandle, "GPA_GetAvailableCounters"));
            SpAssertRet(m_pGetAvailableCountersForDevice != NULL) false;

            if (m_pGetAvailableCountersByGen != nullptr && m_pGetAvailableCountersForDevice != nullptr)
            {
                m_bGPAPerfAPICounterLoaded = true;
            }
            else
            {
                m_bGPAPerfAPICounterLoaded = false;
            }
        }
        else
        {
            m_bGPAPerfAPICounterLoaded = false;
        }
    }

    return m_bGPAPerfAPICounterLoaded;
}


void GPUPerfAPICounterLoader::UnLoadPerfAPICounterDll()
{
    if (m_DllModuleHandle)
    {
        OSUtils::Instance()->GenericUnloadLibrary(m_DllModuleHandle);
        m_bGPAPerfAPICounterLoaded = false;
        m_pGetAvailableCountersByGen = nullptr;
        m_pGetAvailableCountersForDevice = nullptr;
    }
}


GPA_GetAvailableCountersForDeviceProc GPUPerfAPICounterLoader::GetGPAAvailableCountersForDeviceProc()
{
    return m_pGetAvailableCountersForDevice;
}


GPA_GetAvailableCountersByGenerationProc GPUPerfAPICounterLoader::GetGPAAvailableCountersByGenerationProc()
{
    return m_pGetAvailableCountersByGen;
}


bool GPUPerfAPICounterLoader::IsLoaded()
{
    return m_bGPAPerfAPICounterLoaded;
}

GPUPerfAPICounterLoader::GPUPerfAPICounterLoader()
{
    m_bGPAPerfAPICounterLoaded = false;
    m_pGetAvailableCountersByGen = nullptr;
    m_pGetAvailableCountersForDevice = nullptr;
}

GPUPerfAPICounterLoader::~GPUPerfAPICounterLoader()
{
    UnLoadPerfAPICounterDll();
}