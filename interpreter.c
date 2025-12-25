/*
 * EAP Pseudocode Interpreter - Complete C Implementation
 * Single file, minimal dependencies
 *
 * Compile with:
 *   gcc -o eap_interpreter interpreter.c -lm
 *
 * Usage:
 *   ./eap_interpreter program.eap
 *   ./eap_interpreter program.eap --debug
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <stdarg.h>

#define MAX_TOKEN_LEN 256
#define MAX_TOKENS 10000
#define MAX_IDENTIFIERS 1000
#define MAX_ARRAY_DIMS 10
#define MAX_STACK_DEPTH 1000
#define MAX_STRING_LEN 1024
#define MAX_HASH_SIZE 10007

// Token Types
typedef enum
{
    TOK_ALGORITHM,
    TOK_CONSTANTS,
    TOK_DATA,
    TOK_BEGIN,
    TOK_END,
    TOK_PROCEDURE,
    TOK_FUNCTION,
    TOK_INTERFACE,
    TOK_INPUT_PARAM,
    TOK_OUTPUT_PARAM,
    TOK_END_FUNCTION,
    TOK_END_PROCEDURE,
    TOK_IF,
    TOK_THEN,
    TOK_ELSE,
    TOK_END_IF,
    TOK_FOR,
    TOK_TO,
    TOK_STEP,
    TOK_REPEAT,
    TOK_END_FOR,
    TOK_WHILE,
    TOK_END_WHILE,
    TOK_UNTIL,
    TOK_PRINT,
    TOK_READ,
    TOK_CALCULATE,
    TOK_INTEGER_TYPE,
    TOK_REAL_TYPE,
    TOK_BOOLEAN_TYPE,
    TOK_CHAR_TYPE,
    TOK_STRING_TYPE,
    TOK_ARRAY,
    TOK_OF,
    TOK_ASSIGN,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MULTIPLY,
    TOK_DIVIDE,
    TOK_MOD,
    TOK_DIV,
    TOK_EQUALS,
    TOK_NOT_EQUALS,
    TOK_LESS_THAN,
    TOK_GREATER_THAN,
    TOK_LESS_EQUALS,
    TOK_GREATER_EQUALS,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_LEFT_PAREN,
    TOK_RIGHT_PAREN,
    TOK_LEFT_BRACKET,
    TOK_RIGHT_BRACKET,
    TOK_COMMA,
    TOK_COLON,
    TOK_SEMICOLON,
    TOK_DOT,
    TOK_PERCENT,
    TOK_NUMBER,
    TOK_STRING,
    TOK_BOOLEAN_LITERAL,
    TOK_IDENTIFIER,
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef struct
{
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;
    int column;
} Token;

// AST Node Types
typedef enum
{
    AST_PROGRAM,
    AST_CONST_DECL,
    AST_VAR_DECL,
    AST_ARRAY_TYPE,
    AST_FUNC_DECL,
    AST_PROC_DECL,
    AST_PARAMETER,
    AST_ASSIGN,
    AST_PRINT,
    AST_READ,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_CALL,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_ARRAY_ACCESS
} ASTNodeType;

typedef struct ArrayValue ArrayValue; // Προσθέστε αυτό ψηλά στις δηλώσεις
typedef struct ASTNode ASTNode;
typedef struct Environment Environment;

typedef struct
{
    ASTNode *start_expr;
    ASTNode *end_expr;
} ArrayBoundExpr;

typedef struct
{
    int from;
    int to;
} ArrayBound;

typedef struct HashNode
{
    char *key;
    void *value;
    struct HashNode *next;
} HashNode;

typedef struct
{
    HashNode *buckets[MAX_HASH_SIZE];
} HashMap;

typedef struct
{
    HashMap *data;
    ArrayBound bounds[MAX_ARRAY_DIMS];
    int num_dims;
} ArrayObject;

typedef union
{
    int int_val;
    double real_val;
    bool bool_val;
    char *str_val;
    ArrayObject *arr_val;
} Value;

typedef enum
{
    VAL_INT,
    VAL_REAL,
    VAL_BOOL,
    VAL_STRING,
    VAL_ARRAY,
    VAL_NONE
} ValueType;

typedef struct
{
    Value value;
    ValueType type;
} RuntimeValue;
// Array bounds tracking
typedef struct
{
    char *array_name;
    int bounds_start[MAX_ARRAY_DIMS];
    int bounds_end[MAX_ARRAY_DIMS];
    int num_dims;
} ArrayBoundsInfo;

// Variable type tracking
typedef struct
{
    char *var_name;
    char *type_name; // "int", "double", "bool", "char*"
} VarTypeInfo;

// Enhanced Code Generator
typedef struct
{
    FILE *output;
    int indent_level;
    bool in_function;
    char *current_function_name;
    Environment *env;
    ASTNode *program; // Store program AST for lookups

    // Array bounds tracking
    ArrayBoundsInfo array_bounds[100];
    int num_arrays;

    // Variable type tracking
    VarTypeInfo var_types[1000];
    int num_vars;
} CodeGenerator;

struct ASTNode
{
    ASTNodeType type;
    int line;

    union
    {
        struct
        {
            char *name;
            ASTNode **declarations;
            int num_decls;
            ASTNode **body;
            int num_stmts;
        } program;

        struct
        {
            char *name;
            ASTNode *value;
            char *var_type;
            ArrayBoundExpr *arr_bound_exprs;
            int num_arr_dims;
        } decl;

        struct
        {
            char *name;
            char *return_type;
            ASTNode **parameters;
            int num_params;
            ASTNode **local_decls;
            int num_local_decls;
            ASTNode **body;
            int num_stmts;
        } subroutine;

        struct
        {
            char *name;
            char *param_type;
            bool is_reference;
        } param;

        struct
        {
            char *identifier;
            ASTNode **indices;
            int num_indices;
            ASTNode *value;
        } assign;

        struct
        {
            ASTNode **expressions;
            int num_exprs;
        } print;

        struct
        {
            ASTNode **variables;
            int num_vars;
        } read;

        struct
        {
            ASTNode *condition;
            ASTNode **then_branch;
            int num_then;
            ASTNode **else_branch;
            int num_else;
        } if_stmt;

        struct
        {
            char *variable;
            ASTNode *start;
            ASTNode *end;
            ASTNode *step;
            ASTNode **body;
            int num_stmts;
        } for_loop;

        struct
        {
            struct ASTNode *condition;
            struct ASTNode **body;
            int num_stmts;
            bool is_repeat_until;
        } while_loop;

        struct
        {
            char *name;
            ASTNode **arguments;
            int num_args;
            bool is_statement;
        } call;

        struct
        {
            char *operator;
            ASTNode *left;
            ASTNode *right;
        } binary;

        struct
        {
            char *operator;
            ASTNode *operand;
        } unary;

        struct
        {
            RuntimeValue value;
        } literal;

        struct
        {
            char *name;
        } identifier;

        struct
        {
            char *name;
            ASTNode **indices;
            int num_indices;
        } array_access;
    };
};

typedef struct EnvEntry
{
    char *name;
    RuntimeValue value;
    ASTNode *subroutine;
    struct EnvEntry *next;
} EnvEntry;

struct Environment
{
    EnvEntry *entries[MAX_HASH_SIZE];
    Environment *parent;
};

// Global state
static const char *token_type_name(TokenType type);
static Token tokens[MAX_TOKENS];
static int token_count = 0;
static int token_pos = 0;
static bool debug_mode = false;

// Forward declarations
static void execute_statement(ASTNode *stmt, Environment *env);
static RuntimeValue evaluate(ASTNode *expr, Environment *env);
static void free_runtime_value(RuntimeValue *val);
static RuntimeValue copy_runtime_value(RuntimeValue *val);

// Codegen declarations
// Codegen declarations
void codegen_init(CodeGenerator *gen, FILE *out);
void codegen_program(CodeGenerator *gen, ASTNode *prog);
char *sanitize_identifier(const char *name); // Ensure this exists
const char *map_type(const char *eap_type);  // Ensure this exists
void codegen_declaration(CodeGenerator *gen, ASTNode *decl);
void codegen_function(CodeGenerator *gen, ASTNode *func);
void codegen_procedure(CodeGenerator *gen, ASTNode *proc);
void codegen_statement(CodeGenerator *gen, ASTNode *stmt);
void codegen_expression(CodeGenerator *gen, ASTNode *expr);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

static void init_runtime_value(RuntimeValue *val)
{
    memset(val, 0, sizeof(RuntimeValue));
    val->type = VAL_NONE;
}

static double to_real(RuntimeValue *val)
{
    if (val->type == VAL_REAL)
        return val->value.real_val;
    if (val->type == VAL_INT)
        return (double)val->value.int_val;
    return 0.0;
}

static void debug_log(const char *fmt, ...)
{
    if (debug_mode)
    {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "[DEBUG] ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
    }
}

static char *str_upper(const char *str)
{
    char *result = malloc(strlen(str) + 1);
    for (int i = 0; str[i]; i++)
    {
        result[i] = toupper((unsigned char)str[i]);
    }
    result[strlen(str)] = '\0';
    return result;
}

static bool str_equals_ignore_case(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b))
            return false;
        a++;
        b++;
    }
    return *a == *b;
}

static unsigned int hash_string(const char *str)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % MAX_HASH_SIZE;
}

// ============================================================================
// HASHMAP IMPLEMENTATION
// ============================================================================

static HashMap *create_hashmap()
{
    HashMap *map = calloc(1, sizeof(HashMap));
    return map;
}

static void hashmap_set(HashMap *map, const char *key, void *value)
{
    unsigned int idx = hash_string(key);
    HashNode *node = map->buckets[idx];

    while (node)
    {
        if (strcmp(node->key, key) == 0)
        {
            node->value = value;
            return;
        }
        node = node->next;
    }

    HashNode *new_node = malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = map->buckets[idx];
    map->buckets[idx] = new_node;
}

static void *hashmap_get(HashMap *map, const char *key)
{
    unsigned int idx = hash_string(key);
    HashNode *node = map->buckets[idx];

    while (node)
    {
        if (strcmp(node->key, key) == 0)
        {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

static void free_hashmap(HashMap *map)
{
    for (int i = 0; i < MAX_HASH_SIZE; i++)
    {
        HashNode *node = map->buckets[i];
        while (node)
        {
            HashNode *next = node->next;
            free(node->key);
            free(node);
            node = next;
        }
    }
    free(map);
}

// ============================================================================
// ARRAY OBJECT
// ============================================================================

static ArrayObject *create_array(ArrayBound *bounds, int num_dims)
{
    ArrayObject *arr = malloc(sizeof(ArrayObject));
    arr->data = create_hashmap();
    arr->num_dims = num_dims;
    for (int i = 0; i < num_dims; i++)
    {
        arr->bounds[i] = bounds[i];
    }
    return arr;
}

static void validate_array_indices(ArrayObject *arr, int *indices, int num_indices)
{
    if (num_indices != arr->num_dims)
    {
        fprintf(stderr, "Runtime Error: Incorrect number of indices (%d). Expected %d.\n",
                num_indices, arr->num_dims);
        exit(1);
    }

    for (int i = 0; i < num_indices; i++)
    {
        if (indices[i] < arr->bounds[i].from || indices[i] > arr->bounds[i].to)
        {
            fprintf(stderr, "Runtime Error: Array index %d is out of bounds for dimension %d. Expected [%d..%d].\n",
                    indices[i], i + 1, arr->bounds[i].from, arr->bounds[i].to);
            exit(1);
        }
    }
}

static RuntimeValue array_get(ArrayObject *arr, int *indices, int num_indices)
{
    validate_array_indices(arr, indices, num_indices);

    char key[256] = {0};
    for (int i = 0; i < num_indices; i++)
    {
        char temp[32];
        sprintf(temp, "%d", indices[i]);
        if (i > 0)
            strcat(key, ",");
        strcat(key, temp);
    }

    RuntimeValue *val = hashmap_get(arr->data, key);
    if (val)
    {
        return *val;
    }

    RuntimeValue default_val;
    default_val.type = VAL_INT;
    default_val.value.int_val = 0;
    return default_val;
}

void array_set(ArrayObject *arr, int *indices, int num_indices, RuntimeValue val)
{
    // 1. Δημιουργία κλειδιού (ΠΡΕΠΕΙ να είναι ίδια με την array_get)
    char key[256] = {0};
    for (int i = 0; i < num_indices; i++)
    {
        char temp[32];
        sprintf(temp, "%d", indices[i]); // Χρησιμοποιούμε τον δείκτη ως έχει
        if (i > 0)
            strcat(key, ",");
        strcat(key, temp);
    }

    // 2. Δέσμευση μνήμης για την τιμή
    RuntimeValue *new_val = malloc(sizeof(RuntimeValue));
    if (!new_val)
        return;

    // 3. Αντιγραφή της τιμής
    *new_val = copy_runtime_value(&val);

    // 4. Αποθήκευση στο HashMap
    hashmap_set(arr->data, key, new_val);
}

static void free_array(ArrayObject *arr)
{
    // Free all stored values
    for (int i = 0; i < MAX_HASH_SIZE; i++)
    {
        HashNode *node = arr->data->buckets[i];
        while (node)
        {
            RuntimeValue *val = (RuntimeValue *)node->value;
            free_runtime_value(val);
            free(val);
            node = node->next;
        }
    }
    free_hashmap(arr->data);
    free(arr);
}

// ============================================================================
// RUNTIME VALUE FUNCTIONS
// ============================================================================

static RuntimeValue copy_runtime_value(RuntimeValue *val)
{
    RuntimeValue copy;
    copy.type = val->type;

    switch (val->type)
    {
    case VAL_INT:
        copy.value.int_val = val->value.int_val;
        break;
    case VAL_REAL:
        copy.value.real_val = val->value.real_val;
        break;
    case VAL_BOOL:
        copy.value.bool_val = val->value.bool_val;
        break;
    case VAL_STRING:
        if (val->value.str_val)
        {
            if (strcmp(val->value.str_val, "__EOLN__") == 0)
            {
                copy.value.str_val = val->value.str_val;
            }
            else
            {
                copy.value.str_val = strdup(val->value.str_val);
            }
        }
        break;
    case VAL_ARRAY:
        copy.value.arr_val = val->value.arr_val;
        break;
    default:
        break;
    }
    return copy;
}

static void free_runtime_value(RuntimeValue *val)
{
    if (val == NULL)
        return;

    if (val->type == VAL_STRING && val->value.str_val != NULL)
    {
        if (strcmp(val->value.str_val, "__EOLN__") != 0)
        {
            free(val->value.str_val);
        }
        val->value.str_val = NULL;
    }
    val->type = VAL_NONE; // Επαναφορά τύπου για ασφάλεια
}

static bool to_bool(RuntimeValue *val)
{
    switch (val->type)
    {
    case VAL_BOOL:
        return val->value.bool_val;
    case VAL_INT:
        return val->value.int_val != 0;
    case VAL_REAL:
        return val->value.real_val != 0.0;
    case VAL_STRING:
        return strlen(val->value.str_val) > 0;
    default:
        return false;
    }
}

static int to_int(RuntimeValue *val)
{
    switch (val->type)
    {
    case VAL_INT:
        return val->value.int_val;
    case VAL_REAL:
        return (int)val->value.real_val;
    case VAL_BOOL:
        return val->value.bool_val ? 1 : 0;
    default:
        return 0;
    }
}

// ============================================================================
// ENVIRONMENT
// ============================================================================

static Environment *create_environment(Environment *parent)
{
    Environment *env = calloc(1, sizeof(Environment));
    env->parent = parent;
    return env;
}

static void env_define(Environment *env, const char *name, RuntimeValue value)
{
    char *upper_name = str_upper(name);
    unsigned int idx = hash_string(upper_name);

    EnvEntry *existing = env->entries[idx];
    while (existing)
    {
        if (strcmp(existing->name, upper_name) == 0)
        {
            if (existing->value.type != VAL_ARRAY && existing->value.type != VAL_NONE)
            {
                free_runtime_value(&existing->value);
            }
            existing->value = copy_runtime_value(&value);
            free(upper_name);
            return;
        }
        existing = existing->next;
    }

    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->name = upper_name;
    entry->value = copy_runtime_value(&value);
    entry->subroutine = NULL;
    entry->next = env->entries[idx];
    env->entries[idx] = entry;
}

static void env_define_subroutine(Environment *env, const char *name, ASTNode *subroutine)
{
    char *upper_name = str_upper(name);
    unsigned int idx = hash_string(upper_name);

    EnvEntry *entry = malloc(sizeof(EnvEntry));
    entry->name = upper_name;
    entry->value.type = VAL_NONE;
    entry->subroutine = subroutine;
    entry->next = env->entries[idx];
    env->entries[idx] = entry;
}

static RuntimeValue *env_get(Environment *env, const char *name)
{
    char *upper_name = str_upper(name);
    unsigned int idx = hash_string(upper_name);

    EnvEntry *entry = env->entries[idx];
    while (entry)
    {
        if (strcmp(entry->name, upper_name) == 0)
        {
            free(upper_name);
            return &entry->value;
        }
        entry = entry->next;
    }

    if (env->parent)
    {
        free(upper_name);
        return env_get(env->parent, name);
    }

    fprintf(stderr, "Runtime Error: Undefined variable: %s\n", name);
    free(upper_name);
    exit(1);
}

static ASTNode *env_get_subroutine(Environment *env, const char *name)
{
    char *upper_name = str_upper(name);
    unsigned int idx = hash_string(upper_name);

    EnvEntry *entry = env->entries[idx];
    while (entry)
    {
        if (strcmp(entry->name, upper_name) == 0 && entry->subroutine)
        {
            free(upper_name);
            return entry->subroutine;
        }
        entry = entry->next;
    }

    if (env->parent)
    {
        free(upper_name);
        return env_get_subroutine(env->parent, name);
    }

    fprintf(stderr, "Runtime Error: Undefined function or procedure: %s\n", name);
    free(upper_name);
    exit(1);
}

static void env_assign(Environment *env, const char *name, RuntimeValue value)
{
    char *upper_name = str_upper(name);
    unsigned int idx = hash_string(upper_name);

    EnvEntry *entry = env->entries[idx];
    while (entry)
    {
        if (strcmp(entry->name, upper_name) == 0)
        {
            free_runtime_value(&entry->value);
            entry->value = copy_runtime_value(&value);
            free(upper_name);
            return;
        }
        entry = entry->next;
    }

    if (env->parent)
    {
        free(upper_name);
        env_assign(env->parent, name, value);
        return;
    }

    // Define if not found
    free(upper_name);
    env_define(env, name, value);
}

// ============================================================================
// TOKENIZER
// ============================================================================

static bool is_keyword(const char *str, const char *keyword)
{
    return str_equals_ignore_case(str, keyword);
}

static TokenType get_keyword_type(const char *str)
{
    if (is_keyword(str, "ΑΛΓΟΡΙΘΜΟΣ") || is_keyword(str, "ALGORITHM"))
        return TOK_ALGORITHM;
    if (is_keyword(str, "ΣΤΑΘΕΡΕΣ") || is_keyword(str, "CONSTANTS"))
        return TOK_CONSTANTS;
    if (is_keyword(str, "ΔΕΔΟΜΕΝΑ") || is_keyword(str, "DATA"))
        return TOK_DATA;
    if (is_keyword(str, "ΑΡΧΗ") || is_keyword(str, "BEGIN"))
        return TOK_BEGIN;
    if (is_keyword(str, "ΤΕΛΟΣ") || is_keyword(str, "END"))
        return TOK_END;

    if (is_keyword(str, "ΣΥΝΑΡΤΗΣΗ") || is_keyword(str, "FUNCTION"))
        return TOK_FUNCTION;
    if (is_keyword(str, "ΔΙΑΔΙΚΑΣΙΑ") || is_keyword(str, "PROCEDURE"))
        return TOK_PROCEDURE;
    if (is_keyword(str, "ΔΙΕΠΑΦΗ") || is_keyword(str, "INTERFACE"))
        return TOK_INTERFACE;
    if (is_keyword(str, "ΕΙΣΟΔΟΣ"))
        return TOK_INPUT_PARAM;
    if (is_keyword(str, "ΕΞΟΔΟΣ"))
        return TOK_OUTPUT_PARAM;
    if (is_keyword(str, "ΤΕΛΟΣ-ΣΥΝΑΡΤΗΣΗΣ") || is_keyword(str, "END_FUNCTION"))
        return TOK_END_FUNCTION;
    if (is_keyword(str, "ΤΕΛΟΣ-ΔΙΑΔΙΚΑΣΙΑΣ") || is_keyword(str, "END_PROCEDURE"))
        return TOK_END_PROCEDURE;

    if (is_keyword(str, "ΕΑΝ") || is_keyword(str, "IF"))
        return TOK_IF;
    if (is_keyword(str, "ΤΟΤΕ") || is_keyword(str, "THEN"))
        return TOK_THEN;
    if (is_keyword(str, "ΑΛΛΙΩΣ") || is_keyword(str, "ELSE"))
        return TOK_ELSE;
    if (is_keyword(str, "ΕΑΝ-ΤΕΛΟΣ") || is_keyword(str, "END_IF") || is_keyword(str, "ENDIF"))
        return TOK_END_IF;

    if (is_keyword(str, "ΓΙΑ") || is_keyword(str, "FOR"))
        return TOK_FOR;
    if (is_keyword(str, "ΕΩΣ") || is_keyword(str, "TO"))
        return TOK_TO;
    if (is_keyword(str, "ΒΗΜΑ") || is_keyword(str, "ΜΕ") || is_keyword(str, "STEP"))
        return TOK_STEP;
    if (is_keyword(str, "ΕΠΑΝΑΛΑΒΕ") || is_keyword(str, "REPEAT"))
        return TOK_REPEAT;
    if (is_keyword(str, "ΓΙΑ-ΤΕΛΟΣ") || is_keyword(str, "END_FOR") || is_keyword(str, "ENDFOR"))
        return TOK_END_FOR;

    if (is_keyword(str, "ΕΝΟΣΩ") || is_keyword(str, "WHILE"))
        return TOK_WHILE;
    if (is_keyword(str, "ΕΝΟΣΩ-ΤΕΛΟΣ") || is_keyword(str, "END_WHILE") || is_keyword(str, "ENDWHILE"))
        return TOK_END_WHILE;
    if (is_keyword(str, "ΜΕΧΡΙ") || is_keyword(str, "UNTIL"))
        return TOK_UNTIL;

    if (is_keyword(str, "ΤΥΠΩΣΕ") || is_keyword(str, "PRINT"))
        return TOK_PRINT;
    if (is_keyword(str, "ΔΙΑΒΑΣΕ") || is_keyword(str, "READ"))
        return TOK_READ;
    if (is_keyword(str, "ΥΠΟΛΟΓΙΣΕ") || is_keyword(str, "CALCULATE"))
        return TOK_CALCULATE;

    if (is_keyword(str, "ΑΚΕΡΑΙΟΣ") || is_keyword(str, "INTEGER"))
        return TOK_INTEGER_TYPE;
    if (is_keyword(str, "ΠΡΑΓΜΑΤΙΚΟΣ") || is_keyword(str, "REAL"))
        return TOK_REAL_TYPE;
    if (is_keyword(str, "ΛΟΓΙΚΟΣ") || is_keyword(str, "BOOLEAN"))
        return TOK_BOOLEAN_TYPE;
    if (is_keyword(str, "ΧΑΡΑΚΤΗΡΑΣ") || is_keyword(str, "CHAR"))
        return TOK_CHAR_TYPE;
    if (is_keyword(str, "ΣΥΜΒΟΛΟΣΕΙΡΑ") || is_keyword(str, "STRING"))
        return TOK_STRING_TYPE;
    if (is_keyword(str, "ARRAY"))
        return TOK_ARRAY;
    if (is_keyword(str, "OF"))
        return TOK_OF;

    if (is_keyword(str, "ΚΑΙ") || is_keyword(str, "AND"))
        return TOK_AND;
    if (is_keyword(str, "Ή") || is_keyword(str, "OR"))
        return TOK_OR;
    if (is_keyword(str, "ΟΧΙ") || is_keyword(str, "NOT"))
        return TOK_NOT;
    if (is_keyword(str, "MOD"))
        return TOK_MOD;
    if (is_keyword(str, "DIV"))
        return TOK_DIV;

    if (is_keyword(str, "ΑΛΗΘΗΣ") || is_keyword(str, "TRUE"))
        return TOK_BOOLEAN_LITERAL;
    if (is_keyword(str, "ΨΕΥΔΗΣ") || is_keyword(str, "FALSE"))
        return TOK_BOOLEAN_LITERAL;

    return TOK_IDENTIFIER;
}

static void add_token(TokenType type, const char *value, int line, int col)
{
    if (token_count >= MAX_TOKENS)
    {
        fprintf(stderr, "Error: Too many tokens\n");
        exit(1);
    }
    tokens[token_count].type = type;
    strncpy(tokens[token_count].value, value, MAX_TOKEN_LEN - 1);
    tokens[token_count].value[MAX_TOKEN_LEN - 1] = '\0';
    tokens[token_count].line = line;
    tokens[token_count].column = col;
    token_count++;
}

static void tokenize(const char *code)
{
    int pos = 0;
    int line = 1;
    int col = 1;
    int len = strlen(code);

    while (pos < len)
    {
        while (pos < len && isspace((unsigned char)code[pos]))
        {
            if (code[pos] == '\n')
            {
                line++;
                col = 1;
            }
            else
            {
                col++;
            }
            pos++;
        }

        if (pos >= len)
            break;

        int start_col = col;

        if (code[pos] == '/' && pos + 1 < len && code[pos + 1] == '/')
        {
            while (pos < len && code[pos] != '\n')
                pos++;
            continue;
        }

        if (code[pos] == '/' && pos + 1 < len && code[pos + 1] == '*')
        {
            pos += 2;
            while (pos + 1 < len && !(code[pos] == '*' && code[pos + 1] == '/'))
            {
                if (code[pos] == '\n')
                    line++;
                pos++;
            }
            pos += 2;
            continue;
        }

        if (code[pos] == '"')
        {
            char str[MAX_TOKEN_LEN] = {0};
            int str_len = 0;
            pos++;
            col++;
            while (pos < len && code[pos] != '"' && str_len < MAX_TOKEN_LEN - 1)
            {
                str[str_len++] = code[pos++];
                col++;
            }
            if (code[pos] == '"')
            {
                pos++;
                col++;
            }
            add_token(TOK_STRING, str, line, start_col);
            continue;
        }

        if (isdigit((unsigned char)code[pos]))
        {
            char num[MAX_TOKEN_LEN] = {0};
            int num_len = 0;
            bool has_dot = false;

            while (pos < len && num_len < MAX_TOKEN_LEN - 1)
            {
                if (isdigit((unsigned char)code[pos]))
                {
                    num[num_len++] = code[pos++];
                    col++;
                }
                else if (code[pos] == '.' && !has_dot)
                {
                    // Check if this is a decimal point or the .. operator
                    if (pos + 1 < len && code[pos + 1] == '.')
                    {
                        // This is the .. operator, stop parsing the number
                        break;
                    }
                    else if (pos + 1 < len && isdigit((unsigned char)code[pos + 1]))
                    {
                        // This is a decimal point followed by a digit
                        has_dot = true;
                        num[num_len++] = code[pos++];
                        col++;
                    }
                    else
                    {
                        // Single dot not followed by digit - stop here
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            add_token(TOK_NUMBER, num, line, start_col);
            continue;
        }

        if (pos + 1 < len)
        {
            char two_char[3] = {code[pos], code[pos + 1], '\0'};
            TokenType type = TOK_ERROR;

            if (strcmp(two_char, ":=") == 0)
                type = TOK_ASSIGN;
            else if (strcmp(two_char, "<>") == 0)
                type = TOK_NOT_EQUALS;
            else if (strcmp(two_char, "<=") == 0)
                type = TOK_LESS_EQUALS;
            else if (strcmp(two_char, ">=") == 0)
                type = TOK_GREATER_EQUALS;
            else if (strcmp(two_char, "..") == 0)
                type = TOK_DOT;

            if (type != TOK_ERROR)
            {
                add_token(type, two_char, line, start_col);
                pos += 2;
                col += 2;
                continue;
            }
        }

        char ch[2] = {code[pos], '\0'};
        TokenType type = TOK_ERROR;

        switch (code[pos])
        {
        case '+':
            type = TOK_PLUS;
            break;
        case '-':
            type = TOK_MINUS;
            break;
        case '*':
            type = TOK_MULTIPLY;
            break;
        case '/':
            type = TOK_DIVIDE;
            break;
        case '=':
            type = TOK_EQUALS;
            break;
        case '<':
            type = TOK_LESS_THAN;
            break;
        case '>':
            type = TOK_GREATER_THAN;
            break;
        case '(':
            type = TOK_LEFT_PAREN;
            break;
        case ')':
            type = TOK_RIGHT_PAREN;
            break;
        case '[':
            type = TOK_LEFT_BRACKET;
            break;
        case ']':
            type = TOK_RIGHT_BRACKET;
            break;
        case ',':
            type = TOK_COMMA;
            break;
        case ':':
            type = TOK_COLON;
            break;
        case ';':
            type = TOK_SEMICOLON;
            break;
        case '%':
            type = TOK_PERCENT;
            break;
        }

        if (type != TOK_ERROR)
        {
            add_token(type, ch, line, start_col);
            pos++;
            col++;
            continue;
        }

        if (isalpha((unsigned char)code[pos]) || (unsigned char)code[pos] >= 0x80)
        {
            char ident[MAX_TOKEN_LEN] = {0};
            int ident_len = 0;

            // Αντικαταστήστε το while loop στη συνάρτηση tokenize (περίπου γραμμή 650)
            while (pos < len && ident_len < MAX_TOKEN_LEN - 1)
            {
                unsigned char current_char = (unsigned char)code[pos];

                // Επιτρέπουμε γράμματα, αριθμούς, underscores και ελληνικά
                if (isalnum(current_char) || current_char == '_' || current_char >= 0x80)
                {
                    ident[ident_len++] = code[pos++];
                    col++;
                }
                // Επιτρέπουμε την παύλα '-' ΜΟΝΟ αν ΔΕΝ ακολουθείται από ψηφίο
                // Έτσι το "ΕΑΝ-ΤΕΛΟΣ" παραμένει ένα token, αλλά το "j-1" σπάει σε j, - , 1
                else if (current_char == '-' && pos + 1 < len && !isdigit((unsigned char)code[pos + 1]))
                {
                    ident[ident_len++] = code[pos++];
                    col++;
                }
                else
                {
                    break;
                }
            }
            TokenType kw_type = get_keyword_type(ident);
            add_token(kw_type, ident, line, start_col);
            continue;
        }

        fprintf(stderr, "Error: Unexpected character '%c' at line %d:%d\n", code[pos], line, col);
        exit(1);
    }

    add_token(TOK_EOF, "EOF", line, col);
}

// ============================================================================
// PARSER
// ============================================================================

static Token *current_token()
{
    return &tokens[token_pos];
}

static void advance_token()
{
    if (token_pos < token_count - 1)
    {
        token_pos++;
    }
}

static void expect_token(TokenType type)
{
    if (current_token()->type != type)
    {
        fprintf(stderr, "Syntax Error at line %d:\n", current_token()->line);
        fprintf(stderr, "  Expected: %s (type %d)\n", token_type_name(type), type);
        fprintf(stderr, "  Got:      '%s' (type %d)\n", current_token()->value, current_token()->type);

        // Show context
        fprintf(stderr, "  Previous tokens:\n");
        for (int i = 3; i > 0; i--)
        {
            if (token_pos - i >= 0)
            {
                Token *t = &tokens[token_pos - i];
                fprintf(stderr, "    [-%d] '%s' (type %d)\n", i, t->value, t->type);
            }
        }

        exit(1);
    }
    advance_token();
}

static bool match_token(TokenType type)
{
    return current_token()->type == type;
}

static bool match_any(int count, ...)
{
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++)
    {
        if (current_token()->type == va_arg(args, TokenType))
        {
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}

static ASTNode *create_node(ASTNodeType type)
{
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    node->line = current_token()->line;
    return node;
}

static ASTNode *parse_expression();
static ASTNode *parse_statement();
static ASTNode **parse_block(int *num_stmts);

static ASTNode *parse_primary()
{
    if (match_token(TOK_NUMBER))
    {
        ASTNode *node = create_node(AST_LITERAL);
        if (strchr(current_token()->value, '.'))
        {
            node->literal.value.type = VAL_REAL;
            node->literal.value.value.real_val = atof(current_token()->value);
        }
        else
        {
            node->literal.value.type = VAL_INT;
            node->literal.value.value.int_val = atoi(current_token()->value);
        }
        advance_token();
        return node;
    }

    if (match_token(TOK_BOOLEAN_LITERAL))
    {
        ASTNode *node = create_node(AST_LITERAL);
        node->literal.value.type = VAL_BOOL;
        node->literal.value.value.bool_val =
            (str_equals_ignore_case(current_token()->value, "ΑΛΗΘΗΣ") ||
             str_equals_ignore_case(current_token()->value, "TRUE"));
        advance_token();
        return node;
    }

    if (match_token(TOK_STRING))
    {
        ASTNode *node = create_node(AST_LITERAL);
        node->literal.value.type = VAL_STRING;
        node->literal.value.value.str_val = strdup(current_token()->value);
        advance_token();
        return node;
    }

    if (match_token(TOK_IDENTIFIER))
    {
        char *name = strdup(current_token()->value);
        advance_token();

        if (match_token(TOK_LEFT_PAREN))
        {
            advance_token();
            ASTNode *call = create_node(AST_CALL);
            call->call.name = name;
            call->call.is_statement = false;

            int cap = 10;
            call->call.arguments = malloc(cap * sizeof(ASTNode *));
            call->call.num_args = 0;

            if (!match_token(TOK_RIGHT_PAREN))
            {
                do
                {
                    if (call->call.num_args >= cap)
                    {
                        cap *= 2;
                        call->call.arguments = realloc(call->call.arguments, cap * sizeof(ASTNode *));
                    }
                    call->call.arguments[call->call.num_args++] = parse_expression();
                } while (match_token(TOK_COMMA) && (advance_token(), 1));
            }

            expect_token(TOK_RIGHT_PAREN);
            return call;
        }

        if (match_token(TOK_LEFT_BRACKET))
        {
            advance_token();
            ASTNode *arr = create_node(AST_ARRAY_ACCESS);
            arr->array_access.name = name;

            int cap = 5;
            arr->array_access.indices = malloc(cap * sizeof(ASTNode *));
            arr->array_access.num_indices = 0;

            do
            {
                if (arr->array_access.num_indices >= cap)
                {
                    cap *= 2;
                    arr->array_access.indices = realloc(arr->array_access.indices, cap * sizeof(ASTNode *));
                }
                arr->array_access.indices[arr->array_access.num_indices++] = parse_expression();
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_RIGHT_BRACKET);
            return arr;
        }

        ASTNode *id = create_node(AST_IDENTIFIER);
        id->identifier.name = name;
        return id;
    }

    if (match_token(TOK_LEFT_PAREN))
    {
        advance_token();
        ASTNode *expr = parse_expression();
        expect_token(TOK_RIGHT_PAREN);
        return expr;
    }

    fprintf(stderr, "Syntax Error: Unexpected token at line %d\n", current_token()->line);
    exit(1);
}

static ASTNode *parse_unary()
{
    if (match_token(TOK_NOT) || match_token(TOK_MINUS))
    {
        ASTNode *node = create_node(AST_UNARY_OP);
        node->unary.operator = strdup(current_token()->value);
        advance_token();
        node->unary.operand = parse_unary();
        return node;
    }
    return parse_primary();
}

static ASTNode *parse_multiplicative()
{
    ASTNode *left = parse_unary();

    while (match_any(4, TOK_MULTIPLY, TOK_DIVIDE, TOK_MOD, TOK_DIV))
    {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->binary.operator = strdup(current_token()->value);
        node->binary.left = left;
        advance_token();
        node->binary.right = parse_unary();
        left = node;
    }

    return left;
}

static ASTNode *parse_additive()
{
    ASTNode *left = parse_multiplicative();

    while (match_token(TOK_PLUS) || match_token(TOK_MINUS))
    {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->binary.operator = strdup(current_token()->value);
        node->binary.left = left;
        advance_token();
        node->binary.right = parse_multiplicative();
        left = node;
    }

    return left;
}

static ASTNode *parse_comparison()
{
    ASTNode *left = parse_additive();

    while (match_any(6, TOK_EQUALS, TOK_NOT_EQUALS, TOK_LESS_THAN, TOK_GREATER_THAN, TOK_LESS_EQUALS, TOK_GREATER_EQUALS))
    {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->binary.operator = strdup(current_token()->value);
        node->binary.left = left;
        advance_token();
        node->binary.right = parse_additive();
        left = node;
    }

    return left;
}

static ASTNode *parse_and()
{
    ASTNode *left = parse_comparison();

    while (match_token(TOK_AND))
    {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->binary.operator = strdup(current_token()->value);
        node->binary.left = left;
        advance_token();
        node->binary.right = parse_comparison();
        left = node;
    }

    return left;
}

static ASTNode *parse_expression()
{
    ASTNode *left = parse_and();

    while (match_token(TOK_OR))
    {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->binary.operator = strdup(current_token()->value);
        node->binary.left = left;
        advance_token();
        node->binary.right = parse_and();
        left = node;
    }

    return left;
}

static ASTNode *parse_print()
{
    expect_token(TOK_PRINT);
    expect_token(TOK_LEFT_PAREN);

    ASTNode *node = create_node(AST_PRINT);
    int cap = 10;
    node->print.expressions = malloc(cap * sizeof(ASTNode *));
    node->print.num_exprs = 0;

    if (!match_token(TOK_RIGHT_PAREN))
    {
        do
        {
            if (node->print.num_exprs >= cap)
            {
                cap *= 2;
                node->print.expressions = realloc(node->print.expressions, cap * sizeof(ASTNode *));
            }
            node->print.expressions[node->print.num_exprs++] = parse_expression();
        } while (match_token(TOK_COMMA) && (advance_token(), 1));
    }

    expect_token(TOK_RIGHT_PAREN);
    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_read()
{
    expect_token(TOK_READ);
    expect_token(TOK_LEFT_PAREN);

    ASTNode *node = create_node(AST_READ);
    int cap = 10;
    node->read.variables = malloc(cap * sizeof(ASTNode *));
    node->read.num_vars = 0;

    if (!match_token(TOK_RIGHT_PAREN))
    {
        do
        {
            if (node->read.num_vars >= cap)
            {
                cap *= 2;
                node->read.variables = realloc(node->read.variables, cap * sizeof(ASTNode *));
            }
            node->read.variables[node->read.num_vars++] = parse_primary();
        } while (match_token(TOK_COMMA) && (advance_token(), 1));
    }

    expect_token(TOK_RIGHT_PAREN);
    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_assignment()
{
    ASTNode *node = create_node(AST_ASSIGN);
    node->assign.identifier = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);

    if (match_token(TOK_LEFT_BRACKET))
    {
        advance_token();
        int cap = 5;
        node->assign.indices = malloc(cap * sizeof(ASTNode *));
        node->assign.num_indices = 0;

        do
        {
            if (node->assign.num_indices >= cap)
            {
                cap *= 2;
                node->assign.indices = realloc(node->assign.indices, cap * sizeof(ASTNode *));
            }
            node->assign.indices[node->assign.num_indices++] = parse_expression();
        } while (match_token(TOK_COMMA) && (advance_token(), 1));

        expect_token(TOK_RIGHT_BRACKET);
    }

    expect_token(TOK_ASSIGN);
    node->assign.value = parse_expression();

    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_call_statement()
{
    char *name = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);
    expect_token(TOK_LEFT_PAREN);

    ASTNode *node = create_node(AST_CALL);
    node->call.name = name;
    node->call.is_statement = true;

    int cap = 10;
    node->call.arguments = malloc(cap * sizeof(ASTNode *));
    node->call.num_args = 0;

    if (!match_token(TOK_RIGHT_PAREN))
    {
        do
        {
            if (match_token(TOK_PERCENT))
                advance_token();

            if (node->call.num_args >= cap)
            {
                cap *= 2;
                node->call.arguments = realloc(node->call.arguments, cap * sizeof(ASTNode *));
            }
            node->call.arguments[node->call.num_args++] = parse_expression();
        } while (match_token(TOK_COMMA) && (advance_token(), 1));
    }

    expect_token(TOK_RIGHT_PAREN);
    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_if()
{
    expect_token(TOK_IF);

    ASTNode *node = create_node(AST_IF);
    node->if_stmt.condition = parse_expression();

    expect_token(TOK_THEN);
    node->if_stmt.then_branch = parse_block(&node->if_stmt.num_then);

    if (match_token(TOK_ELSE))
    {
        advance_token();
        node->if_stmt.else_branch = parse_block(&node->if_stmt.num_else);
    }

    expect_token(TOK_END_IF);
    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_for()
{
    expect_token(TOK_FOR);

    ASTNode *node = create_node(AST_FOR);
    node->for_loop.variable = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);
    expect_token(TOK_ASSIGN);

    node->for_loop.start = parse_expression();
    expect_token(TOK_TO);
    node->for_loop.end = parse_expression();

    if (match_token(TOK_STEP))
    {
        advance_token();
        node->for_loop.step = parse_expression();
    }
    else
    {
        ASTNode *step = create_node(AST_LITERAL);
        step->literal.value.type = VAL_INT;
        step->literal.value.value.int_val = 1;
        node->for_loop.step = step;
    }

    expect_token(TOK_REPEAT);
    node->for_loop.body = parse_block(&node->for_loop.num_stmts);
    expect_token(TOK_END_FOR);

    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}

static ASTNode *parse_while()
{
    expect_token(TOK_WHILE);

    ASTNode *node = create_node(AST_WHILE);
    node->while_loop.is_repeat_until = false;

    node->while_loop.condition = parse_expression();

    expect_token(TOK_REPEAT);

    node->while_loop.body = parse_block(&node->while_loop.num_stmts);

    expect_token(TOK_END_WHILE);

    if (match_token(TOK_SEMICOLON))
        advance_token();

    return node;
}
static ASTNode *parse_repeat()
{
    expect_token(TOK_REPEAT);

    ASTNode *node = create_node(AST_WHILE);
    node->while_loop.is_repeat_until = true;

    node->while_loop.body = parse_block(&node->while_loop.num_stmts);

    expect_token(TOK_UNTIL);

    node->while_loop.condition = parse_expression();

    // Δέχεται προαιρετικό semicolon μετά την συνθήκη
    if (match_token(TOK_SEMICOLON))
    {
        advance_token();
    }

    return node;
}

static ASTNode *parse_statement()
{
    if (match_token(TOK_PRINT))
    {
        return parse_print();
    }
    else if (match_token(TOK_READ))
    {
        return parse_read();
    }
    else if (match_token(TOK_CALCULATE))
    {
        advance_token();
        return parse_call_statement();
    }
    else if (match_token(TOK_IF))
    {
        return parse_if();
    }
    else if (match_token(TOK_FOR))
    {
        return parse_for();
    }
    else if (match_token(TOK_WHILE))
    {
        return parse_while();
    }
    else if (match_token(TOK_REPEAT))
    {
        return parse_repeat();
    }
    else if (match_token(TOK_IDENTIFIER))
    {
        if (tokens[token_pos + 1].type == TOK_LEFT_PAREN)
        {
            return parse_call_statement();
        }
        else
        {
            return parse_assignment();
        }
    }

    fprintf(stderr, "Syntax Error: Unexpected token in statement at line %d\n", current_token()->line);
    exit(1);
}

static ASTNode **parse_block(int *num_stmts)
{
    int cap = 20;
    ASTNode **stmts = malloc(cap * sizeof(ASTNode *));
    *num_stmts = 0;

    while (!match_any(9, TOK_END, TOK_ELSE, TOK_UNTIL, TOK_END_IF, TOK_END_FOR, TOK_END_WHILE, TOK_EOF, TOK_END_FUNCTION, TOK_END_PROCEDURE))
    {
        if (*num_stmts >= cap)
        {
            cap *= 2;
            stmts = realloc(stmts, cap * sizeof(ASTNode *));
        }
        stmts[(*num_stmts)++] = parse_statement();
    }

    return stmts;
}

static ASTNode **parse_parameters(int *num_params, char *func_name);

static ASTNode **parse_interface(int *num_params, char *func_name)
{
    expect_token(TOK_INTERFACE);

    int cap = 10;
    ASTNode **params = malloc(cap * sizeof(ASTNode *));
    *num_params = 0;

    // INPUT parameters
    if (match_token(TOK_INPUT_PARAM))
    {
        advance_token();

        while (match_token(TOK_IDENTIFIER))
        {
            int name_cap = 10;
            char **names = malloc(name_cap * sizeof(char *));
            int name_count = 0;

            do
            {
                if (name_count >= name_cap)
                {
                    name_cap *= 2;
                    names = realloc(names, name_cap * sizeof(char *));
                }
                names[name_count++] = strdup(current_token()->value);
                expect_token(TOK_IDENTIFIER);
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_COLON);

            // Parse type (could be ARRAY or simple type)
            char *type_str = NULL;
            if (match_token(TOK_ARRAY))
            {
                // For now, just skip the full array type specification
                // We'll store it as a simple type string
                int depth = 0;
                char type_buf[256] = {0};
                int buf_pos = 0;

                // Capture the entire array type as a string
                while (buf_pos < 255 && !match_token(TOK_SEMICOLON))
                {
                    if (buf_pos > 0)
                        type_buf[buf_pos++] = ' ';
                    strncpy(type_buf + buf_pos, current_token()->value, 255 - buf_pos);
                    buf_pos = strlen(type_buf);
                    advance_token();
                }
                type_str = strdup(type_buf);
            }
            else
            {
                // Simple type
                if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
                {
                    type_str = strdup(current_token()->value);
                    advance_token();
                }
                else if (match_token(TOK_IDENTIFIER))
                {
                    type_str = strdup(current_token()->value);
                    advance_token();
                }
            }

            expect_token(TOK_SEMICOLON);

            for (int i = 0; i < name_count; i++)
            {
                if (*num_params >= cap)
                {
                    cap *= 2;
                    params = realloc(params, cap * sizeof(ASTNode *));
                }

                ASTNode *param = create_node(AST_PARAMETER);
                param->param.name = names[i];
                param->param.param_type = type_str ? strdup(type_str) : strdup("INTEGER");
                param->param.is_reference = false;
                params[(*num_params)++] = param;
            }

            free(names);
            if (type_str)
                free(type_str);
        }
    }

    // OUTPUT parameters
    if (match_token(TOK_OUTPUT_PARAM))
    {
        advance_token();

        while (match_token(TOK_IDENTIFIER))
        {
            int name_cap = 10;
            char **names = malloc(name_cap * sizeof(char *));
            int name_count = 0;

            do
            {
                if (name_count >= name_cap)
                {
                    name_cap *= 2;
                    names = realloc(names, name_cap * sizeof(char *));
                }
                names[name_count++] = strdup(current_token()->value);
                expect_token(TOK_IDENTIFIER);
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_COLON);

            // Parse type (could be ARRAY or simple type)
            char *type_str = NULL;
            if (match_token(TOK_ARRAY))
            {
                // Skip the full array type specification
                char type_buf[256] = {0};
                int buf_pos = 0;

                while (buf_pos < 255 && !match_token(TOK_SEMICOLON))
                {
                    if (buf_pos > 0)
                        type_buf[buf_pos++] = ' ';
                    strncpy(type_buf + buf_pos, current_token()->value, 255 - buf_pos);
                    buf_pos = strlen(type_buf);
                    advance_token();
                }
                type_str = strdup(type_buf);
            }
            else
            {
                // Simple type
                if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
                {
                    type_str = strdup(current_token()->value);
                    advance_token();
                }
                else if (match_token(TOK_IDENTIFIER))
                {
                    type_str = strdup(current_token()->value);
                    advance_token();
                }
            }

            expect_token(TOK_SEMICOLON);

            for (int i = 0; i < name_count; i++)
            {
                // Skip function return variable
                if (func_name && str_equals_ignore_case(names[i], func_name))
                {
                    free(names[i]);
                    continue;
                }

                // Check if already in INPUT
                bool found = false;
                for (int j = 0; j < *num_params; j++)
                {
                    if (str_equals_ignore_case(params[j]->param.name, names[i]))
                    {
                        params[j]->param.is_reference = true;
                        found = true;
                        free(names[i]);
                        break;
                    }
                }

                if (!found)
                {
                    if (*num_params >= cap)
                    {
                        cap *= 2;
                        params = realloc(params, cap * sizeof(ASTNode *));
                    }

                    ASTNode *param = create_node(AST_PARAMETER);
                    param->param.name = names[i];
                    param->param.param_type = type_str ? strdup(type_str) : strdup("INTEGER");
                    param->param.is_reference = true;
                    params[(*num_params)++] = param;
                }
            }

            free(names);
            if (type_str)
                free(type_str);
        }
    }

    return params;
}
static ASTNode *parse_function()
{
    expect_token(TOK_FUNCTION);

    ASTNode *node = create_node(AST_FUNC_DECL);
    node->subroutine.name = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);

    // Skip simple parameter list in header
    if (match_token(TOK_LEFT_PAREN))
    {
        advance_token();
        while (!match_token(TOK_RIGHT_PAREN))
        {
            if (!match_any(2, TOK_IDENTIFIER, TOK_COMMA))
            {
                fprintf(stderr, "Syntax Error: Expected parameter name or comma at line %d\n", current_token()->line);
                exit(1);
            }
            advance_token();
        }
        expect_token(TOK_RIGHT_PAREN);
    }

    expect_token(TOK_COLON);
    node->subroutine.return_type = strdup(current_token()->value);
    advance_token();

    node->subroutine.parameters = parse_interface(&node->subroutine.num_params, node->subroutine.name);

    // Local DATA - NOW PROPERLY PARSE IT
    node->subroutine.local_decls = NULL;
    node->subroutine.num_local_decls = 0;

    if (match_token(TOK_DATA))
    {
        advance_token();

        // Parse local variable declarations
        int cap = 20;
        node->subroutine.local_decls = malloc(cap * sizeof(ASTNode *));

        while (match_token(TOK_IDENTIFIER))
        {
            int name_cap = 10;
            char **names = malloc(name_cap * sizeof(char *));
            int name_count = 0;

            do
            {
                if (name_count >= name_cap)
                {
                    name_cap *= 2;
                    names = realloc(names, name_cap * sizeof(char *));
                }
                names[name_count++] = strdup(current_token()->value);
                expect_token(TOK_IDENTIFIER);
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_COLON);

            char *type_str = NULL;
            if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
            {
                type_str = strdup(current_token()->value);
                advance_token();
            }
            else if (match_token(TOK_IDENTIFIER))
            {
                type_str = strdup(current_token()->value);
                advance_token();
            }
            else
            {
                fprintf(stderr, "Syntax Error: Expected type at line %d\n", current_token()->line);
                exit(1);
            }

            expect_token(TOK_SEMICOLON);

            for (int i = 0; i < name_count; i++)
            {
                if (node->subroutine.num_local_decls >= cap)
                {
                    cap *= 2;
                    node->subroutine.local_decls = realloc(node->subroutine.local_decls, cap * sizeof(ASTNode *));
                }

                ASTNode *var_decl = create_node(AST_VAR_DECL);
                var_decl->decl.name = names[i];
                var_decl->decl.var_type = strdup(type_str);
                var_decl->decl.num_arr_dims = 0;
                node->subroutine.local_decls[node->subroutine.num_local_decls++] = var_decl;
            }

            free(names);
            free(type_str);
        }
    }

    expect_token(TOK_BEGIN);
    node->subroutine.body = parse_block(&node->subroutine.num_stmts);
    expect_token(TOK_END_FUNCTION);

    return node;
}

static ASTNode *parse_procedure()
{
    expect_token(TOK_PROCEDURE);

    ASTNode *node = create_node(AST_PROC_DECL);
    node->subroutine.name = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);

    // Skip simple parameter list in header
    if (match_token(TOK_LEFT_PAREN))
    {
        advance_token();
        while (!match_token(TOK_RIGHT_PAREN))
        {
            if (!match_any(3, TOK_IDENTIFIER, TOK_COMMA, TOK_PERCENT))
            {
                fprintf(stderr, "Syntax Error: Expected parameter name, comma or %% at line %d\n", current_token()->line);
                exit(1);
            }
            advance_token();
        }
        expect_token(TOK_RIGHT_PAREN);
    }

    node->subroutine.parameters = parse_interface(&node->subroutine.num_params, NULL);

    // Local DATA - NOW PROPERLY PARSE IT
    node->subroutine.local_decls = NULL;
    node->subroutine.num_local_decls = 0;

    if (match_token(TOK_DATA))
    {
        advance_token();

        // Parse local variable declarations (same as global DATA section)
        int cap = 20;
        node->subroutine.local_decls = malloc(cap * sizeof(ASTNode *));

        while (match_token(TOK_IDENTIFIER))
        {
            int name_cap = 10;
            char **names = malloc(name_cap * sizeof(char *));
            int name_count = 0;

            // Collect variable names
            do
            {
                if (name_count >= name_cap)
                {
                    name_cap *= 2;
                    names = realloc(names, name_cap * sizeof(char *));
                }
                names[name_count++] = strdup(current_token()->value);
                expect_token(TOK_IDENTIFIER);
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_COLON);

            // Parse type
            char *type_str = NULL;
            if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
            {
                type_str = strdup(current_token()->value);
                advance_token();
            }
            else if (match_token(TOK_IDENTIFIER))
            {
                type_str = strdup(current_token()->value);
                advance_token();
            }
            else
            {
                fprintf(stderr, "Syntax Error: Expected type at line %d\n", current_token()->line);
                exit(1);
            }

            expect_token(TOK_SEMICOLON);

            // Create variable declarations
            for (int i = 0; i < name_count; i++)
            {
                if (node->subroutine.num_local_decls >= cap)
                {
                    cap *= 2;
                    node->subroutine.local_decls = realloc(node->subroutine.local_decls, cap * sizeof(ASTNode *));
                }

                ASTNode *var_decl = create_node(AST_VAR_DECL);
                var_decl->decl.name = names[i];
                var_decl->decl.var_type = strdup(type_str);
                var_decl->decl.num_arr_dims = 0;
                node->subroutine.local_decls[node->subroutine.num_local_decls++] = var_decl;
            }

            free(names);
            free(type_str);
        }
    }

    expect_token(TOK_BEGIN);
    node->subroutine.body = parse_block(&node->subroutine.num_stmts);
    expect_token(TOK_END_PROCEDURE);

    return node;
}

static ASTNode *parse_program()
{
    expect_token(TOK_ALGORITHM);

    ASTNode *prog = create_node(AST_PROGRAM);
    prog->program.name = strdup(current_token()->value);
    expect_token(TOK_IDENTIFIER);

    int cap = 50;
    prog->program.declarations = malloc(cap * sizeof(ASTNode *));
    prog->program.num_decls = 0;

    // CONSTANTS
    if (match_token(TOK_CONSTANTS))
    {
        advance_token();

        while (match_token(TOK_IDENTIFIER))
        {
            ASTNode *const_decl = create_node(AST_CONST_DECL);
            const_decl->decl.name = strdup(current_token()->value);
            expect_token(TOK_IDENTIFIER);
            expect_token(TOK_EQUALS);
            const_decl->decl.value = parse_expression();

            if (!match_token(TOK_SEMICOLON))
            {
                fprintf(stderr, "Syntax Error: Expected semicolon after constant declaration at line %d, got token type %d\n",
                        current_token()->line, current_token()->type);
                exit(1);
            }
            expect_token(TOK_SEMICOLON);

            if (prog->program.num_decls >= cap)
            {
                cap *= 2;
                prog->program.declarations = realloc(prog->program.declarations, cap * sizeof(ASTNode *));
            }
            prog->program.declarations[prog->program.num_decls++] = const_decl;
        }
    }

    // DATA
    if (match_token(TOK_DATA))
    {
        advance_token();

        while (match_token(TOK_IDENTIFIER))
        {
            int name_cap = 10;
            char **names = malloc(name_cap * sizeof(char *));
            int name_count = 0;

            do
            {
                if (name_count >= name_cap)
                {
                    name_cap *= 2;
                    names = realloc(names, name_cap * sizeof(char *));
                }
                names[name_count++] = strdup(current_token()->value);
                expect_token(TOK_IDENTIFIER);
            } while (match_token(TOK_COMMA) && (advance_token(), 1));

            expect_token(TOK_COLON);

            // Parse type (could be simple type or ARRAY type)
            bool is_array = false;
            ArrayBoundExpr array_bound_exprs[MAX_ARRAY_DIMS];
            int num_dims = 0;
            char *base_type = NULL;

            if (match_token(TOK_ARRAY))
            {
                is_array = true;
                advance_token();
                expect_token(TOK_LEFT_BRACKET);

                // Parse array dimensions
                do
                {
                    // Parse start..end
                    array_bound_exprs[num_dims].start_expr = parse_expression();
                    expect_token(TOK_DOT);
                    array_bound_exprs[num_dims].end_expr = parse_expression();
                    num_dims++;
                } while (match_token(TOK_COMMA) && (advance_token(), 1));

                expect_token(TOK_RIGHT_BRACKET);
                expect_token(TOK_OF);

                // Accept either type keyword or identifier
                if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
                {
                    base_type = strdup(current_token()->value);
                    advance_token();
                }
                else if (match_token(TOK_IDENTIFIER))
                {
                    base_type = strdup(current_token()->value);
                    advance_token();
                }
                else
                {
                    fprintf(stderr, "Syntax Error: Expected type after OF at line %d\n", current_token()->line);
                    exit(1);
                }
            }
            else
            {
                // Simple type
                if (match_any(5, TOK_INTEGER_TYPE, TOK_REAL_TYPE, TOK_BOOLEAN_TYPE, TOK_CHAR_TYPE, TOK_STRING_TYPE))
                {
                    base_type = strdup(current_token()->value);
                    advance_token();
                }
                else if (match_token(TOK_IDENTIFIER))
                {
                    base_type = strdup(current_token()->value);
                    advance_token();
                }
                else
                {
                    fprintf(stderr, "Syntax Error: Expected type at line %d\n", current_token()->line);
                    exit(1);
                }
            }

            expect_token(TOK_SEMICOLON);

            for (int i = 0; i < name_count; i++)
            {
                ASTNode *var_decl = create_node(AST_VAR_DECL);
                var_decl->decl.name = names[i];

                if (is_array)
                {
                    var_decl->decl.arr_bound_exprs = malloc(num_dims * sizeof(ArrayBoundExpr));
                    memcpy(var_decl->decl.arr_bound_exprs, array_bound_exprs, num_dims * sizeof(ArrayBoundExpr));
                    var_decl->decl.num_arr_dims = num_dims;
                    var_decl->decl.var_type = base_type;
                }
                else
                {
                    var_decl->decl.var_type = base_type;
                    var_decl->decl.num_arr_dims = 0;
                }

                if (prog->program.num_decls >= cap)
                {
                    cap *= 2;
                    prog->program.declarations = realloc(prog->program.declarations, cap * sizeof(ASTNode *));
                }
                prog->program.declarations[prog->program.num_decls++] = var_decl;
            }

            free(names);
            if (!is_array)
            {
                free(base_type);
            }
        }
    }

    // Functions and Procedures
    while (match_token(TOK_FUNCTION) || match_token(TOK_PROCEDURE))
    {
        if (prog->program.num_decls >= cap)
        {
            cap *= 2;
            prog->program.declarations = realloc(prog->program.declarations, cap * sizeof(ASTNode *));
        }

        if (match_token(TOK_FUNCTION))
        {
            prog->program.declarations[prog->program.num_decls++] = parse_function();
        }
        else
        {
            prog->program.declarations[prog->program.num_decls++] = parse_procedure();
        }
    }

    expect_token(TOK_BEGIN);
    prog->program.body = parse_block(&prog->program.num_stmts);
    expect_token(TOK_END);

    return prog;
}

// ============================================================================
// INTERPRETER
// ============================================================================
static RuntimeValue evaluate(ASTNode *expr, Environment *env)
{
    RuntimeValue result;
    memset(&result, 0, sizeof(RuntimeValue));
    result.type = VAL_NONE;

    if (!expr)
        return result;

    switch (expr->type)
    {
    case AST_LITERAL:
        // ΠΑΝΤΑ επιστρέφουμε αντίγραφο για να αποφύγουμε double-free με το AST
        return copy_runtime_value(&expr->literal.value);

    case AST_IDENTIFIER:
    {
        RuntimeValue *val = env_get(env, expr->identifier.name);
        return copy_runtime_value(val);
    }

    case AST_BINARY_OP:
    {
        RuntimeValue left = evaluate(expr->binary.left, env);
        RuntimeValue right = evaluate(expr->binary.right, env);

        result.type = VAL_INT;

        if (strcmp(expr->binary.operator, "+") == 0)
        {
            if (left.type == VAL_REAL || right.type == VAL_REAL)
            {
                result.type = VAL_REAL;
                result.value.real_val = (left.type == VAL_REAL ? left.value.real_val : left.value.int_val) +
                                        (right.type == VAL_REAL ? right.value.real_val : right.value.int_val);
            }
            else
            {
                result.value.int_val = left.value.int_val + right.value.int_val;
            }
        }
        else if (strcmp(expr->binary.operator, "-") == 0)
        {
            if (left.type == VAL_REAL || right.type == VAL_REAL)
            {
                result.type = VAL_REAL;
                result.value.real_val = (left.type == VAL_REAL ? left.value.real_val : left.value.int_val) -
                                        (right.type == VAL_REAL ? right.value.real_val : right.value.int_val);
            }
            else
            {
                result.value.int_val = left.value.int_val - right.value.int_val;
            }
        }
        else if (strcmp(expr->binary.operator, "*") == 0)
        {
            if (left.type == VAL_REAL || right.type == VAL_REAL)
            {
                result.type = VAL_REAL;
                result.value.real_val = (left.type == VAL_REAL ? left.value.real_val : left.value.int_val) *
                                        (right.type == VAL_REAL ? right.value.real_val : right.value.int_val);
            }
            else
            {
                result.value.int_val = left.value.int_val * right.value.int_val;
            }
        }
        else if (strcmp(expr->binary.operator, "/") == 0)
        {
            result.type = VAL_REAL;
            double l = left.type == VAL_REAL ? left.value.real_val : left.value.int_val;
            double r = right.type == VAL_REAL ? right.value.real_val : right.value.int_val;
            if (r == 0)
            {
                fprintf(stderr, "Runtime Error: Division by zero\n");
                exit(1);
            }
            result.value.real_val = l / r;
        }
        else if (strcmp(expr->binary.operator, "DIV") == 0)
        {
            int r = to_int(&right);
            if (r == 0)
            {
                fprintf(stderr, "Runtime Error: Division by zero\n");
                exit(1);
            }
            result.value.int_val = to_int(&left) / r;
        }
        else if (strcmp(expr->binary.operator, "MOD") == 0)
        {
            int r = to_int(&right);
            if (r == 0)
            {
                fprintf(stderr, "Runtime Error: Modulo by zero\n");
                exit(1);
            }
            result.value.int_val = to_int(&left) % r;
        }
        else if (strcmp(expr->binary.operator, "=") == 0)
        {
            result.type = VAL_BOOL;
            double l = left.type == VAL_REAL ? left.value.real_val : (left.type == VAL_INT ? left.value.int_val : 0);
            double r = right.type == VAL_REAL ? right.value.real_val : (right.type == VAL_INT ? right.value.int_val : 0);
            result.value.bool_val = (l == r);
        }
        else if (strcmp(expr->binary.operator, "<>") == 0)
        {
            result.type = VAL_BOOL;
            double l = left.type == VAL_REAL ? left.value.real_val : (left.type == VAL_INT ? left.value.int_val : 0);
            double r = right.type == VAL_REAL ? right.value.real_val : (right.type == VAL_INT ? right.value.int_val : 0);
            result.value.bool_val = (l != r);
        }
        else if (strcmp(expr->binary.operator, "<") == 0)
        {
            result.type = VAL_BOOL;
            double l = (left.type == VAL_REAL) ? left.value.real_val : (double)left.value.int_val;
            double r = (right.type == VAL_REAL) ? right.value.real_val : (double)right.value.int_val;
            result.value.bool_val = l < r;
        }
        else if (strcmp(expr->binary.operator, ">") == 0)
        {
            result.type = VAL_BOOL;
            double l = (left.type == VAL_REAL) ? left.value.real_val : (double)left.value.int_val;
            double r = (right.type == VAL_REAL) ? right.value.real_val : (double)right.value.int_val;
            result.value.bool_val = l > r;
        }
        else if (strcmp(expr->binary.operator, "<=") == 0)
        {
            result.type = VAL_BOOL;
            double l = (left.type == VAL_REAL) ? left.value.real_val : (double)left.value.int_val;
            double r = (right.type == VAL_REAL) ? right.value.real_val : (double)right.value.int_val;
            result.value.bool_val = l <= r;
        }
        else if (strcmp(expr->binary.operator, ">=") == 0)
        {
            result.type = VAL_BOOL;
            double l = (left.type == VAL_REAL) ? left.value.real_val : (double)left.value.int_val;
            double r = (right.type == VAL_REAL) ? right.value.real_val : (double)right.value.int_val;
            result.value.bool_val = l >= r;
        }
        else if (str_equals_ignore_case(expr->binary.operator, "AND") || str_equals_ignore_case(expr->binary.operator, "ΚΑΙ"))
        {
            result.type = VAL_BOOL;
            result.value.bool_val = to_bool(&left) && to_bool(&right);
        }
        else if (str_equals_ignore_case(expr->binary.operator, "OR") || str_equals_ignore_case(expr->binary.operator, "Ή"))
        {
            result.type = VAL_BOOL;
            result.value.bool_val = to_bool(&left) || to_bool(&right);
        }

        // Καθαρισμός προσωρινών τιμών
        free_runtime_value(&left);
        free_runtime_value(&right);
        return result;
    }

    case AST_UNARY_OP:
    {
        RuntimeValue operand = evaluate(expr->unary.operand, env);
        if (strcmp(expr->unary.operator, "-") == 0)
        {
            if (operand.type == VAL_REAL)
            {
                result.type = VAL_REAL;
                result.value.real_val = -operand.value.real_val;
            }
            else
            {
                result.type = VAL_INT;
                result.value.int_val = -operand.value.int_val;
            }
        }
        else if (str_equals_ignore_case(expr->unary.operator, "NOT") || str_equals_ignore_case(expr->unary.operator, "ΟΧΙ"))
        {

            result.type = VAL_BOOL;
            bool operand_bool = to_bool(&operand);
            result.value.bool_val = !operand_bool;
            result.value.bool_val = !to_bool(&operand);
        }

        free_runtime_value(&operand);
        return result;
    }

    case AST_ARRAY_ACCESS:
    {
        RuntimeValue *arr_val = env_get(env, expr->array_access.name);
        int indices[MAX_ARRAY_DIMS];
        for (int i = 0; i < expr->array_access.num_indices; i++)
        {
            RuntimeValue idx = evaluate(expr->array_access.indices[i], env);
            indices[i] = to_int(&idx);
            free_runtime_value(&idx);
        }
        // ΠΡΟΣΟΧΗ: Παίρνουμε την τιμή από τον πίνακα και επιστρέφουμε ΑΝΤΙΓΡΑΦΟ
        RuntimeValue val_in_array = array_get(arr_val->value.arr_val, indices, expr->array_access.num_indices);
        return copy_runtime_value(&val_in_array);
    }

    case AST_CALL:
    {

        ASTNode *function = env_get_subroutine(env, expr->call.name);

        if (!function || function->type != AST_FUNC_DECL)
        {
            fprintf(stderr, "Runtime Error: %s is not a function\n", expr->call.name);
            exit(1);
        }

        // Create environment with parent for global access
        Environment *func_env = create_environment(env);

        // Initialize local variables
        if (function->subroutine.local_decls)
        {
            for (int i = 0; i < function->subroutine.num_local_decls; i++)
            {
                ASTNode *decl = function->subroutine.local_decls[i];
                RuntimeValue val;
                val.type = VAL_INT;
                val.value.int_val = 0;
                env_define(func_env, decl->decl.name, val);
            }
        }

        // Bind parameters
        for (int i = 0; i < function->subroutine.num_params && i < expr->call.num_args; i++)
        {
            ASTNode *param = function->subroutine.parameters[i];
            ASTNode *arg = expr->call.arguments[i];

            // Handle array parameters
            if (arg->type == AST_IDENTIFIER)
            {
                RuntimeValue *potential_array = env_get(env, arg->identifier.name);
                if (potential_array && potential_array->type == VAL_ARRAY)
                { // <-- ΑΦΑΙΡΕΣΗ του && param->param.is_reference
                    RuntimeValue array_ref;
                    array_ref.type = VAL_ARRAY;
                    array_ref.value.arr_val = potential_array->value.arr_val;
                    env_define(func_env, param->param.name, array_ref);
                    continue;
                }
            }

            RuntimeValue arg_val = evaluate(arg, env);
            env_define(func_env, param->param.name, arg_val);
            free_runtime_value(&arg_val);
        }

        // Initialize return variable
        RuntimeValue ret_val;
        ret_val.type = VAL_REAL;
        ret_val.value.real_val = 0.0;
        env_define(func_env, function->subroutine.name, ret_val);

        // Execute function body
        for (int i = 0; i < function->subroutine.num_stmts; i++)
        {
            execute_statement(function->subroutine.body[i], func_env);
        }

        // Copy back reference parameters
        for (int i = 0; i < function->subroutine.num_params && i < expr->call.num_args; i++)
        {
            ASTNode *param = function->subroutine.parameters[i];
            ASTNode *arg = expr->call.arguments[i];

            if (param->param.is_reference)
            {
                RuntimeValue *func_val = env_get(func_env, param->param.name);

                if (func_val->type == VAL_ARRAY)
                    continue;

                if (arg->type == AST_IDENTIFIER)
                {
                    env_assign(env, arg->identifier.name, *func_val);
                }
            }
        }

        // Get return value
        RuntimeValue *return_val = env_get(func_env, function->subroutine.name);
        RuntimeValue fn_result = copy_runtime_value(return_val);

        return fn_result;
    }

    default:
        result.type = VAL_INT;
        result.value.int_val = 0;
        return result;
    }
}

static void execute_statement(ASTNode *stmt, Environment *env)
{
    switch (stmt->type)
    {

    case AST_ASSIGN:
    {
        RuntimeValue val = evaluate(stmt->assign.value, env);

        if (stmt->assign.num_indices > 0)
        {
            // Ανάθεση σε πίνακα: A[indices] := value
            RuntimeValue *arr_runtime = env_get(env, stmt->assign.identifier);
            if (arr_runtime && arr_runtime->type == VAL_ARRAY)
            {
                int indices[MAX_ARRAY_DIMS];
                for (int i = 0; i < stmt->assign.num_indices; i++)
                {
                    RuntimeValue idx_val = evaluate(stmt->assign.indices[i], env);
                    indices[i] = to_int(&idx_val);
                    free_runtime_value(&idx_val);
                }
                // Κλήση της συνάρτησης που φτιάξαμε
                array_set(arr_runtime->value.arr_val, indices, stmt->assign.num_indices, val);
            }
        }
        else
        {
            // Απλή μεταβλητή
            env_assign(env, stmt->assign.identifier, val);
        }
        free_runtime_value(&val);
        break;
    }

    case AST_PRINT:
    {
        for (int i = 0; i < stmt->print.num_exprs; i++)
        {
            RuntimeValue val = evaluate(stmt->print.expressions[i], env);

            // Check for EOLN
            if (val.type == VAL_STRING && strcmp(val.value.str_val, "__EOLN__") == 0)
            {
                printf("\n");
            }
            else
            {
                if (i > 0)
                    printf(" ");

                switch (val.type)
                {
                case VAL_INT:
                    printf("%d", val.value.int_val);
                    break;
                case VAL_REAL:
                    printf("%g", val.value.real_val);
                    break;
                case VAL_BOOL:
                    printf("%s", val.value.bool_val ? "TRUE" : "FALSE");
                    break;
                case VAL_STRING:
                    printf("%s", val.value.str_val);
                    break;
                default:
                    break;
                }
            }

            free_runtime_value(&val);
        }
        break;
    }

    case AST_READ:
    {
        for (int i = 0; i < stmt->read.num_vars; i++)
        {
            ASTNode *var = stmt->read.variables[i];
            char input[256];

            fflush(stdout);

            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                // EOF handling
                break;
            }

            input[strcspn(input, "\n")] = 0;

            // Δημιουργία RuntimeValue από το input
            RuntimeValue val;
            memset(&val, 0, sizeof(RuntimeValue));

            if (strlen(input) == 0)
            {
                val.type = VAL_INT;
                val.value.int_val = -1;
            }
            else if (strchr(input, '.'))
            {
                val.type = VAL_REAL;
                val.value.real_val = atof(input);
            }
            else if (isdigit(input[0]) || input[0] == '-')
            {
                val.type = VAL_INT;
                val.value.int_val = atoi(input);
            }
            else
            {
                val.type = VAL_STRING;
                val.value.str_val = strdup(input);
            }

            // ΝΕΟ: Χειρισμός array access
            if (var->type == AST_ARRAY_ACCESS)
            {
                // Βρες τον πίνακα
                RuntimeValue *arr_val = env_get(env, var->array_access.name);

                if (arr_val && arr_val->type == VAL_ARRAY)
                {
                    // Υπολόγισε τους δείκτες
                    int indices[MAX_ARRAY_DIMS];
                    for (int j = 0; j < var->array_access.num_indices; j++)
                    {
                        RuntimeValue idx = evaluate(var->array_access.indices[j], env);
                        indices[j] = to_int(&idx);
                        free_runtime_value(&idx);
                    }

                    // ΚΡΙΣΙΜΟ: Αποθήκευσε την τιμή στον πίνακα
                    array_set(arr_val->value.arr_val, indices, var->array_access.num_indices, val);

                    debug_log("READ: Set %s[%d] = %d", var->array_access.name, indices[0],
                              val.type == VAL_INT ? val.value.int_val : 0);
                }
            }
            else if (var->type == AST_IDENTIFIER)
            {
                // Απλή μεταβλητή
                env_assign(env, var->identifier.name, val);
            }

            if (val.type == VAL_STRING)
            {
                free_runtime_value(&val);
            }
        }
        break;
    }

    case AST_IF:
    {
        RuntimeValue cond = evaluate(stmt->if_stmt.condition, env);

        if (to_bool(&cond))
        {
            for (int i = 0; i < stmt->if_stmt.num_then; i++)
            {
                execute_statement(stmt->if_stmt.then_branch[i], env);
            }
        }
        else if (stmt->if_stmt.else_branch)
        {
            for (int i = 0; i < stmt->if_stmt.num_else; i++)
            {
                execute_statement(stmt->if_stmt.else_branch[i], env);
            }
        }

        free_runtime_value(&cond);
        break;
    }

    case AST_FOR:
    {
        fflush(stdout);

        RuntimeValue start_val = evaluate(stmt->for_loop.start, env);
        RuntimeValue end_val = evaluate(stmt->for_loop.end, env);
        RuntimeValue step_val = evaluate(stmt->for_loop.step, env);

        int start = to_int(&start_val);
        int end = to_int(&end_val);
        int step = to_int(&step_val);

        fflush(stdout);

        if (start_val.type != VAL_ARRAY)
            free_runtime_value(&start_val);
        if (end_val.type != VAL_ARRAY)
            free_runtime_value(&end_val);
        if (step_val.type != VAL_ARRAY)
            free_runtime_value(&step_val);

        fflush(stdout);

        int current = start;

        if (step > 0)
        {
            while (current <= end)
            {
                fflush(stdout);

                RuntimeValue loop_var;
                loop_var.type = VAL_INT;
                loop_var.value.int_val = current;

                fflush(stdout);

                env_assign(env, stmt->for_loop.variable, loop_var);

                fflush(stdout);

                for (int i = 0; i < stmt->for_loop.num_stmts; i++)
                {
                    execute_statement(stmt->for_loop.body[i], env);
                }

                fflush(stdout);
                current += step;
            }
        }
        else
        {
            while (current >= end)
            {
                RuntimeValue loop_var;
                loop_var.type = VAL_INT;
                loop_var.value.int_val = current;
                env_assign(env, stmt->for_loop.variable, loop_var);

                for (int i = 0; i < stmt->for_loop.num_stmts; i++)
                {
                    execute_statement(stmt->for_loop.body[i], env);
                }

                current += step;
            }
        }

        fflush(stdout);
        break;
    }

    case AST_WHILE:
    {
        if (stmt->while_loop.is_repeat_until)
        {
            // REPEAT-UNTIL: Execute body FIRST, then check to STOP when TRUE
            do
            {
                for (int i = 0; i < stmt->while_loop.num_stmts; i++)
                {
                    execute_statement(stmt->while_loop.body[i], env);
                }

                RuntimeValue cond = evaluate(stmt->while_loop.condition, env);
                bool should_stop = to_bool(&cond);
                free_runtime_value(&cond);

                if (should_stop)
                    break; // Stop when UNTIL condition is TRUE
            } while (true);
        }
        else
        {
            // WHILE: Check condition FIRST, continue while TRUE
            while (true)
            {
                RuntimeValue cond = evaluate(stmt->while_loop.condition, env);
                bool should_continue = to_bool(&cond);
                free_runtime_value(&cond);

                if (!should_continue)
                    break; // Stop when WHILE condition is FALSE

                for (int i = 0; i < stmt->while_loop.num_stmts; i++)
                {
                    execute_statement(stmt->while_loop.body[i], env);
                }
            }
        }
        break;
    }

    case AST_CALL:
    {

        ASTNode *subroutine = env_get_subroutine(env, stmt->call.name);

        if (!subroutine)
        {
            fprintf(stderr, "Runtime Error: Undefined subroutine: %s\n", stmt->call.name);
            exit(1);
        }

        // Create new environment for subroutine
        Environment *sub_env = create_environment(env);

        // Initialize local variables from DATA section
        if (subroutine->subroutine.local_decls)
        {
            for (int i = 0; i < subroutine->subroutine.num_local_decls; i++)
            {
                ASTNode *decl = subroutine->subroutine.local_decls[i];
                RuntimeValue val;
                val.type = VAL_INT;
                val.value.int_val = 0;
                env_define(sub_env, decl->decl.name, val);
            }
        }

        for (int i = 0; i < subroutine->subroutine.num_params && i < stmt->call.num_args; i++)
        {
            ASTNode *param = subroutine->subroutine.parameters[i];
            ASTNode *arg = stmt->call.arguments[i];

            // Special handling for arrays passed by reference
            if (arg->type == AST_IDENTIFIER)
            {
                RuntimeValue *potential_array = env_get(env, arg->identifier.name);
                if (potential_array && potential_array->type == VAL_ARRAY)
                {
                    // Pass the array reference directly (shallow copy for arrays)
                    RuntimeValue array_ref;
                    array_ref.type = VAL_ARRAY;
                    array_ref.value.arr_val = potential_array->value.arr_val;
                    env_define(sub_env, param->param.name, array_ref);
                    continue;
                }
            }

            // Regular parameter handling
            if (param->param.is_reference)
            {
                // Pass by reference - evaluate and bind
                RuntimeValue arg_val = evaluate(arg, env);
                env_define(sub_env, param->param.name, arg_val);
                // Don't free here - we need to copy back later
            }
            else
            {
                // Pass by value
                RuntimeValue arg_val = evaluate(arg, env);
                env_define(sub_env, param->param.name, arg_val);
                free_runtime_value(&arg_val);
            }
        }

        // Execute subroutine body
        for (int i = 0; i < subroutine->subroutine.num_stmts; i++)
        {
            execute_statement(subroutine->subroutine.body[i], sub_env);
        }

        // Copy back reference parameters (but NOT arrays - they're already shared)
        for (int i = 0; i < subroutine->subroutine.num_params && i < stmt->call.num_args; i++)
        {
            ASTNode *param = subroutine->subroutine.parameters[i];
            ASTNode *arg = stmt->call.arguments[i];

            if (param->param.is_reference)
            {
                // Get the value from subroutine environment
                RuntimeValue *sub_val = env_get(sub_env, param->param.name);

                // Skip arrays - they're already shared by reference
                if (sub_val->type == VAL_ARRAY)
                {
                    continue;
                }

                // Copy back simple variables
                if (arg->type == AST_IDENTIFIER)
                {
                    env_assign(env, arg->identifier.name, *sub_val);
                }
                else if (arg->type == AST_ARRAY_ACCESS)
                {
                    // Array element - assign back
                    RuntimeValue *arr_val = env_get(env, arg->array_access.name);
                    if (arr_val->type == VAL_ARRAY)
                    {
                        int indices[MAX_ARRAY_DIMS];
                        for (int j = 0; j < arg->array_access.num_indices; j++)
                        {
                            RuntimeValue idx = evaluate(arg->array_access.indices[j], env);
                            indices[j] = to_int(&idx);
                            free_runtime_value(&idx);
                        }
                        array_set(arr_val->value.arr_val, indices, arg->array_access.num_indices, *sub_val);
                    }
                }
            }
        }

        break;
    }

    default:
        fprintf(stderr, "Runtime Error: Unknown statement type\n");
        exit(1);
    }
}

static void execute_program(ASTNode *prog)
{
    Environment *env = create_environment(NULL);

    // Define EOLN constant - use a static string to avoid malloc
    RuntimeValue eoln;
    eoln.type = VAL_STRING;
    eoln.value.str_val = "__EOLN__"; // Static string, not malloc'd
    env_define(env, "EOLN", eoln);

    // Phase 1: Define constants and subroutines
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];

        if (decl->type == AST_CONST_DECL)
        {
            RuntimeValue val = evaluate(decl->decl.value, env);
            env_define(env, decl->decl.name, val);
            debug_log("Defined constant: %s", decl->decl.name);
        }
        else if (decl->type == AST_FUNC_DECL || decl->type == AST_PROC_DECL)
        {
            env_define_subroutine(env, decl->subroutine.name, decl);
            debug_log("Defined subroutine: %s", decl->subroutine.name);
        }
    }

    // Phase 2: Define variables
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];

        if (decl->type == AST_VAR_DECL)
        {
            RuntimeValue val;

            if (decl->decl.num_arr_dims > 0)
            {
                // Array variable - evaluate bounds
                ArrayBound bounds[MAX_ARRAY_DIMS];
                for (int j = 0; j < decl->decl.num_arr_dims; j++)
                {
                    RuntimeValue start_val = evaluate(decl->decl.arr_bound_exprs[j].start_expr, env);
                    RuntimeValue end_val = evaluate(decl->decl.arr_bound_exprs[j].end_expr, env);
                    bounds[j].from = to_int(&start_val);
                    bounds[j].to = to_int(&end_val);
                    free_runtime_value(&start_val);
                    free_runtime_value(&end_val);
                }

                val.type = VAL_ARRAY;
                val.value.arr_val = create_array(bounds, decl->decl.num_arr_dims);
                debug_log("Declared array: %s", decl->decl.name);
            }
            else
            {
                // Simple variable
                val.type = VAL_INT;
                val.value.int_val = 0;
                debug_log("Declared variable: %s", decl->decl.name);
            }

            env_define(env, decl->decl.name, val);
        }
    }

    // Execute main body
    for (int i = 0; i < prog->program.num_stmts; i++)
    {
        execute_statement(prog->program.body[i], env);
    }
}

// ============================================================================
// MAIN
// ============================================================================

// Helper function for token names
// ============================================================================
static const char *token_type_name(TokenType type)
{
    switch (type)
    {
    case TOK_ALGORITHM:
        return "ALGORITHM";
    case TOK_CONSTANTS:
        return "CONSTANTS";
    case TOK_DATA:
        return "DATA";
    case TOK_BEGIN:
        return "BEGIN";
    case TOK_END:
        return "END";
    case TOK_PROCEDURE:
        return "PROCEDURE";
    case TOK_FUNCTION:
        return "FUNCTION";
    case TOK_INTERFACE:
        return "INTERFACE";
    case TOK_END_FUNCTION:
        return "END_FUNCTION";
    case TOK_END_PROCEDURE:
        return "END_PROCEDURE";
    case TOK_IF:
        return "IF";
    case TOK_THEN:
        return "THEN";
    case TOK_ELSE:
        return "ELSE";
    case TOK_END_IF:
        return "END_IF";
    case TOK_FOR:
        return "FOR";
    case TOK_TO:
        return "TO";
    case TOK_STEP:
        return "STEP";
    case TOK_REPEAT:
        return "REPEAT/ΕΠΑΝΑΛΑΒΕ";
    case TOK_END_FOR:
        return "END_FOR";
    case TOK_WHILE:
        return "WHILE/ΕΝΟΣΩ";
    case TOK_END_WHILE:
        return "END_WHILE/ΕΝΟΣΩ-ΤΕΛΟΣ";
    case TOK_UNTIL:
        return "UNTIL/ΜΕΧΡΙ";
    case TOK_PRINT:
        return "PRINT/ΤΥΠΩΣΕ";
    case TOK_READ:
        return "READ/ΔΙΑΒΑΣΕ";
    case TOK_SEMICOLON:
        return "SEMICOLON ;";
    case TOK_IDENTIFIER:
        return "IDENTIFIER";
    case TOK_INTEGER_TYPE:
        return "INTEGER_TYPE";
    case TOK_LEFT_PAREN:
        return "LEFT_PAREN (";
    case TOK_RIGHT_PAREN:
        return "RIGHT_PAREN )";
    case TOK_LEFT_BRACKET:
        return "LEFT_BRACKET [";
    case TOK_RIGHT_BRACKET:
        return "RIGHT_BRACKET ]";
    case TOK_ASSIGN:
        return "ASSIGN :=";
    case TOK_COLON:
        return "COLON :";
    case TOK_COMMA:
        return "COMMA ,";
    default:
        return "OTHER";
    }
}

// Windows-1253 (Greek) to UTF-8 conversion table for Greek characters
static void convert_windows1253_to_utf8(char *content, size_t size)
{
    // Mapping table for Windows-1253 Greek characters to UTF-8
    // This covers the Greek character range (0x80-0xFF in Windows-1253)
    static const struct
    {
        unsigned char win1253;
        const char *utf8;
    } greek_map[] = {
        // Greek uppercase letters
        {0xC1, "Α"},
        {0xC2, "Β"},
        {0xC3, "Γ"},
        {0xC4, "Δ"},
        {0xC5, "Ε"},
        {0xC6, "Ζ"},
        {0xC7, "Η"},
        {0xC8, "Θ"},
        {0xC9, "Ι"},
        {0xCA, "Κ"},
        {0xCB, "Λ"},
        {0xCC, "Μ"},
        {0xCD, "Ν"},
        {0xCE, "Ξ"},
        {0xCF, "Ο"},
        {0xD0, "Π"},
        {0xD1, "Ρ"},
        {0xD3, "Σ"},
        {0xD4, "Τ"},
        {0xD5, "Υ"},
        {0xD6, "Φ"},
        {0xD7, "Χ"},
        {0xD8, "Ψ"},
        {0xD9, "Ω"},

        // Greek lowercase letters
        {0xE1, "α"},
        {0xE2, "β"},
        {0xE3, "γ"},
        {0xE4, "δ"},
        {0xE5, "ε"},
        {0xE6, "ζ"},
        {0xE7, "η"},
        {0xE8, "θ"},
        {0xE9, "ι"},
        {0xEA, "κ"},
        {0xEB, "λ"},
        {0xEC, "μ"},
        {0xED, "ν"},
        {0xEE, "ξ"},
        {0xEF, "ο"},
        {0xF0, "π"},
        {0xF1, "ρ"},
        {0xF2, "ς"},
        {0xF3, "σ"},
        {0xF4, "τ"},
        {0xF5, "υ"},
        {0xF6, "φ"},
        {0xF7, "χ"},
        {0xF8, "ψ"},
        {0xF9, "ω"},

        // Greek uppercase with tonos (accented)
        {0xA1, "Ά"},
        {0xA2, "Έ"},
        {0xA3, "Ή"},
        {0xA4, "Ί"},
        {0xA5, "Ό"},
        {0xA6, "Ύ"},
        {0xA7, "Ώ"},

        // Greek lowercase with tonos (accented)
        {0xDC, "ά"},
        {0xDD, "έ"},
        {0xDE, "ή"},
        {0xDF, "ί"},
        {0xFC, "ό"},
        {0xFD, "ύ"},
        {0xFE, "ώ"},

        // Greek lowercase with dialytika
        {0xCA, "ϊ"},
        {0xCB, "ϋ"},

        {0, NULL} // Terminator
    };

    // Calculate new size (UTF-8 can be up to 3 bytes per Greek char)
    size_t new_size = 0;
    for (size_t i = 0; i < size; i++)
    {
        unsigned char c = (unsigned char)content[i];
        if (c >= 0x80)
        {
            // Find in map
            int found = 0;
            for (int j = 0; greek_map[j].utf8 != NULL; j++)
            {
                if (greek_map[j].win1253 == c)
                {
                    new_size += strlen(greek_map[j].utf8);
                    found = 1;
                    break;
                }
            }
            if (!found)
                new_size++; // Keep as-is if not in map
        }
        else
        {
            new_size++;
        }
    }

    // Allocate new buffer
    char *new_content = malloc(new_size + 1);
    if (!new_content)
        return;

    // Convert
    size_t out_pos = 0;
    for (size_t i = 0; i < size; i++)
    {
        unsigned char c = (unsigned char)content[i];
        if (c >= 0x80)
        {
            // Find in map
            int found = 0;
            for (int j = 0; greek_map[j].utf8 != NULL; j++)
            {
                if (greek_map[j].win1253 == c)
                {
                    const char *utf8 = greek_map[j].utf8;
                    while (*utf8)
                    {
                        new_content[out_pos++] = *utf8++;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                new_content[out_pos++] = c; // Keep as-is
            }
        }
        else
        {
            new_content[out_pos++] = c;
        }
    }
    new_content[out_pos] = '\0';

    // Copy back to original buffer
    if (out_pos <= size)
    {
        memcpy(content, new_content, out_pos + 1);
    }
    else
    {
        // Need to inform caller about size change
        // For now, just copy what fits
        memcpy(content, new_content, size);
        content[size] = '\0';
    }

    free(new_content);
}

// Better version that returns new buffer
static char *convert_windows1253_to_utf8_new(const char *content, size_t size)
{
    // Mapping table
    static const struct
    {
        unsigned char win1253;
        const char *utf8;
    } greek_map[] = {
        {0xC1, "Α"}, {0xC2, "Β"}, {0xC3, "Γ"}, {0xC4, "Δ"}, {0xC5, "Ε"}, {0xC6, "Ζ"}, {0xC7, "Η"}, {0xC8, "Θ"}, {0xC9, "Ι"}, {0xCA, "Κ"}, {0xCB, "Λ"}, {0xCC, "Μ"}, {0xCD, "Ν"}, {0xCE, "Ξ"}, {0xCF, "Ο"}, {0xD0, "Π"}, {0xD1, "Ρ"}, {0xD3, "Σ"}, {0xD4, "Τ"}, {0xD5, "Υ"}, {0xD6, "Φ"}, {0xD7, "Χ"}, {0xD8, "Ψ"}, {0xD9, "Ω"}, {0xE1, "α"}, {0xE2, "β"}, {0xE3, "γ"}, {0xE4, "δ"}, {0xE5, "ε"}, {0xE6, "ζ"}, {0xE7, "η"}, {0xE8, "θ"}, {0xE9, "ι"}, {0xEA, "κ"}, {0xEB, "λ"}, {0xEC, "μ"}, {0xED, "ν"}, {0xEE, "ξ"}, {0xEF, "ο"}, {0xF0, "π"}, {0xF1, "ρ"}, {0xF2, "ς"}, {0xF3, "σ"}, {0xF4, "τ"}, {0xF5, "υ"}, {0xF6, "φ"}, {0xF7, "χ"}, {0xF8, "ψ"}, {0xF9, "ω"}, {0xDC, "ά"}, {0xDD, "έ"}, {0xDE, "ή"}, {0xDF, "ί"}, {0xFC, "ό"}, {0xFD, "ύ"}, {0xFE, "ώ"}, {0xA1, "Ά"}, {0xA2, "Έ"}, {0xA3, "Ή"}, {0xA4, "Ί"}, {0xA5, "Ό"}, {0xA6, "Ύ"}, {0xA7, "Ώ"}, {0, NULL}};

    // Calculate required size
    size_t new_size = 0;
    for (size_t i = 0; i < size; i++)
    {
        unsigned char c = (unsigned char)content[i];
        if (c >= 0x80)
        {
            int found = 0;
            for (int j = 0; greek_map[j].utf8 != NULL; j++)
            {
                if (greek_map[j].win1253 == c)
                {
                    new_size += strlen(greek_map[j].utf8);
                    found = 1;
                    break;
                }
            }
            if (!found)
                new_size++;
        }
        else
        {
            new_size++;
        }
    }

    // Allocate and convert
    char *new_content = malloc(new_size + 1);
    if (!new_content)
        return NULL;

    size_t out_pos = 0;
    for (size_t i = 0; i < size; i++)
    {
        unsigned char c = (unsigned char)content[i];
        if (c >= 0x80)
        {
            int found = 0;
            for (int j = 0; greek_map[j].utf8 != NULL; j++)
            {
                if (greek_map[j].win1253 == c)
                {
                    const char *utf8 = greek_map[j].utf8;
                    while (*utf8)
                    {
                        new_content[out_pos++] = *utf8++;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found)
                new_content[out_pos++] = c;
        }
        else
        {
            new_content[out_pos++] = c;
        }
    }
    new_content[out_pos] = '\0';

    return new_content;
}

bool is_valid_utf8(const char *str, size_t len)
{
    const unsigned char *bytes = (const unsigned char *)str;
    for (size_t i = 0; i < len;)
    {
        if (bytes[i] <= 0x7F)
            i++;
        else if (bytes[i] >= 0xC2 && bytes[i] <= 0xDF && i + 1 < len && bytes[i + 1] >= 0x80 && bytes[i + 1] <= 0xBF)
            i += 2;
        else if (bytes[i] == 0xE0 && i + 2 < len && bytes[i + 1] >= 0xA0 && bytes[i + 1] <= 0xBF && bytes[i + 2] >= 0x80 && bytes[i + 2] <= 0xBF)
            i += 3;
        else if (bytes[i] >= 0xE1 && bytes[i] <= 0xEF && i + 2 < len && bytes[i + 1] >= 0x80 && bytes[i + 1] <= 0xBF && bytes[i + 2] >= 0x80 && bytes[i + 2] <= 0xBF)
            i += 3;
        else if (bytes[i] == 0xF0 && i + 3 < len && bytes[i + 1] >= 0x90 && bytes[i + 1] <= 0xBF && bytes[i + 2] >= 0x80 && bytes[i + 2] <= 0xBF && bytes[i + 3] >= 0x80 && bytes[i + 3] <= 0xBF)
            i += 4;
        else
            return false;
    }
    return true;
}
static char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);

    // --- ΑΡΧΗ ΔΙΟΡΘΩΣΗΣ ---

    // 1. Ορίζουμε τη λέξη "ΑΛΓΟΡΙΘΜΟΣ" σε UTF-8 format (hex bytes)
    const unsigned char utf8_algo[] = {0xCE, 0x91, 0xCE, 0x9B, 0xCE, 0x93, 0xCE, 0x9F, 0xCE, 0xA1, 0xCE, 0x99, 0xCE, 0x98, 0xCE, 0x9C, 0xCE, 0x9F, 0xCE, 0xA3, 0};

    // 2. Ελέγχουμε αν το αρχείο περιέχει ήδη τη λέξη ΑΛΓΟΡΙΘΜΟΣ σε UTF-8
    bool is_already_utf8 = (strstr(content, (const char *)utf8_algo) != NULL);

    // 3. Simple heuristic για Windows-1253
    int has_greek_win1253 = 0;
    for (long i = 0; i < size; i++)
    {
        unsigned char c = (unsigned char)content[i];
        if ((c >= 0xC1 && c <= 0xD9) || (c >= 0xE1 && c <= 0xF9))
        {
            has_greek_win1253 = 1;
            break;
        }
    }

    // ΜΕΤΑΤΡΟΠΗ: Μόνο αν βρήκαμε bytes της Win-1253 ΚΑΙ ΔΕΝ βρήκαμε το UTF-8 anchor
    if (has_greek_win1253 && !is_already_utf8)
    {
        if (debug_mode)
        {
            fprintf(stderr, "[DEBUG] Detected Windows-1253 (UTF-8 anchor not found), converting...\n");
        }
        char *utf8_content = convert_windows1253_to_utf8_new(content, size);
        free(content);
        return utf8_content;
    }

    // --- ΤΕΛΟΣ ΔΙΟΡΘΩΣΗΣ ---

    return content;
}
// ============================================================================
// CODE GENERATOR IMPLEMENTATION (Missing Helpers)
// ============================================================================

char *sanitize_identifier(const char *name)
{
    if (!name)
        return NULL;
    static char buffer[MAX_TOKEN_LEN];
    int j = 0;
    for (int i = 0; name[i] && j < MAX_TOKEN_LEN - 1; i++)
    {
        unsigned char c = (unsigned char)name[i];
        // C identifiers can only have alphanumeric and underscores
        // Replace Greek characters or other special characters with underscores
        // or a safe mapping. For simplicity, we convert non-standard chars to '_'
        if (isalnum(c) || c == '_')
        {
            buffer[j++] = c;
        }
        else
        {
            // Check for common Greek characters if you want more readable C code,
            // otherwise replace with underscore:
            buffer[j++] = '_';
        }
    }
    buffer[j] = '\0';
    return buffer;
}
const char *map_type(const char *eap_type)
{
    if (!eap_type)
        return "int";

    // Use a more robust check for Greek characters
    if (str_equals_ignore_case(eap_type, "ΑΚΕΡΑΙΟΣ") ||
        str_equals_ignore_case(eap_type, "INTEGER"))
    {
        return "int";
    }
    if (str_equals_ignore_case(eap_type, "ΠΡΑΓΜΑΤΙΚΟΣ") ||
        str_equals_ignore_case(eap_type, "REAL"))
    {
        return "double";
    }
    if (str_equals_ignore_case(eap_type, "ΛΟΓΙΚΟΣ") ||
        str_equals_ignore_case(eap_type, "BOOLEAN"))
    {
        return "bool";
    }
    if (str_equals_ignore_case(eap_type, "ΧΑΡΑΚΤΗΡΑΣ") ||
        str_equals_ignore_case(eap_type, "CHAR"))
    {
        return "char";
    }

    // Default to int instead of void to avoid "void sum;" errors
    return "int";
}

// ============================================================================
// CODEGEN HELPER FUNCTIONS
// ============================================================================

// Register array bounds
void codegen_register_array(CodeGenerator *gen, const char *name, int start, int end, int dimension)
{
    // Find existing array or create new
    ArrayBoundsInfo *info = NULL;
    for (int i = 0; i < gen->num_arrays; i++)
    {
        if (strcmp(gen->array_bounds[i].array_name, name) == 0)
        {
            info = &gen->array_bounds[i];
            break;
        }
    }

    if (!info && gen->num_arrays < 100)
    {
        info = &gen->array_bounds[gen->num_arrays++];
        info->array_name = strdup(name);
        info->num_dims = 0;
    }

    if (info && dimension < MAX_ARRAY_DIMS)
    {
        info->bounds_start[dimension] = start;
        info->bounds_end[dimension] = end;
        if (dimension >= info->num_dims)
        {
            info->num_dims = dimension + 1;
        }
    }
}

// Get array offset for dimension
int codegen_get_array_offset(CodeGenerator *gen, const char *name, int dimension)
{
    for (int i = 0; i < gen->num_arrays; i++)
    {
        if (strcmp(gen->array_bounds[i].array_name, name) == 0)
        {
            if (dimension < gen->array_bounds[i].num_dims)
            {
                return gen->array_bounds[i].bounds_start[dimension];
            }
        }
    }
    return 1; // Default to 1-based
}

// Get array size for dimension
int codegen_get_array_size(CodeGenerator *gen, const char *name, int dimension)
{
    for (int i = 0; i < gen->num_arrays; i++)
    {
        if (strcmp(gen->array_bounds[i].array_name, name) == 0)
        {
            if (dimension < gen->array_bounds[i].num_dims)
            {
                return gen->array_bounds[i].bounds_end[dimension] -
                       gen->array_bounds[i].bounds_start[dimension] + 1;
            }
        }
    }
    return 100; // Default
}

// Register variable type
void codegen_register_var_type(CodeGenerator *gen, const char *name, const char *type)
{
    if (gen->num_vars >= 1000)
        return;

    // Check if already exists
    for (int i = 0; i < gen->num_vars; i++)
    {
        if (strcmp(gen->var_types[i].var_name, name) == 0)
        {
            free(gen->var_types[i].type_name);
            gen->var_types[i].type_name = strdup(type);
            return;
        }
    }

    VarTypeInfo *info = &gen->var_types[gen->num_vars++];
    info->var_name = strdup(name);
    info->type_name = strdup(type);
}

// Get variable type
const char *codegen_get_var_type(CodeGenerator *gen, const char *name)
{
    for (int i = 0; i < gen->num_vars; i++)
    {
        if (strcmp(gen->var_types[i].var_name, name) == 0)
        {
            return gen->var_types[i].type_name;
        }
    }
    return "int";
}

// Infer printf format specifier
const char *codegen_infer_printf_format(CodeGenerator *gen, ASTNode *expr)
{
    if (!expr)
        return "%d";

    switch (expr->type)
    {
    case AST_LITERAL:
        switch (expr->literal.value.type)
        {
        case VAL_INT:
            return "%d";
        case VAL_REAL:
            return "%g";
        case VAL_BOOL:
            return "%s";
        case VAL_STRING:
            return "%s";
        default:
            return "%d";
        }

    case AST_IDENTIFIER:
    {
        if (str_equals_ignore_case(expr->identifier.name, "EOLN"))
        {
            return "%c";
        }
        const char *type = codegen_get_var_type(gen, expr->identifier.name);
        if (strcmp(type, "double") == 0)
            return "%g";
        if (strcmp(type, "bool") == 0)
            return "%d";
        if (strcmp(type, "char*") == 0)
            return "%s";
        return "%d";
    }

    case AST_BINARY_OP:
    {
        if (strcmp(expr->binary.operator, "/") == 0)
            return "%g";

        const char *left_fmt = codegen_infer_printf_format(gen, expr->binary.left);
        const char *right_fmt = codegen_infer_printf_format(gen, expr->binary.right);

        if (strcmp(left_fmt, "%g") == 0 || strcmp(right_fmt, "%g") == 0)
        {
            return "%g";
        }
        return "%d";
    }

    default:
        return "%d";
    }
}

// Find subroutine by name
ASTNode *codegen_find_subroutine(CodeGenerator *gen, const char *name)
{
    if (!gen->program)
        return NULL;

    for (int i = 0; i < gen->program->program.num_decls; i++)
    {
        ASTNode *decl = gen->program->program.declarations[i];
        if ((decl->type == AST_FUNC_DECL || decl->type == AST_PROC_DECL) &&
            str_equals_ignore_case(decl->subroutine.name, name))
        {
            return decl;
        }
    }
    return NULL;
}

// Check if parameter is by reference
bool codegen_is_param_by_ref(ASTNode *subroutine, int param_index)
{
    if (!subroutine || param_index >= subroutine->subroutine.num_params)
    {
        return false;
    }
    return subroutine->subroutine.parameters[param_index]->param.is_reference;
}
void codegen_init(CodeGenerator *gen, FILE *out)
{
    gen->output = out;
    gen->indent_level = 0;
    gen->in_function = false;
    gen->current_function_name = NULL;
    gen->env = NULL;
    gen->program = NULL;
    gen->num_arrays = 0;
    gen->num_vars = 0;
}
void codegen_indent(CodeGenerator *gen)
{
    for (int i = 0; i < gen->indent_level; i++)
    {
        fprintf(gen->output, "    ");
    }
}

void codegen_line(CodeGenerator *gen, const char *fmt, ...)
{
    codegen_indent(gen);
    va_list args;
    va_start(args, fmt);
    vfprintf(gen->output, fmt, args);
    va_end(args);
    fprintf(gen->output, "\n");
}

void codegen_expression(CodeGenerator *gen, ASTNode *expr)
{
    if (!expr)
    {
        fprintf(gen->output, "0");
        return;
    }

    switch (expr->type)
    {
    case AST_LITERAL:
        switch (expr->literal.value.type)
        {
        case VAL_INT:
            fprintf(gen->output, "%d", expr->literal.value.value.int_val);
            break;
        case VAL_REAL:
            fprintf(gen->output, "%g", expr->literal.value.value.real_val);
            break;
        case VAL_BOOL:
            fprintf(gen->output, "%s", expr->literal.value.value.bool_val ? "true" : "false");
            break;
        case VAL_STRING:
            // Handle EOLN marker vs regular string
            if (strcmp(expr->literal.value.value.str_val, "__EOLN__") == 0 ||
                strcmp(expr->literal.value.value.str_val, "EOLN") == 0)
            {
                // If we are in a printf argument list, use '\n'
                fprintf(gen->output, "'\\n'");
            }
            else
            {
                fprintf(gen->output, "\"%s\"", expr->literal.value.value.str_val);
            }
            break;
        default:
            fprintf(gen->output, "0");
        }
        break;

    case AST_IDENTIFIER:
        if (str_equals_ignore_case(expr->identifier.name, "EOLN"))
        {
            fprintf(gen->output, "'\\n'");
        }
        else
        {
            fprintf(gen->output, "%s", sanitize_identifier(expr->identifier.name));
        }
        break;

    case AST_BINARY_OP:
        fprintf(gen->output, "(");
        codegen_expression(gen, expr->binary.left);

        // Map operators
        if (strcmp(expr->binary.operator, "=") == 0)
        {
            fprintf(gen->output, " == ");
        }
        else if (strcmp(expr->binary.operator, "<>") == 0)
        {
            fprintf(gen->output, " != ");
        }
        else if (str_equals_ignore_case(expr->binary.operator, "DIV"))
        {
            fprintf(gen->output, " / ");
        }
        else if (str_equals_ignore_case(expr->binary.operator, "MOD"))
        {
            fprintf(gen->output, " %% ");
        }
        else if (str_equals_ignore_case(expr->binary.operator, "AND") ||
                 str_equals_ignore_case(expr->binary.operator, "ΚΑΙ"))
        {
            fprintf(gen->output, " && ");
        }
        else if (str_equals_ignore_case(expr->binary.operator, "OR") ||
                 str_equals_ignore_case(expr->binary.operator, "Ή"))
        {
            fprintf(gen->output, " || ");
        }
        else
        {
            fprintf(gen->output, " %s ", expr->binary.operator);
        }

        codegen_expression(gen, expr->binary.right);
        fprintf(gen->output, ")");
        break;

    case AST_UNARY_OP:
        if (str_equals_ignore_case(expr->unary.operator, "NOT") ||
            str_equals_ignore_case(expr->unary.operator, "ΟΧΙ"))
        {
            fprintf(gen->output, "!(");
        }
        else
        {
            fprintf(gen->output, "%s(", expr->unary.operator);
        }
        codegen_expression(gen, expr->unary.operand);
        fprintf(gen->output, ")");
        break;

    case AST_ARRAY_ACCESS:
    {
        fprintf(gen->output, "%s[", sanitize_identifier(expr->array_access.name));
        for (int i = 0; i < expr->array_access.num_indices; i++)
        {
            if (i > 0)
                fprintf(gen->output, "][");

            int offset = codegen_get_array_offset(gen, expr->array_access.name, i);

            if (offset != 0)
            {
                fprintf(gen->output, "(");
                codegen_expression(gen, expr->array_access.indices[i]);
                fprintf(gen->output, " - %d)", offset);
            }
            else
            {
                codegen_expression(gen, expr->array_access.indices[i]);
            }
        }
        fprintf(gen->output, "]");
        break;
    }

    case AST_CALL:
        fprintf(gen->output, "%s(", sanitize_identifier(expr->call.name));
        for (int i = 0; i < expr->call.num_args; i++)
        {
            if (i > 0)
                fprintf(gen->output, ", ");
            codegen_expression(gen, expr->call.arguments[i]);
        }
        fprintf(gen->output, ")");
        break;

    default:
        fprintf(gen->output, "0");
    }
}

// ============================================================================
// STATEMENT GENERATION
// ============================================================================

void codegen_statement(CodeGenerator *gen, ASTNode *stmt)
{
    if (!stmt)
        return;

    switch (stmt->type)
    {
    case AST_ASSIGN:
    {
        codegen_indent(gen);

        // Check if assigning to function return value
        if (gen->in_function && gen->current_function_name &&
            str_equals_ignore_case(stmt->assign.identifier, gen->current_function_name))
        {
            fprintf(gen->output, "%s_result = ", sanitize_identifier(gen->current_function_name));
        }
        else
        {
            if (stmt->assign.num_indices > 0)
            {
                // Array element assignment
                fprintf(gen->output, "%s[", sanitize_identifier(stmt->assign.identifier));
                for (int i = 0; i < stmt->assign.num_indices; i++)
                {
                    if (i > 0)
                        fprintf(gen->output, "][");

                    int offset = codegen_get_array_offset(gen, stmt->assign.identifier, i);

                    if (offset != 0)
                    {
                        fprintf(gen->output, "(");
                        codegen_expression(gen, stmt->assign.indices[i]);
                        fprintf(gen->output, " - %d)", offset);
                    }
                    else
                    {
                        codegen_expression(gen, stmt->assign.indices[i]);
                    }
                }
                fprintf(gen->output, "] = ");
            }
            else
            {
                fprintf(gen->output, "%s = ", sanitize_identifier(stmt->assign.identifier));
            }
        }

        codegen_expression(gen, stmt->assign.value);
        fprintf(gen->output, ";\n");
        break;
    }

    case AST_PRINT:
    {
        codegen_indent(gen);
        fprintf(gen->output, "printf(\"");

        // Build format string with type inference
        for (int i = 0; i < stmt->print.num_exprs; i++)
        {
            const char *format = codegen_infer_printf_format(gen, stmt->print.expressions[i]);
            fprintf(gen->output, "%s", format);
        }
        fprintf(gen->output, "\"");

        // Arguments
        for (int i = 0; i < stmt->print.num_exprs; i++)
        {
            fprintf(gen->output, ", ");

            ASTNode *expr = stmt->print.expressions[i];

            if (expr->type == AST_LITERAL && expr->literal.value.type == VAL_BOOL)
            {
                fprintf(gen->output, "(%s ? \"true\" : \"false\")",
                        expr->literal.value.value.bool_val ? "1" : "0");
            }
            else
            {
                codegen_expression(gen, expr);
            }
        }
        fprintf(gen->output, ");\n");
        break;
    }

    case AST_READ:
    {
        for (int i = 0; i < stmt->read.num_vars; i++)
        {
            ASTNode *var = stmt->read.variables[i];
            codegen_indent(gen);

            if (var->type == AST_IDENTIFIER)
            {
                fprintf(gen->output, "scanf(\"%%d\", &%s);\n",
                        sanitize_identifier(var->identifier.name));
            }
            else if (var->type == AST_ARRAY_ACCESS)
            {
                fprintf(gen->output, "scanf(\"%%d\", &%s[",
                        sanitize_identifier(var->array_access.name));
                for (int j = 0; j < var->array_access.num_indices; j++)
                {
                    if (j > 0)
                        fprintf(gen->output, "][");
                    codegen_expression(gen, var->array_access.indices[j]);
                    fprintf(gen->output, " - 1");
                }
                fprintf(gen->output, "]);\n");
            }
        }
        break;
    }

    case AST_IF:
    {
        codegen_indent(gen);
        fprintf(gen->output, "if (");
        codegen_expression(gen, stmt->if_stmt.condition);
        fprintf(gen->output, ") {\n");

        gen->indent_level++;
        for (int i = 0; i < stmt->if_stmt.num_then; i++)
        {
            codegen_statement(gen, stmt->if_stmt.then_branch[i]);
        }
        gen->indent_level--;

        if (stmt->if_stmt.else_branch && stmt->if_stmt.num_else > 0)
        {
            codegen_indent(gen);
            fprintf(gen->output, "} else {\n");
            gen->indent_level++;
            for (int i = 0; i < stmt->if_stmt.num_else; i++)
            {
                codegen_statement(gen, stmt->if_stmt.else_branch[i]);
            }
            gen->indent_level--;
        }

        codegen_indent(gen);
        fprintf(gen->output, "}\n");
        break;
    }

    case AST_FOR:
    {
        codegen_indent(gen);
        const char *var = sanitize_identifier(stmt->for_loop.variable);

        bool is_positive_step = true;
        if (stmt->for_loop.step->type == AST_LITERAL &&
            stmt->for_loop.step->literal.value.type == VAL_INT)
        {
            is_positive_step = stmt->for_loop.step->literal.value.value.int_val >= 0;
        }

        fprintf(gen->output, "for (%s = ", var);
        codegen_expression(gen, stmt->for_loop.start);

        fprintf(gen->output, "; %s %s ", var, is_positive_step ? "<=" : ">=");
        codegen_expression(gen, stmt->for_loop.end);
        
        fprintf(gen->output, "; %s += ", var); 
        codegen_expression(gen, stmt->for_loop.step);
        fprintf(gen->output, ") {\n");

        gen->indent_level++;
        for (int i = 0; i < stmt->for_loop.num_stmts; i++)
        {
            codegen_statement(gen, stmt->for_loop.body[i]);
        }
        gen->indent_level--;

        codegen_indent(gen);
        fprintf(gen->output, "}\n");
        break;
    }

    case AST_WHILE:
    {
        if (stmt->while_loop.is_repeat_until)
        {
            // REPEAT-UNTIL
            codegen_indent(gen);
            fprintf(gen->output, "do {\n");

            gen->indent_level++;
            for (int i = 0; i < stmt->while_loop.num_stmts; i++)
            {
                codegen_statement(gen, stmt->while_loop.body[i]);
            }
            gen->indent_level--;

            codegen_indent(gen);
            fprintf(gen->output, "} while (!(");
            codegen_expression(gen, stmt->while_loop.condition);
            fprintf(gen->output, "));\n");
        }
        else
        {
            // WHILE
            codegen_indent(gen);
            fprintf(gen->output, "while (");
            codegen_expression(gen, stmt->while_loop.condition);
            fprintf(gen->output, ") {\n");

            gen->indent_level++;
            for (int i = 0; i < stmt->while_loop.num_stmts; i++)
            {
                codegen_statement(gen, stmt->while_loop.body[i]);
            }
            gen->indent_level--;

            codegen_indent(gen);
            fprintf(gen->output, "}\n");
        }
        break;
    }
    case AST_CALL:
    {
        codegen_indent(gen);

        ASTNode *subroutine = codegen_find_subroutine(gen, stmt->call.name);
        fprintf(gen->output, "%s(", sanitize_identifier(stmt->call.name));

        for (int i = 0; i < stmt->call.num_args; i++)
        {
            if (i > 0)
                fprintf(gen->output, ", ");

            ASTNode *arg = stmt->call.arguments[i];
            bool needs_ref = subroutine && codegen_is_param_by_ref(subroutine, i);

            if (needs_ref && (arg->type == AST_IDENTIFIER || arg->type == AST_ARRAY_ACCESS))
            {
                fprintf(gen->output, "&");
            }

            codegen_expression(gen, arg);
        }

        fprintf(gen->output, ");\n");
        break;
    }
    default:
        codegen_line(gen, "// Unknown statement type");
    }
}

// ============================================================================
// DECLARATION GENERATION
// ============================================================================
void codegen_declaration(CodeGenerator *gen, ASTNode *decl)
{
    if (decl->type == AST_CONST_DECL)
    {
        codegen_indent(gen);
        fprintf(gen->output, "#define %s ", sanitize_identifier(decl->decl.name));
        codegen_expression(gen, decl->decl.value);
        fprintf(gen->output, "\n");
    }
    else if (decl->type == AST_VAR_DECL)
    {
        const char *c_type = map_type(decl->decl.var_type);
        codegen_register_var_type(gen, decl->decl.name, c_type);

        codegen_indent(gen);

        if (decl->decl.num_arr_dims > 0)
        {
            // Array declaration
            fprintf(gen->output, "%s %s", c_type, sanitize_identifier(decl->decl.name));

            // Process each dimension and register bounds
            for (int i = 0; i < decl->decl.num_arr_dims; i++)
            {
                ASTNode *start_expr = decl->decl.arr_bound_exprs[i].start_expr;
                ASTNode *end_expr = decl->decl.arr_bound_exprs[i].end_expr;

                int start = 1, end = 100; // defaults

                // Try to evaluate the bounds
                if (start_expr->type == AST_LITERAL && start_expr->literal.value.type == VAL_INT)
                {
                    start = start_expr->literal.value.value.int_val;
                }
                else if (start_expr->type == AST_IDENTIFIER && gen->env)
                {
                    // Try to resolve constant
                    RuntimeValue *val = env_get(gen->env, start_expr->identifier.name);
                    if (val && val->type == VAL_INT)
                    {
                        start = val->value.int_val;
                    }
                }

                if (end_expr->type == AST_LITERAL && end_expr->literal.value.type == VAL_INT)
                {
                    end = end_expr->literal.value.value.int_val;
                }
                else if (end_expr->type == AST_IDENTIFIER && gen->env)
                {
                    // Try to resolve constant
                    RuntimeValue *val = env_get(gen->env, end_expr->identifier.name);
                    if (val && val->type == VAL_INT)
                    {
                        end = val->value.int_val;
                    }
                }

                int size = end - start + 1;
                fprintf(gen->output, "[%d]", size);

                // CRITICAL: Register bounds for THIS dimension
                codegen_register_array(gen, decl->decl.name, start, end, i);
            }

            fprintf(gen->output, "; /* bounds: ");
            for (int i = 0; i < decl->decl.num_arr_dims; i++)
            {
                if (i > 0)
                    fprintf(gen->output, ", ");
                int start = codegen_get_array_offset(gen, decl->decl.name, i);
                int size = codegen_get_array_size(gen, decl->decl.name, i);
                fprintf(gen->output, "[%d..%d]", start, start + size - 1);
            }
            fprintf(gen->output, " */\n");
        }
        else
        {
            // Simple variable
            fprintf(gen->output, "%s %s;\n", c_type, sanitize_identifier(decl->decl.name));
        }
    }
}

