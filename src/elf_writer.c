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

/**
 * @brief ELF HEADER MAGIC VALUE
 * 
 */
#define ELF_HEADER_MAGIC {0x7F, 0x45, 0x4C, 0x46}

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Default ELF 32 headers
 * 
 */
static const elf_32_header_t prv_default_elf_23_header = {
    .magic.bytes = ELF_HEADER_MAGIC,
    .version = 1,
    .e_version = 1,
    .flags = 0,
    .header_size = sizeof(elf_32_header_t),
    .bit_length = 1,
    .endianness = 1,
    .ph_offset = sizeof(elf_32_header_t),
    .ph_header_size = sizeof(elf_program_header_t),
};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Determines the offsets and sizes for program headers
 * 
 * @param ctx Pointer to 
 */
static void prv_calculate_program_header_offset(elf_writer_ctx_t *ctx) {
    elf_writer_program_entry_t *ptr = ctx->ph_head;
    
    if (ptr == NULL) {
        return;
    }
    
    // Calculate offsets for all program header entries
    while(ptr) {
        // Offset of the payload of the first program entry is the size of all the headers
        if (ptr->prev == NULL) {
            ptr->ph.p_offset = ctx->elf_header.ph_offset + (sizeof(elf_program_header_t) * ctx->elf_header.ph_count);
        } else {
            elf_writer_program_entry_t *prev = (elf_writer_program_entry_t*)ptr->prev;
            ptr->ph.p_offset = prev->ph.p_offset + prev->ph.p_file_size;
        }
        
        ptr = (elf_writer_program_entry_t*)ptr->next;
    }
}

