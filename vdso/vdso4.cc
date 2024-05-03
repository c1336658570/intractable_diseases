// 针对 vDSO 操作系统还给用户程序暴露了一些接口。我们可以通过 getauxval 找到 vDSO 共享库在当前进程用户态内存中的地址，然后根据共享库文件格式找到对应函数的地址进行调用。

// ldd /bin/ls    可以看到有	linux-vdso.so.1 (0x00007ffcdeac2000)
// 虽然没有对应的磁盘文件，在文件系统中找不到，但是操作系统在启动进程时已经将其作为虚拟共享库加载过，
// 所以再次使用 dlopen 时能够正确找到该共享库，然后再使用 dlsym 找到对应的导出函数即可。

// cat /proc/self/maps    也可以看到vdso

#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/time.h>

typedef unsigned char u8;

// vdso_sym用于从vdso中查找给定符号名称的地址
// 先通过 getauxval 函数获取 vDSO 在当前进程中的内存地址，然后根据 ELF 结构进行解析，从而找到指定函数的地址。
void* vdso_sym(const char* symname) {
  // 通过 getauxval 找到 vDSO 共享库在当前进程用户态内存中的地址，然后根据共享库文件格式找到对应函数的地址进行调用。
  auto vdso_addr = (u8*)getauxval(AT_SYSINFO_EHDR);   // 获取 vDSO 的基地址
  auto elf_header = (Elf64_Ehdr*)vdso_addr;           // 指向 VSDO 的 ELF 头
  // 节头
  auto section_header = (Elf64_Shdr*)(vdso_addr + elf_header->e_shoff);
  char* dynstr = 0; // 用于存储.dynstr节的地址，包含所有动态字符串，例如符号名称

  // 遍历所有节
  for (int i = 0; i < elf_header->e_shnum; i++) {
    auto& s = section_header[i];
    auto& ss_ = section_header[elf_header->e_shstrndx];   // 节头字符串表
    // VSDO ELF头的起始地址 + 节头字符串表相对于VSDO ELF头的偏移 + s的节名在字符串表中中的偏移
    auto name = (char*)(vdso_addr + ss_.sh_offset + s.sh_name);   // 节名称
    if (strcmp(name, ".dynstr") == 0) {   // 如果是 .dynstr 节
      dynstr = (char*)(vdso_addr + s.sh_offset);  // 获取 .dynstr 的地址
      break;
    }
  }

  void* ret = NULL;
  // 再次遍历节头，这次查找.dynsym节
  for (int i = 0; i < elf_header->e_shnum; i++) {
    auto name =
        (char*)(vdso_addr + section_header[elf_header->e_shstrndx].sh_offset +
                section_header[i].sh_name);
    if (strcmp(name, ".dynsym") == 0) {
      // 遍历.dynsym节中的符号
      for (int si = 0;
           si < (section_header[i].sh_size / section_header[i].sh_entsize);
           si++) {
        auto name =
            dynstr + ((Elf64_Sym*)(vdso_addr + section_header[i].sh_offset))[si].st_name;
        if (strcmp(name, symname) == 0) {
          // 如果找到所需符号，返回其地址
          ret = (vdso_addr +
                 ((Elf64_Sym*)(vdso_addr + section_header[i].sh_offset))[si].st_value);
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
  // 通过vdso_sym函数获取__vdso_gettimeofday函数的地址
  auto my_gettimeofday = (gettimeofday_t*)vdso_sym("__vdso_gettimeofday");
  struct timeval tv;
  struct timezone tz;

  my_gettimeofday(&tv, &tz);
  printf("tv_sec=%ld, tv_usec=%ld\n", tv.tv_sec, tv.tv_usec);

  return 0;
}