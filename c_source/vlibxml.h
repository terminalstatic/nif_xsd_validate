#ifndef NIF_VLIBXML_H
#define NIF_VLIBXML_H

#include <libxml/xmlschemastypes.h>

#define GO_ERR_INIT 512
#define P_ERR_DEFAULT 1
#define P_ERR_VERBOSE 2

struct xsdParserResult {
	xmlSchemaPtr schemaPtr;
	char *errorStr;
};
struct xmlParserResult {
	xmlDocPtr docPtr;
	char *errorStr;
};
struct errCtx {
	char *errBuf;
};
typedef enum {
	NO_ERROR 		= 0,
	LIBXML2_ERROR		= 1,
	XSD_PARSER_ERROR	= 2,
	XML_PARSER_ERROR 	= 3,
	VALIDATION_ERROR 	= 4
} errorType;
struct simpleXmlError {
	errorType 	type;
	int		code;
	char*		message;
	int 		level;
	int		line;
	char*		node;
};
typedef struct _errArray {
	struct 	simpleXmlError *data;
	size_t 	len;
	size_t 	cap;
} errArray;

void init();
void cleanup();

void freeErrArray(errArray *errArr);

struct xsdParserResult cParseUrlSchema(const char *url, const short int options);
errArray *cValidateBuf(const void *goXmlSource, const int goXmlSourceLen, const short int xmlParserOptions, const xmlSchemaPtr schema);

#endif