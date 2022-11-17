
#include <Hobgoblin/RmlUi.hpp>

namespace hg = jbatnozic::hobgoblin;

int main() {
    const auto* PATH = R"_(D:\SYNCHRONIZED\DEV2\C++\rmlui-sfml-techdemo\RmlUiAssets\assets\FPTEST\_fptest.rcss.fp)_";
    hg::rml::PreprocessRcssFile(PATH);
}