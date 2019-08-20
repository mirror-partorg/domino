
#ifdef _WIN32
#define DO_EXPORT extern __declspec(dllexport)
#else
#define DO_EXPORT extern
#endif
