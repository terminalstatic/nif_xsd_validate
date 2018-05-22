#ifndef NIF_VLIBXML_H
#define NIF_VLIBXML_H

#include <libxml/xmlschemastypes.h>

#define P_ERR_INIT 512
#define P_ERR_DEFAULT 1
#define P_ERR_VERBOSE 2

struct vXsdParserResult {
	xmlSchemaPtr schemaPtr;
	char *errorStr;
};
struct vXmlParserResult {
	xmlDocPtr docPtr;
	char *errorStr;
};
struct vErrCtx {
	char *errBuf;
};
typedef enum {
	NO_ERROR 		= 0,
	LIBXML2_ERROR		= 1,
	XSD_PARSER_ERROR	= 2,
	XML_PARSER_ERROR 	= 3,
	VALIDATION_ERROR 	= 4
} vErrorType;

struct vSimpleXmlError {
	vErrorType 	type;
	int			code;
	char*		message;
	int 		level;
	int			line;
	char*		node;
};
typedef struct _vErrArray {
	struct 	vSimpleXmlError *data;
	size_t 	len;
	size_t 	cap;
} vErrArray;

void vLibxmlInit();
void vLibxmlCleanup();

void vFreeErrArray(vErrArray *errArr);

struct vXsdParserResult vParseUrlSchema(const char *url, const short int options);
vErrArray *vValidateBuf(const void *xmlSource, const int xmlSourceLen, const short int xmlParserOptions, const xmlSchemaPtr schema);

#endif