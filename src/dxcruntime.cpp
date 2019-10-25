#include <dxgi1_6.h> // must include before dxcapi
#include "dxc/Support/dxcapi.use.h"
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
IDxcBlob* CreateShaderBlob(IDxcLibrary* library, LPCWSTR filename) {
  IDxcBlobEncoding* blob = nullptr;
  auto hr = library->CreateBlobFromFile(filename, nullptr, &blob);
  if (FAILED(hr)) {
    logerror(L"CreateShaderBlob failed. {} {}", filename, hr);
  }
  return blob;
}
IDxcBlob* Compile(IDxcCompiler* compiler, IDxcBlob *source, LPCWSTR sourceName,
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
    logwarn("Compile failed. {} {}", (char*)sourceName, hr);
    IDxcBlobEncoding* error = nullptr;
    hr = operationResult->GetErrorBuffer(&error);
    if (SUCCEEDED(hr)) {
      std::vector<char> errlog(error->GetBufferSize() + 1);
      memcpy(errlog.data(), error->GetBufferPointer(), errlog.size());
      logwarn("Compile error message. {}", errlog.data());
    }
    return nullptr;
  }
  IDxcBlob* result = nullptr;
  hr = operationResult->GetResult(&result);
  if (FAILED(hr)) {
    logwarn(L"Compiler GetResult failed. {} {}", sourceName, hr);
    return nullptr;
  }
  return result;
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
  auto blob = CreateShaderBlob(library, L"test.hlsl");
  CHECK(blob != nullptr);
}
