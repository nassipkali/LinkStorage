#if defined (unix)
typedef int fd_t;
#endif

#if defined (WIN32)
typedef struct fd {
  void* hFile;
  void* hMapping;
} fd_t;
#endif
