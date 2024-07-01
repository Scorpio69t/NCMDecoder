#include "algodllpraser.h"

AlgoDllPraser::AlgoDllPraser(QObject *parent)
        : QObject{parent} {

}

AlgoDllPraser::~AlgoDllPraser() {
    UnLoadDll();
}

bool AlgoDllPraser::LoadDll(const std::string &ip_dllPath) {
    //获取句柄
#if _WIN32
    m_hMod = LoadLibraryA(ip_dllPath.data());
#else

#endif
    if (nullptr == m_hMod) {
        return false;
    }

    return true;
}

bool AlgoDllPraser::UnLoadDll() {
    //释放资源
    if (m_hMod == nullptr) {
        return true;
    }

    auto b = FreeLibrary(m_hMod);
    if (!b) {
        return false;
    }

    m_hMod = nullptr;
    return true;
}
