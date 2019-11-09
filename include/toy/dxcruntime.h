#ifndef __TOY_DXCRUNTIME_H__
#define __TOY_DXCRUNTIME_H__
#include <cstdint>
#include <D3Dcommon.h>
#include <dxc/dxcapi.h>
#include <dxgi1_6.h> // must include before dxcapi
#include "dxc/Support/dxcapi.use.h"
namespace toy::dxcruntime {
enum class ShaderTarget : uint8_t {
  PS, VS, CS, GS, DS, HS, LIB,
};
/**
 * Don't release this class until pso is created
 * or dxil.dll might be released and fail to create pso.
 **/
class Compiler final {
 public:
  Compiler();
  ~Compiler();
  ID3DBlob* CompileShader(LPCWSTR filename, LPCWSTR entryName, const ShaderTarget shaderTarget,
                          const DxcDefine* const defines, const UINT32 defineCount);
  ID3DBlob* CompileShader(LPCWSTR filename, LPCWSTR entryName, const LPCWSTR targetProfile,
                          const DxcDefine* const defines, const UINT32 defineCount);
 private:
  dxc::DxcDllSupport support_;
  IDxcLibrary* library_;
  IDxcCompiler* compiler_;
  IDxcIncludeHandler* include_;
 private:
  Compiler(const Compiler&) = delete;
  void operator=(const Compiler&) = delete;
};
}
#endif
