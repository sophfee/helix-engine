#include "model_manager.hpp"

ModelManager * ModelManager::singleton() {
	thread_local ModelManager singleton_;
	return &singleton_;
}
void ModelManager::dispose() {
	vertices_.clear();
	indices_.clear();
	materials_.clear();
	
	vertex_buffer_.dispose();
	index_buffer_.dispose();
	material_buffer_.dispose();
}

bool ModelManager::disposed() const {
	return vertex_buffer_.disposed() ||
			index_buffer_.disposed() ||
			material_buffer_.disposed();
}
