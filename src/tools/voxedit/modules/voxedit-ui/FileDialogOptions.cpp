/**
 * @file
 */

#include "FileDialogOptions.h"
#include "ui/imgui/IMGUIApp.h"
#include "ui/imgui/IMGUIEx.h"
#include "voxelformat/VolumeFormat.h"

void fileDialogOptions(video::OpenFileMode mode, const io::FormatDescription *desc) {
	if (mode == video::OpenFileMode::Directory) {
		return;
	}
	if (desc == nullptr) {
		return;
	}

	const bool forceApplyOptions = (desc->flags & FORMAT_FLAG_ALL) == FORMAT_FLAG_ALL;
	if (forceApplyOptions || voxelformat::isMeshFormat(*desc)) {
		ImGui::InputVarFloat("Uniform scale", cfg::VoxformatScale);
		ImGui::InputVarFloat("X axis scale", cfg::VoxformatScaleX);
		ImGui::InputVarFloat("Y axis scale", cfg::VoxformatScaleY);
		ImGui::InputVarFloat("Z axis scale", cfg::VoxformatScaleZ);

		if (mode == video::OpenFileMode::Save) {
			ImGui::CheckboxVar("Merge quads", cfg::VoxformatMergequads);
			ImGui::CheckboxVar("Reuse vertices", cfg::VoxformatReusevertices);
			ImGui::CheckboxVar("Ambient occlusion", cfg::VoxformatAmbientocclusion);
			ImGui::CheckboxVar("Apply transformations", cfg::VoxformatTransform);
			ImGui::CheckboxVar("Exports quads", cfg::VoxformatQuads);
			ImGui::CheckboxVar("Vertex colors", cfg::VoxformatWithcolor);
			ImGui::CheckboxVar("Texture coordinates", cfg::VoxformatWithtexcoords);
		} else if (mode == video::OpenFileMode::Open) {
			ImGui::CheckboxVar("Fill hollow", cfg::VoxformatFillHollow);
		}
	} else if (forceApplyOptions || (desc->name == "Tiberian Sun" && desc->matchesExtension("vxl"))) {
		if (mode == video::OpenFileMode::Save) {
			const char *normalTypes[] = {nullptr, nullptr, "Tiberian Sun", nullptr, "Red Alert"};
			const core::VarPtr &normalTypeVar = core::Var::getSafe(cfg::VoxformatVXLNormalType);
			const int currentNormalType = normalTypeVar->intVal();

			if (ImGui::BeginCombo("Normal type", normalTypes[currentNormalType])) {
				for (int i = 0; i < lengthof(normalTypes); ++i) {
					const char *normalType = normalTypes[i];
					if (normalType == nullptr) {
						continue;
					}
					const bool selected = i == currentNormalType;
					if (ImGui::Selectable(normalType, selected)) {
						normalTypeVar->setVal(core::string::toString(i));
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
	} else if (mode == video::OpenFileMode::Save && desc->matchesExtension("qbt")) {
		ImGui::CheckboxVar("Palette mode", cfg::VoxformatQBTPaletteMode);
	}
}
