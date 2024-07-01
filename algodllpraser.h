#ifndef ALGODLLPRASER_H
#define ALGODLLPRASER_H

#include <QObject>
#include <QDebug>
#include <map>
#include <functional>
#include <string>

#if _WIN32

#include <Windows.h>

#else
#include <dlfcn.h>
#endif


class AlgoDllPraser : public QObject {
Q_OBJECT
public:
    explicit AlgoDllPraser(QObject *parent = nullptr);

    ~AlgoDllPraser();

    bool LoadDll(const std::string &ip_dllPath);

    bool UnLoadDll();

    template<typename T>
    std::function<T> AlgoDllFunc(const std::string &ip_funcName) {
        //把从dll中加载进来的函数保存到字典中
        auto it = m_dllFuncHandleMap.find(ip_funcName);
        if (it == m_dllFuncHandleMap.end())// 如果搜索到最后说明没搜索到，那就需要继续加载
        {
#if _WIN32
            auto addr = GetProcAddress(m_hMod, ip_funcName.c_str()); // 通过dll的句柄和需要加载额函数名称进行加载
#else
            auto addr = dlsym(m_hMod, ip_funcName.c_str()); // 通过dll的句柄和需要加载额函数名称进行加载
#endif \
    // 获取到函数的地址后判断是否为空然后进行保存
            if (!addr) {
                return nullptr;
            }
            m_dllFuncHandleMap.insert(std::make_pair(ip_funcName, addr)); // 把加载到的函数和地址进行保存
            // 然后在进行查找
            it = m_dllFuncHandleMap.find(ip_funcName);
        }


        // 把查找到的地址进程强制类型转换即 (T*) (it->second)，然后返回function的函数对象
        return std::function<T>((T *) (it->second));
    }

    // T是函数类型，ip_funcName函数名称，获取到function类型的f后，把参数传入f进行计算
    template<typename T, typename ...Args>
    typename std::result_of<std::function<T>(Args...)>::type
    ExcecuteAlgoFunc(const std::string &ip_funcName, Args &&...args) {
        auto f = AlgoDllFunc<T>(ip_funcName);
        if (f == nullptr) {
            exit(1);
        }
        // 使用std::forward完美转发，即args是右转引用，通过完美转发后仍然是右转引用类型
        return f(std::forward<Args>(args)...);
    }

private:
    bool m_dllLoadedState;
#if _WIN32
    HMODULE m_hMod; //dll的句柄
#else
    int m_hMod;     //dll的句柄
#endif
    std::map<std::string, FARPROC> m_dllFuncHandleMap;
};

#endif // ALGODLLPRASER_H
