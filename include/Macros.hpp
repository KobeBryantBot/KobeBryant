#pragma once

#ifdef _WIN32

#ifdef KobeBryant_EXPORTS
#define KobeBryant_API [[maybe_unused]] __declspec(dllexport)
#else
#define KobeBryant_API [[maybe_unused]] __declspec(dllimport)
#endif

#ifndef KobeBryant_NDAPI
#define KobeBryant_NDAPI [[nodiscard]] KobeBryant_API
#endif

#else

#define KobeBryant_API [[maybe_unused]] __attribute__((visibility("default")))

#ifndef KobeBryant_NDAPI
#define KobeBryant_NDAPI [[nodiscard]] KobeBryant_API
#endif

#endif