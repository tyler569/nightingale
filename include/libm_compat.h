#pragma once

/* Compatibility macros for sortix-libm */

#ifndef __warn_references
#define __warn_references(sym, msg)
#endif

#ifndef _C_LABEL
#define _C_LABEL(x) x
#endif

#ifndef _C_LABEL_STRING
#define _C_LABEL_STRING(x) #x
#endif

#ifndef __strong_alias
#define __strong_alias(alias, sym) /* temporarily disabled */
#endif

#ifndef __weak_alias
#define __weak_alias(alias, sym) /* temporarily disabled */
#endif