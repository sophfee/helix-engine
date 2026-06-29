#pragma once

#include "types.hpp"

typedef  struct {
	u32 count;
	u32 instanceCount;
	u32 firstVertex;
	u32 baseInstance;
	// Optional user-defined data goes here - if nothing, stride is 0
} DrawArraysIndirectCommand;

typedef struct {
	u32 count;
	u32 instanceCount;
	u32 firstIndex;
	i32 baseVertex;
	u32 baseInstance;
} DrawElementsIndirectCommand;

typedef struct {
	u32 header;
} TerminateSequenceCommandNV;

typedef struct {
	u32 header;
} NOPCommandNV;

typedef  struct {
	u32 header;
	u32 count;
	u32 firstIndex;
	u32 baseVertex;
} DrawElementsCommandNV;

typedef  struct {
	u32 header;
	u32 count;
	u32 first;
} DrawArraysCommandNV;

typedef  struct {
	u32 header;
	u32 mode;
	u32 count;
	u32 instanceCount;
	u32 firstIndex;
	u32 baseVertex;
	u32 baseInstance;
} DrawElementsInstancedCommandNV;

typedef  struct {
	u32 header;
	u32 mode;
	u32 count;
	u32 instanceCount;
	u32 first;
	u32 baseInstance;
} DrawArraysInstancedCommandNV;

typedef struct {
	u32 header;
	u32 addressLo;
	u32 addressHi;
	u32 typeSizeInByte;
} ElementAddressCommandNV;

typedef struct {
	u32 header;
	u32 index;
	u32 addressLo;
	u32 addressHi;
} AttributeAddressCommandNV;

typedef struct {
	u32 header;
	u16 index;
	u16 stage;
	u32 addressLo;
	u32 addressHi;
} UniformAddressCommandNV;

typedef struct {
	u32 header;
	f32 red;
	f32 green;
	f32 blue;
	f32 alpha;
} BlendColorCommandNV;

typedef struct {
	u32 header;
	u32 frontStencilRef;
	u32 backStencilRef;
} StencilRefCommandNV;

typedef struct {
	u32 header;
	f32 lineWidth;
} LineWidthCommandNV;

typedef struct {
	u32 header;
	f32 scale;
	f32 bias;
} PolygonOffsetCommandNV;

typedef struct {
	u32 header;
	f32 alphaRef;
} AlphaRefCommandNV;

typedef struct {
	u32 header;
	u32 x;
	u32 y;
	u32 width;
	u32 height;
} ViewportCommandNV; // only ViewportIndex 0

typedef struct {
	u32 header;
	u32 x;
	u32 y;
	u32 width;
	u32 height;
} ScissorCommandNV; // only ViewportIndex 0

typedef struct {
	u32 header;
	u32 frontFace; // 0 for CW, 1 for CCW
} FrontFaceCommandNV;