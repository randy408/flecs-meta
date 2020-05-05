/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef NESTED_STRUCT_BAKE_CONFIG_H
#define NESTED_STRUCT_BAKE_CONFIG_H

/* Headers of public dependencies */
#include <flecs.h>
#include <flecs_components_meta.h>

/* Headers of private dependencies */
#ifdef NESTED_STRUCT_IMPL
/* No dependencies */
#endif

/* Convenience macro for exporting symbols */
#ifndef NESTED_STRUCT_STATIC
  #if NESTED_STRUCT_IMPL && (defined(_MSC_VER) || defined(__MINGW32__))
    #define NESTED_STRUCT_EXPORT __declspec(dllexport)
  #elif NESTED_STRUCT_IMPL
    #define NESTED_STRUCT_EXPORT __attribute__((__visibility__("default")))
  #elif defined _MSC_VER
    #define NESTED_STRUCT_EXPORT __declspec(dllimport)
  #else
    #define NESTED_STRUCT_EXPORT
  #endif
#else
  #define NESTED_STRUCT_EXPORT
#endif

#endif

