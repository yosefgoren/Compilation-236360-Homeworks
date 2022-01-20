/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.ypp"

	int yylex();
	void yyerror(const char* s);
	#include "stdio.h"
	#include "Symtab.hpp"
	#include "AuxTypes.hpp" 
	#include "hw3_output.hpp"
	#include <iostream>
	#include <algorithm>
	#include <set>
	extern int yylineno;

	//#define MYDB
	#ifdef MYDB
		#define YYERROR_VERBOSE 1
		#define YYDEBUG 1
		extern int yydebug;
#endif

	SimpleSymtab symtab;
	using namespace std;

	#define check(assertion, error) do{if(!(assertion)){error; exit(1);}}while(false);

	static int loop_depth;
	
	bool isNumeralType(ExpType type){
		return type == INT_EXP || type == BYTE_EXP;
	}
	bool canImplicitCast(ExpType source_type, ExpType dst_type){
		return (source_type == dst_type)
			|| (source_type == BYTE_EXP && dst_type == INT_EXP);
	}
	bool canExplicitCast(ExpType source_type, ExpType dst_type){
		if(source_type == dst_type)
			return true;
		return isNumeralType(source_type) && isNumeralType(dst_type);
	}

	void checkMismatch(ExpType source_type, ExpType dst_type){
		check(canImplicitCast(source_type, dst_type), output::errorMismatch(yylineno));
	}
	
	void checkBool(ExpType type) {
		checkMismatch(type, BOOL_EXP);
	}

	void checkNumeralType(ExpType type){
		check(isNumeralType(type), output::errorMismatch(yylineno));
	}

	ExpType maxNumeralType(ExpType first_operand_type, ExpType second_operand_type){
		return first_operand_type == INT_EXP ? INT_EXP : second_operand_type;
	}

	void checkFuncDec(const string& func_id, const vector<Parameter>& parameters){
		std::set<std::string> tmp_params_set;
		for(auto it = parameters.rbegin(); it != parameters.rend(); ++it){
			auto param = *it;
			//check that the new parameter identifiers don't conflict with the id of the new function:
			check(func_id != param.id, output::errorDef(param.line_of_origin, param.id));
			//check that the new parameter identifiers dont confilict with existing ones: 
			check(symtab.declarableValidId(param.id), output::errorDef(param.line_of_origin, param.id));
			//check that there are not conflitcts between the new identifiers:
			check(tmp_params_set.count(param.id) == 0, output::errorDef(param.line_of_origin, param.id));
			tmp_params_set.insert(param.id);
		}
	}

	void checkPrototypeMismatch(const string& func_id, const vector<ExpType>& reverse_exp_types_in_call){
		vector<ExpType> exp_types_required = symtab.getFunctionType(func_id).getParameterTypes();
		//TODO: add something to remember the line of each expression so we can add the correct 
		// 		line of the expression to this error (something like 'line_of_origin').
		auto required_types_it = exp_types_required.begin();
		for(auto called_type_it = reverse_exp_types_in_call.rbegin();
				called_type_it != reverse_exp_types_in_call.rend(); ++called_type_it){
			ExpType called_type = *called_type_it;
			if(required_types_it == exp_types_required.end() || !canImplicitCast(called_type, *required_types_it)){
				output::errorPrototypeMismatch(yylineno, func_id, ExpTypeStringVector(exp_types_required, true));
				exit(1);
			}
			++required_types_it;
		}
		if(required_types_it != exp_types_required.end()){
				output::errorPrototypeMismatch(yylineno, func_id, ExpTypeStringVector(exp_types_required, true));
				exit(1);			
		}
	}
	void checkMainMissing(){
		if (symtab.callableValidId("main")) {
			const FunctionType& func = symtab.getFunctionType("main");
			if (func.return_type != VOID_EXP || !func.getParameterTypes().empty()){
				output::errorMainMissing();
				exit(1);
			} 
		}else {
			output::errorMainMissing();
			exit(1);
		}
	}

	void checkByteTooLarge(int b) {
		if (b > 255 || b < 0) {
			output::errorByteTooLarge(yylineno, to_string(b));
			exit(1);
		}
	}