// ============================================================================
// FUNCTION/PROCEDURE GENERATION
// ============================================================================

void codegen_function(CodeGenerator *gen, ASTNode *func)
{
    fprintf(gen->output, "\n");

    // Return type
    fprintf(gen->output, "%s %s(",
            map_type(func->subroutine.return_type),
            sanitize_identifier(func->subroutine.name));

    // Parameters
    for (int i = 0; i < func->subroutine.num_params; i++)
    {
        if (i > 0)
            fprintf(gen->output, ", ");
        ASTNode *param = func->subroutine.parameters[i];

        if (param->param.is_reference)
        {
            fprintf(gen->output, "%s *%s",
                    map_type(param->param.param_type),
                    sanitize_identifier(param->param.name));
        }
        else
        {
            fprintf(gen->output, "%s %s",
                    map_type(param->param.param_type),
                    sanitize_identifier(param->param.name));
        }
    }

    fprintf(gen->output, ") {\n");
    gen->indent_level++;

    // Return variable
    codegen_indent(gen);
    fprintf(gen->output, "%s %s_result = 0;\n",
            map_type(func->subroutine.return_type),
            sanitize_identifier(func->subroutine.name));

    // Local variables
    if (func->subroutine.local_decls)
    {
        for (int i = 0; i < func->subroutine.num_local_decls; i++)
        {
            codegen_declaration(gen, func->subroutine.local_decls[i]);
        }
    }

    // Body
    gen->in_function = true;
    gen->current_function_name = func->subroutine.name;

    for (int i = 0; i < func->subroutine.num_stmts; i++)
    {
        codegen_statement(gen, func->subroutine.body[i]);
    }

    gen->in_function = false;

    // Return
    codegen_indent(gen);
    fprintf(gen->output, "return %s_result;\n",
            sanitize_identifier(func->subroutine.name));

    gen->indent_level--;
    fprintf(gen->output, "}\n");
}

