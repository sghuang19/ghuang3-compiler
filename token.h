#ifndef TOKEN_H
#define TOKEN_H

/* Keywords */
#define T_ARRAY				256
#define T_AUTO				257
#define T_BOOLEAN			258
#define T_CHAR				259
#define T_ELSE				260
#define T_FALSE				261
#define T_FLOAT				262
#define T_FOR				263
#define T_FUNCTION			264
#define T_IF				265
#define T_INTEGER			266
#define T_PRINT				267
#define T_RETURN    		268
#define T_STRING			269
#define T_TRUE				270
#define T_VOID				271
#define T_WHILE				272

/* Operators */
#define T_INCREMENT			273
#define T_DECREMENT			274
#define T_EXPONENT			275
#define T_MULTIPLY			276
#define T_DIVIDE			277
#define T_MODULO    		278
#define T_ADD				279
#define T_MINUS				280
#define T_LESS				281
#define T_LESS_OR_EQUAL		282
#define T_GREATER			283
#define T_GREATER_OR_EQUAL	284
#define T_EQUAL				285
#define T_NOT_EQUAL			286
#define T_LOGICAL_AND		287
#define T_LOGICAL_OR    	288
#define T_LOGICAL_NOT		289
#define T_ASSIGN			290

/* Symbols */
#define T_LPAREN			291
#define T_RPAREN			292
#define T_LBRACKET			293
#define T_RBRACKET			294
#define T_LBRACE			295
#define T_RBRACE			296
#define T_SEMICOLON			297
#define T_COLON				298
#define T_COMMA				299

/* Identifiers and Literals */
#define T_IDENTIFIER		300
#define T_INTEGER_LITERAL	301
#define T_FLOAT_LITERAL		302
#define T_CHAR_LITERAL		303
#define T_STRING_LITERAL	304

/* Error */
#define T_EOF               305
#define T_ERROR				306

#endif // TOKEN_H