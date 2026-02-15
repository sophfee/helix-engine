#include "mesh.hpp"
#include "gltf.h"

CMesh::CMesh() {
}
CMesh::CMesh(GltfData_t &data) {
	size_t const numMeshes = data.accessors.size() / 4; //< This cane be redone in a more reliable way, this assumes there are 4 accessors for each mesh: pos, nor, tex, elem.
	for (size_t i = 0; i < numMeshes; i++) {
		CVertexArray vertex_array;
		CBuffer positions, normals, texcoords, elements;
		vertex_array.setAttribute(GenericPositionAttribute);
		vertex_array.setAttribute(GenericNormalAttribute);
		vertex_array.setAttribute(GenericTexCoordAttribute);
		
	}
}

CMesh::~CMesh() {
}