void codegen_procedure(CodeGenerator *gen, ASTNode *proc)
{
    fprintf(gen->output, "\n");

    // Return type is void
    fprintf(gen->output, "void %s(", sanitize_identifier(proc->subroutine.name));

    // Parameters
    for (int i = 0; i < proc->subroutine.num_params; i++)
    {
        if (i > 0)
            fprintf(gen->output, ", ");
        ASTNode *param = proc->subroutine.parameters[i];

        if (param->param.is_reference)
        {
            fprintf(gen->output, "%s *%s",
                    map_type(param->param.param_type),
                    sanitize_identifier(param->param.name));
        }
        else
        {
            fprintf(gen->output, "%s %s",
                    map_type(param->param.param_type),
                    sanitize_identifier(param->param.name));
        }
    }

    fprintf(gen->output, ") {\n");
    gen->indent_level++;

    // Local variables
    if (proc->subroutine.local_decls)
    {
        for (int i = 0; i < proc->subroutine.num_local_decls; i++)
        {
            codegen_declaration(gen, proc->subroutine.local_decls[i]);
        }
    }

    // Body
    for (int i = 0; i < proc->subroutine.num_stmts; i++)
    {
        codegen_statement(gen, proc->subroutine.body[i]);
    }

    gen->indent_level--;
    fprintf(gen->output, "}\n");
}

