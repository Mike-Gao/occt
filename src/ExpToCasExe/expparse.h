
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
  int num;
  char *str;
  TColStd_HSequenceOfHAsciiString *tlist;
  Express_HSequenceOfField *flist;
  Express_HSequenceOfItem *ilist;
  Express_Field *field;
  Express_Item *item;
  Express_Type *type;
  Express_Schema *schema;
  Express_Reference *ref;
} YYSTYPE;
extern YYSTYPE yylval;
# define KSCHEM 257
# define KENDS 258
# define KTYP 259
# define KENDT 260
# define KENT 261
# define KENDE 262
# define KREF 263
# define KFROM 264
# define KSEL 265
# define KENUM 266
# define KLIST 267
# define KARR 268
# define KBAG 269
# define KSET 270
# define KOF 271
# define KNUM 272
# define KINT 273
# define KDBL 274
# define KSTR 275
# define KLOG 276
# define KBOOL 277
# define KOPT 278
# define KUNIQ 279
# define KSELF 280
# define KABSTR 281
# define KSUBT 282
# define KSPRT 283
# define KANDOR 284
# define K1OF 285
# define KAND 286
# define NUMBER 287
# define NAME 288