#line 180 "parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INT = 3,                        /* INT  */
  YYSYMBOL_VOID = 4,                       /* VOID  */
  YYSYMBOL_BYTE = 5,                       /* BYTE  */
  YYSYMBOL_B = 6,                          /* B  */
  YYSYMBOL_BOOL = 7,                       /* BOOL  */
  YYSYMBOL_CONST = 8,                      /* CONST  */
  YYSYMBOL_TRUE = 9,                       /* TRUE  */
  YYSYMBOL_FALSE = 10,                     /* FALSE  */
  YYSYMBOL_RETURN = 11,                    /* RETURN  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_BREAK = 13,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 14,                  /* CONTINUE  */
  YYSYMBOL_SC = 15,                        /* SC  */
  YYSYMBOL_STRING = 16,                    /* STRING  */
  YYSYMBOL_COMMA = 17,                     /* COMMA  */
  YYSYMBOL_ID = 18,                        /* ID  */
  YYSYMBOL_NUM = 19,                       /* NUM  */
  YYSYMBOL_ASSIGN = 20,                    /* ASSIGN  */
  YYSYMBOL_OR = 21,                        /* OR  */
  YYSYMBOL_AND = 22,                       /* AND  */
  YYSYMBOL_RELOP = 23,                     /* RELOP  */
  YYSYMBOL_BINOP = 24,                     /* BINOP  */
  YYSYMBOL_NOT = 25,                       /* NOT  */
  YYSYMBOL_LPAREN = 26,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 27,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 28,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 29,                    /* RBRACE  */
  YYSYMBOL_IF = 30,                        /* IF  */
  YYSYMBOL_ELSE = 31,                      /* ELSE  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_Program = 33,                   /* Program  */
  YYSYMBOL_OpenScope = 34,                 /* OpenScope  */
  YYSYMBOL_CloseScope = 35,                /* CloseScope  */
  YYSYMBOL_OpenLoop = 36,                  /* OpenLoop  */
  YYSYMBOL_CloseLoop = 37,                 /* CloseLoop  */
  YYSYMBOL_Block = 38,                     /* Block  */
  YYSYMBOL_Funcs = 39,                     /* Funcs  */
  YYSYMBOL_FuncDecl = 40,                  /* FuncDecl  */
  YYSYMBOL_41_1 = 41,                      /* $@1  */
  YYSYMBOL_42_2 = 42,                      /* $@2  */
  YYSYMBOL_RetType = 43,                   /* RetType  */
  YYSYMBOL_Formals = 44,                   /* Formals  */
  YYSYMBOL_FormalsList = 45,               /* FormalsList  */
  YYSYMBOL_LineCapture = 46,               /* LineCapture  */
  YYSYMBOL_Statements = 47,                /* Statements  */
  YYSYMBOL_Call = 48,                      /* Call  */
  YYSYMBOL_ExpList = 49,                   /* ExpList  */
  YYSYMBOL_Type = 50,                      /* Type  */
  YYSYMBOL_TypeAnnotation = 51,            /* TypeAnnotation  */
  YYSYMBOL_Exp = 52,                       /* Exp  */
  YYSYMBOL_NumericExp = 53,                /* NumericExp  */
  YYSYMBOL_BoolExp = 54,                   /* BoolExp  */
  YYSYMBOL_Statement = 55,                 /* Statement  */
  YYSYMBOL_OpenStatment = 56,              /* OpenStatment  */
  YYSYMBOL_ClosedStatment = 57,            /* ClosedStatment  */
  YYSYMBOL_IfStart = 58,                   /* IfStart  */
  YYSYMBOL_59_3 = 59,                      /* $@3  */
  YYSYMBOL_WhileStart = 60,                /* WhileStart  */
  YYSYMBOL_61_4 = 61,                      /* $@4  */
  YYSYMBOL_SimpleStatement = 62,           /* SimpleStatement  */
  YYSYMBOL_63_5 = 63,                      /* $@5  */
  YYSYMBOL_64_6 = 64,                      /* $@6  */
  YYSYMBOL_VarDecStart = 65                /* VarDecStart  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   165

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  127

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   155,   155,   157,   158,   159,   160,   161,   163,   164,
     166,   167,   166,   172,   173,   175,   176,   178,   179,   181,
     183,   184,   186,   192,   201,   202,   204,   205,   206,   208,
     209,   211,   212,   213,   218,   219,   223,   224,   227,   228,
     229,   235,   236,   237,   238,   239,   240,   243,   244,   246,
     247,   248,   251,   252,   253,   255,   255,   256,   256,   258,
     259,   264,   269,   275,   276,   276,   277,   277,   281,   282,
     284
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INT", "VOID", "BYTE",
  "B", "BOOL", "CONST", "TRUE", "FALSE", "RETURN", "WHILE", "BREAK",
  "CONTINUE", "SC", "STRING", "COMMA", "ID", "NUM", "ASSIGN", "OR", "AND",
  "RELOP", "BINOP", "NOT", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "IF",
  "ELSE", "$accept", "Program", "OpenScope", "CloseScope", "OpenLoop",
  "CloseLoop", "Block", "Funcs", "FuncDecl", "$@1", "$@2", "RetType",
  "Formals", "FormalsList", "LineCapture", "Statements", "Call", "ExpList",
  "Type", "TypeAnnotation", "Exp", "NumericExp", "BoolExp", "Statement",
  "OpenStatment", "ClosedStatment", "IfStart", "$@3", "WhileStart", "$@4",
  "SimpleStatement", "$@5", "$@6", "VarDecStart", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
#endif

#define YYPACT_NINF (-79)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-20)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     121,   -79,   -79,   -79,   -79,    11,   -79,   121,    -3,   -79,
     -79,   -79,   -79,     3,    -6,   -79,   -79,   -79,    40,    19,
      35,    30,    45,   105,    49,    89,    44,    56,    60,   -14,
     -79,    47,   -79,    74,    63,    40,   -79,   -79,   -79,   -79,
     -79,   -79,    -7,    72,   -79,   -79,   -79,    55,    83,    89,
      58,   -79,   138,   -79,   -79,    76,    89,   -79,   -79,    89,
     111,   105,    89,   -79,   -79,   -79,    88,   105,   -79,   -79,
      89,   -79,   -79,   -79,    95,   131,    89,    89,    89,    89,
      86,   -79,   138,     1,   -79,   104,   127,    82,   138,   -79,
     -79,    78,   105,   119,    89,   -79,    -4,    28,   108,   -79,
     -79,   112,   -79,   -79,    89,   -79,   118,   -79,   115,   -79,
     -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,
     105,   -79,   -79,   -79,   -79,   -79,   -79
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       9,    26,    14,    27,    28,     0,     2,     9,     0,    13,
       1,     8,    10,     0,    30,    29,    11,    16,     0,     0,
       0,     0,    17,    30,     0,    64,     0,     0,     0,     0,
       3,     0,    59,    30,     0,     0,    20,    47,    48,     3,
       5,    52,     0,    30,    45,    46,    34,    33,    39,     0,
       0,    32,    66,    36,    37,     0,     0,    68,    69,     0,
       0,    30,     0,    12,    21,    63,     0,    30,     3,    60,
       0,    18,    40,    44,     0,     0,     0,     0,     0,     0,
       0,    65,    57,     0,    23,     0,    24,    30,    55,    70,
       4,    48,    30,     0,     0,    31,    42,    41,    43,    38,
      67,     0,    62,    22,     0,     4,     0,    49,     0,     4,
       4,    61,    35,    58,    25,     7,    56,     3,     6,     6,
      30,    51,    54,     4,     4,    50,    53
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -79,   -79,   -38,   -74,   -79,    37,   -79,   140,   -79,   -79,
     -79,   -79,   -79,   114,   -79,   102,   -23,    61,    -9,   -10,
     -22,   -79,   -79,   -28,   -78,   -60,   -79,   -79,   -79,   -79,
     -79,   -79,   -79,   -79
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     5,    61,   107,    68,   121,    32,     6,     7,    13,
      19,     8,    16,    17,    24,    33,    51,    85,     9,    35,
      86,    53,    54,    36,    37,    38,    39,   106,    40,   101,
      41,    55,    80,    42
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      34,    67,    15,    52,    18,    64,    59,    91,    69,    20,
      34,    10,    60,    70,   109,    12,   102,   108,    77,    78,
      79,   -15,    76,    77,    78,    79,    66,    73,    75,    14,
      92,   115,   110,    18,    82,   118,   119,    83,    34,    90,
      88,    74,   123,     1,    34,     3,    21,     4,    93,   125,
     126,    78,    79,    22,    96,    97,    98,    99,    23,    64,
     124,     1,   -19,     3,    34,     4,    43,    44,    45,    34,
      56,    57,   112,    62,    46,    58,    47,    48,    65,   120,
      15,    60,    15,    49,    50,    25,    26,    27,    28,    72,
      15,    81,    29,    25,    26,    27,    28,    34,    44,    45,
      29,   100,    30,    63,    31,    46,    89,    47,    48,    -4,
      30,   105,    31,    15,    49,    50,    25,    26,    27,    28,
      44,    45,    94,    29,     1,     2,     3,    46,     4,    47,
      48,   103,    79,    30,   111,    31,    49,    50,    84,   113,
      76,    77,    78,    79,   104,   116,   117,    11,    76,    77,
      78,    79,    76,    77,    78,    79,   122,    71,    95,    76,
      77,    78,    79,    87,     0,   114
};

static const yytype_int8 yycheck[] =
{
      23,    39,     8,    25,    14,    33,    20,    67,    15,    18,
      33,     0,    26,    20,    92,    18,    15,    91,    22,    23,
      24,    27,    21,    22,    23,    24,    35,    49,    50,    26,
      68,   105,    92,    43,    56,   109,   110,    59,    61,    67,
      62,    50,   120,     3,    67,     5,    27,     7,    70,   123,
     124,    23,    24,    18,    76,    77,    78,    79,    28,    87,
     120,     3,    17,     5,    87,     7,    17,     9,    10,    92,
      26,    15,    94,    26,    16,    15,    18,    19,    15,   117,
       8,    26,     8,    25,    26,    11,    12,    13,    14,     6,
       8,    15,    18,    11,    12,    13,    14,   120,     9,    10,
      18,    15,    28,    29,    30,    16,    18,    18,    19,    31,
      28,    29,    30,     8,    25,    26,    11,    12,    13,    14,
       9,    10,    27,    18,     3,     4,     5,    16,     7,    18,
      19,    27,    24,    28,    15,    30,    25,    26,    27,    27,
      21,    22,    23,    24,    17,    27,    31,     7,    21,    22,
      23,    24,    21,    22,    23,    24,   119,    43,    27,    21,
      22,    23,    24,    61,    -1,   104
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     7,    33,    39,    40,    43,    50,
       0,    39,    18,    41,    26,     8,    44,    45,    51,    42,
      50,    27,    18,    28,    46,    11,    12,    13,    14,    18,
      28,    30,    38,    47,    48,    51,    55,    56,    57,    58,
      60,    62,    65,    17,     9,    10,    16,    18,    19,    25,
      26,    48,    52,    53,    54,    63,    26,    15,    15,    20,
      26,    34,    26,    29,    55,    15,    50,    34,    36,    15,
      20,    45,     6,    52,    50,    52,    21,    22,    23,    24,
      64,    15,    52,    52,    27,    49,    52,    47,    52,    18,
      55,    57,    34,    52,    27,    27,    52,    52,    52,    52,
      15,    61,    15,    27,    17,    29,    59,    35,    35,    56,
      57,    15,    52,    27,    49,    35,    27,    31,    35,    35,
      34,    37,    37,    56,    57,    35,    35
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    35,    36,    37,    38,    39,    39,
      41,    42,    40,    43,    43,    44,    44,    45,    45,    46,
      47,    47,    48,    48,    49,    49,    50,    50,    50,    51,
      51,    52,    52,    52,    52,    52,    52,    52,    53,    53,
      53,    54,    54,    54,    54,    54,    54,    55,    55,    56,
      56,    56,    57,    57,    57,    59,    58,    61,    60,    62,
      62,    62,    62,    62,    63,    62,    64,    62,    62,    62,
      65
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     0,     0,     0,     5,     2,     0,
       0,     0,    10,     1,     1,     0,     1,     3,     6,     0,
       1,     2,     4,     3,     1,     3,     1,     1,     1,     1,
       0,     3,     1,     1,     1,     4,     1,     1,     3,     1,
       2,     3,     3,     3,     2,     1,     1,     1,     1,     4,
       8,     6,     1,     8,     6,     0,     5,     0,     5,     1,
       2,     4,     4,     2,     0,     3,     0,     4,     2,     2,
       3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* OpenScope: %empty  */
