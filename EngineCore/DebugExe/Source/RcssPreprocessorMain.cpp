// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

// clang-format off


#include <Hobgoblin/RmlUi.hpp>

namespace hg = jbatnozic::hobgoblin;

int main() {
    const auto* PATH = R"_(D:\SYNCHRONIZED\DEV2\C++\rmlui-sfml-techdemo\RmlUiAssets\assets\FPTEST\_fptest.rcss.fp)_";
    hg::rml::PreprocessRcssFile(PATH);
}

// clang-format on
