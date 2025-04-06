#include "koopatoriscv.hpp"

static Regmanager regmanager;

void koopa_dump(const char* str) {
  // 解析字符串 str, 得到 Koopa IR 程序
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  // 确保解析时没有出错
  assert(ret == KOOPA_EC_SUCCESS);
  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  // 处理 raw program
  Visit(raw);

  // 处理完成, 释放 raw program builder 占用的内存
  // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
  // 所以不要在 raw program 处理完毕之前释放 builder
  koopa_delete_raw_program_builder(builder);
}

// 函数声明略
// ...

// 访问 raw program
void Visit(const koopa_raw_program_t &program) {
    // 执行一些其他的必要操作
    // ...
    // 访问所有全局变量
    Visit(program.values);
    // 访问所有函数
    std::cout<<"\t.text\n";
    Visit(program.funcs);
  }
  
  // 访问 raw slice
  void Visit(const koopa_raw_slice_t &slice) {
    for (size_t i = 0; i < slice.len; ++i) {
      auto ptr = slice.buffer[i];
      // 根据 slice 的 kind 决定将 ptr 视作何种元素
      switch (slice.kind) {
        case KOOPA_RSIK_FUNCTION:
          // 访问函数
          Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
          break;
        case KOOPA_RSIK_BASIC_BLOCK:
          // 访问基本块
          Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
          break;
        case KOOPA_RSIK_VALUE:
          // 访问指令
          Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
          break;
        default:
          // 我们暂时不会遇到其他内容, 于是不对其做任何处理
          assert(false);
      }
    }
  }
  
  // 访问函数
  void Visit(const koopa_raw_function_t &func) {
    // 执行一些其他的必要操作
    // ...
    // 访问所有基本块

    std::string func_name(func->name);
    if (func_name[0] == '@') {
        func_name = func_name.substr(1);
    }
    std::cout << "\t.globl " << func_name << "\n" << func_name << ":\n";
    Visit(func->bbs);
  }
  
  // 访问基本块
  void Visit(const koopa_raw_basic_block_t &bb) {
    // 执行一些其他的必要操作
    // ...
    // 访问所有指令
    Visit(bb->insts);
  }
  
  // 访问指令
  void Visit(const koopa_raw_value_t &value) {
    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    switch (kind.tag) {
      case KOOPA_RVT_RETURN:
        // 访问 return 指令
        Visit(kind.data.ret);
        break;
      case KOOPA_RVT_BINARY:
        regmanager.DumpBinary(kind.data.binary, value);
        break;
      default:
        // 其他类型暂时遇不到
        assert(false);
    }
  }
  
  // 访问对应类型指令的函数定义略
  // 视需求自行实现
  // ...
void Visit(const koopa_raw_return_t& ret)
{
  if (ret.value->kind.tag == KOOPA_RVT_INTEGER)
  {
    std::cout<<"\tli a0, "<<ret.value->kind.data.integer.value<<"\n";
  }
  else if (ret.value->kind.tag == KOOPA_RVT_BINARY)
  {
    std::cout<<"\tmv a0, "<<regmanager.regmap[ret.value]<<"\n";
  }
  else std::cout<<"\tli a0, 0\n";
  std::cout<<"\tret\n";

}