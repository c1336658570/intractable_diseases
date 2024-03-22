// 针对 vDSO 操作系统还给用户程序暴露了一些接口。我们可以通过 getauxval 找到 vDSO 共享库在当前进程用户态内存中的地址，然后根据共享库文件格式找到对应函数的地址进行调用。

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/time.h>

typedef unsigned char u8;

// vdso_sym用于从vdso中查找给定符号名称的地址
void* vdso_sym(const char* symname) {
  auto vdso_addr = (u8*)getauxval(AT_SYSINFO_EHDR);
  auto elf_header = (Elf64_Ehdr*)vdso_addr; // ELF头
  // 节头
  auto section_header = (Elf64_Shdr*)(vdso_addr + elf_header->e_shoff);
  char* dynstr = 0; // 用于存储.dynstr节的地址，包含所有动态字符串，例如符号名称

  for (int i = 0; i < elf_header->e_shnum; i++) {
    auto& s = section_header[i];
    auto& ss_ = section_header[elf_header->e_shstrndx];
    auto name = (char*)(vdso_addr + ss_.sh_offset + s.sh_name);
    if (strcmp(name, ".dynstr") == 0) {
      dynstr = (char*)(vdso_addr + s.sh_offset);
      break;
    }
  }

  void* ret = NULL;
  for (int i = 0; i < elf_header->e_shnum; i++) {
    auto name =
        (char*)(vdso_addr + section_header[elf_header->e_shstrndx].sh_offset +
                section_header[i].sh_name);
    if (strcmp(name, ".dynsym") == 0) {
      for (int si = 0;
           si < (section_header[i].sh_size / section_header[i].sh_entsize);
           si++) {
        auto name =
            dynstr +
            ((Elf64_Sym*)(vdso_addr + section_header[i].sh_offset))[si].st_name;
        if (strcmp(name, symname) == 0) {
          ret = (vdso_addr +
                 ((Elf64_Sym*)(vdso_addr + section_header[i].sh_offset))[si]
                     .st_value);
          break;
        }
      }
      if (ret) break;
    }
  }

  return ret;
}

// 定义一个指向gettimeofday函数的指针类型
typedef int(gettimeofday_t)(struct timeval* tv, struct timezone* tz);

int main() {
  auto my_gettimeofday = (gettimeofday_t*)vdso_sym("__vdso_gettimeofday");
  struct timeval tv;
  struct timezone tz;

  my_gettimeofday(&tv, &tz);
  printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);

  return 0;
}