#line 157 "parser.ypp"
                                {symtab.pushScope();}
#line 1334 "parser.tab.cpp"
    break;

  case 4: /* CloseScope: %empty  */
#line 158 "parser.ypp"
                                {symtab.popScope();}
#line 1340 "parser.tab.cpp"
    break;

  case 5: /* OpenLoop: %empty  */
#line 159 "parser.ypp"
                                {++loop_depth;}
#line 1346 "parser.tab.cpp"
    break;

  case 6: /* CloseLoop: %empty  */
#line 160 "parser.ypp"
                                {--loop_depth;}
#line 1352 "parser.tab.cpp"
    break;

  case 10: /* $@1: %empty  */
#line 166 "parser.ypp"
                                           {check(symtab.declarableValidId(*(yyvsp[0].id)), output::errorDef(yylineno, *(yyvsp[0].id)));}
#line 1358 "parser.tab.cpp"
    break;

  case 11: /* $@2: %empty  */
#line 167 "parser.ypp"
                                                       {	
						checkFuncDec(*(yyvsp[-3].id), *(yyvsp[0].formals_list));
						symtab.declareFunc(*(yyvsp[-3].id), (yyvsp[-4].exp_type), (yyvsp[0].formals_list));
					}
#line 1367 "parser.tab.cpp"
    break;

  case 12: /* FuncDecl: RetType ID $@1 LPAREN Formals $@2 RPAREN LBRACE Statements RBRACE  */
