#define C16NAPI_ERR_GENERIC "C16NAPI_ERR_GENERIC"
#define C16NAPI_ERR_ARGCOUNT "C16NAPI_ERR_ARGCOUNT"
#define C16NAPI_ERR_TYPE "C16NAPI_ERR_TYPE"
#define C16NAPI_ERR_VALUE "C16NAPI_ERR_VALUE"

enum c16napi_text_function {
    c16napi_fun_textread,
    c16napi_fun_textcreate,
    c16napi_fun_textdelete,
    c16napi_fun_textcopy,
    c16napi_fun_textrename,
    c16napi_fun_getprocsmetadata,
    c16napi_fun_getprocmetadata
};

struct c16napi_options {
    enum c16napi_text_function func;
    vFLAGS funcopts;
};