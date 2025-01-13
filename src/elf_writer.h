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

#include <stddef.h>

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
    /**
     * @brief Will open file to write ELF to
     *
     * @param size Total size of ELF
     * @param file_ctx Optional file context pointer. Interface context will set this pointer
     * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on successful file open
     */
    elf_writer_ret_t (*open)(size_t size, void *file_ctx);

    /**
     * @brief Write portion of ELF to file
     *
     * @param file_ctx Optional file context pointer. May be NULL
     * @param offset Offset into file
     * @param buffer Pointer to payload to write to file
     * @param len Length of buffer to write
     * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on successful write
     */
    elf_writer_ret_t (*write)(void *file_ctx, size_t offset, void *buffer, size_t len);

    /**
     * @brief Close file at end of writing ELF
     *
     * @param file_ctx Optional file context pointer. May be NULL
     * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on successful close
     */
    elf_writer_ret_t (*close)(void *file_ctx);
} elf_writer_interface_t;

/**
 * @brief Entry for program header entry
 * 
 */
typedef struct elf_writer_program_entry_t {
    /**
     * @brief Pointer to previous entry
     * 
     */
    void *prev;
    
    /**
     * @brief Pointer to next entry
     * 
     */
    void *next;
    
    /**
     * @brief Program header
     * 
     */
    elf_program_header_t ph;
    
    /**
     * @brief Pointer to payload. Must remain in scope until ELF committed
     * 
     */
    void *payload;
} elf_writer_program_entry_t;

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
    
    /**
     * @brief Head of PH entry linked list
     * 
     */
    elf_writer_program_entry_t *ph_head;
    
    /**
     * @brief Tail of PH entry linked list
     * 
     */
    elf_writer_program_entry_t *ph_tail;
    
    /**
     * @brief Number of Section Header sections
     * 
     */
    uint32_t num_sh_sections;
    
    /**
     * @brief Total size of ELF
     * 
     */
    size_t total_size;
    
    /**
     * @brief Global offset of ELF output buffer
     * 
     */
    size_t offset;
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

/**
 * @brief Set type of ELF
 * 
 * @param ctx Pointer to ELF writer context
 * @param type Type of ELF
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_writer_set_elf_type(elf_writer_ctx_t *ctx, elf_type_t type);

/**
 * @brief Set machine/ABI information for ELF header
 * 
 * @param ctx Pointer to writer context
 * @param os_abi OS ABI
 * @param abi_version ABI Version
 * @param machine Machine type
 * @param flags Flags
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_writer_set_machine_info(elf_writer_ctx_t *ctx, uint8_t os_abi, uint8_t abi_version, uint16_t machine, uint32_t flags);

/**
 * @brief Commit ELF file to output
 * 
 * @param ctx Pointer to writer context
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success.
 */
elf_writer_ret_t elf_writer_commit(elf_writer_ctx_t *ctx);

/**
 * @brief Add program header entry with payload
 *
 * @param ctx Pointer to writer context
 * @param ctx Pointer to program entry.  Must remain in scope until ELF file commited.
 * @param type Type of program header
 * @param paddr Physical address
 * @param vaddr Virtual address
 * @param flags Flags
 * @param payload Pointer to payload
 * @param payload_size Size of payload
 * @param file_size File size (Usually payload size or payload size plus alignment)
 * @return elf_writer_ret_t Returns ELF_WRITER_RET_SUCCESS on success
 */
elf_writer_ret_t elf_writer_add_program_entry(elf_writer_ctx_t *ctx, elf_writer_program_entry_t *program_entry, uint32_t type, uint32_t paddr, uint32_t vaddr, uint32_t flags, void *payload, uint32_t payload_size, uint32_t file_size);

#ifdef __cplusplus
}
#endif
#endif /* ELF_WRITER_H_ */