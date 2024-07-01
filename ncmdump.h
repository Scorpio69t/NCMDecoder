#ifndef NCMDUMP_H
#define NCMDUMP_H

#include <QObject>
#include <windows.h>
#include <memory>
#include "algodllpraser.h"

#define CREATE_NETEASE_CRYPT "CreateNeteaseCrypt"
#define DUMP "Dump"
#define FIX_METADATA "FixMetadata"
#define DESTROY_NETEASE_CRYPT "DestroyNeteaseCrypt"

class NcmDump : public QObject {
Q_OBJECT
public:
    explicit NcmDump(QObject *parent = nullptr);

    bool loadLibrary(const std::string &configPath);

    void *CreateNeteaseCrypt(const char *path);

    int Dump(void *handle);

    void FixMetadata(void *handle);

    void DestroyNeteaseCrypt(void *handle);

private:
    bool m_isLoaded;
    std::string m_libraryPath;
    std::unique_ptr<AlgoDllPraser> m_algoDllPraser;
};

#endif // NCMDUMP_H
