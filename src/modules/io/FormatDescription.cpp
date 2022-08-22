/**
 * @file
 */

#include "FormatDescription.h"
#include "core/Algorithm.h"
#include "core/FourCC.h"
#include "core/String.h"
#include "core/StringUtil.h"

namespace io {

namespace format {

const FormatDescription* images() {
	static FormatDescription desc[] = {
		{"Portable Network Graphics", {"png"}, nullptr, 0u},
		{"JPEG", {"jpeg", "jpg"}, nullptr, 0u},
		{"Targa image file", {"tga"}, nullptr, 0u},
		{"Bitmap", {"bmp"}, nullptr, 0u},
		{"Photoshop", {"psd"}, nullptr, 0u},
		{"Graphics Interchange Format", {"gif"}, nullptr, 0u},
		{"Radiance rgbE", {"hdr"}, nullptr, 0u},
		{"Softimage PIC", {"pic"}, nullptr, 0u},
		{"Portable Anymap", {"pnm"}, nullptr, 0u},
		{"", {}, nullptr, 0u}
	};
	return desc;
}

const FormatDescription* gimpPalette() {
	static FormatDescription desc[] = {
		{"Gimp Palette", {"gpl"}, nullptr, 0u},
		{"", {}, nullptr, 0u}
	};
	return desc;
}

const FormatDescription* png() {
	static FormatDescription desc[] = {
		{"Portable Network Graphics", {"png"}, nullptr, 0u},
		{"", {}, nullptr, 0u}
	};
	return desc;
}

const FormatDescription* lua() {
	static FormatDescription desc[] = {
		{"LUA script", {"lua"}, nullptr, 0},
		{"", {}, nullptr, 0u}
	};
	return desc;
}

}

bool FormatDescription::matchesExtension(const core::String &fileExt) const {
	const core::String &lowerExt = fileExt.toLower();
	for (const core::String& ext : exts) {
		if (lowerExt == ext) {
			return true;
		}
	}
	return false;
}

bool FormatDescription::operator<(const FormatDescription &rhs) const {
	return name < rhs.name;
}

core::String FormatDescription::wildCard() const {
	core::String pattern;
	for (size_t i = 0; i < exts.size(); ++i) {
		if (i > 0) {
			pattern.append(",");
		}
		pattern += core::string::format("*.%s", exts[i].c_str());
	}
	return pattern;
}

bool isA(const core::String& file, const io::FormatDescription *desc) {
	const core::String& ext = core::string::extractExtension(file).toLower();
	for (; desc->valid(); ++desc) {
		if (desc->matchesExtension(ext)) {
			return true;
		}
	}
	return false;
}

bool isImage(const core::String& file) {
	return isA(file, io::format::images());
}

void createGroupPatterns(const FormatDescription *inputDesc, core::DynamicArray<io::FormatDescription> &groups) {
	core::DynamicArray<io::FormatDescription> descs;
	for (; inputDesc->valid(); ++inputDesc) {
		descs.emplace_back(*inputDesc);
	}
	core::sort(descs.begin(), descs.end(), core::Less<io::FormatDescription>());
	core::DynamicArray<io::FormatDescription> temp;
	core::String lastName;
	for (const io::FormatDescription &desc : descs) {
		core::String firstWord = desc.name;
		auto iter = firstWord.find_first_of(" ");
		if (iter != core::String::npos) {
			firstWord = firstWord.substr(0, iter);
		}
		if (lastName != firstWord) {
			if (temp.size() >= 2) {
				io::FormatDescriptionExtensions exts{};
				for (const io::FormatDescription &tmpDesc : temp) {
					for (const core::String &ext : tmpDesc.exts) {
						exts.push_back(ext);
					}
				}
				io::FormatDescription val{lastName, exts, nullptr, 0};
				groups.push_back(val);
			}
			lastName = firstWord;
			temp.clear();
		}
		temp.push_back(desc);
	}
	if (temp.size() >= 2) {
		io::FormatDescriptionExtensions exts{};
		for (const io::FormatDescription &tmpDesc : temp) {
			for (const core::String &ext : tmpDesc.exts) {
				exts.push_back(ext);
			}
		}
		io::FormatDescription val{lastName, exts, nullptr, 0};
		groups.push_back(val);
	}
}

core::String convertToFilePattern(const FormatDescription &desc) {
	core::String pattern;
	bool showName = false;
	if (!desc.name.empty()) {
		pattern += desc.name;
		if (!desc.exts.empty()) {
			pattern.append(" (");
			showName = true;
		}
	}
	pattern += desc.wildCard();
	if (showName) {
		pattern.append(")");
	}
	return pattern;
}

core::String convertToAllFilePattern(const FormatDescription *desc) {
	core::String pattern;
	int j = 0;
	while (desc->valid()) {
		if (j > 0) {
			pattern.append(",");
		}
		pattern += desc->wildCard();
		++desc;
		++j;
	}
	return j > 1 ? pattern : "";
}

}