// ============================================================================
// PROGRAM GENERATION
// ============================================================================
void codegen_program(CodeGenerator *gen, ASTNode *prog)
{
    // Store program for lookups
    gen->program = prog;

    // Header
    fprintf(gen->output, "/*\n");
    fprintf(gen->output, " * Generated C code from EAP pseudocode\n");
    fprintf(gen->output, " * Program: %s\n", prog->program.name);
    fprintf(gen->output, " */\n\n");

    // Includes
    fprintf(gen->output, "#include <stdio.h>\n");
    fprintf(gen->output, "#include <stdlib.h>\n");
    fprintf(gen->output, "#include <stdbool.h>\n");
    fprintf(gen->output, "#include <math.h>\n");
    fprintf(gen->output, "#include <string.h>\n\n");

    // Create environment for constant evaluation
    Environment *const_env = create_environment(NULL);
    gen->env = const_env;

    // First pass: Constants (and evaluate them)
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];
        if (decl->type == AST_CONST_DECL)
        {
            RuntimeValue val = evaluate(decl->decl.value, const_env);
            env_define(const_env, decl->decl.name, val);
            codegen_declaration(gen, decl);
        }
    }

    fprintf(gen->output, "\n");

    // Forward declarations for functions/procedures
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];
        if (decl->type == AST_FUNC_DECL)
        {
            fprintf(gen->output, "%s %s(",
                    map_type(decl->subroutine.return_type),
                    sanitize_identifier(decl->subroutine.name));
            for (int j = 0; j < decl->subroutine.num_params; j++)
            {
                if (j > 0)
                    fprintf(gen->output, ", ");
                ASTNode *param = decl->subroutine.parameters[j];
                fprintf(gen->output, "%s%s",
                        map_type(param->param.param_type),
                        param->param.is_reference ? "*" : "");
            }
            fprintf(gen->output, ");\n");
        }
        else if (decl->type == AST_PROC_DECL)
        {
            fprintf(gen->output, "void %s(", sanitize_identifier(decl->subroutine.name));
            for (int j = 0; j < decl->subroutine.num_params; j++)
            {
                if (j > 0)
                    fprintf(gen->output, ", ");
                ASTNode *param = decl->subroutine.parameters[j];
                fprintf(gen->output, "%s%s",
                        map_type(param->param.param_type),
                        param->param.is_reference ? "*" : "");
            }
            fprintf(gen->output, ");\n");
        }
    }

    // Global variables
    fprintf(gen->output, "\n");
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];
        if (decl->type == AST_VAR_DECL)
        {
            codegen_declaration(gen, decl);
        }
    }

    // Functions and procedures
    for (int i = 0; i < prog->program.num_decls; i++)
    {
        ASTNode *decl = prog->program.declarations[i];
        if (decl->type == AST_FUNC_DECL)
        {
            codegen_function(gen, decl);
        }
        else if (decl->type == AST_PROC_DECL)
        {
            codegen_procedure(gen, decl);
        }
    }

    // Main function
    fprintf(gen->output, "\nint main() {\n");
    gen->indent_level = 1;

    for (int i = 0; i < prog->program.num_stmts; i++)
    {
        codegen_statement(gen, prog->program.body[i]);
    }

    codegen_line(gen, "return 0;");
    fprintf(gen->output, "}\n");
}

