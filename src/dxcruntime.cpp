#include <dxgi1_6.h> // must include before dxcapi
#include "dxc/Support/dxcapi.use.h"
#include "doctest/doctest.h"
TEST_CASE("dxc test") {
  dxc::DxcDllSupport dxcdllsupport;
  auto dllresult = dxcdllsupport.Initialize();
  CHECK(dllresult == 0);
}