#line 170 "parser.ypp"
                                                                          {symtab.finishFunc();}
#line 1373 "parser.tab.cpp"
    break;

  case 13: /* RetType: Type  */
#line 172 "parser.ypp"
                                     {(yyval.exp_type) = (yyvsp[0].exp_type);}
#line 1379 "parser.tab.cpp"
    break;

  case 14: /* RetType: VOID  */
#line 173 "parser.ypp"
                                              {(yyval.exp_type) = VOID_EXP;}
#line 1385 "parser.tab.cpp"
    break;

  case 15: /* Formals: %empty  */
#line 175 "parser.ypp"
                                {(yyval.formals_list) = new vector<Parameter>();}
#line 1391 "parser.tab.cpp"
    break;

  case 16: /* Formals: FormalsList  */
#line 176 "parser.ypp"
                                                     {(yyval.formals_list) = (yyvsp[0].formals_list);}
#line 1397 "parser.tab.cpp"
    break;

  case 17: /* FormalsList: TypeAnnotation Type ID  */
#line 178 "parser.ypp"
                                               {(yyval.formals_list) = new vector<Parameter>(); (yyval.formals_list)->push_back(Parameter(*(yyvsp[0].id), (yyvsp[-1].exp_type), yylineno, (yyvsp[-2].is_const))); delete (yyvsp[0].id);}
