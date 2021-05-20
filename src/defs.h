#ifndef DEFS_H
#define DEFS_H

#include <iostream>

#define UNUSED(x) (void)(x)

#define _throwError(x) (std::cerr << "-| ERROR :: " << __FILE__ << " :: " << __LINE__ \
                                          << " :: " << __func__ << std::endl \
                                  << " | MESSAGE :: " << x << std::endl << std::endl)

#ifdef DEBUG
#define _printWarning(x) (std::cerr << "-| WARNING :: " << __FILE__ << " :: " << __LINE__ \
                                              << " :: " << __func__ << std::endl \
                                    << " | MESSAGE :: " << x << std::endl << std::endl)
#else
#define _printWarning(x)
#endif

#ifdef DEBUG
#define _printDebug(x) (std::cout << "-| DEBUG :: " << __FILE__ << " :: " << __LINE__ \
                                          << " :: " << __func__ << std::endl \
                                  << " | " << x << std::endl << std::endl)
#else
#define _printDebug(x)
#endif

#ifdef DEBUG
#define _log(x) (std::cout << "-| LOG :: " << __DATE__ << " :: " << __TIME__ << "\n" \
                           << " | " << x << std::endl << std::endl)
#else
#define _log(x)
#endif

extern bool GLFW_INITIALIZED;

#endif
