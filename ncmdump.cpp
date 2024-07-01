#include "ncmdump.h"

NcmDump::NcmDump(QObject *parent)
        : QObject{parent} {
    m_algoDllPraser = std::make_unique<AlgoDllPraser>();
}

bool NcmDump::loadLibrary(const std::string &configPath) {
    m_libraryPath = configPath;
    m_isLoaded = m_algoDllPraser->LoadDll(m_libraryPath);
    return m_isLoaded;
}

void *NcmDump::CreateNeteaseCrypt(const char *path) {
    return m_algoDllPraser->ExcecuteAlgoFunc < void *(const char*)>(CREATE_NETEASE_CRYPT, path);
}

int NcmDump::Dump(void *handle) {
    return m_algoDllPraser->ExcecuteAlgoFunc<int(void *)>(DUMP, handle);
}

void NcmDump::FixMetadata(void *handle) {
    return m_algoDllPraser->ExcecuteAlgoFunc<void(void *)>(FIX_METADATA, handle);
}

void NcmDump::DestroyNeteaseCrypt(void *handle) {
    return m_algoDllPraser->ExcecuteAlgoFunc<void(void *)>(DESTROY_NETEASE_CRYPT, handle);
}
