/*
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ajsmn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Allocates a fresh unused token from the token pool.
 */
static inline jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  jsmntok_t *tok;

  tok = NULL;
  if (parser->toknext <= num_tokens) {
    tok = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
    #ifdef JSMN_PARENT_LINKS
    tok->parent = -1;
    #endif
  }

  return tok;
}

/**
 * Fills token type and boundaries.
 */
#define jsmn_fill_token(token, jtype, jstart, jend)  \
  token->type = (jtype); \
  token->start = (jstart); \
  token->end = (jend); \
  token->size = 0

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                                const size_t len, jsmntok_t *tokens,
                                const size_t num_tokens) {
  jsmntok_t *token;
  int start;

  start = parser->pos;
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
    #ifndef JSMN_STRICT
    /* In strict mode primitive must be followed by "," or "}" or "]" */
    case ':':
    #endif
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto found;
    default:
                   /* to quiet a warning from gcc*/
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
  #ifdef JSMN_STRICT
  /* In strict mode primitive must be followed by a comma/object/array */
  parser->pos = start;
  return JSMN_ERROR_PART;
  #endif

  found:
  if ((token = jsmn_alloc_token(parser, tokens, num_tokens)) == NULL) {
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
  #ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
  #endif
  parser->pos--;
  return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
                             const size_t len, jsmntok_t *tokens,
                             const size_t num_tokens) {
  jsmntok_t *token;
  jsmntype_t  type;
  int start;
  char c;
  
  type = JSMN_STRING;
  start = ++(parser->pos);  /* Skip starting quote */

  for (; parser->pos < len && (c = js[parser->pos]) != '\0'; parser->pos++) {
    /* Quote: end of string */
    if (c == '\"') {
      if ((token = jsmn_alloc_token(parser, tokens, num_tokens)) == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      if (parser->toksuper != -1 && tokens[parser->toksuper].type == JSMN_OBJECT)
        type = JSMN_KEY;
      jsmn_fill_token(token, type, start, parser->pos);
      #ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
      #endif
      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      parser->pos++;
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        for (int i = 0; i < 4 && parser->pos < len && (c = js[parser->pos]) != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((c >= 48 && c <= 57) ||   /* 0-9 */
                (c >= 65 && c <= 70) ||   /* A-F */
                (c >= 97 && c <= 102))) { /* a-f */
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      /* Unexpected symbol */
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

#define ISHALFTOK(tok) ((tok)->start != -1 && (tok)->end == -1)

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens) {
  int r;              /* Return value of primitive/string parse */
  int i;              /* Loop counter */
  int count;          /* Number of parsed toks to be returned */
  char c;             /* Read buffer */
  jsmntype_t type;    /* tmp use */
  jsmntok_t *token;   /* tmp use */
  jsmn_parser parser; /* parser */
  
  if (!js || !tokens)
    return JSMN_ERROR_INVAL;
  jsmn_init(&parser);
  count = parser.toknext;   /* Init value 0 */
  for (; parser.pos < len && (c = js[parser.pos]) != '\0'; parser.pos++) {
    switch (c) {
    case '{':
    case '[':
      count++;
      if ((token = jsmn_alloc_token(&parser, tokens, num_tokens)) == NULL)
        return JSMN_ERROR_NOMEM;
      if (parser.toksuper != -1) {
        tokens[parser.toksuper].size++;
        #ifdef JSMN_STRICT
        /* In strict mode an object or array can't become a key */
        if (tokens[parser.toksuper].type == JSMN_OBJECT) {
          return JSMN_ERROR_INVAL;
        }
        #endif
        #ifdef JSMN_PARENT_LINKS
        token->parent = parser.toksuper;
        #endif
      }
      token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
      token->start = parser.pos;   /* token->end == -1 */
      parser.toksuper = parser.toknext - 1;
      break;

    case '}':
    case ']':
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
      #ifdef JSMN_PARENT_LINKS
      if (parser.toknext < 1)
        return JSMN_ERROR_INVAL;
      token = &tokens[parser.toknext - 1];
      for (;;) {
        if (ISHALFTOK(token)) {
          if (token->type != type)
            return JSMN_ERROR_INVAL;
          token->end = parser.pos + 1;
          parser.toksuper = token->parent;
          break;
        }
        if (token->parent == -1) {
          if (token->type != type || parser.toksuper == -1)
            return JSMN_ERROR_INVAL;
          break;
        }
        token = &tokens[token->parent];
      }
      #else
      for (i = parser.toknext - 1; i >= 0; i--) {
        token = &tokens[i];
        if (ISHALFTOK(token)) {
          if (token->type != type) {
            return JSMN_ERROR_INVAL;
          }
          parser.toksuper = -1;
          token->end = parser.pos + 1;
          break;
        }
      }
      /* Error if unmatched closing bracket */
      if (i == -1) {
        return JSMN_ERROR_INVAL;
      }
      for (; i >= 0; i--) {
        token = &tokens[i];
        if (ISHALFTOK(token)) {
          parser.toksuper = i;
          break;
        }
      }
      #endif
      break;

    case '\"':
      count++;
      if ((r = jsmn_parse_string(&parser, js, len, tokens, num_tokens)) < 0)
        return r;
      if (parser.toksuper != -1)
        tokens[parser.toksuper].size++;
      break;

    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;

    case ':':
      parser.toksuper = parser.toknext - 1;
      break;

    case ',':
      if (parser.toksuper != -1 &&
          tokens[parser.toksuper].type != JSMN_ARRAY &&
          tokens[parser.toksuper].type != JSMN_OBJECT) {
        #ifdef JSMN_PARENT_LINKS
        parser.toksuper = tokens[parser.toksuper].parent;
        #else
        for (i = parser.toknext - 1; i >= 0; i--) {
          if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
            if (ISHALFTOK(&token[i])) {
              parser.toksuper = i;
              break;
            }
          }
        }
        #endif
      }
      break;

    #ifdef JSMN_STRICT
    /* In strict mode primitives are: numbers and booleans */
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
      /* And they must not be keys of the object */
      if (parser.toksuper != -1) {
        if (tokens[parser.toksuper].type == JSMN_OBJECT ||
            (tokens[parser.toksuper].type == JSMN_STRING && tokens[parser.toksuper].size != 0)) {
          return JSMN_ERROR_INVAL;
        }
      }
    #else
    /* In non-strict mode every unquoted value is a primitive */
    default:
    #endif
      count++;
      if ((r = jsmn_parse_primitive(&parser, js, len, tokens, num_tokens)) < 0)
        return r;
      if (parser.toksuper != -1) {
        tokens[parser.toksuper].size++;
      }
      break;

    #ifdef JSMN_STRICT
    /* Unexpected char in strict mode */
    default:
      return JSMN_ERROR_INVAL;
    #endif
    }
  }

  for (i = parser.toknext - 1; i >= 0; i--) {
    /* Unmatched opened object or array */
    if (ISHALFTOK(&tokens[i])) {
      return JSMN_ERROR_PART;
    }
  }


  return count;
}

/**
 * hight level api of jsmn_parse
 * input:   js, jsLen
 * output:  tokens
 * return:  size of tokens
 */
JSMN_API int jsmn_Parse(const char *js, size_t jsLen, jsmntok_t **tokens)
{
  int ret;

  if (jsLen < 2)
    return JSMN_ERROR_INVAL;
  *tokens = (jsmntok_t *)Malloc((jsLen >> 1) * sizeof(jsmntok_t));
  ret = jsmn_parse(js, jsLen, *tokens, jsLen >> 1);
  if (ret > 0)  /* parse sucess */
    *tokens = Realloc(*tokens, ret * sizeof(jsmntok_t));
  else          /* parse fail */
    free(*tokens);
  
  return ret;
}

/**
 * Search the tokens to find out current token matchs key.
 * Return the next token.
 * input:   js, key, tokens, numTokens
 * output:  none
 * return:  address of next token
 */
JSMN_API jsmntok_t *jsmn_NextTok(const char *js, const char *key, jsmntok_t *tokens,size_t numTokens)
{
  size_t pos;

  for (pos = 0; pos < numTokens; pos++) {
    if (tokens[pos].type == JSMN_KEY 
        && strlen(key) == tokens[pos].end - tokens[pos].start 
        && strncmp(key, js + tokens[pos].start, tokens[pos].end - tokens[pos].start) == 0)
      return &tokens[pos + 1];
  }
  return NULL;
}