#line 1403 "parser.tab.cpp"
    break;

  case 18: /* FormalsList: TypeAnnotation Type ID LineCapture COMMA FormalsList  */
#line 179 "parser.ypp"
                                                                                              {(yyval.formals_list) = (yyvsp[0].formals_list); (yyval.formals_list)->push_back(Parameter(*(yyvsp[-3].id), (yyvsp[-4].exp_type), (yyvsp[-2].line_number), (yyvsp[-5].is_const))); delete (yyvsp[-3].id);}
#line 1409 "parser.tab.cpp"
    break;

  case 19: /* LineCapture: %empty  */
#line 181 "parser.ypp"
                        {(yyval.line_number) = yylineno;}
#line 1415 "parser.tab.cpp"
    break;

  case 22: /* Call: ID LPAREN ExpList RPAREN  */
#line 186 "parser.ypp"
                                                         {
						check(symtab.callableValidId(*(yyvsp[-3].id)), output::errorUndefFunc(yylineno, *(yyvsp[-3].id)));
						checkPrototypeMismatch(*(yyvsp[-3].id), *(yyvsp[-1].exp_type_list));
						(yyval.exp_type) = symtab.getReturnType(*(yyvsp[-3].id));
						delete (yyvsp[-3].id);
					}
#line 1426 "parser.tab.cpp"
    break;

  case 23: /* Call: ID LPAREN RPAREN  */
