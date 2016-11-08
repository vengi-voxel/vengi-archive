#include "EditorScene.h"
#include "core/Common.h"
#include "core/Var.h"
#include "video/GLDebug.h"
#include "video/ScopedViewPort.h"
#include "core/Color.h"
#include "video/ScopedPolygonMode.h"
#include "video/ScopedScissor.h"
#include "video/ScopedFrameBuffer.h"
#include "voxel/model/MeshExporter.h"
#include "ui/UIApp.h"
#include "Model.h"

static inline Model& m() {
	static Model editorModel;
	return editorModel;
}

#define VOXELIZER_IMPLEMENTATION
#include "voxelizer.h"

EditorScene::EditorScene() :
		Super(),
		_bitmap((tb::UIRendererGL*) tb::g_renderer) {
	//_rawVolumeRenderer.setAmbientColor(core::Color::White.xyz());
	SetIsFocusable(true);
}

EditorScene::~EditorScene() {
	_axis.shutdown();
	_frameBuffer.shutdown();
	m().shutdown();
}

void EditorScene::render() {
	core_trace_scoped(EditorSceneRender);
	Model& mdl = m();
	const video::Camera& camera = _controller.camera();
	{
		video::ScopedPolygonMode polygonMode(camera.polygonMode());
		mdl.render(camera);
	}
	{
		video::ScopedPolygonMode polygonMode(video::PolygonMode::WireFrame);
		mdl.renderSelection(camera);
	}
	if (mdl.renderAxis()) {
		_axis.render(camera);
	}
}

void EditorScene::setKeyAction(Action action) {
	m()._keyAction = action;
}

void EditorScene::setInternalAction(Action action) {
	m().setAction(action);
}

void EditorScene::setAction(Action action) {
	m()._uiAction = action;
}

void EditorScene::setSelectionType(SelectType type) {
	m()._selectionType = type;
}

SelectType EditorScene::selectionType() const {
	return m()._selectionType;
}

bool EditorScene::newModel(bool force) {
	core_trace_scoped(EditorSceneNewModel);
	if (!m().newVolume(force)) {
		return false;
	}
	return true;
}

bool EditorScene::saveModel(std::string_view file) {
	core_trace_scoped(EditorSceneSaveModel);
	return m().save(file);
}

bool EditorScene::voxelizeModel(const video::MeshPtr& meshPtr) {
	const video::Mesh::Vertices& positions = meshPtr->vertices();
	const video::Mesh::Indices& indices = meshPtr->indices();
	vx_mesh_t* mesh = vx_mesh_alloc(positions.size(), indices.size());

	for (size_t f = 0; f < indices.size(); f++) {
		mesh->indices[f] = indices[f];
	}

	for (size_t v = 0; v < positions.size() / 3; v++) {
		const core::Vertex& vertex = positions[v];
		mesh->vertices[v].x = vertex._pos.x;
		mesh->vertices[v].y = vertex._pos.y;
		mesh->vertices[v].z = vertex._pos.z;
	}

	const glm::vec3& maxs = meshPtr->maxs();
	const float size = m().size();
	const glm::vec3& scale = maxs / size;
	const float precision = scale.x / 10.0f;

	vx_mesh_t* result = vx_voxelize(mesh, scale.x, scale.y, scale.z, precision);

	Log::info("Number of vertices: %i", (int)result->nvertices);
	Log::info("Number of indices: %i", (int)result->nindices);

	vx_mesh_free(result);
	vx_mesh_free(mesh);
	return false;
}

bool EditorScene::isEmpty() const {
	return m().empty();
}

bool EditorScene::exportModel(std::string_view file) {
	core_trace_scoped(EditorSceneExportModel);
	const io::FilePtr& filePtr = core::App::getInstance()->filesystem()->open(std::string(file));
	if (!(bool)filePtr) {
		return false;
	}
	return voxel::exportMesh(m().rawVolumeRenderer().mesh(), filePtr->getName().c_str());
}

bool EditorScene::loadModel(std::string_view file) {
	core_trace_scoped(EditorSceneLoadModel);
	if (!m().load(file)) {
		return false;
	}
	return true;
}

void EditorScene::resetCamera() {
	_controller.resetCamera(m().modelVolume());
}

void EditorScene::setVoxelType(voxel::VoxelType type) {
	m().setVoxelType(type);
}

void EditorScene::select(const glm::ivec3& pos) {
	m().select(pos);
}

bool EditorScene::renderAABB() const {
	return m().rawVolumeRenderer().renderAABB();
}

void EditorScene::setRenderAABB(bool renderAABB) {
	Model& mdl = m();
	mdl.rawVolumeRenderer().setRenderAABB(renderAABB);
}

bool EditorScene::renderGrid() const {
	return m().rawVolumeRenderer().renderGrid();
}

void EditorScene::setRenderGrid(bool renderGrid) {
	m().rawVolumeRenderer().setRenderGrid(renderGrid);
}

inline long EditorScene::actionExecutionDelay() const {
	return m()._actionExecutionDelay;
}

void EditorScene::setActionExecutionDelay(long actionExecutionDelay) {
	m()._actionExecutionDelay = actionExecutionDelay;
}

bool EditorScene::renderAxis() const {
	return m()._renderAxis;
}

