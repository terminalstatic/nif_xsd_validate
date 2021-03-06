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
	
	vErrArray *errs = vValidateBuf(xmlStr,xmlSize, P_ERR_VERBOSE, *schemaRes);
	free(xmlStr);

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
	
	ERL_NIF_TERM returnTerm;
	if (errs->len>0) {
		ERL_NIF_TERM returnArr = enif_make_list_from_array(env, erlErrs,errs->len);
		ERL_NIF_TERM returnState = enif_make_atom(env, "error");
		returnTerm = enif_make_tuple2(env, returnState, returnArr);
	} else {
		returnTerm = enif_make_atom(env, "ok");
	}
	vFreeErrArray(errs);
	
	return returnTerm;
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
	
	struct vXsdParserResult xsdres = vParseUrlSchema(path, P_ERR_VERBOSE);
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
	vLibxmlInit();
	int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;
	XMLSCHEMA_TYPE = enif_open_resource_type(env, NULL, "xsd_schema_resource", freeSchema, flags, NULL);
	printf("Info: nif_xsd_validate load\n");
	return(0);
}

static void
unload(ErlNifEnv *env, void *priv) {
	printf("Info: nif_xsd_validate unload\n");
}

static int
reload(ErlNifEnv *env, void **priv, ERL_NIF_TERM info) {
	printf("Info: nif_xsd_validate reload\n");
	vLibxmlCleanup();
	return(load(env, priv, info));
}

static int
upgrade(ErlNifEnv *env, void **priv, void **old_priv, ERL_NIF_TERM info) {
	printf("Info: nif_xsd_validate upgrade\n");
	vLibxmlCleanup();
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

ERL_NIF_INIT(Elixir.NifXsd, funcs, &load, &reload, &upgrade, &unload)