#line 192 "parser.ypp"
                                                          {
						check(symtab.callableValidId(*(yyvsp[-2].id)), output::errorUndefFunc(yylineno, *(yyvsp[-2].id)));
						//there are no arguments used in the call so vector is empty:
						vector<ExpType> call_arg_types = {};
						checkPrototypeMismatch(*(yyvsp[-2].id), call_arg_types);
						(yyval.exp_type) = symtab.getReturnType(*(yyvsp[-2].id));
						delete (yyvsp[-2].id);	
					}
#line 1439 "parser.tab.cpp"
    break;

  case 24: /* ExpList: Exp  */
#line 201 "parser.ypp"
                                    {(yyval.exp_type_list) = new vector<ExpType>(); (yyval.exp_type_list)->push_back((yyvsp[0].exp_type));}
#line 1445 "parser.tab.cpp"
    break;

  case 25: /* ExpList: Exp COMMA ExpList  */
#line 202 "parser.ypp"
                                                           {(yyval.exp_type_list) = (yyvsp[0].exp_type_list); (yyval.exp_type_list)->push_back((yyvsp[-2].exp_type));}
#line 1451 "parser.tab.cpp"
    break;

  case 26: /* Type: INT  */
#line 204 "parser.ypp"
                                    {(yyval.exp_type) = INT_EXP;}
#line 1457 "parser.tab.cpp"
    break;

  case 27: /* Type: BYTE  */
#line 205 "parser.ypp"
                                              {(yyval.exp_type) = BYTE_EXP;}
#line 1463 "parser.tab.cpp"
    break;

  case 28: /* Type: BOOL  */
#line 206 "parser.ypp"
                                              {(yyval.exp_type) = BOOL_EXP;}
#line 1469 "parser.tab.cpp"
    break;

  case 29: /* TypeAnnotation: CONST  */
#line 208 "parser.ypp"
                              {(yyval.is_const) = true;}
#line 1475 "parser.tab.cpp"
    break;

  case 30: /* TypeAnnotation: %empty  */
#line 209 "parser.ypp"
                                          {(yyval.is_const) = false;}
#line 1481 "parser.tab.cpp"
    break;

  case 31: /* Exp: LPAREN Exp RPAREN  */
#line 211 "parser.ypp"
                                                  {(yyval.exp_type) = (yyvsp[-1].exp_type);}
#line 1487 "parser.tab.cpp"
    break;

  case 32: /* Exp: Call  */
#line 212 "parser.ypp"
                                               {(yyval.exp_type) = (yyvsp[0].exp_type);}
#line 1493 "parser.tab.cpp"
    break;

  case 33: /* Exp: ID  */
#line 213 "parser.ypp"
                                             {
						check(symtab.rvalValidId(*(yyvsp[0].id)), output::errorUndef(yylineno, *(yyvsp[0].id)));
						(yyval.exp_type) = symtab.getVariableType(*(yyvsp[0].id));
						delete (yyvsp[0].id);
					}