void EditorScene::setRenderAxis(bool renderAxis) {
	m()._renderAxis = renderAxis;
}

bool EditorScene::isDirty() const {
	return m().dirty();
}

bool EditorScene::OnEvent(const tb::TBWidgetEvent &ev) {
	core_trace_scoped(EditorSceneOnEvent);
	const long now = core::App::getInstance()->currentMillis();
	Model& mdl = m();
	bool& mouseDown = _controller._mouseDown;
	if (ev.type == tb::EVENT_TYPE_POINTER_DOWN) {
		mouseDown = true;
		if (mdl.keyAction() != Action::None) {
			setInternalAction(mdl.keyAction());
		} else {
			setInternalAction(mdl.uiAction());
		}
		mdl.executeAction(mouseDown, now);
		return true;
	} else if (ev.type == tb::EVENT_TYPE_POINTER_UP) {
		mouseDown = false;
		setInternalAction(Action::None);
		return true;
	} else if (ev.type == tb::EVENT_TYPE_KEY_DOWN) {
		if (ev.modifierkeys) {
			if (ev.modifierkeys & tb::TB_ALT) {
				setKeyAction(Action::CopyVoxel);
			} else if (ev.modifierkeys & tb::TB_SHIFT) {
				setKeyAction(Action::OverrideVoxel);
			} else if (ev.modifierkeys & tb::TB_CTRL) {
				setKeyAction(Action::DeleteVoxel);
			}
			if (mouseDown) {
				setInternalAction(mdl.keyAction());
			}
			return true;
		}
	} else if (ev.type == tb::EVENT_TYPE_KEY_UP) {
		if (ev.modifierkeys && mdl.keyAction() != Action::None) {
			setKeyAction(Action::None);
			if (mouseDown) {
				setInternalAction(mdl.uiAction());
			}
			return true;
		}
	} else if (ev.type == tb::EVENT_TYPE_WHEEL && ev.delta_y != 0) {
		_controller.zoom((float)(ev.delta_y * 100));
		return true;
	} else if (ev.type == tb::EVENT_TYPE_POINTER_MOVE) {
		const bool relative = isRelativeMouseMode();
		const bool middle = isMiddleMouseButtonPressed();
		const bool alt = mdl.action() == Action::None && (ev.modifierkeys & tb::TB_ALT);
		if (_controller.move(relative || middle || alt, ev.target_x, ev.target_y)) {
			mdl.executeAction(mouseDown, now);
		}
		mdl.setMousePos(ev.target_x, ev.target_y);
		return true;
	}
	return Super::OnEvent(ev);
}

void EditorScene::OnResized(int oldw, int oldh) {
	core_trace_scoped(EditorSceneOnResized);
	Super::OnResized(oldw, oldh);
	const tb::TBRect& rect = GetRect();
	const glm::ivec2 dim(rect.w, rect.h);
	_controller.onResize(dim);
	_frameBuffer.shutdown();
	_frameBuffer.init(dim);
	_bitmap.Init(dim.x, dim.y, _frameBuffer.texture());
	m().onResize(dim);
}

void EditorScene::OnPaint(const PaintProps &paintProps) {
	core_trace_scoped(EditorSceneOnPaint);
	Super::OnPaint(paintProps);
	const glm::ivec2& dimension = _frameBuffer.dimension();
	const ui::UIRect& rect = GetRect();
	// the fbo is flipped in memory, we have to deal with it here
	const tb::TBRect srcRect(0, dimension.y, rect.w, -rect.h);
	tb::g_renderer->DrawBitmap(rect, srcRect, &_bitmap);
	tb::TBFontFace* font = GetFont();
	font->DrawString(0, 0, tb::TBColor(255.0f, 255.0f, 255.0f, 255.0f), _cameraMode.c_str());
}

void EditorScene::OnInflate(const tb::INFLATE_INFO &info) {
	Super::OnInflate(info);
	_axis.init();
	m().init();

	Controller::SceneCameraMode mode = Controller::SceneCameraMode::Free;
	const char *cameraMode = info.node->GetValueString("camera", "free");
	if (!strcmp(cameraMode, "top")) {
		mode = Controller::SceneCameraMode::Top;
	} else if (!strcmp(cameraMode, "front")) {
		mode = Controller::SceneCameraMode::Front;
	} else if (!strcmp(cameraMode, "left")) {
		mode = Controller::SceneCameraMode::Left;
	}
	_cameraMode = cameraMode;
	_controller.init(mode);
}

void EditorScene::OnProcess() {
	Super::OnProcess();
	if (!GetVisibilityCombined()) {
		return;
	}
	core_trace_scoped(EditorSceneOnProcess);

	const long deltaFrame = core::App::getInstance()->deltaFrame();
	_controller.update(deltaFrame);

	const bool skipCursor = isRelativeMouseMode();
	if (!m().trace(skipCursor, _controller.camera())) {
		return;
	}

	glClearColor(core::Color::Clear.r, core::Color::Clear.g, core::Color::Clear.b, core::Color::Clear.a);
	core_trace_scoped(EditorSceneRenderFramebuffer);
	_frameBuffer.bind(false);
	render();
	_frameBuffer.unbind();
}

namespace tb {
TB_WIDGET_FACTORY(EditorScene, TBValue::TYPE_NULL, WIDGET_Z_TOP) {}
}
