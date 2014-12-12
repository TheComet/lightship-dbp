void* module_open(const char* filename);
void* module_sym(void* handle, const char* symbol);
void module_close(void* handle);