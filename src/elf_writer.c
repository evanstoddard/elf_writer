/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file elf_writer.c
 * @author Evan Stoddard
 * @brief Module for writing ELF files
 */

#include "elf_writer.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Definition of ELF header magic
 * 
 */
static const struct {
    const uint8_t magic_bytes[4];
    const uint32_t magic_word;
} prv_elf_magic = {
    .magic_bytes = {0x7F, 0x45, 0x4C, 0x46}
};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Functions
 *****************************************************************************/

elf_writer_ret_t elf_writer_init(elf_writer_ctx_t *ctx, elf_writer_interface_t *interface) {
    if (ctx == NULL || interface == NULL) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    if (interface == NULL) {
        return ELF_WRITER_RET_BAD_INTERFACE;
    }
    
    memset(ctx, 0, sizeof(elf_writer_ctx_t));
    ctx->interface = interface;
    
    ctx->elf_header.magic = prv_elf_magic.magic_word;
    ctx->elf_header.e_version = 1;
    ctx->elf_header.version = 1;
    ctx->elf_header.flags = 0;
    ctx->elf_header.header_size = sizeof(elf_32_header_t);
    
    return ELF_WRITER_RET_SUCCESS;
}