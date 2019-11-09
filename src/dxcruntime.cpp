#include <Windows.h>
#include "toy/dxcruntime.h"
#include "logger.h"
// ref. https://posts.tanki.ninja/2019/07/11/Using-DXC-In-Practice/
namespace toy::dxcruntime {
IDxcLibrary* CreateLibrary(dxc::DxcDllSupport* const support) {
  IDxcLibrary* library = nullptr;
  auto hr = support->CreateInstance(CLSID_DxcLibrary, &library);
  if (FAILED(hr)) {
    logerror("CreateLibrary failed. {}", hr);
  }
  return library;
}
IDxcCompiler* CreateCompiler(dxc::DxcDllSupport* const support) {
  IDxcCompiler* compiler = nullptr;
  auto hr = support->CreateInstance(CLSID_DxcCompiler, &compiler);
  if (FAILED(hr)) {
    logerror("CreateCompiler failed. {}", hr);
  }
  return compiler;
}
IDxcIncludeHandler* CreateDefaultIncludeHandler(IDxcLibrary* library) {
  IDxcIncludeHandler* includeHandler = nullptr;
  auto hr = library->CreateIncludeHandler(&includeHandler);
  if (FAILED(hr)) {
    logerror("CreateIncludeHandler failed. {}", hr);
  }
  return includeHandler;
}
IDxcBlob* CreateShaderBlob(IDxcLibrary* library, LPCWSTR filename) {
  IDxcBlobEncoding* blob = nullptr;
  auto hr = library->CreateBlobFromFile(filename, nullptr, &blob);
  if (FAILED(hr)) {
    logerror(L"CreateShaderBlob failed. {} {}", filename, hr);
  }
  return blob;
}
ID3DBlob* Compile(IDxcCompiler* compiler, IDxcBlob *source, LPCWSTR sourceName,
                  LPCWSTR entryName, LPCWSTR targetProfile,
                  LPCWSTR* const arguments, const UINT32 argCount,
                  const DxcDefine* const defines, const UINT32 defineCount,
                  IDxcIncludeHandler* const include) {
  IDxcOperationResult* operationResult = nullptr;
  auto hr = compiler->Compile(source, sourceName, entryName, targetProfile,
                              arguments, argCount,
                              defines, defineCount,
                              include, &operationResult);
  if (FAILED(hr)) {
    logwarn(L"Compile failed. {} {}", sourceName, hr);
    IDxcBlobEncoding* error = nullptr;
    hr = operationResult->GetErrorBuffer(&error);
    if (SUCCEEDED(hr)) {
      std::vector<char> errlog(error->GetBufferSize() + 1);
      memcpy(errlog.data(), error->GetBufferPointer(), errlog.size());
      logwarn("Compile error message. {}", errlog.data());
    } else {
      logwarn("get compile error message failed. {}", hr);
    }
    return nullptr;
  }
  ID3DBlob* result = nullptr;
  hr = operationResult->GetResult((IDxcBlob**)&result);
  if (FAILED(hr)) {
    logwarn(L"Compiler GetResult failed. {} {}", sourceName, hr);
    return nullptr;
  }
  return result;
}
std::wstring GetAbsolutePath(LPCWSTR filename) {
  auto len = GetCurrentDirectory(0, nullptr);
  std::vector<char> buf(len);
  len = GetCurrentDirectory(buf.size(), buf.data());
  if (len + 1 != buf.size()) {
    logwarn(L"GetCurrentDirectory failed. {} {}!={}", filename, len + 1, buf.size());
    return L"";
  }
  std::wstring ret(len, L'\0');
  auto convlen = std::mbstowcs(ret.data(), buf.data(), len);
  if (convlen != len) {
    logwarn(L"mbstowcs failed. {} {}!={}", filename, convlen, len);
    return L"";
  }
  ret.append(L"\\");
  ret.append(filename);
  return ret;
}
Compiler::Compiler()
    : library_(nullptr)
    , compiler_(nullptr)
    , include_(nullptr)
{
  support_.Initialize();
  library_ = CreateLibrary(&support_);
  compiler_ = CreateCompiler(&support_);
  include_ =CreateDefaultIncludeHandler(library_);
}
Compiler::~Compiler() {
  compiler_->Release();
  library_->Release();
}
ID3DBlob* Compiler::CompileShader(LPCWSTR filename, LPCWSTR entryName, const LPCWSTR targetProfile,
                                  const DxcDefine* const defines, const UINT32 defineCount) {
  auto filepath = GetAbsolutePath(filename);
  auto shaderSource = CreateShaderBlob(library_, filepath.c_str());
  if (shaderSource == nullptr) return nullptr;
  auto shaderBinary = Compile(compiler_, shaderSource, filename, entryName, targetProfile, nullptr, 0, defines, defineCount, include_);
  shaderSource->Release();
  return shaderBinary;
}
ID3DBlob* Compiler::CompileShader(LPCWSTR filename, LPCWSTR entryName, const ShaderTarget shaderTarget,
                                  const DxcDefine* const defines, const UINT32 defineCount) {
  LPCWSTR targetlist[] = {
    L"ps_6_4",
    L"vs_6_4",
    L"cs_6_4",
    L"gs_6_4",
    L"ds_6_4",
    L"hs_6_4",
    L"lib_6_4",
  };
  return CompileShader(filename, entryName, targetlist[static_cast<uint8_t>(shaderTarget)],
                       defines, defineCount);
}
}
#include "doctest/doctest.h"
TEST_CASE("dxc test") {
  using namespace toy::dxcruntime;
  dxc::DxcDllSupport support;
  auto dllresult = support.Initialize();
  CHECK(dllresult == 0);
  auto library = CreateLibrary(&support);
  CHECK(library != nullptr);
  auto compiler = CreateCompiler(&support);
  CHECK(compiler != nullptr);
  auto include = CreateDefaultIncludeHandler(library);
  CHECK(include != nullptr);
  auto filename = L"shader\\test.hlsl";
  auto filepath = GetAbsolutePath(filename);
  loginfo(L"{}", filepath.c_str());
  CHECK(filepath.length() > wcslen(filename));
  auto shaderSource = CreateShaderBlob(library, filepath.c_str());
  CHECK(shaderSource != nullptr);
  auto shaderBinary = Compile(compiler, shaderSource, filename, L"main", L"vs_6_1", nullptr, 0, nullptr, 0, include);
  CHECK(shaderBinary != nullptr);
  shaderBinary->Release();
  shaderSource->Release();
  compiler->Release();
  library->Release();
}
TEST_CASE("dxc compile test") {
  using namespace toy::dxcruntime;
  Compiler compiler;
  auto filename = L"shader\\test.hlsl";
  auto shader1 = compiler.CompileShader(filename, L"main", ShaderTarget::VS, nullptr, 0);
  CHECK(shader1 != nullptr);
  auto shader2 = compiler.CompileShader(filename, L"main", L"vs_6_4", nullptr, 0);
  CHECK(shader2 != nullptr);
  CHECK(shader1->GetBufferSize() == shader2->GetBufferSize());
  CHECK(memcmp(shader1->GetBufferPointer(), shader2->GetBufferPointer(), shader2->GetBufferSize()) == 0);
  shader1->Release();
  shader2->Release();
}
