#ifndef __TOY_DXCRUNTIME_H__
#define __TOY_DXCRUNTIME_H__
#include <cstdint>
#include <D3Dcommon.h>
namespace toy::dxcruntime {
enum class ShaderTarget : uint8_t {
  PS, VS, CS, GS, DS, HS, LIB,
};
bool Init();
void Term();
ID3DBlob* CompileShader(LPCWSTR filename, LPCWSTR entryName, const ShaderTarget shaderTarget,
                        const DxcDefine* const defines, const UINT32 defineCount);
ID3DBlob* CompileShader(LPCWSTR filename, LPCWSTR entryName, const LPCWSTR targetProfile,
                        const DxcDefine* const defines, const UINT32 defineCount);
}
#endif
