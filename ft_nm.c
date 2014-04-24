/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/21 16:32:09 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/25 00:07:32 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mach-o/loader.h>
#include "libft.h"

/*
uint32_t magic; mach magic number identifier
cpu_type_t cputype; cpu specifier
cpu_subtype_t cpusubtype; machine specifier
uint32_t filetype; type of file
uint32_t ncmds; number of load commands
uint32_t sizeofcmds; the size of all the load commands
uint32_t flags; flags
};
*/
static char     read_tab(int i)
{
    char        *tab;

    tab = "0123456789abcdefghijklmnopqrstuvwxyz";
    return (tab[i]);
}

char            *ft_itoa_base(int val, int base, int output_size)
{
    char            buffer[output_size + 1];
    char            *p;
    unsigned int    unsigned_val;

    unsigned_val = (unsigned int)val;
    if (base > 36 || base < 2)
        return (NULL);
    buffer[output_size] = 0;
    p = buffer + output_size - 1;
    *p = '0';
    if (unsigned_val > 0)
    {
        while (unsigned_val > 0)
        {
            *p-- = read_tab(unsigned_val % base);
            unsigned_val = unsigned_val / base;
            if (p < buffer)
                break ;
        }
        p++;
    }
    while (p > buffer)
        *--p = '0';
    return (ft_strdup(p));
}

void	check_text(struct segment_command_64 *seg, struct mach_header_64 *mo)
{
	struct section_64	*sec;
	unsigned int	i;
	char				*ret;
	unsigned int					j;
	char				*str;

	j = 0;
	i = 0;
//	sec = (struct section *) (&(seg->flags) + (sizeof(seg->flags) / sizeof(void *)));
	sec = (struct section_64  *) (seg + sizeof(struct segment_command_64 *) / sizeof(void *));
	while (i < seg->nsects)
	{
		if (!ft_strcmp(sec->sectname, "__text"))
		{
//			ret = ft_strsub((char *)mo + sec->offset, 0, sec->size);
			ret = (char *)mo + sec->offset;
			while (j < sec->size)
			{
				str = ft_itoa_base(ret[j], 16, 2);
				printf("RES=%s ", str);
				j++;
			}
		}
		printf("sectname = %s\nadresse de ta mere =%p\n", sec->sectname, sec);
//		sec = (struct section  *) (seg  + (sizeof(struct segment_command_64 *)  ) / sizeof(void *));
		sec = (struct section_64  *) (((void *) sec ) +  sizeof(struct section_64));
//		sec += sizeof(struct section_64*) / sizeof(void *);
		i++;
	}
}

void	check_64(struct load_command *com, struct mach_header_64 *mo)
{
	struct segment_command_64	*seg;

	seg = (struct segment_command_64*)com;
	if (ft_strcmp(seg->segname, "__TEXT") == 0)
		check_text(seg, mo);
}

void	browse_cmd(struct load_command *com, struct mach_header_64 *mo)
{
	unsigned int		i;

	i = 0;
	printf("NCMD= %lu\nsize void=%lu\n", (unsigned long)mo->ncmds, sizeof(struct section_64));
	while (i < mo->ncmds)
	{
		printf("CMDSIZE=%lu\nCOM=%p\n", (unsigned long)com->cmdsize, com);
		if (com->cmd == LC_SEGMENT_64)
			check_64(com, mo);
		com +=  com->cmdsize / sizeof(void *);
		i++;
	}
}

int		main(int argc, char **argv)
{
	int							fd;
//	int				ret;
	struct stat					s;
	void						*mem;
	struct mach_header_64		*mo;
	struct load_command			*com;

//	ret = 0;
	if (argc != 2)
		write(1, "#YOLO!\n", 7);
	else
	{
//		s = NULL;
		fd = open(argv[1], O_RDONLY);
		fstat(fd, &s);
//		printf("la size = %d\n", (int)s.st_size);
		mem = mmap(0, s.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
		mo = mem;
		printf("\nmagic=%lu\ncpu-type=%lu\n", (unsigned long)mo->magic, (unsigned long)mo->cputype);
//		printf("real-magic: %lu\n", MH_MAGIC_64);
		com = mem + sizeof(struct mach_header_64);
		browse_cmd(com, mo);
/*		if (com->cmd == LC_SEGMENT_64)
			printf("OK LC_SEGMENT_64\n");
		else
		printf("NONONO\n");*/
		
		
//		write(1, mem, s.st_size);
//		printf("ret de write: %d", ret);
	}
	return (0);
}
