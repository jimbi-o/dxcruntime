#include <Windows.h>
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
  IDxcBlob* result = nullptr;
  hr = operationResult->GetResult(&result);
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
