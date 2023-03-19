#include <hex/plugin.hpp>
#include <hex/api/content_registry.hpp>
#include <hex/ui/view.hpp>

#include <romfs/romfs.hpp>
#include <nlohmann/json.hpp>

IMHEX_PLUGIN_SETUP("Pattern Helper", "jcreedon", "Pattern Helper") {

    using namespace hex;

    for (auto &path : romfs::list("lang"))
        hex::ContentRegistry::Language::addLocalization(nlohmann::json::parse(romfs::get(path).string()));

    ContentRegistry::Interface::addMenuItem("hex.builtin.menu.edit", 1150, [&] {
        bool providerValid = ImHexApi::Provider::isValid();
        auto selection = ImHexApi::HexEditor::getSelection();

        if (ImGui::BeginMenu("hex.builtin.menu.edit.copy_as_pattern"_lang, selection.has_value() && providerValid)) {
            if (ImGui::MenuItem("hex.builtin.menu.edit.copy_as_pattern.array_u8"_lang))
                ImGui::SetClipboardText(format("u8 unknown_{0:x}[{1:d}] @ 0x{0:x};", selection->getStartAddress(),
                                               selection->size).c_str());
            if (ImGui::MenuItem("hex.builtin.menu.edit.copy_as_pattern.array_u16"_lang, nullptr, false,
                                selection->size % 2 == 0))
                ImGui::SetClipboardText(format("u16 unknown_{0:x}[{1:d}] @ 0x{0:x};", selection->getStartAddress(),
                                               selection->size / 2).c_str());
            if (ImGui::MenuItem("hex.builtin.menu.edit.copy_as_pattern.array_u32"_lang, nullptr, false,
                                selection->size % 4 == 0))
                ImGui::SetClipboardText(format("u32 unknown_{0:x}[{1:d}] @ 0x{0:x};", selection->getStartAddress(),
                                               selection->size / 4).c_str());
            if (ImGui::MenuItem("hex.builtin.menu.edit.copy_as_pattern.struct"_lang)) {
                int count = 0;
                int start_padding = static_cast<int>((4ull - (selection->getStartAddress() % 4ull)) & 0b11);
                int fill = static_cast<int>(selection->size / 4);
                int end_padding = static_cast<int>((selection->size - start_padding) & 0b11);

                std::string struct_def = format("struct Unknown_{0:x} {{\n", selection->getStartAddress());

                for (int i = 0; i < start_padding; i++) {
                    struct_def.append(format("\tu8 unknown_{:02};\n", count));
                    count++;
                }
                for (int i = 0; i < fill; i++) {
                    struct_def.append(format("\tu32 unknown_{:02};\n", count));
                    count++;
                }
                for (int i = 0; i < end_padding; i++) {
                    struct_def.append(format("\tu8 unknown_{:02};\n", count));
                    count++;
                }

                struct_def.append("};\n");
                struct_def.append(format("Unknown_{0:x} unknown_{0:x}[1] @ 0x{0:x};", selection->getStartAddress()));
                ImGui::SetClipboardText(struct_def.c_str());
            }

            ImGui::EndMenu();
        }
    });
}