#line 1503 "parser.tab.cpp"
    break;

  case 34: /* Exp: STRING  */
#line 218 "parser.ypp"
                                                 {(yyval.exp_type) = STRING_EXP;}
#line 1509 "parser.tab.cpp"
    break;

  case 35: /* Exp: LPAREN Type RPAREN Exp  */
#line 219 "parser.ypp"
                                                                 {
						check(canExplicitCast((yyvsp[0].exp_type), (yyvsp[-2].exp_type)), output::errorMismatch(yylineno));
						(yyval.exp_type) = (yyvsp[-2].exp_type);
					}
#line 1518 "parser.tab.cpp"
    break;

  case 38: /* NumericExp: Exp BINOP Exp  */
#line 227 "parser.ypp"
                                              {checkNumeralType((yyvsp[-2].exp_type)); checkNumeralType((yyvsp[0].exp_type)); (yyval.exp_type) = maxNumeralType((yyvsp[-2].exp_type), (yyvsp[0].exp_type));}
#line 1524 "parser.tab.cpp"
    break;

  case 39: /* NumericExp: NUM  */
#line 228 "parser.ypp"
                                              {(yyval.exp_type) = INT_EXP;}
#line 1530 "parser.tab.cpp"
    break;

  case 40: /* NumericExp: NUM B  */
#line 229 "parser.ypp"
                                                {
						(yyval.exp_type) = BYTE_EXP;
						checkByteTooLarge((yyvsp[-1].number_literal));
					}
#line 1539 "parser.tab.cpp"
    break;

  case 41: /* BoolExp: Exp AND Exp  */
#line 235 "parser.ypp"
                                            {checkMismatch((yyvsp[-2].exp_type), BOOL_EXP); checkMismatch((yyvsp[0].exp_type), BOOL_EXP); (yyval.exp_type) = BOOL_EXP;}
#line 1545 "parser.tab.cpp"
    break;

  case 42: /* BoolExp: Exp OR Exp  */
#line 236 "parser.ypp"
                                                     {checkMismatch((yyvsp[-2].exp_type), BOOL_EXP); checkMismatch((yyvsp[0].exp_type), BOOL_EXP); (yyval.exp_type) = BOOL_EXP;}
#line 1551 "parser.tab.cpp"
    break;

  case 43: /* BoolExp: Exp RELOP Exp  */
#line 237 "parser.ypp"
                                                        {check(isNumeralType((yyvsp[-2].exp_type)) && isNumeralType((yyvsp[0].exp_type)), output::errorMismatch(yylineno)); (yyval.exp_type) = BOOL_EXP;}
#line 1557 "parser.tab.cpp"
    break;

  case 44: /* BoolExp: NOT Exp  */
#line 238 "parser.ypp"
                                                  {checkMismatch((yyvsp[0].exp_type), BOOL_EXP); (yyval.exp_type) = BOOL_EXP;}
#line 1563 "parser.tab.cpp"
    break;

  case 45: /* BoolExp: TRUE  */
#line 239 "parser.ypp"
                                               {(yyval.exp_type) = BOOL_EXP;}
#line 1569 "parser.tab.cpp"
    break;

  case 46: /* BoolExp: FALSE  */
#line 240 "parser.ypp"
                                                {(yyval.exp_type) = BOOL_EXP;}
#line 1575 "parser.tab.cpp"
    break;

  case 55: /* $@3: %empty  */
#line 255 "parser.ypp"
                                              {checkBool((yyvsp[0].exp_type));}
#line 1581 "parser.tab.cpp"
    break;

  case 57: /* $@4: %empty  */
#line 256 "parser.ypp"
                                                 {checkBool((yyvsp[0].exp_type));}
#line 1587 "parser.tab.cpp"
    break;

  case 60: /* SimpleStatement: VarDecStart SC  */
#line 259 "parser.ypp"
                                                        {
						check(!(yyvsp[-1].dec_info).is_const, output::errorConstDef(yylineno));
						symtab.declareVar(*(yyvsp[-1].dec_info).id, (yyvsp[-1].dec_info).raw_type, (yyvsp[-1].dec_info).is_const);
						delete (yyvsp[-1].dec_info).id;
					}
