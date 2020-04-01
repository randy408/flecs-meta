#include "parser.h"

static
const char* skip_ws(const char *ptr) {
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

static
const char* skip_scope(const char *ptr, ecs_meta_parse_ctx_t *ctx) {
    /* Keep track of which characters were used to open the scope */
    char stack[256];
    int32_t sp = 0;
    char ch;

    while ((ch = *ptr)) {
        if (ch == '(') {
            stack[sp++] = ch;
        } else if (ch == '<') {
            stack[sp++] = ch;
        } else if (ch == '>') {
            if (stack[sp--] != '<') {
                ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl,
                    "mismatching < > in type definition");
            }
        } else if (ch == ')') {
            if (stack[sp--] != '(') {
                ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl,
                    "mismatching ( ) in type definition");                
            }            
        }

        ptr ++;
    }

    return ptr;
}

static
const char* parse_token(const char *ptr, char *buff, ecs_meta_parse_ctx_t *ctx) {
    char *bptr = buff, ch;

    /* Ignore whitespaces */
    ptr = skip_ws(ptr);

    if (!isalpha(*ptr)) {
        ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
            "invalid identifier (starts with '%c')", *ptr);
    }

    while ((ch = *ptr) && !isspace(ch) && ch != ';' && ch != ',') {
        /* Type definitions can contain macro's or templates */
        if (ch == '(' || ch == '<') {
            ptr = skip_scope(ptr, ctx);
        } else {
            *bptr = ch;
            bptr ++;
        }

        ptr ++;
    }

    *bptr = '\0';

    if (!ch) {
        ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
            "unexpected end of type definition");        
    }

    return ptr;
}

const char * ecs_meta_open_scope(
    const char *ptr,
    ecs_meta_parse_ctx_t *ctx)    
{
    /* Skip initial whitespaces */
    ptr = skip_ws(ptr);

    /* Is this the start of the type definition? */
    if (ctx->decl == ptr) {
        if (*ptr != '{') {
            ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
                "missing '{' in struct definition");            
        }

        ptr ++;

        ptr = skip_ws(ptr);
    }

    /* Is this the end of the type definition? */
    if (!*ptr) {
        ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
            "missing '}' at end of struct definition");        
    }   

    /* Is this the end of the type definition? */
    if (*ptr == '}') {
        ptr = skip_ws(ptr + 1);
        if (*ptr) {
            ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
                "stray characters after struct definition");
        }
        return NULL;
    }

    return ptr;
}

const char* ecs_meta_parse_enum(
    const char *ptr,
    ecs_def_token_t *token_out,
    ecs_meta_parse_ctx_t *ctx)
{
    ptr = ecs_meta_open_scope(ptr, ctx);
    if (!ptr) {
        return NULL;
    }   

    char token[ECS_META_IDENTIFIER_LENGTH];

    /* Parse token, constant identifier */
    ptr = parse_token(ptr, token, ctx);

    /* Expect a , or '}' */
    if (*ptr != ',' && ptr != '}') {
        ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
            "missing , after enum constant");
    }

    if (*ptr == ',') {
        return ptr + 1;
    } else {
        return ptr;
    }
}

const char* ecs_meta_parse_struct(
    const char *ptr,
    ecs_def_token_t *token_out,
    ecs_meta_parse_ctx_t *ctx)
{
    ptr = ecs_meta_open_scope(ptr, ctx);
    if (!ptr) {
        return NULL;
    }

    token_out->is_const = false;

    char token[ECS_META_IDENTIFIER_LENGTH];

    /* Parse token, expect type identifier or ECS_PROPERTY */
    ptr = parse_token(ptr, token, ctx);

    if (strcmp(token, "ECS_PROPERTY")) {
        /* If the first token is not ECS_PROPERTY, don't care about what
         * comes next as we won't need to / be able to parse it */
        return NULL;
    }

    /* ECS_PROPERTY is found, next token is the type */
    ptr = parse_token(ptr + 1, token, ctx);

    /* If token is const, set const flag and continue parsing type */
    if (!strcmp(token, "const")) {
        token_out->is_const = true;

        /* Parse type after const */
        ptr = parse_token(ptr + 1, token, ctx);
    }

    strcpy(token_out->type, token);

    /* Check if type is a pointer */
    ptr = skip_ws(ptr);
    if (*ptr == '*') {
        token_out->is_ptr = true;
        ptr ++;
    }

    /* Next token is the identifier */
    ptr = parse_token(ptr, token, ctx);
    strcpy(token_out->name, token);

    /* Expect a ; */
    if (*ptr != ';') {
        ecs_parser_error(ctx->name, ctx->decl, ptr - ctx->decl, 
            "missing ; after member declaration");
    }

    return ptr + 1;
}