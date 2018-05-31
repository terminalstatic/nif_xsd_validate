#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vlibxml.h"

void vFreeErrArray(vErrArray *errArr) {
	for (int i=0;i<errArr->len;i++) {
		free(errArr->data[i].message);
		free(errArr->data[i].node);
	}
	free(errArr->data);
	free(errArr);
}
static void noOutputCallback(void *ctx, const char *message, ...) {}
void vLibxmlInit() {
	xmlInitParser();
}
void vLibxmlCleanup() {
	xmlSchemaCleanupTypes();
	xmlCleanupParser();
}
static void genErrorCallback(void *ctx, const char *message, ...) {
	struct vErrCtx *ectx = ctx;
	char *newLine = malloc(P_ERR_INIT);
	va_list varArgs;
	va_start(varArgs, message);
	int oldLen = strlen(ectx->errBuf) + 1;
	int lineLen = 1 + vsnprintf(newLine, P_ERR_INIT, message, varArgs);
	if (lineLen  > P_ERR_INIT) {
		va_end(varArgs);
		va_start(varArgs, message);
		free(newLine);
		newLine = malloc(lineLen);
		vsnprintf(newLine, lineLen, message, varArgs);
		va_end(varArgs);
	} else {
		va_end(varArgs);
	}
	char *tmp = malloc(oldLen + lineLen);
	memcpy(tmp, ectx->errBuf, oldLen);
	strcat(tmp, newLine);
	free(newLine);
	free(ectx->errBuf);
	ectx->errBuf = tmp;
}
static void simpleStructErrorCallback(void *ctx, xmlErrorPtr p) {
	vErrArray *sErrArr = ctx;
	struct vSimpleXmlError sErr;
	sErr.message = calloc(P_ERR_INIT, sizeof(char));
	sErr.node = calloc(P_ERR_INIT, sizeof(char));
	sErr.type = VALIDATION_ERROR;
	sErr.code = p->code;
	sErr.level = p->level;
	sErr.line = p->line;
	int cpyLen = 1 + snprintf(sErr.message, P_ERR_INIT, "%s", p->message);
	if (cpyLen > P_ERR_INIT) {
		free(sErr.message);
		sErr.message = malloc(cpyLen);
		snprintf(sErr.message, cpyLen, "%s", p->message);
	}
	if (p->node !=NULL) {
		cpyLen = 1 + snprintf(sErr.node, P_ERR_INIT, "%s", (((xmlNodePtr) p->node)->name));
		if (cpyLen > P_ERR_INIT) {
			free(sErr.node);
			sErr.node= malloc(cpyLen);
			snprintf(sErr.node, cpyLen, "%s", (((xmlNodePtr) p->node)->name));
		}
	}
	if (sErrArr->len >= sErrArr->cap) {
		sErrArr->cap = sErrArr->cap * 2;
		struct vSimpleXmlError *tmp = calloc(sErrArr->cap, sizeof(*tmp));
		memcpy(tmp, sErrArr->data, sErrArr->len * sizeof(*tmp));
		free(sErrArr->data);
		sErrArr->data = tmp;
	}
	sErrArr->data[sErrArr->len] = sErr;
	sErrArr->len++;
}
struct vXsdParserResult vParseUrlSchema(const char *url, const short int options) {
	xmlLineNumbersDefault(1);
	bool err = false;
	struct vXsdParserResult parserResult;
	char *errBuf=NULL;
	struct vErrCtx ectx;
	ectx.errBuf=calloc(P_ERR_INIT, sizeof(char));
	xmlSchemaPtr schema = NULL;
	xmlSchemaParserCtxtPtr schemaParserCtxt = NULL;
	schemaParserCtxt = xmlSchemaNewParserCtxt(url);
	if (schemaParserCtxt == NULL) {
		err = true;
		strcpy(ectx.errBuf, "Xsd parser internal error");
	}
	else
	{
		if (options & P_ERR_VERBOSE) {
			xmlSchemaSetParserErrors(schemaParserCtxt, noOutputCallback, noOutputCallback, NULL);
			xmlSetGenericErrorFunc(&ectx, genErrorCallback);
		} else {
			xmlSetGenericErrorFunc(NULL, noOutputCallback);
			xmlSchemaSetParserErrors(schemaParserCtxt, genErrorCallback, noOutputCallback, &ectx);
		}
		schema = xmlSchemaParse(schemaParserCtxt);
		xmlSchemaFreeParserCtxt(schemaParserCtxt);
		if (schema == NULL) {
			err = true;
			char *tmp = malloc(strlen(ectx.errBuf) + 1);
			memcpy(tmp, ectx.errBuf, strlen(ectx.errBuf) + 1);
			free(ectx.errBuf);
			ectx.errBuf = tmp;
		}
	}
	errBuf=malloc(strlen(ectx.errBuf)+1);
	memcpy(errBuf,  ectx.errBuf, strlen(ectx.errBuf)+1);
	free(ectx.errBuf);
	parserResult.schemaPtr=schema;
	parserResult.errorStr=errBuf;
	errno = err ? -1 : 0;
	return parserResult;
}
static struct vXmlParserResult vParseDoc(const void *xmlSource, const int xmlSourceLen, const short int options) {
	xmlLineNumbersDefault(1);
	bool err = false;
	struct vXmlParserResult parserResult;
	char *errBuf=NULL;
	struct vErrCtx ectx;
	ectx.errBuf=calloc(P_ERR_INIT, sizeof(char));;
	xmlDocPtr doc=NULL;
	xmlParserCtxtPtr xmlParserCtxt=NULL;
	if (xmlSourceLen == 0) {
		err = true;
		if (options & P_ERR_VERBOSE) {
			strcpy(ectx.errBuf, "parser error : Document is empty");
		} else {
			strcpy(ectx.errBuf, "Malformed xml document");
		}
	} else {
		xmlParserCtxt = xmlNewParserCtxt();
		if (xmlParserCtxt == NULL) {
			err = true;
			strcpy(ectx.errBuf, "Xml parser internal error");
		}
		else
		{
			if (options & P_ERR_VERBOSE) {
				xmlSetGenericErrorFunc(&ectx, genErrorCallback);
			} else {
				xmlSetGenericErrorFunc(NULL, noOutputCallback);
			}
			xmlCtxtUseOptions(xmlParserCtxt, XML_PARSE_IGNORE_ENC);
			doc = xmlParseMemory(xmlSource, xmlSourceLen);
			xmlFreeParserCtxt(xmlParserCtxt);
			if (doc == NULL) {
				err = true;
				if (options & P_ERR_VERBOSE) {
					char *tmp = calloc(strlen(ectx.errBuf) + 1, sizeof(*tmp));					
					memcpy(tmp, ectx.errBuf, strlen(ectx.errBuf));
					free(ectx.errBuf);
					ectx.errBuf = tmp;
				} else {
					strcpy(ectx.errBuf, "Malformed xml document");
				}
			}
		}
	}
	errBuf=calloc(strlen(ectx.errBuf) + 1, sizeof(char));
	memcpy(errBuf,  ectx.errBuf, strlen(ectx.errBuf));	
	free(ectx.errBuf);

