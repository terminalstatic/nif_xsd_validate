#include <erl_nif.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vlibxml.h"

void freeSchema(ErlNifEnv *env, void *res);
static void chopCRLF(char *str);
static void chopPoint(char *str);

ErlNifResourceType *XMLSCHEMA_TYPE;
static ERL_NIF_TERM
validate(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	ErlNifBinary xml;
	if (!enif_inspect_binary(env, argv[1], &xml))
	{
		return enif_make_badarg(env);
	}

	xmlSchemaPtr *schemaRes;
	if (!enif_get_resource(env, argv[0], XMLSCHEMA_TYPE, (void *) &schemaRes)) {
		return enif_make_badarg(env);
	}
	
	size_t xmlSize = xml.size + 1;
	char *xmlStr = malloc(xmlSize);
	memcpy(xmlStr, xml.data, xml.size);
	enif_release_binary(&xml);
	xmlStr[xml.size] = '\0';
	
	errArray *errs = cValidateBuf(xmlStr,xmlSize, P_ERR_VERBOSE, *schemaRes);
	free(xmlStr);

	// Code for String result
	// ERL_NIF_TERM result;
	// int rsize = GO_ERR_INIT;
	// char *errMsg = calloc(rsize, sizeof(char));
	// for (int i=0;i<errs->len;i++) {
	// 	int tlen = strlen(errMsg) + strlen(errs->data[i].message) + 1;
	// 	if (tlen > rsize) {
	// 		char *tmp = malloc(rsize + tlen * 2);
	// 		memcpy(tmp, errMsg, strlen(errMsg) + 1);
	// 		free(errMsg);				
	// 		errMsg = tmp;
	// 	} 
	// 	strcat(errMsg, errs->data[i].message);				
	// }

	// freeErrArray(errs);
	// chopCRLF(errMsg);
	// chopPoint(errMsg);
	// size_t size = strlen(errMsg);

	// ErlNifBinary errStr;
	// enif_alloc_binary(size, &errStr);
	// memcpy(errStr.data, errMsg, size);		
	// free(errMsg);

	// result = enif_make_tuple2(env, size > 0 ? enif_make_atom(env, "error") : enif_make_atom(env, "ok"), enif_make_binary(env, &errStr));
	// return result;

	// Code for array result
	ERL_NIF_TERM erlErrs[errs->len];
	for (int i=0;i<errs->len;i++) {
		chopCRLF(errs->data[i].message);
		chopPoint(errs->data[i].message);
		size_t lSize = strlen(errs->data[i].message);
		ErlNifBinary erlErr;
		enif_alloc_binary(lSize, &erlErr);
		memcpy(erlErr.data, errs->data[i].message, lSize);
		erlErrs[i] = enif_make_binary(env, &erlErr);
	}
	ERL_NIF_TERM returnArr = errs->len>0 ? enif_make_list_from_array(env, erlErrs,errs->len) : enif_make_list(env, 0);
	ERL_NIF_TERM returnState = errs->len>0 ? enif_make_atom(env, "error") : enif_make_atom(env, "ok");
	
	return enif_make_tuple2(env, returnState, returnArr);
}

static ERL_NIF_TERM
loadSchema(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
	ErlNifBinary erlPath;
	if (!enif_inspect_binary(env, argv[0], &erlPath))
	{
		return enif_make_badarg(env);
	}

	ERL_NIF_TERM result; 

	char *path = malloc(erlPath.size + 1);
	memcpy(path, erlPath.data, erlPath.size);
	enif_release_binary(&erlPath);
	path[erlPath.size] = '\0';
	
	struct xsdParserResult xsdres = cParseUrlSchema(path, P_ERR_VERBOSE);
	free(path);
	if (xsdres.schemaPtr==NULL) {
		chopCRLF(xsdres.errorStr);
		chopPoint(xsdres.errorStr);
		size_t size = strlen(xsdres.errorStr);

		ErlNifBinary errStr;
		enif_alloc_binary(size, &errStr);
		memcpy(errStr.data, xsdres.errorStr, size);		
		
		result = enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_binary(env, &errStr));
	} else {
		//xsdres.schemaPtr->preserve = 12;
		xmlSchemaPtr *schemaRes = enif_alloc_resource(XMLSCHEMA_TYPE, sizeof(xmlSchemaPtr *));
		memcpy((void *) schemaRes, (void *) &xsdres.schemaPtr, sizeof(xmlSchemaPtr *));
		
		ERL_NIF_TERM schema = enif_make_resource(env, schemaRes);
		enif_release_resource(schemaRes);
		
		result = enif_make_tuple2(env, enif_make_atom(env, "ok"), schema);
	}
	free(xsdres.errorStr);	
	return result;
}

void 
freeSchema(ErlNifEnv *env, void *res) {
	//printf("IntGC: %d\n", (*s)->preserve);
	xmlSchemaPtr *xsdres = res;
	xmlSchemaFree(*xsdres);
	printf("Schema garbage collected");
}

#ifdef ERL_NIF_DIRTY_SCHEDULER_SUPPORT 
static ErlNifFunc funcs[] = {
	    {"validate", 2, validate, ERL_NIF_DIRTY_JOB_CPU_BOUND},
		{"load_schema", 1, loadSchema, ERL_NIF_DIRTY_JOB_IO_BOUND}
		};
#else
static ErlNifFunc funcs[] = {
		{"validate", 2, validate},
		{"load_schema", 1, loadSchema}
		};
#endif

static int
load(ErlNifEnv *env, void **priv, ERL_NIF_TERM info) {
	init();
	int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
	XMLSCHEMA_TYPE = enif_open_resource_type(env, NULL, "xsd_schema_resource", freeSchema, flags, NULL);
	printf("Nif info: load\n");
	return(0);
}

static void
unload(ErlNifEnv *env, void *priv) {
	cleanup();
	printf("Nif info: unload");
}

static int
reload(ErlNifEnv *env, void **priv, ERL_NIF_TERM info) {
	unload(env,priv);
	printf("Nif info: reload\n");
	return(load(env, priv, info));
}

static int
upgrade(ErlNifEnv *env, void **priv, void **old_priv, ERL_NIF_TERM info) {
	unload(env,priv);
	printf("Nif info: upgrade\n");
	return load(env, priv, info);
}

static void chopCRLF(char *str) {
		if (str !=NULL && strlen(str) > 0 && str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0';
}
static void chopPoint(char *str) {
		if (str !=NULL && strlen(str) > 0 && str[strlen(str) - 1] == '.')
			str[strlen(str) - 1] = '\0';
}

//ERL_NIF_INIT(Elixir.ValidateXsd, funcs, &load, &reload, &upgrade, &unload)
ERL_NIF_INIT(Elixir.ValidateXsd, funcs, &load, &reload, &upgrade, &unload)

