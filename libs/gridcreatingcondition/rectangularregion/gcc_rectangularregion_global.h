#ifndef GCC_RECTANGULARREGION_GLOBAL_H
#define GCC_RECTANGULARREGION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCC_RECTANGULARREGION_LIBRARY)
#  define GCC_RECTANGULARREGION_EXPORT Q_DECL_EXPORT
#else
#  define GCC_RECTANGULARREGION_EXPORT Q_DECL_IMPORT
#endif

#endif // GCC_RECTANGULARREGION_GLOBAL_H
