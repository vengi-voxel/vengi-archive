/**
 * @file
 */

#include "SceneGraphNode.h"
#include "voxel/RawVolume.h"

namespace voxel {

SceneGraphNode::SceneGraphNode(SceneGraphNode &&move) noexcept {
	_volume = move._volume;
	move._volume = nullptr;
	_name = move._name;
	_id = move._id;
	move._id = -1;
	_parent = move._parent;
	move._parent = -1;
	_modelId = move._modelId;
	move._modelId = -1;
	_mat = move._mat;
	_referencedNodeId = move._referencedNodeId;
	move._referencedNodeId = -1;
	_properties = core::move(move._properties);
	_children = core::move(move._children);
	_type = move._type;
	move._type = SceneGraphNodeType::Max;
	_visible = move._visible;
	_locked = move._locked;
	_pivot = move._pivot;
	_volumeOwned = move._volumeOwned;
	move._volumeOwned = false;
}

SceneGraphNode &SceneGraphNode::operator=(SceneGraphNode &&move) noexcept {
	if (&move == this) {
		return *this;
	}
	setVolume(move._volume, move._volumeOwned);
	move._volume = nullptr;
	_name = move._name;
	_id = move._id;
	move._id = -1;
	_parent = move._parent;
	move._parent = -1;
	_modelId = move._modelId;
	move._modelId = -1;
	_mat = move._mat;
	_referencedNodeId = move._referencedNodeId;
	move._referencedNodeId = -1;
	_properties = core::move(move._properties);
	_children = core::move(move._children);
	_type = move._type;
	_visible = move._visible;
	_locked = move._locked;
	_pivot = move._pivot;
	move._volumeOwned = false;
	return *this;
}

void SceneGraphNode::release() {
	if (_volumeOwned) {
		delete _volume;
	}
	_volume = nullptr;
}

void SceneGraphNode::releaseOwnership() {
	_volumeOwned = false;
}

void SceneGraphNode::setVolume(voxel::RawVolume *volume, bool transferOwnership) {
	release();
	_volumeOwned = transferOwnership;
	_volume = volume;
}

void SceneGraphNode::setVolume(const voxel::RawVolume *volume, bool transferOwnership) {
	release();
	_volumeOwned = transferOwnership;
	_volume = (voxel::RawVolume *)volume;
}

const voxel::Region &SceneGraphNode::region() const {
	if (_volume == nullptr) {
		return voxel::Region::InvalidRegion;
	}
	return _volume->region();
}

void SceneGraphNode::translate(const glm::ivec3 &v) {
	if (_volume != nullptr) {
		_volume->translate(v);
	}
}

void SceneGraphNode::addChild(int id) {
	_children.push_back(id);
}

const core::Buffer<int, 32> &SceneGraphNode::children() const {
	return _children;
}

const core::StringMap<core::String> &SceneGraphNode::properties() const {
	return _properties;
}

core::StringMap<core::String> &SceneGraphNode::properties() {
	return _properties;
}

core::String SceneGraphNode::property(const core::String& key) const {
	core::String value;
	_properties.get(key, value);
	return value;
}

void SceneGraphNode::addProperties(const core::StringMap<core::String>& map) {
	for (const auto& entry : map) {
		setProperty(entry->key, entry->value);
	}
}

} // namespace voxel
