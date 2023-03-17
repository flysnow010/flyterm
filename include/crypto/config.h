#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/qglobal.h>

#if defined(CRYPTO_LIBRARY)
#  define CRYPTO_EXPORT Q_DECL_EXPORT
#else
#  define CRYPTO_EXPORT Q_DECL_IMPORT
#endif

#endif // CONFIG_H