/**
 * @brief Write ELF header
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_elf_header(elf_writer_ctx_t *ctx) {
    return ctx->interface->write(ctx->file_ctx, 0, &ctx->elf_header, sizeof(elf_32_header_t));
}

/**
 * @brief Write program entry headers
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_ph_headers(elf_writer_ctx_t *ctx) {
    elf_writer_program_entry_t *ptr = ctx->ph_head;
    size_t offset = sizeof(elf_32_header_t);
    
    while(ptr) {
        elf_writer_ret_t ret = ctx->interface->write(ctx->file_ctx, offset, &ptr->ph, sizeof(elf_program_header_t));
        
        if (ret != ELF_WRITER_RET_SUCCESS) {
            return ret;
        }

        offset += sizeof(elf_program_header_t);
        ptr = (elf_writer_program_entry_t *)ptr->next;
    }
    
    return ELF_WRITER_RET_SUCCESS;
}

/**
 * @brief Write section entry headers
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_sh_headers(elf_writer_ctx_t *ctx) {
    return ELF_WRITER_RET_SUCCESS;
}

/**
 * @brief Write all ELF headers
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_headers(elf_writer_ctx_t *ctx) {
    elf_writer_ret_t ret = prv_write_elf_header(ctx);
    if (ret != ELF_WRITER_RET_SUCCESS) {
        return ret;
    }
    
    ret = prv_write_ph_headers(ctx);
    if (ret != ELF_WRITER_RET_SUCCESS) {
        return ret;
    }
    
    ret = prv_write_sh_headers(ctx);
    
    return ret;
}

/**
 * @brief Writes payload for each program entry
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_ph_entries(elf_writer_ctx_t *ctx) {
    elf_writer_program_entry_t *ptr = ctx->ph_head;

    while (ptr)
    {
        elf_writer_ret_t ret = ctx->interface->write(ctx->file_ctx, ptr->ph.p_offset, ptr->payload, ptr->ph.p_mem_size);

        if (ret != ELF_WRITER_RET_SUCCESS)
        {
            return ret;
        }

        ptr = (elf_writer_program_entry_t *)ptr->next;
    }

    return ELF_WRITER_RET_SUCCESS;
}

/**
 * @brief Writes payload for each section entry
 * 
 * @param ctx Pointer to elf writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
static elf_writer_ret_t prv_write_sh_entries(elf_writer_ctx_t *ctx) {
    return ELF_WRITER_RET_SUCCESS;
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

elf_writer_ret_t elf_writer_init(elf_writer_ctx_t * ctx, elf_writer_interface_t *interface)
{
    if (ctx == NULL || interface == NULL) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    if (interface == NULL) {
        return ELF_WRITER_RET_BAD_INTERFACE;
    }
    
    if (interface->open == NULL || interface->write == NULL, interface->close == NULL) {
        return ELF_WRITER_RET_BAD_INTERFACE;
    }
    
    memset(ctx, 0, sizeof(elf_writer_ctx_t));
    ctx->interface = interface;
    
    ctx->elf_header = prv_default_elf_23_header;
    ctx->total_size = sizeof(elf_32_header_t);
    
    return ELF_WRITER_RET_SUCCESS;
}

elf_writer_ret_t elf_writer_set_elf_type(elf_writer_ctx_t *ctx, elf_type_t type) {
    if (ctx == NULL) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    ctx->elf_header.type = type;
    
    return ELF_WRITER_RET_SUCCESS;
}

elf_writer_ret_t elf_writer_set_machine_info(elf_writer_ctx_t *ctx, uint8_t os_abi, uint8_t abi_version, uint16_t machine, uint32_t flags) {
    if (ctx == NULL) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    ctx->elf_header.os_abi = os_abi;
    ctx->elf_header.abi_version = abi_version;
    ctx->elf_header.machine = machine;
    ctx->elf_header.flags = flags;
    
    return ELF_WRITER_RET_SUCCESS;
}

elf_writer_ret_t elf_writer_commit(elf_writer_ctx_t *ctx) {
    if (ctx == NULL) {
        return ELF_WRITER_RET_BAD_ARGS;
    }
    
    // Calcuate and generate proper offsets for all PH & SH entries
    prv_calculate_program_header_offset(ctx);
    // TODO: Generate SH offsets
    
    elf_writer_ret_t ret = ctx->interface->open(ctx->total_size, NULL);
    if (ret != ELF_WRITER_RET_SUCCESS) {
        return ret;
    }
    
    // Write headers
    ret = prv_write_headers(ctx);
    if (ret != ELF_WRITER_RET_SUCCESS) {
        return ret;
    }
    
    ret = prv_write_ph_entries(ctx);
    if (ret != ELF_WRITER_RET_SUCCESS)
    {
        return ret;
    }

    return ctx->interface->close(ctx->file_ctx);
}

elf_writer_ret_t elf_writer_add_program_entry(elf_writer_ctx_t *ctx, elf_writer_program_entry_t *program_entry, uint32_t type, uint32_t paddr, uint32_t vaddr, uint32_t flags, void *payload, uint32_t payload_size, uint32_t file_size) {
if (ctx == NULL || payload == NULL)
{
    return ELF_WRITER_RET_BAD_ARGS;
    }
    
    memset(program_entry, 0, sizeof(elf_writer_program_entry_t));
    
    program_entry->next = NULL;
    program_entry->payload = payload;

    program_entry->ph.type = type;
    program_entry->ph.p_align = 1;
    program_entry->ph.p_paddr = paddr;
    program_entry->ph.p_vaddr = vaddr;
    program_entry->ph.p_mem_size = payload_size;
    program_entry->ph.p_file_size = file_size;
    program_entry->ph.p_flags = flags;
    
    if (ctx->ph_head == NULL) {
        ctx->ph_head = program_entry;
        ctx->ph_tail = program_entry;
    } else {
        program_entry->prev = ctx->ph_tail;
        ctx->ph_tail->next = program_entry;
        ctx->ph_tail = program_entry;
    }
    
    ctx->elf_header.ph_count++;
    ctx->total_size += sizeof(elf_program_header_t) + file_size;
    
    return ELF_WRITER_RET_SUCCESS;
}