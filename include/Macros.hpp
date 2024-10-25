#pragma once

#ifdef KobeBryant_EXPORTS
#define KobeBryant_API [[maybe_unused]] __declspec(dllexport)
#else
#define KobeBryant_API [[maybe_unused]] __declspec(dllimport)
#endif

#ifndef KobeBryant_NDAPI
#define KobeBryant_NDAPI [[nodiscard]] KobeBryant_API
#endif