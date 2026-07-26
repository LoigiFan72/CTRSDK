#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void nnMain();
void nninitStartUp();

typedef void (*nninitStaticInitFunc)();

void nninitCallStaticInitializers();
void nninitSetup();
void nninitSetupDaemons();
void nninitSystem();

#ifdef __cplusplus
}   // extern "C"
#endif