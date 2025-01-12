/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file elf_writer.h
 * @author Evan Stoddard
 * @brief 
 */

#ifndef ELF_WRITER_H_
#define ELF_WRITER_H_

#include "elf_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

typedef enum
{
    ELF_WRITER_RET_SUCCESS = 0,
    ELF_WRITER_RET_BAD_ARGS,
    ELF_WRITER_RET_BAD_INTERFACE,
} elf_writer_ret_t;

/**
 * @brief Interface for writing ELF
 *
 */
typedef struct elf_writer_interface_t
{
    uint32_t unused;
} elf_writer_interface_t;

/**
 * @brief Struct containing context for ELF document
 *
 */
typedef struct elf_writer_ctx_t
{
    /**
     * @brief Pointer to writer file interface. This is used to write the final ELF binary.
     * This pointer must remain in scope the duration of the elf_writer_context_t pointer.
     * 
     */
    elf_writer_interface_t *interface;
    
    /**
     * @brief This is an optional context for the file to be written to. This must remain in scope
     * the duration of the file. For example, this might be a FILE pointer.
     * 
     */
    void *file_ctx;
    
    /**
     * @brief The context of the ELF header
     * 
     */
    elf_32_header_t elf_header;
} elf_writer_ctx_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize ELF writer context
 *
 * @param ctx Pointer to context struct
 * @param interface Pointer to interface. Must remain in scope during duration of context context.
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on successful initialization
 */
elf_writer_ret_t elf_writer_init(elf_writer_ctx_t *ctx, elf_writer_interface_t *interface);

#ifdef __cplusplus
}
#endif
#endif /* ELF_WRITER_H_ */