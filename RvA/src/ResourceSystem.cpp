#include "ResourceSystem.h"

void ResourceSystem::loadResources() {
	addShader("hole", LoadShader(nullptr, "assets/shaders/hole_shader.fs"));
}

void ResourceSystem::unloadResources() {
	for (auto& [id, shader] : m_shaders) {
		UnloadShader(shader);
	}
	m_shaders.clear();
}

Shader ResourceSystem::getShader(const std::string& id) {
	auto it = m_shaders.find(id);
	if (it != m_shaders.end()) {
		return it->second;
	}
	return {};
}

void ResourceSystem::addShader(std::string id, Shader shader) {
	m_shaders.insert({std::move(id), std::move(shader)});
}