	parserResult.docPtr=doc;
	parserResult.errorStr=errBuf;
	errno = err ? -1 : 0;
	return parserResult;
}
static vErrArray *vValidate(const xmlDocPtr doc, const xmlSchemaPtr schema) {
	xmlLineNumbersDefault(1);
	vErrArray *errArr = malloc(sizeof(*errArr));
	errArr->data = calloc(2, sizeof(*errArr->data));
	errArr->len=0;
	errArr->cap=2;
	struct vSimpleXmlError simpleError;
	simpleError.message = calloc(P_ERR_INIT, sizeof(char));
	if (schema == NULL) {
		simpleError.type = LIBXML2_ERROR;
		strcpy(simpleError.message, "Xsd schema null pointer");
		errArr->data[errArr->len] = simpleError;
		errArr->len++;
	}
	else if (doc == NULL) {
		simpleError.type = LIBXML2_ERROR;
		errArr->len++;
	}
	else
	{
		xmlSchemaValidCtxtPtr schemaCtxt;
		schemaCtxt = xmlSchemaNewValidCtxt(schema);
		if (schemaCtxt == NULL) {
			simpleError.type = LIBXML2_ERROR;
			strcpy(simpleError.message, "Xml validation internal error");
			errArr->data[errArr->len] = simpleError;
			errArr->len++;
		}
		else
		{
			xmlSchemaSetValidStructuredErrors(schemaCtxt, simpleStructErrorCallback, errArr);
			int schemaErr = xmlSchemaValidateDoc(schemaCtxt, doc);
			xmlSchemaFreeValidCtxt(schemaCtxt);
			if (schemaErr < 0 && errArr->len == 0)
			{
				simpleError.type = LIBXML2_ERROR;
				strcpy(simpleError.message, "Xml validation internal error");
				errArr->data[errArr->len] = simpleError;
				errArr->len++;
			}
			else {
				simpleError.type = NO_ERROR;
			}
		}
	}
	free(simpleError.message);
	errno = errArr->len == NO_ERROR ? 0 : -1;
	return errArr;
}
vErrArray *vValidateBuf(const void *xmlSource, const int xmlSourceLen, const short int xmlParserOptions, const xmlSchemaPtr schema) {
	xmlLineNumbersDefault(1);
	vErrArray *errArr = malloc(sizeof(*errArr));
	errArr->data = calloc(2, sizeof(*errArr->data));
	errArr->len=0;
	errArr->cap=2;
	struct vSimpleXmlError simpleError;
	simpleError.message = calloc(P_ERR_INIT, sizeof(char));
	struct vXmlParserResult parserResult = vParseDoc(xmlSource, xmlSourceLen, xmlParserOptions);
	if (schema == NULL) {
		simpleError.type = LIBXML2_ERROR;
		strcpy(simpleError.message, "Xsd schema null pointer");
		errArr->data[errArr->len] = simpleError;
		errArr->len++;
		xmlFreeDoc(parserResult.docPtr);
	}
	else if (parserResult.docPtr == NULL) {
		simpleError.type = XML_PARSER_ERROR;
		free(simpleError.message);
		simpleError.message = calloc(P_ERR_INIT, strlen(parserResult.errorStr) + 1);
		memcpy(simpleError.message, parserResult.errorStr, strlen(parserResult.errorStr));
		errArr->data[errArr->len] = simpleError;
		errArr->len++;
	}
	else
	{
		free(simpleError.message);
		free(errArr->data);
		free(errArr);
		errArr = vValidate(parserResult.docPtr, schema);
		xmlFreeDoc(parserResult.docPtr);
	}
	free(parserResult.errorStr);
	errno = errArr->len == NO_ERROR ? 0 : -1;
	return errArr;
}
