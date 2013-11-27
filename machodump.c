
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef _MACHO_LOADER_H
#	include <mach-o/loader.h>
#	include <mach-o/fat.h>
#	include <mach-o/nlist.h>
#endif

#ifdef _DEBUG
#define debug_printf printf
#else
#define debug_printf
#endif

uint32_t LoadCommandType[] = {
    LC_REQ_DYLD,
    LC_SEGMENT,	
    LC_SYMTAB,
    LC_SYMSEG,
    LC_THREAD,
    LC_UNIXTHREAD,
    LC_LOADFVMLIB,
    LC_IDFVMLIB,
    LC_IDENT,
    LC_FVMFILE,
    LC_PREPAGE,   
    LC_DYSYMTAB,
    LC_LOAD_DYLIB,
    LC_ID_DYLIB	,
    LC_LOAD_DYLINKER ,
    LC_ID_DYLINKER,
    LC_PREBOUND_DYLIB ,
    LC_ROUTINES	,
    LC_SUB_FRAMEWORK ,
    LC_SUB_UMBRELLA ,
    LC_SUB_CLIENT	,
    LC_SUB_LIBRARY ,
    LC_TWOLEVEL_HINTS ,
    LC_PREBIND_CKSUM,
    LC_LOAD_WEAK_DYLIB ,
    LC_SEGMENT_64,
    LC_ROUTINES_64,
    LC_UUID,
    LC_RPATH,
    LC_CODE_SIGNATURE,
    LC_SEGMENT_SPLIT_INFO ,
    LC_REEXPORT_DYLIB,
    LC_LAZY_LOAD_DYLIB,
    LC_ENCRYPTION_INFO,
    LC_DYLD_INFO,
    LC_DYLD_INFO_ONLY,
    LC_LOAD_UPWARD_DYLIB,
    LC_VERSION_MIN_MACOSX,
    LC_VERSION_MIN_IPHONEOS,
    LC_FUNCTION_STARTS,
    LC_DYLD_ENVIRONMENT,
};

uint32_t LoadCommandType_Number = sizeof(LoadCommandType) / sizeof(uint32_t);

struct macho_desc_t {
    char    path[256];
    char    *top;
    int     fd;
};

int map_binary(char *binpath, struct macho_desc_t *bin_struct)
{
    int fd;
    struct stat fs;
    char *top = NULL;
    int ret;
    fd = open(binpath, O_RDONLY);
    if (fd == -1) 
            goto CLOSING;
    if (fstat(fd, &fs) < 0) {	goto CLOSING;	}
    top = mmap(NULL, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (top == MAP_FAILED) 	{	
            top = NULL;
            goto CLOSING;	
    }
    ret = 1;
CLOSING:
    strcpy(bin_struct->path, binpath);
    bin_struct->top = top;
    bin_struct->fd = fd;
    return ret;
}

void unmap_binary(struct macho_desc_t *p)
{
    if (p != NULL && p->top != NULL) {
        close(p->fd);
        p->top == NULL;
        debug_printf("unmap_binary done\n");
    }
}

char *get_first_loadcommand(struct macho_desc_t *mapdesc)
{
    char *ret = NULL;
    if (mapdesc != NULL && mapdesc->top != NULL) {
        uint32_t magic = *(uint32_t*)mapdesc->top;
        if (magic == MH_MAGIC || magic == MH_CIGAM) {
                ret = mapdesc->top + sizeof(struct mach_header);
        } else if (magic == MH_MAGIC_64 || magic == MH_CIGAM_64) {
                ret = mapdesc->top + sizeof(struct mach_header_64);
        }
    }
    return ret;
}

/*char *get_next_loadcommand(char *mapped_binary, char *pcurrent)
 */
char *get_next_loadcommand(struct macho_desc_t *mapdesc, char *pcurrent)
{
    int i;
    int check = 0;
    char *ret = NULL;
    struct load_command *current_struct = (struct load_command*)pcurrent;
    for(i = 0; i < LoadCommandType_Number; i++) {
        if (current_struct->cmd == LoadCommandType[i]) {
            check = 1;
            break;
        }
    }
    if (check == 1) {
        ret = pcurrent + current_struct->cmdsize;
    }
    return ret;
}
int main(void)
{
    char *fname = "a.out";
    struct macho_desc_t f;
    printf("%s map --- %d\n", fname, map_binary(fname, &f));
    return 0;
}