#line 1597 "parser.tab.cpp"
    break;

  case 61: /* SimpleStatement: VarDecStart ASSIGN Exp SC  */
#line 264 "parser.ypp"
                                                                   {
						checkMismatch((yyvsp[-1].exp_type), (yyvsp[-3].dec_info).raw_type);
						symtab.declareVar(*(yyvsp[-3].dec_info).id, (yyvsp[-3].dec_info).raw_type, (yyvsp[-3].dec_info).is_const);
						delete (yyvsp[-3].dec_info).id;
					}
#line 1607 "parser.tab.cpp"
    break;

  case 62: /* SimpleStatement: ID ASSIGN Exp SC  */
#line 269 "parser.ypp"
                                                          {
						check(symtab.containsVar(*(yyvsp[-3].id)), output::errorUndef(yylineno, *(yyvsp[-3].id)));
						check(!symtab.isConst(*(yyvsp[-3].id)), output::errorConstMismatch(yylineno));
						checkMismatch((yyvsp[-1].exp_type), symtab.getVariableType(*(yyvsp[-3].id)));
						delete (yyvsp[-3].id);
					}
#line 1618 "parser.tab.cpp"
    break;

  case 64: /* $@5: %empty  */
#line 276 "parser.ypp"
                                                {checkMismatch(VOID_EXP, symtab.getCurrentlyParsedFuncType().return_type);}
#line 1624 "parser.tab.cpp"
    break;

  case 66: /* $@6: %empty  */
#line 277 "parser.ypp"
                                                    {
						checkMismatch((yyvsp[0].exp_type), symtab.getCurrentlyParsedFuncType().return_type);
						check((yyvsp[0].exp_type) != VOID_EXP, output::errorMismatch(yylineno));
					}
#line 1633 "parser.tab.cpp"
    break;

  case 68: /* SimpleStatement: BREAK SC  */
#line 281 "parser.ypp"
                                                  {check(loop_depth!=0, output::errorUnexpectedBreak(yylineno));}
#line 1639 "parser.tab.cpp"
    break;

  case 69: /* SimpleStatement: CONTINUE SC  */
#line 282 "parser.ypp"
                                                     {check(loop_depth!=0, output::errorUnexpectedContinue(yylineno));}
#line 1645 "parser.tab.cpp"
    break;

  case 70: /* VarDecStart: TypeAnnotation Type ID  */
#line 284 "parser.ypp"
                                               {
						check(symtab.declarableValidId(*(yyvsp[0].id)), output::errorDef(yylineno, *(yyvsp[0].id)));
						(yyval.dec_info) = {.is_const = (yyvsp[-2].is_const), .raw_type = (yyvsp[-1].exp_type), .id = (yyvsp[0].id)};
					}
#line 1654 "parser.tab.cpp"
    break;


#line 1658 "parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 289 "parser.ypp"

void yyerror(const char* s){
	output::errorSyn(yylineno);
	exit(1);
}

void declareLibraryFuncs(){
	//FunctionType creates a shared pinter for these allocations:
	std::vector<Parameter>* print_params = new std::vector<Parameter>();
	std::vector<Parameter>* printi_params = new std::vector<Parameter>();

	//in print/i line of origin and is_const parameters are irrelevant, but have to be given some value:
	print_params->push_back(Parameter("str", STRING_EXP, 0, true));
	printi_params->push_back(Parameter("i", INT_EXP, 0, true));
	symtab.declareFunc("print", VOID_EXP, print_params);
	symtab.finishFunc(false);
	symtab.declareFunc("printi", VOID_EXP, printi_params);
	symtab.finishFunc(false);
	
}

int main(){
	#ifdef MYDB
		yydebug = 1;
	#endif
	symtab = SimpleSymtab();
	declareLibraryFuncs();
	loop_depth = 0;
	int res = yyparse();
	checkMainMissing();
	output::endScope();//this is the global scope.
	symtab.printFuncDecls();
	return res;
}
