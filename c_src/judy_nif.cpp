#include "erl_nif.h"

#ifdef __cplusplus
extern "C" {
#endif


/// TODO - Document.
static ERL_NIF_TERM
judy_hs_new(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // TODO - Write.
    return enif_make_int(env, 0);
}


/// TODO - Document.
static ERL_NIF_TERM
judy_hs_reserve(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // TODO - Write.
    return enif_make_int(env, 0);
}


/// TODO - Document.
static ERL_NIF_TERM
judy_hs_insert(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // TODO - Write.
    return enif_make_int(env, 0);
}


/// TODO - Document.
static ERL_NIF_TERM
judy_hs_delete(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // TODO - Write.
    return enif_make_int(env, 0);
}


/// TODO - Document.
static ERL_NIF_TERM
judy_hs_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // TODO - Write.
    return enif_make_int(env, 0);
}


/// Array of function descriptors for all the implemented NIFs in this library.
static ErlNifFunc judy_hs_funs[] = {
  {"new",         0, judy_hs_new},
  {"reserve",     2, judy_hs_reserve},
  {"nif_insert",  3, judy_hs_insert},
  {"nif_delete",  2, judy_hs_delete},
  {"nif_get",     2, judy_hs_get}
};

// Initialize the NIF library.
ERL_NIF_INIT(judy_hs, judy_hs_funs, NULL, NULL, NULL, NULL);


#ifdef __cplusplus
}
#endif