// Make these functions available to codegen.c
bool str_equals_ignore_case(const char *a, const char *b); // Already exists

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("EAP Pseudocode Interpreter\n");
        printf("Usage: %s <file.eap> [--debug|--transpile]\n", argv[0]);
        printf("\nExample:\n");
        printf("  %s program.eap\n", argv[0]);
        printf("  %s program.eap --debug --transpile\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    bool transpile_mode = false;

    debug_mode = (argc > 2 && strcmp(argv[2], "--debug") == 0);

    // Check for flags
    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug_mode = true;
        }
        else if (strcmp(argv[i], "--transpile") == 0)
        {
            transpile_mode = true;
        }
    }

    char *code = read_file(filename);

    if (debug_mode)
    {
        fprintf(stderr, "[DEBUG] File size: %zu characters\n", strlen(code));
    }

    // Tokenize
    tokenize(code);
    if (debug_mode)
    {
        fprintf(stderr, "[DEBUG] Generated %d tokens\n", token_count);
    }

    // Parse
    ASTNode *program = parse_program();
    if (debug_mode)
    {
        fprintf(stderr, "[DEBUG] Parsed program: %s\n", program->program.name);
        fprintf(stderr, "[DEBUG] Declarations: %d\n", program->program.num_decls);
        fprintf(stderr, "[DEBUG] Statements: %d\n", program->program.num_stmts);
    }

    if (transpile_mode)
    {
        CodeGenerator gen;
        codegen_init(&gen, stdout);
        codegen_program(&gen, program);
        free(code);
        return 0;
    }

    // Execute
    execute_program(program);

    free(code);
    return 0;
}
