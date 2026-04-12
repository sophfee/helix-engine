#pragma once

#include "opengl_enums.hpp"

namespace gl {
	constexpr auto ToString(AttribMask value) {
		switch(value){
			case AttribMask::CurrentBit:return "CurrentBit";
			case AttribMask::PointBit:return "PointBit";
			case AttribMask::LineBit:return "LineBit";
			case AttribMask::PolygonBit:return "PolygonBit";
			case AttribMask::PolygonStippleBit:return "PolygonStippleBit";
			case AttribMask::PixelModeBit:return "PixelModeBit";
			case AttribMask::LightingBit:return "LightingBit";
			case AttribMask::FogBit:return "FogBit";
			case AttribMask::DepthBufferBit:return "DepthBufferBit";
			case AttribMask::AccumBufferBit:return "AccumBufferBit";
			case AttribMask::StencilBufferBit:return "StencilBufferBit";
			case AttribMask::ViewportBit:return "ViewportBit";
			case AttribMask::TransformBit:return "TransformBit";
			case AttribMask::EnableBit:return "EnableBit";
			case AttribMask::ColorBufferBit:return "ColorBufferBit";
			case AttribMask::HintBit:return "HintBit";
			case AttribMask::EvalBit:return "EvalBit";
			case AttribMask::ListBit:return "ListBit";
			case AttribMask::TextureBit:return "TextureBit";
			case AttribMask::ScissorBit:return "ScissorBit";
			case AttribMask::MultisampleBit:return "MultisampleBit";
			case AttribMask::/* Guaranteed to mark all attribute groups at once */ AllAttribBits:return "/* Guaranteed to mark all attribute groups at once */ AllAttribBits";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClearBufferMask value) {
		switch(value){
			case ClearBufferMask::DepthBufferBit:return "DepthBufferBit";
			case ClearBufferMask::AccumBufferBit:return "AccumBufferBit";
			case ClearBufferMask::StencilBufferBit:return "StencilBufferBit";
			case ClearBufferMask::ColorBufferBit:return "ColorBufferBit";
			case ClearBufferMask::/* Collides with AttribMask bit GL_HINT_BIT. OK since this token is for OpenGL ES 2, which doesn't have attribute groups. */ CoverageBufferBitNv:return "/* Collides with AttribMask bit GL_HINT_BIT. OK since this token is for OpenGL ES 2, which doesn't have attribute groups. */ CoverageBufferBitNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferStorageMask value) {
		switch(value){
			case BufferStorageMask::DynamicStorageBit:return "DynamicStorageBit";
			case BufferStorageMask::ClientStorageBit:return "ClientStorageBit";
			case BufferStorageMask::SparseStorageBitArb:return "SparseStorageBitArb";
			case BufferStorageMask::LgpuSeparateStorageBitNvx:return "LgpuSeparateStorageBitNvx";
			case BufferStorageMask::ExternalStorageBitNvx:return "ExternalStorageBitNvx";
			case BufferStorageMask::MapReadBit:return "MapReadBit";
			case BufferStorageMask::MapWriteBit:return "MapWriteBit";
			case BufferStorageMask::MapPersistentBit:return "MapPersistentBit";
			case BufferStorageMask::MapCoherentBit:return "MapCoherentBit";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClientAttribMask value) {
		switch(value){
			case ClientAttribMask::ClientPixelStoreBit:return "ClientPixelStoreBit";
			case ClientAttribMask::ClientVertexArrayBit:return "ClientVertexArrayBit";
			case ClientAttribMask::ClientAllAttribBits:return "ClientAllAttribBits";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ContextFlagMask value) {
		switch(value){
			case ContextFlagMask::ContextFlagForwardCompatibleBit:return "ContextFlagForwardCompatibleBit";
			case ContextFlagMask::ContextFlagDebugBit:return "ContextFlagDebugBit";
			case ContextFlagMask::ContextFlagRobustAccessBit:return "ContextFlagRobustAccessBit";
			case ContextFlagMask::ContextFlagNoErrorBit:return "ContextFlagNoErrorBit";
			case ContextFlagMask::ContextFlagProtectedContentBitExt:return "ContextFlagProtectedContentBitExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ContextProfileMask value) {
		switch(value){
			case ContextProfileMask::ContextCoreProfileBit:return "ContextCoreProfileBit";
			case ContextProfileMask::ContextCompatibilityProfileBit:return "ContextCompatibilityProfileBit";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapBufferAccessMask value) {
		switch(value){
			case MapBufferAccessMask::MapReadBit:return "MapReadBit";
			case MapBufferAccessMask::MapWriteBit:return "MapWriteBit";
			case MapBufferAccessMask::MapInvalidateRangeBit:return "MapInvalidateRangeBit";
			case MapBufferAccessMask::MapInvalidateBufferBit:return "MapInvalidateBufferBit";
			case MapBufferAccessMask::MapFlushExplicitBit:return "MapFlushExplicitBit";
			case MapBufferAccessMask::MapUnsynchronizedBit:return "MapUnsynchronizedBit";
			case MapBufferAccessMask::MapPersistentBit:return "MapPersistentBit";
			case MapBufferAccessMask::MapCoherentBit:return "MapCoherentBit";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MemoryBarrierMask value) {
		switch(value){
			case MemoryBarrierMask::VertexAttribArrayBarrierBit:return "VertexAttribArrayBarrierBit";
			case MemoryBarrierMask::ElementArrayBarrierBit:return "ElementArrayBarrierBit";
			case MemoryBarrierMask::UniformBarrierBit:return "UniformBarrierBit";
			case MemoryBarrierMask::TextureFetchBarrierBit:return "TextureFetchBarrierBit";
			case MemoryBarrierMask::ShaderGlobalAccessBarrierBitNv:return "ShaderGlobalAccessBarrierBitNv";
			case MemoryBarrierMask::ShaderImageAccessBarrierBit:return "ShaderImageAccessBarrierBit";
			case MemoryBarrierMask::CommandBarrierBit:return "CommandBarrierBit";
			case MemoryBarrierMask::PixelBufferBarrierBit:return "PixelBufferBarrierBit";
			case MemoryBarrierMask::TextureUpdateBarrierBit:return "TextureUpdateBarrierBit";
			case MemoryBarrierMask::BufferUpdateBarrierBit:return "BufferUpdateBarrierBit";
			case MemoryBarrierMask::FramebufferBarrierBit:return "FramebufferBarrierBit";
			case MemoryBarrierMask::TransformFeedbackBarrierBit:return "TransformFeedbackBarrierBit";
			case MemoryBarrierMask::AtomicCounterBarrierBit:return "AtomicCounterBarrierBit";
			case MemoryBarrierMask::ShaderStorageBarrierBit:return "ShaderStorageBarrierBit";
			case MemoryBarrierMask::ClientMappedBufferBarrierBit:return "ClientMappedBufferBarrierBit";
			case MemoryBarrierMask::QueryBufferBarrierBit:return "QueryBufferBarrierBit";
			case MemoryBarrierMask::AllBarrierBits:return "AllBarrierBits";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(OcclusionQueryEventMaskAMD value) {
		switch(value){
			case OcclusionQueryEventMaskAMD::QueryDepthPassEventBitAmd:return "QueryDepthPassEventBitAmd";
			case OcclusionQueryEventMaskAMD::QueryDepthFailEventBitAmd:return "QueryDepthFailEventBitAmd";
			case OcclusionQueryEventMaskAMD::QueryStencilFailEventBitAmd:return "QueryStencilFailEventBitAmd";
			case OcclusionQueryEventMaskAMD::QueryDepthBoundsFailEventBitAmd:return "QueryDepthBoundsFailEventBitAmd";
			case OcclusionQueryEventMaskAMD::QueryAllEventBitsAmd:return "QueryAllEventBitsAmd";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SyncObjectMask value) {
		switch(value){
			case SyncObjectMask::SyncFlushCommandsBit:return "SyncFlushCommandsBit";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(UseProgramStageMask value) {
		switch(value){
			case UseProgramStageMask::VertexShaderBit:return "VertexShaderBit";
			case UseProgramStageMask::FragmentShaderBit:return "FragmentShaderBit";
			case UseProgramStageMask::GeometryShaderBit:return "GeometryShaderBit";
			case UseProgramStageMask::TessControlShaderBit:return "TessControlShaderBit";
			case UseProgramStageMask::TessEvaluationShaderBit:return "TessEvaluationShaderBit";
			case UseProgramStageMask::ComputeShaderBit:return "ComputeShaderBit";
			case UseProgramStageMask::MeshShaderBitNv:return "MeshShaderBitNv";
			case UseProgramStageMask::TaskShaderBitNv:return "TaskShaderBitNv";
			case UseProgramStageMask::AllShaderBits:return "AllShaderBits";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SubgroupSupportedFeatures value) {
		switch(value){
			case SubgroupSupportedFeatures::SubgroupFeatureBasicBitKhr:return "SubgroupFeatureBasicBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureVoteBitKhr:return "SubgroupFeatureVoteBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureArithmeticBitKhr:return "SubgroupFeatureArithmeticBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureBallotBitKhr:return "SubgroupFeatureBallotBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureShuffleBitKhr:return "SubgroupFeatureShuffleBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureShuffleRelativeBitKhr:return "SubgroupFeatureShuffleRelativeBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureClusteredBitKhr:return "SubgroupFeatureClusteredBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeatureQuadBitKhr:return "SubgroupFeatureQuadBitKhr";
			case SubgroupSupportedFeatures::SubgroupFeaturePartitionedBitNv:return "SubgroupFeaturePartitionedBitNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureStorageMaskAMD value) {
		switch(value){
			case TextureStorageMaskAMD::TextureStorageSparseBitAmd:return "TextureStorageSparseBitAmd";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderDestMaskATI value) {
		switch(value){
			case FragmentShaderDestMaskATI::RedBitAti:return "RedBitAti";
			case FragmentShaderDestMaskATI::GreenBitAti:return "GreenBitAti";
			case FragmentShaderDestMaskATI::BlueBitAti:return "BlueBitAti";
			case FragmentShaderDestMaskATI::None:return "None";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderDestModMaskATI value) {
		switch(value){
			case FragmentShaderDestModMaskATI::_2XBitAti:return "_2XBitAti";
			case FragmentShaderDestModMaskATI::_4XBitAti:return "_4XBitAti";
			case FragmentShaderDestModMaskATI::_8XBitAti:return "_8XBitAti";
			case FragmentShaderDestModMaskATI::HalfBitAti:return "HalfBitAti";
			case FragmentShaderDestModMaskATI::QuarterBitAti:return "QuarterBitAti";
			case FragmentShaderDestModMaskATI::EighthBitAti:return "EighthBitAti";
			case FragmentShaderDestModMaskATI::SaturateBitAti:return "SaturateBitAti";
			case FragmentShaderDestModMaskATI::None:return "None";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderColorModMaskATI value) {
		switch(value){
			case FragmentShaderColorModMaskATI::_2XBitAti:return "_2XBitAti";
			case FragmentShaderColorModMaskATI::CompBitAti:return "CompBitAti";
			case FragmentShaderColorModMaskATI::NegateBitAti:return "NegateBitAti";
			case FragmentShaderColorModMaskATI::BiasBitAti:return "BiasBitAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TraceMaskMESA value) {
		switch(value){
			case TraceMaskMESA::TraceOperationsBitMesa:return "TraceOperationsBitMesa";
			case TraceMaskMESA::TracePrimitivesBitMesa:return "TracePrimitivesBitMesa";
			case TraceMaskMESA::TraceArraysBitMesa:return "TraceArraysBitMesa";
			case TraceMaskMESA::TraceTexturesBitMesa:return "TraceTexturesBitMesa";
			case TraceMaskMESA::TracePixelsBitMesa:return "TracePixelsBitMesa";
			case TraceMaskMESA::TraceErrorsBitMesa:return "TraceErrorsBitMesa";
			case TraceMaskMESA::TraceAllBitsMesa:return "TraceAllBitsMesa";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathFontStyle value) {
		switch(value){
			case PathFontStyle::BoldBitNv:return "BoldBitNv";
			case PathFontStyle::ItalicBitNv:return "ItalicBitNv";
			case PathFontStyle::None:return "None";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathMetricMask value) {
		switch(value){
			case PathMetricMask::GlyphWidthBitNv:return "GlyphWidthBitNv";
			case PathMetricMask::GlyphHeightBitNv:return "GlyphHeightBitNv";
			case PathMetricMask::GlyphHorizontalBearingXBitNv:return "GlyphHorizontalBearingXBitNv";
			case PathMetricMask::GlyphHorizontalBearingYBitNv:return "GlyphHorizontalBearingYBitNv";
			case PathMetricMask::GlyphHorizontalBearingAdvanceBitNv:return "GlyphHorizontalBearingAdvanceBitNv";
			case PathMetricMask::GlyphVerticalBearingXBitNv:return "GlyphVerticalBearingXBitNv";
			case PathMetricMask::GlyphVerticalBearingYBitNv:return "GlyphVerticalBearingYBitNv";
			case PathMetricMask::GlyphVerticalBearingAdvanceBitNv:return "GlyphVerticalBearingAdvanceBitNv";
			case PathMetricMask::GlyphHasKerningBitNv:return "GlyphHasKerningBitNv";
			case PathMetricMask::FontXMinBoundsBitNv:return "FontXMinBoundsBitNv";
			case PathMetricMask::FontYMinBoundsBitNv:return "FontYMinBoundsBitNv";
			case PathMetricMask::FontXMaxBoundsBitNv:return "FontXMaxBoundsBitNv";
			case PathMetricMask::FontYMaxBoundsBitNv:return "FontYMaxBoundsBitNv";
			case PathMetricMask::FontUnitsPerEmBitNv:return "FontUnitsPerEmBitNv";
			case PathMetricMask::FontAscenderBitNv:return "FontAscenderBitNv";
			case PathMetricMask::FontDescenderBitNv:return "FontDescenderBitNv";
			case PathMetricMask::FontHeightBitNv:return "FontHeightBitNv";
			case PathMetricMask::FontMaxAdvanceWidthBitNv:return "FontMaxAdvanceWidthBitNv";
			case PathMetricMask::FontMaxAdvanceHeightBitNv:return "FontMaxAdvanceHeightBitNv";
			case PathMetricMask::FontUnderlinePositionBitNv:return "FontUnderlinePositionBitNv";
			case PathMetricMask::FontUnderlineThicknessBitNv:return "FontUnderlineThicknessBitNv";
			case PathMetricMask::FontHasKerningBitNv:return "FontHasKerningBitNv";
			case PathMetricMask::FontNumGlyphIndicesBitNv:return "FontNumGlyphIndicesBitNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PerformanceQueryCapsMaskINTEL value) {
		switch(value){
			case PerformanceQueryCapsMaskINTEL::PerfquerySingleContextIntel:return "PerfquerySingleContextIntel";
			case PerformanceQueryCapsMaskINTEL::PerfqueryGlobalContextIntel:return "PerfqueryGlobalContextIntel";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexHintsMaskPGI value) {
		switch(value){
			case VertexHintsMaskPGI::Vertex23BitPgi:return "Vertex23BitPgi";
			case VertexHintsMaskPGI::Vertex4BitPgi:return "Vertex4BitPgi";
			case VertexHintsMaskPGI::Color3BitPgi:return "Color3BitPgi";
			case VertexHintsMaskPGI::Color4BitPgi:return "Color4BitPgi";
			case VertexHintsMaskPGI::EdgeFlagBitPgi:return "EdgeflagBitPgi";
			case VertexHintsMaskPGI::IndexBitPgi:return "IndexBitPgi";
			case VertexHintsMaskPGI::MatAmbientBitPgi:return "MatAmbientBitPgi";
			case VertexHintsMaskPGI::MatAmbientAndDiffuseBitPgi:return "MatAmbientAndDiffuseBitPgi";
			case VertexHintsMaskPGI::MatDiffuseBitPgi:return "MatDiffuseBitPgi";
			case VertexHintsMaskPGI::MatEmissionBitPgi:return "MatEmissionBitPgi";
			case VertexHintsMaskPGI::MatColorIndexesBitPgi:return "MatColorIndexesBitPgi";
			case VertexHintsMaskPGI::MatShininessBitPgi:return "MatShininessBitPgi";
			case VertexHintsMaskPGI::MatSpecularBitPgi:return "MatSpecularBitPgi";
			case VertexHintsMaskPGI::NormalBitPgi:return "NormalBitPgi";
			case VertexHintsMaskPGI::TexCoord1BitPgi:return "Texcoord1BitPgi";
			case VertexHintsMaskPGI::TexCoord2BitPgi:return "Texcoord2BitPgi";
			case VertexHintsMaskPGI::TexCoord3BitPgi:return "Texcoord3BitPgi";
			case VertexHintsMaskPGI::TexCoord4BitPgi:return "Texcoord4BitPgi";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferBitQCOM value) {
		switch(value){
			case BufferBitQCOM::ColorBufferBit0Qcom:return "ColorBufferBit0Qcom";
			case BufferBitQCOM::ColorBufferBit1Qcom:return "ColorBufferBit1Qcom";
			case BufferBitQCOM::ColorBufferBit2Qcom:return "ColorBufferBit2Qcom";
			case BufferBitQCOM::ColorBufferBit3Qcom:return "ColorBufferBit3Qcom";
			case BufferBitQCOM::ColorBufferBit4Qcom:return "ColorBufferBit4Qcom";
			case BufferBitQCOM::ColorBufferBit5Qcom:return "ColorBufferBit5Qcom";
			case BufferBitQCOM::ColorBufferBit6Qcom:return "ColorBufferBit6Qcom";
			case BufferBitQCOM::ColorBufferBit7Qcom:return "ColorBufferBit7Qcom";
			case BufferBitQCOM::DepthBufferBit0Qcom:return "DepthBufferBit0Qcom";
			case BufferBitQCOM::DepthBufferBit1Qcom:return "DepthBufferBit1Qcom";
			case BufferBitQCOM::DepthBufferBit2Qcom:return "DepthBufferBit2Qcom";
			case BufferBitQCOM::DepthBufferBit3Qcom:return "DepthBufferBit3Qcom";
			case BufferBitQCOM::DepthBufferBit4Qcom:return "DepthBufferBit4Qcom";
			case BufferBitQCOM::DepthBufferBit5Qcom:return "DepthBufferBit5Qcom";
			case BufferBitQCOM::DepthBufferBit6Qcom:return "DepthBufferBit6Qcom";
			case BufferBitQCOM::DepthBufferBit7Qcom:return "DepthBufferBit7Qcom";
			case BufferBitQCOM::StencilBufferBit0Qcom:return "StencilBufferBit0Qcom";
			case BufferBitQCOM::StencilBufferBit1Qcom:return "StencilBufferBit1Qcom";
			case BufferBitQCOM::StencilBufferBit2Qcom:return "StencilBufferBit2Qcom";
			case BufferBitQCOM::StencilBufferBit3Qcom:return "StencilBufferBit3Qcom";
			case BufferBitQCOM::StencilBufferBit4Qcom:return "StencilBufferBit4Qcom";
			case BufferBitQCOM::StencilBufferBit5Qcom:return "StencilBufferBit5Qcom";
			case BufferBitQCOM::StencilBufferBit6Qcom:return "StencilBufferBit6Qcom";
			case BufferBitQCOM::StencilBufferBit7Qcom:return "StencilBufferBit7Qcom";
			case BufferBitQCOM::MultisampleBufferBit0Qcom:return "MultisampleBufferBit0Qcom";
			case BufferBitQCOM::MultisampleBufferBit1Qcom:return "MultisampleBufferBit1Qcom";
			case BufferBitQCOM::MultisampleBufferBit2Qcom:return "MultisampleBufferBit2Qcom";
			case BufferBitQCOM::MultisampleBufferBit3Qcom:return "MultisampleBufferBit3Qcom";
			case BufferBitQCOM::MultisampleBufferBit4Qcom:return "MultisampleBufferBit4Qcom";
			case BufferBitQCOM::MultisampleBufferBit5Qcom:return "MultisampleBufferBit5Qcom";
			case BufferBitQCOM::MultisampleBufferBit6Qcom:return "MultisampleBufferBit6Qcom";
			case BufferBitQCOM::MultisampleBufferBit7Qcom:return "MultisampleBufferBit7Qcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FoveationConfigBitQCOM value) {
		switch(value){
			case FoveationConfigBitQCOM::FoveationEnableBitQcom:return "FoveationEnableBitQcom";
			case FoveationConfigBitQCOM::FoveationScaledBinMethodBitQcom:return "FoveationScaledBinMethodBitQcom";
			case FoveationConfigBitQCOM::FoveationSubsampledLayoutMethodBitQcom:return "FoveationSubsampledLayoutMethodBitQcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FfdMaskSGIX value) {
		switch(value){
			case FfdMaskSGIX::TextureDeformationBitSgix:return "TextureDeformationBitSgix";
			case FfdMaskSGIX::GeometryDeformationBitSgix:return "GeometryDeformationBitSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CommandOpcodesNV value) {
		switch(value){
			case CommandOpcodesNV::TerminateSequenceCommandNv:return "TerminateSequenceCommandNv";
			case CommandOpcodesNV::NopCommandNv:return "NopCommandNv";
			case CommandOpcodesNV::DrawElementsCommandNv:return "DrawElementsCommandNv";
			case CommandOpcodesNV::DrawArraysCommandNv:return "DrawArraysCommandNv";
			case CommandOpcodesNV::DrawElementsStripCommandNv:return "DrawElementsStripCommandNv";
			case CommandOpcodesNV::DrawArraysStripCommandNv:return "DrawArraysStripCommandNv";
			case CommandOpcodesNV::DrawElementsInstancedCommandNv:return "DrawElementsInstancedCommandNv";
			case CommandOpcodesNV::DrawArraysInstancedCommandNv:return "DrawArraysInstancedCommandNv";
			case CommandOpcodesNV::ElementAddressCommandNv:return "ElementAddressCommandNv";
			case CommandOpcodesNV::AttributeAddressCommandNv:return "AttributeAddressCommandNv";
			case CommandOpcodesNV::UniformAddressCommandNv:return "UniformAddressCommandNv";
			case CommandOpcodesNV::BlendColorCommandNv:return "BlendColorCommandNv";
			case CommandOpcodesNV::StencilRefCommandNv:return "StencilRefCommandNv";
			case CommandOpcodesNV::LineWidthCommandNv:return "LineWidthCommandNv";
			case CommandOpcodesNV::PolygonOffsetCommandNv:return "PolygonOffsetCommandNv";
			case CommandOpcodesNV::AlphaRefCommandNv:return "AlphaRefCommandNv";
			case CommandOpcodesNV::ViewportCommandNv:return "ViewportCommandNv";
			case CommandOpcodesNV::ScissorCommandNv:return "ScissorCommandNv";
			case CommandOpcodesNV::FrontFaceCommandNv:return "FrontFaceCommandNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapTextureFormatINTEL value) {
		switch(value){
			case MapTextureFormatINTEL::LayoutDefaultIntel:return "LayoutDefaultIntel";
			case MapTextureFormatINTEL::LayoutLinearIntel:return "LayoutLinearIntel";
			case MapTextureFormatINTEL::LayoutLinearCpuCachedIntel:return "LayoutLinearCpuCachedIntel";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathCoordType value) {
		switch(value){
			case PathCoordType::ClosePathNv:return "ClosePathNv";
			case PathCoordType::MoveToNv:return "MoveToNv";
			case PathCoordType::RelativeMoveToNv:return "RelativeMoveToNv";
			case PathCoordType::LineToNv:return "LineToNv";
			case PathCoordType::RelativeLineToNv:return "RelativeLineToNv";
			case PathCoordType::HorizontalLineToNv:return "HorizontalLineToNv";
			case PathCoordType::RelativeHorizontalLineToNv:return "RelativeHorizontalLineToNv";
			case PathCoordType::VerticalLineToNv:return "VerticalLineToNv";
			case PathCoordType::RelativeVerticalLineToNv:return "RelativeVerticalLineToNv";
			case PathCoordType::QuadraticCurveToNv:return "QuadraticCurveToNv";
			case PathCoordType::RelativeQuadraticCurveToNv:return "RelativeQuadraticCurveToNv";
			case PathCoordType::CubicCurveToNv:return "CubicCurveToNv";
			case PathCoordType::RelativeCubicCurveToNv:return "RelativeCubicCurveToNv";
			case PathCoordType::SmoothQuadraticCurveToNv:return "SmoothQuadraticCurveToNv";
			case PathCoordType::RelativeSmoothQuadraticCurveToNv:return "RelativeSmoothQuadraticCurveToNv";
			case PathCoordType::SmoothCubicCurveToNv:return "SmoothCubicCurveToNv";
			case PathCoordType::RelativeSmoothCubicCurveToNv:return "RelativeSmoothCubicCurveToNv";
			case PathCoordType::SmallCcwArcToNv:return "SmallCcwArcToNv";
			case PathCoordType::RelativeSmallCcwArcToNv:return "RelativeSmallCcwArcToNv";
			case PathCoordType::SmallCwArcToNv:return "SmallCwArcToNv";
			case PathCoordType::RelativeSmallCwArcToNv:return "RelativeSmallCwArcToNv";
			case PathCoordType::LargeCcwArcToNv:return "LargeCcwArcToNv";
			case PathCoordType::RelativeLargeCcwArcToNv:return "RelativeLargeCcwArcToNv";
			case PathCoordType::LargeCwArcToNv:return "LargeCwArcToNv";
			case PathCoordType::RelativeLargeCwArcToNv:return "RelativeLargeCwArcToNv";
			case PathCoordType::ConicCurveToNv:return "ConicCurveToNv";
			case PathCoordType::RelativeConicCurveToNv:return "RelativeConicCurveToNv";
			case PathCoordType::RoundedRectNv:return "RoundedRectNv";
			case PathCoordType::RelativeRoundedRectNv:return "RelativeRoundedRectNv";
			case PathCoordType::RoundedRect2Nv:return "RoundedRect2Nv";
			case PathCoordType::RelativeRoundedRect2Nv:return "RelativeRoundedRect2Nv";
			case PathCoordType::RoundedRect4Nv:return "RoundedRect4Nv";
			case PathCoordType::RelativeRoundedRect4Nv:return "RelativeRoundedRect4Nv";
			case PathCoordType::RoundedRect8Nv:return "RoundedRect8Nv";
			case PathCoordType::RelativeRoundedRect8Nv:return "RelativeRoundedRect8Nv";
			case PathCoordType::RestartPathNv:return "RestartPathNv";
			case PathCoordType::DupFirstCubicCurveToNv:return "DupFirstCubicCurveToNv";
			case PathCoordType::DupLastCubicCurveToNv:return "DupLastCubicCurveToNv";
			case PathCoordType::RectNv:return "RectNv";
			case PathCoordType::RelativeRectNv:return "RelativeRectNv";
			case PathCoordType::CircularCcwArcToNv:return "CircularCcwArcToNv";
			case PathCoordType::CircularCwArcToNv:return "CircularCwArcToNv";
			case PathCoordType::CircularTangentArcToNv:return "CircularTangentArcToNv";
			case PathCoordType::ArcToNv:return "ArcToNv";
			case PathCoordType::RelativeArcToNv:return "RelativeArcToNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(All value) {
		switch(value){
			case All::SharedEdgeNv:return "SharedEdgeNv";
			case All::QuadsOes:return "QuadsOes";
			case All::LinesAdjacencyOes:return "LinesAdjacencyOes";
			case All::LineStripAdjacencyOes:return "LineStripAdjacencyOes";
			case All::TrianglesAdjacencyOes:return "TrianglesAdjacencyOes";
			case All::TriangleStripAdjacencyOes:return "TriangleStripAdjacencyOes";
			case All::PatchesOes:return "PatchesOes";
			case All::SrcAlphaSaturateExt:return "SrcAlphaSaturateExt";
			case All::StackOverflowKhr:return "StackOverflowKhr";
			case All::StackUnderflowKhr:return "StackUnderflowKhr";
			case All::ContextLost:return "ContextLost";
			case All::PolygonModeNv:return "PolygonModeNv";
			case All::PathModelViewStackDepthNv:return "PathModelViewStackDepthNv";
			case All::PathProjectionStackDepthNv:return "PathProjectionStackDepthNv";
			case All::PathModelViewMatrixNv:return "PathModelViewMatrixNv";
			case All::PathProjectionMatrixNv:return "PathProjectionMatrixNv";
			case All::MaxClipPlanesImg:return "MaxClipPlanesImg";
			case All::PathMaxModelViewStackDepthNv:return "PathMaxModelViewStackDepthNv";
			case All::PathMaxProjectionStackDepthNv:return "PathMaxProjectionStackDepthNv";
			case All::TextureBorderColorExt:return "TextureBorderColorExt";
			case All::TextureTarget:return "TextureTarget";
			case All::_2BytesNv:return "_2BytesNv";
			case All::_3BytesNv:return "_3BytesNv";
			case All::_4BytesNv:return "_4BytesNv";
			case All::FixedOes:return "FixedOes";
			case All::XorNv:return "XorNv";
			case All::ModelView0Arb:return "ModelView0Arb";
			case All::PathProjectionNv:return "PathProjectionNv";
			case All::RedNv:return "RedNv";
			case All::GreenNv:return "GreenNv";
			case All::BlueNv:return "BlueNv";
			case All::PointNv:return "PointNv";
			case All::LineNv:return "LineNv";
			case All::EyeLinearNv:return "EyeLinearNv";
			case All::ObjectLinearNv:return "ObjectLinearNv";
			case All::PolygonOffsetPointNv:return "PolygonOffsetPointNv";
			case All::PolygonOffsetLineNv:return "PolygonOffsetLineNv";
			case All::ClipPlane0Img:return "ClipPlane0Img";
			case All::ClipPlane1Img:return "ClipPlane1Img";
			case All::ClipPlane2Img:return "ClipPlane2Img";
			case All::ClipPlane3Img:return "ClipPlane3Img";
			case All::ClipPlane4Img:return "ClipPlane4Img";
			case All::ClipPlane5Img:return "ClipPlane5Img";
			case All::ClipDistance6Ext:return "ClipDistance6Ext";
			case All::ClipDistance7Ext:return "ClipDistance7Ext";
			case All::ConstantColorExt:return "ConstantColorExt";
			case All::OneMinusConstantColorExt:return "OneMinusConstantColorExt";
			case All::ConstantAlphaExt:return "ConstantAlphaExt";
			case All::OneMinusConstantAlphaExt:return "OneMinusConstantAlphaExt";
			case All::FuncAddOes:return "FuncAddOes";
			case All::BlendEquationRgbExt:return "BlendEquationRgbExt";
			case All::FuncSubtractOes:return "FuncSubtractOes";
			case All::FuncReverseSubtractOes:return "FuncReverseSubtractOes";
			case All::PolygonOffsetExt:return "PolygonOffsetExt";
			case All::PolygonOffsetFactorExt:return "PolygonOffsetFactorExt";
			case All::RescaleNormal:return "RescaleNormal";
			case All::IntensityExt:return "IntensityExt";
			case All::TextureRedSizeExt:return "TextureRedSizeExt";
			case All::TextureGreenSizeExt:return "TextureGreenSizeExt";
			case All::TextureBlueSizeExt:return "TextureBlueSizeExt";
			case All::TextureAlphaSizeExt:return "TextureAlphaSizeExt";
			case All::TextureLuminanceSizeExt:return "TextureLuminanceSizeExt";
			case All::TextureIntensitySizeExt:return "TextureIntensitySizeExt";
			case All::TextureResidentExt:return "TextureResidentExt";
			case All::Texture1DBindingExt:return "Texture1DBindingExt";
			case All::Texture2DBindingExt:return "Texture2DBindingExt";
			case All::Texture3DBindingOes:return "Texture3DBindingOes";
			case All::TextureDepth:return "TextureDepth";
			case All::Max3DTextureSizeOes:return "Max3DTextureSizeOes";
			case All::VertexArrayExt:return "VertexArrayExt";
			case All::NormalArrayExt:return "NormalArrayExt";
			case All::ColorArrayExt:return "ColorArrayExt";
			case All::IndexArrayExt:return "IndexArrayExt";
			case All::TextureCoordArrayExt:return "TextureCoordArrayExt";
			case All::EdgeFlagArrayExt:return "EdgeFlagArrayExt";
			case All::VertexArraySizeExt:return "VertexArraySizeExt";
			case All::VertexArrayTypeExt:return "VertexArrayTypeExt";
			case All::VertexArrayStrideExt:return "VertexArrayStrideExt";
			case All::NormalArrayTypeExt:return "NormalArrayTypeExt";
			case All::NormalArrayStrideExt:return "NormalArrayStrideExt";
			case All::ColorArraySizeExt:return "ColorArraySizeExt";
			case All::ColorArrayTypeExt:return "ColorArrayTypeExt";
			case All::ColorArrayStrideExt:return "ColorArrayStrideExt";
			case All::IndexArrayTypeExt:return "IndexArrayTypeExt";
			case All::IndexArrayStrideExt:return "IndexArrayStrideExt";
			case All::TextureCoordArraySizeExt:return "TextureCoordArraySizeExt";
			case All::TextureCoordArrayTypeExt:return "TextureCoordArrayTypeExt";
			case All::TextureCoordArrayStrideExt:return "TextureCoordArrayStrideExt";
			case All::EdgeFlagArrayStrideExt:return "EdgeFlagArrayStrideExt";
			case All::MultisampleArb:return "MultisampleArb";
			case All::SampleAlphaToCoverageArb:return "SampleAlphaToCoverageArb";
			case All::SampleAlphaToOneArb:return "SampleAlphaToOneArb";
			case All::SampleCoverageArb:return "SampleCoverageArb";
			case All::SampleBuffersArb:return "SampleBuffersArb";
			case All::SamplesArb:return "SamplesArb";
			case All::SampleCoverageValueArb:return "SampleCoverageValueArb";
			case All::SampleCoverageInvertArb:return "SampleCoverageInvertArb";
			case All::SamplePatternExt:return "SamplePatternExt";
			case All::ColorMatrix:return "ColorMatrix";
			case All::ColorMatrixStackDepth:return "ColorMatrixStackDepth";
			case All::MaxColorMatrixStackDepth:return "MaxColorMatrixStackDepth";
			case All::TextureCompareFailValueArb:return "TextureCompareFailValueArb";
			case All::BlendDstRgbExt:return "BlendDstRgbExt";
			case All::BlendSrcRgbExt:return "BlendSrcRgbExt";
			case All::BlendDstAlphaExt:return "BlendDstAlphaExt";
			case All::BlendSrcAlphaExt:return "BlendSrcAlphaExt";
			case All::_422Ext:return "_422Ext";
			case All::_422RevExt:return "_422RevExt";
			case All::_422AverageExt:return "_422AverageExt";
			case All::_422RevAverageExt:return "_422RevAverageExt";
			case All::ColorIndex1Ext:return "ColorIndex1Ext";
			case All::ColorIndex2Ext:return "ColorIndex2Ext";
			case All::ColorIndex4Ext:return "ColorIndex4Ext";
			case All::ColorIndex8Ext:return "ColorIndex8Ext";
			case All::ColorIndex12Ext:return "ColorIndex12Ext";
			case All::ColorIndex16Ext:return "ColorIndex16Ext";
			case All::MaxElementsVerticesExt:return "MaxElementsVerticesExt";
			case All::MaxElementsIndicesExt:return "MaxElementsIndicesExt";
			case All::PhongWin:return "PhongWin";
			case All::FogSpecularTextureWin:return "FogSpecularTextureWin";
			case All::TextureIndexSizeExt:return "TextureIndexSizeExt";
			case All::ParameterBufferArb:return "ParameterBufferArb";
			case All::ParameterBufferBinding:return "ParameterBufferBinding";
			case All::TextureMaxLevelApple:return "TextureMaxLevelApple";
			case All::IgnoreBorderHp:return "IgnoreBorderHp";
			case All::ConstantBorder:return "ConstantBorder";
			case All::ReplicateBorder:return "ReplicateBorder";
			case All::ConvolutionBorderColorHp:return "ConvolutionBorderColorHp";
			case All::CubicHp:return "CubicHp";
			case All::AverageHp:return "AverageHp";
			case All::PostImageTransformColorTableHp:return "PostImageTransformColorTableHp";
			case All::ProxyPostImageTransformColorTableHp:return "ProxyPostImageTransformColorTableHp";
			case All::OcclusionTestHp:return "OcclusionTestHp";
			case All::OcclusionTestResultHp:return "OcclusionTestResultHp";
			case All::TextureLightingModeHp:return "TextureLightingModeHp";
			case All::TexturePostSpecularHp:return "TexturePostSpecularHp";
			case All::TexturePreSpecularHp:return "TexturePreSpecularHp";
			case All::MaxDeformationOrderSgix:return "MaxDeformationOrderSgix";
			case All::ArrayElementLockFirstExt:return "ArrayElementLockFirstExt";
			case All::ArrayElementLockCountExt:return "ArrayElementLockCountExt";
			case All::CullVertexExt:return "CullVertexExt";
			case All::IuiV2fExt:return "IuiV2fExt";
			case All::IuiV3fExt:return "IuiV3fExt";
			case All::IuiN3fV2fExt:return "IuiN3fV2fExt";
			case All::IuiN3fV3fExt:return "IuiN3fV3fExt";
			case All::T2fIuiV2fExt:return "T2fIuiV2fExt";
			case All::T2fIuiV3fExt:return "T2fIuiV3fExt";
			case All::T2fIuiN3fV2fExt:return "T2fIuiN3fV2fExt";
			case All::T2fIuiN3fV3fExt:return "T2fIuiN3fV3fExt";
			case All::IndexTestExt:return "IndexTestExt";
			case All::IndexTestFuncExt:return "IndexTestFuncExt";
			case All::IndexTestRefExt:return "IndexTestRefExt";
			case All::IndexMaterialExt:return "IndexMaterialExt";
			case All::IndexMaterialParameterExt:return "IndexMaterialParameterExt";
			case All::IndexMaterialFaceExt:return "IndexMaterialFaceExt";
			case All::WrapBorderSun:return "WrapBorderSun";
			case All::UnpackConstantDataSunx:return "UnpackConstantDataSunx";
			case All::TextureConstantDataSunx:return "TextureConstantDataSunx";
			case All::TriangleListSun:return "TriangleListSun";
			case All::ReplacementCodeSun:return "ReplacementCodeSun";
			case All::GlobalAlphaSun:return "GlobalAlphaSun";
			case All::GlobalAlphaFactorSun:return "GlobalAlphaFactorSun";
			case All::TextureColorWritemaskSgis:return "TextureColorWritemaskSgis";
			case All::FramebufferDefault:return "FramebufferDefault";
			case All::FramebufferUndefinedOes:return "FramebufferUndefinedOes";
			case All::BufferImmutableStorageExt:return "BufferImmutableStorageExt";
			case All::BufferStorageFlagsExt:return "BufferStorageFlagsExt";
			case All::PrimitiveRestartForPatchesSupported:return "PrimitiveRestartForPatchesSupported";
			case All::Index:return "Index";
			case All::RgExt:return "RgExt";
			case All::SyncClEventArb:return "SyncClEventArb";
			case All::SyncClEventCompleteArb:return "SyncClEventCompleteArb";
			case All::DebugOutputSynchronousArb:return "DebugOutputSynchronousArb";
			case All::DebugNextLoggedMessageLength:return "DebugNextLoggedMessageLength";
			case All::DebugCallbackFunctionArb:return "DebugCallbackFunctionArb";
			case All::DebugCallbackUserParamArb:return "DebugCallbackUserParamArb";
			case All::DebugSourceApiArb:return "DebugSourceApiArb";
			case All::DebugSourceWindowSystemArb:return "DebugSourceWindowSystemArb";
			case All::DebugSourceShaderCompilerArb:return "DebugSourceShaderCompilerArb";
			case All::DebugSourceThirdPartyArb:return "DebugSourceThirdPartyArb";
			case All::DebugSourceApplicationArb:return "DebugSourceApplicationArb";
			case All::DebugSourceOtherArb:return "DebugSourceOtherArb";
			case All::DebugTypeErrorArb:return "DebugTypeErrorArb";
			case All::DebugTypeDeprecatedBehaviorArb:return "DebugTypeDeprecatedBehaviorArb";
			case All::DebugTypeUndefinedBehaviorArb:return "DebugTypeUndefinedBehaviorArb";
			case All::DebugTypePortabilityArb:return "DebugTypePortabilityArb";
			case All::DebugTypePerformanceArb:return "DebugTypePerformanceArb";
			case All::DebugTypeOtherArb:return "DebugTypeOtherArb";
			case All::LoseContextOnReset:return "LoseContextOnReset";
			case All::GuiltyContextResetArb:return "GuiltyContextResetArb";
			case All::InnocentContextResetArb:return "InnocentContextResetArb";
			case All::UnknownContextResetArb:return "UnknownContextResetArb";
			case All::ResetNotificationStrategy:return "ResetNotificationStrategy";
			case All::ProgramSeparableExt:return "ProgramSeparableExt";
			case All::/* For the OpenGL ES version of EXT_separate_shader_objects */ ActiveProgramExt:return "/* For the OpenGL ES version of EXT_separate_shader_objects */ ActiveProgramExt";
			case All::ProgramPipelineBindingExt:return "ProgramPipelineBindingExt";
			case All::MaxViewportsNv:return "MaxViewportsNv";
			case All::ViewportSubpixelBitsExt:return "ViewportSubpixelBitsExt";
			case All::ViewportBoundsRangeExt:return "ViewportBoundsRangeExt";
			case All::LayerProvokingVertexExt:return "LayerProvokingVertexExt";
			case All::ViewportIndexProvokingVertexExt:return "ViewportIndexProvokingVertexExt";
			case All::UndefinedVertex:return "UndefinedVertex";
			case All::NoResetNotification:return "NoResetNotification";
			case All::MaxComputeSharedMemorySize:return "MaxComputeSharedMemorySize";
			case All::DebugTypeMarkerKhr:return "DebugTypeMarkerKhr";
			case All::DebugTypePushGroupKhr:return "DebugTypePushGroupKhr";
			case All::DebugTypePopGroupKhr:return "DebugTypePopGroupKhr";
			case All::DebugSeverityNotificationKhr:return "DebugSeverityNotificationKhr";
			case All::MaxDebugGroupStackDepthKhr:return "MaxDebugGroupStackDepthKhr";
			case All::DebugGroupStackDepthKhr:return "DebugGroupStackDepthKhr";
			case All::MaxCombinedDimensions:return "MaxCombinedDimensions";
			case All::DepthComponents:return "DepthComponents";
			case All::StencilComponents:return "StencilComponents";
			case All::ManualGenerateMipmap:return "ManualGenerateMipmap";
			case All::SrgbDecodeArb:return "SrgbDecodeArb";
			case All::FullSupport:return "FullSupport";
			case All::CaveatSupport:return "CaveatSupport";
			case All::ImageClass4X32:return "ImageClass4X32";
			case All::ImageClass2X32:return "ImageClass2X32";
			case All::ImageClass1X32:return "ImageClass1X32";
			case All::ImageClass4X16:return "ImageClass4X16";
			case All::ImageClass2X16:return "ImageClass2X16";
			case All::ImageClass1X16:return "ImageClass1X16";
			case All::ImageClass4X8:return "ImageClass4X8";
			case All::ImageClass2X8:return "ImageClass2X8";
			case All::ImageClass1X8:return "ImageClass1X8";
			case All::ImageClass111110:return "ImageClass111110";
			case All::ImageClass1010102:return "ImageClass1010102";
			case All::ViewClass128Bits:return "ViewClass128Bits";
			case All::ViewClass96Bits:return "ViewClass96Bits";
			case All::ViewClass64Bits:return "ViewClass64Bits";
			case All::ViewClass48Bits:return "ViewClass48Bits";
			case All::ViewClass32Bits:return "ViewClass32Bits";
			case All::ViewClass24Bits:return "ViewClass24Bits";
			case All::ViewClass16Bits:return "ViewClass16Bits";
			case All::ViewClass8Bits:return "ViewClass8Bits";
			case All::ViewClassS3tcDxt1Rgb:return "ViewClassS3tcDxt1Rgb";
			case All::ViewClassS3tcDxt1Rgba:return "ViewClassS3tcDxt1Rgba";
			case All::ViewClassS3tcDxt3Rgba:return "ViewClassS3tcDxt3Rgba";
			case All::ViewClassS3tcDxt5Rgba:return "ViewClassS3tcDxt5Rgba";
			case All::ViewClassRgtc1Red:return "ViewClassRgtc1Red";
			case All::ViewClassRgtc2Rg:return "ViewClassRgtc2Rg";
			case All::ViewClassBptcUnorm:return "ViewClassBptcUnorm";
			case All::ViewClassBptcFloat:return "ViewClassBptcFloat";
			case All::TextureViewMinLevel:return "TextureViewMinLevel";
			case All::TextureViewNumLevels:return "TextureViewNumLevels";
			case All::TextureViewMinLayer:return "TextureViewMinLayer";
			case All::TextureViewNumLayers:return "TextureViewNumLayers";
			case All::TextureImmutableLevels:return "TextureImmutableLevels";
			case All::BufferKhr:return "BufferKhr";
			case All::ShaderKhr:return "ShaderKhr";
			case All::ProgramKhr:return "ProgramKhr";
			case All::QueryKhr:return "QueryKhr";
			case All::ProgramPipelineKhr:return "ProgramPipelineKhr";
			case All::MaxVertexAttribStride:return "MaxVertexAttribStride";
			case All::SamplerKhr:return "SamplerKhr";
			case All::DisplayList:return "DisplayList";
			case All::MaxLabelLengthKhr:return "MaxLabelLengthKhr";
			case All::NumShadingLanguageVersions:return "NumShadingLanguageVersions";
			case All::TransformFeedbackOverflowArb:return "TransformFeedbackOverflowArb";
			case All::TransformFeedbackStreamOverflow:return "TransformFeedbackStreamOverflow";
			case All::VerticesSubmittedArb:return "VerticesSubmittedArb";
			case All::PrimitivesSubmittedArb:return "PrimitivesSubmittedArb";
			case All::VertexShaderInvocationsArb:return "VertexShaderInvocationsArb";
			case All::TessControlShaderPatches:return "TessControlShaderPatches";
			case All::TessEvaluationShaderInvocations:return "TessEvaluationShaderInvocations";
			case All::GeometryShaderPrimitivesEmitted:return "GeometryShaderPrimitivesEmitted";
			case All::FragmentShaderInvocations:return "FragmentShaderInvocations";
			case All::ComputeShaderInvocations:return "ComputeShaderInvocations";
			case All::ClippingInputPrimitives:return "ClippingInputPrimitives";
			case All::ClippingOutputPrimitives:return "ClippingOutputPrimitives";
			case All::SparseBufferPageSizeArb:return "SparseBufferPageSizeArb";
			case All::MaxCullDistances:return "MaxCullDistances";
			case All::MaxCombinedClipAndCullDistances:return "MaxCombinedClipAndCullDistances";
			case All::ContextReleaseBehavior:return "ContextReleaseBehavior";
			case All::ContextReleaseBehaviorFlush:return "ContextReleaseBehaviorFlush";
			case All::/* Reserved for future */ RobustGpuTimeoutMsKhr:return "/* Reserved for future */ RobustGpuTimeoutMsKhr";
			case All::DepthPassInstrumentSgix:return "DepthPassInstrumentSgix";
			case All::DepthPassInstrumentCountersSgix:return "DepthPassInstrumentCountersSgix";
			case All::DepthPassInstrumentMaxSgix:return "DepthPassInstrumentMaxSgix";
			case All::FragmentsInstrumentSgix:return "FragmentsInstrumentSgix";
			case All::FragmentsInstrumentCountersSgix:return "FragmentsInstrumentCountersSgix";
			case All::FragmentsInstrumentMaxSgix:return "FragmentsInstrumentMaxSgix";
			case All::YcrcbSgix:return "YcrcbSgix";
			case All::YcrcbaSgix:return "YcrcbaSgix";
			case All::UnpackCompressedSizeSgix:return "UnpackCompressedSizeSgix";
			case All::PackMaxCompressedSizeSgix:return "PackMaxCompressedSizeSgix";
			case All::PackCompressedSizeSgix:return "PackCompressedSizeSgix";
			case All::Slim8uSgix:return "Slim8uSgix";
			case All::Slim10uSgix:return "Slim10uSgix";
			case All::Slim12sSgix:return "Slim12sSgix";
			case All::CubicExt:return "CubicExt";
			case All::AverageExt:return "AverageExt";
			case All::PixelTransform2DStackDepthExt:return "PixelTransform2DStackDepthExt";
			case All::MaxPixelTransform2DStackDepthExt:return "MaxPixelTransform2DStackDepthExt";
			case All::PixelTransform2DMatrixExt:return "PixelTransform2DMatrixExt";
			case All::TextureApplicationModeExt:return "TextureApplicationModeExt";
			case All::TextureLightExt:return "TextureLightExt";
			case All::TextureMaterialFaceExt:return "TextureMaterialFaceExt";
			case All::TextureMaterialParameterExt:return "TextureMaterialParameterExt";
			case All::PixelGroupColorSgis:return "PixelGroupColorSgis";
			case All::RgbS3tc:return "RgbS3tc";
			case All::Rgb4S3tc:return "Rgb4S3tc";
			case All::RgbaS3tc:return "RgbaS3tc";
			case All::Rgba4S3tc:return "Rgba4S3tc";
			case All::RgbaDxt5S3tc:return "RgbaDxt5S3tc";
			case All::Rgba4Dxt5S3tc:return "Rgba4Dxt5S3tc";
			case All::ParallelArraysIntel:return "ParallelArraysIntel";
			case All::VertexArrayParallelPointersIntel:return "VertexArrayParallelPointersIntel";
			case All::NormalArrayParallelPointersIntel:return "NormalArrayParallelPointersIntel";
			case All::ColorArrayParallelPointersIntel:return "ColorArrayParallelPointersIntel";
			case All::TextureCoordArrayParallelPointersIntel:return "TextureCoordArrayParallelPointersIntel";
			case All::BlackholeRenderIntel:return "BlackholeRenderIntel";
			case All::ConservativeRasterizationIntel:return "ConservativeRasterizationIntel";
			case All::CurrentRasterNormalSgix:return "CurrentRasterNormalSgix";
			case All::TangentArrayExt:return "TangentArrayExt";
			case All::BinormalArrayExt:return "BinormalArrayExt";
			case All::CurrentTangentExt:return "CurrentTangentExt";
			case All::CurrentBinormalExt:return "CurrentBinormalExt";
			case All::TangentArrayTypeExt:return "TangentArrayTypeExt";
			case All::TangentArrayStrideExt:return "TangentArrayStrideExt";
			case All::BinormalArrayTypeExt:return "BinormalArrayTypeExt";
			case All::BinormalArrayStrideExt:return "BinormalArrayStrideExt";
			case All::TangentArrayPointerExt:return "TangentArrayPointerExt";
			case All::BinormalArrayPointerExt:return "BinormalArrayPointerExt";
			case All::Map1TangentExt:return "Map1TangentExt";
			case All::Map2TangentExt:return "Map2TangentExt";
			case All::Map1BinormalExt:return "Map1BinormalExt";
			case All::Map2BinormalExt:return "Map2BinormalExt";
			case All::FogCoordinateSource:return "FogCoordinateSource";
			case All::CurrentFogCoordinate:return "CurrentFogCoordinate";
			case All::FogCoordinateArrayType:return "FogCoordinateArrayType";
			case All::FogCoordinateArrayStride:return "FogCoordinateArrayStride";
			case All::FogCoordinateArrayPointer:return "FogCoordinateArrayPointer";
			case All::FogCoordinateArray:return "FogCoordinateArray";
			case All::ColorSum:return "ColorSum";
			case All::CurrentSecondaryColor:return "CurrentSecondaryColor";
			case All::SecondaryColorArraySize:return "SecondaryColorArraySize";
			case All::SecondaryColorArrayType:return "SecondaryColorArrayType";
			case All::SecondaryColorArrayStride:return "SecondaryColorArrayStride";
			case All::SecondaryColorArrayPointer:return "SecondaryColorArrayPointer";
			case All::SecondaryColorArray:return "SecondaryColorArray";
			case All::CurrentRasterSecondaryColor:return "CurrentRasterSecondaryColor";
			case All::ScreenCoordinatesRend:return "ScreenCoordinatesRend";
			case All::InvertedScreenWRend:return "InvertedScreenWRend";
			case All::Texture2Arb:return "Texture2Arb";
			case All::Texture3Arb:return "Texture3Arb";
			case All::Texture4Arb:return "Texture4Arb";
			case All::Texture5Arb:return "Texture5Arb";
			case All::Texture6Arb:return "Texture6Arb";
			case All::Texture7Arb:return "Texture7Arb";
			case All::Texture8Arb:return "Texture8Arb";
			case All::Texture9Arb:return "Texture9Arb";
			case All::Texture10Arb:return "Texture10Arb";
			case All::Texture11Arb:return "Texture11Arb";
			case All::Texture12Arb:return "Texture12Arb";
			case All::Texture13Arb:return "Texture13Arb";
			case All::Texture14Arb:return "Texture14Arb";
			case All::Texture15Arb:return "Texture15Arb";
			case All::Texture16Arb:return "Texture16Arb";
			case All::Texture17Arb:return "Texture17Arb";
			case All::Texture18Arb:return "Texture18Arb";
			case All::Texture19Arb:return "Texture19Arb";
			case All::Texture20Arb:return "Texture20Arb";
			case All::Texture21Arb:return "Texture21Arb";
			case All::Texture22Arb:return "Texture22Arb";
			case All::Texture23Arb:return "Texture23Arb";
			case All::Texture24Arb:return "Texture24Arb";
			case All::Texture25Arb:return "Texture25Arb";
			case All::Texture26Arb:return "Texture26Arb";
			case All::Texture27Arb:return "Texture27Arb";
			case All::Texture28Arb:return "Texture28Arb";
			case All::Texture29Arb:return "Texture29Arb";
			case All::Texture30Arb:return "Texture30Arb";
			case All::Texture31Arb:return "Texture31Arb";
			case All::ActiveTextureArb:return "ActiveTextureArb";
			case All::ClientActiveTexture:return "ClientActiveTexture";
			case All::MaxTextureUnits:return "MaxTextureUnits";
			case All::TransposeModelViewMatrix:return "TransposeModelViewMatrix";
			case All::TransposeProjectionMatrix:return "TransposeProjectionMatrix";
			case All::TransposeTextureMatrix:return "TransposeTextureMatrix";
			case All::TransposeColorMatrix:return "TransposeColorMatrix";
			case All::Subtract:return "Subtract";
			case All::MaxRenderbufferSizeExt:return "MaxRenderbufferSizeExt";
			case All::CompressedAlpha:return "CompressedAlpha";
			case All::CompressedLuminance:return "CompressedLuminance";
			case All::CompressedLuminanceAlpha:return "CompressedLuminanceAlpha";
			case All::CompressedIntensity:return "CompressedIntensity";
			case All::CompressedRgbArb:return "CompressedRgbArb";
			case All::CompressedRgbaArb:return "CompressedRgbaArb";
			case All::MaxRectangleTextureSizeArb:return "MaxRectangleTextureSizeArb";
			case All::MaxTextureLodBiasExt:return "MaxTextureLodBiasExt";
			case All::TextureMaxAnisotropyExt:return "TextureMaxAnisotropyExt";
			case All::MaxTextureMaxAnisotropy:return "MaxTextureMaxAnisotropy";
			case All::TextureFilterControlExt:return "TextureFilterControlExt";
			case All::TextureLodBiasExt:return "TextureLodBiasExt";
			case All::ModelView1StackDepthExt:return "ModelView1StackDepthExt";
			case All::Combine4Nv:return "Combine4Nv";
			case All::MaxShininessNv:return "MaxShininessNv";
			case All::MaxSpotExponentNv:return "MaxSpotExponentNv";
			case All::ModelView1MatrixExt:return "ModelView1MatrixExt";
			case All::IncrWrapExt:return "IncrWrapExt";
			case All::DecrWrapExt:return "DecrWrapExt";
			case All::VertexWeightingExt:return "VertexWeightingExt";
			case All::ModelView1Arb:return "ModelView1Arb";
			case All::CurrentVertexWeightExt:return "CurrentVertexWeightExt";
			case All::VertexWeightArrayExt:return "VertexWeightArrayExt";
			case All::VertexWeightArraySizeExt:return "VertexWeightArraySizeExt";
			case All::VertexWeightArrayTypeExt:return "VertexWeightArrayTypeExt";
			case All::VertexWeightArrayStrideExt:return "VertexWeightArrayStrideExt";
			case All::VertexWeightArrayPointerExt:return "VertexWeightArrayPointerExt";
			case All::VertexArrayRangeApple:return "VertexArrayRangeApple";
			case All::VertexArrayRangeLengthApple:return "VertexArrayRangeLengthApple";
			case All::VertexArrayRangeValidNv:return "VertexArrayRangeValidNv";
			case All::MaxVertexArrayRangeElementNv:return "MaxVertexArrayRangeElementNv";
			case All::VertexArrayRangePointerApple:return "VertexArrayRangePointerApple";
			case All::RegisterCombinersNv:return "RegisterCombinersNv";
			case All::ConstantColor0Nv:return "ConstantColor0Nv";
			case All::ConstantColor1Nv:return "ConstantColor1Nv";
			case All::ETimesFNv:return "ETimesFNv";
			case All::Spare0PlusSecondaryColorNv:return "Spare0PlusSecondaryColorNv";
			case All::VertexArrayRangeWithoutFlushNv:return "VertexArrayRangeWithoutFlushNv";
			case All::PerStageConstantsNv:return "PerStageConstantsNv";
			case All::CombinerAbDotProductNv:return "CombinerAbDotProductNv";
			case All::CombinerCdDotProductNv:return "CombinerCdDotProductNv";
			case All::CombinerMuxSumNv:return "CombinerMuxSumNv";
			case All::CombinerScaleNv:return "CombinerScaleNv";
			case All::CombinerBiasNv:return "CombinerBiasNv";
			case All::CombinerAbOutputNv:return "CombinerAbOutputNv";
			case All::CombinerCdOutputNv:return "CombinerCdOutputNv";
			case All::CombinerSumOutputNv:return "CombinerSumOutputNv";
			case All::MaxGeneralCombinersNv:return "MaxGeneralCombinersNv";
			case All::NumGeneralCombinersNv:return "NumGeneralCombinersNv";
			case All::ColorSumClampNv:return "ColorSumClampNv";
			case All::PrimitiveRestartNv:return "PrimitiveRestartNv";
			case All::PrimitiveRestartIndexNv:return "PrimitiveRestartIndexNv";
			case All::FogDistanceModeNv:return "FogDistanceModeNv";
			case All::EyeRadialNv:return "EyeRadialNv";
			case All::EyePlaneAbsoluteNv:return "EyePlaneAbsoluteNv";
			case All::EmbossLightNv:return "EmbossLightNv";
			case All::EmbossConstantNv:return "EmbossConstantNv";
			case All::EmbossMapNv:return "EmbossMapNv";
			case All::RedMinClampIngr:return "RedMinClampIngr";
			case All::GreenMinClampIngr:return "GreenMinClampIngr";
			case All::BlueMinClampIngr:return "BlueMinClampIngr";
			case All::AlphaMinClampIngr:return "AlphaMinClampIngr";
			case All::RedMaxClampIngr:return "RedMaxClampIngr";
			case All::GreenMaxClampIngr:return "GreenMaxClampIngr";
			case All::BlueMaxClampIngr:return "BlueMaxClampIngr";
			case All::AlphaMaxClampIngr:return "AlphaMaxClampIngr";
			case All::InterlaceReadIngr:return "InterlaceReadIngr";
			case All::TextureNormalExt:return "TextureNormalExt";
			case All::LightModelSpecularVectorApple:return "LightModelSpecularVectorApple";
			case All::UnpackClientStorageApple:return "UnpackClientStorageApple";
			case All::BufferObjectApple:return "BufferObjectApple";
			case All::VertexArrayBindingApple:return "VertexArrayBindingApple";
			case All::TextureRangeLengthApple:return "TextureRangeLengthApple";
			case All::TextureRangePointerApple:return "TextureRangePointerApple";
			case All::Ycbcr422Apple:return "Ycbcr422Apple";
			case All::UnsignedShort88Apple:return "UnsignedShort88Apple";
			case All::UnsignedShort88RevApple:return "UnsignedShort88RevApple";
			case All::StoragePrivateApple:return "StoragePrivateApple";
			case All::ReplacementCodeArraySun:return "ReplacementCodeArraySun";
			case All::ReplacementCodeArrayTypeSun:return "ReplacementCodeArrayTypeSun";
			case All::ReplacementCodeArrayStrideSun:return "ReplacementCodeArrayStrideSun";
			case All::ReplacementCodeArrayPointerSun:return "ReplacementCodeArrayPointerSun";
			case All::R1uiV3fSun:return "R1uiV3fSun";
			case All::R1uiC4ubV3fSun:return "R1uiC4ubV3fSun";
			case All::R1uiC3fV3fSun:return "R1uiC3fV3fSun";
			case All::R1uiN3fV3fSun:return "R1uiN3fV3fSun";
			case All::R1uiC4fN3fV3fSun:return "R1uiC4fN3fV3fSun";
			case All::R1uiT2fV3fSun:return "R1uiT2fV3fSun";
			case All::R1uiT2fN3fV3fSun:return "R1uiT2fN3fV3fSun";
			case All::R1uiT2fC4fN3fV3fSun:return "R1uiT2fC4fN3fV3fSun";
			case All::SliceAccumSun:return "SliceAccumSun";
			case All::QuadMeshSun:return "QuadMeshSun";
			case All::TriangleMeshSun:return "TriangleMeshSun";
			case All::VertexProgramNv:return "VertexProgramNv";
			case All::VertexStateProgramNv:return "VertexStateProgramNv";
			case All::VertexAttribArrayEnabledArb:return "VertexAttribArrayEnabledArb";
			case All::AttribArraySizeNv:return "AttribArraySizeNv";
			case All::AttribArrayStrideNv:return "AttribArrayStrideNv";
			case All::AttribArrayTypeNv:return "AttribArrayTypeNv";
			case All::CurrentAttribNv:return "CurrentAttribNv";
			case All::ProgramLengthArb:return "ProgramLengthArb";
			case All::ProgramStringNv:return "ProgramStringNv";
			case All::ModelViewProjectionNv:return "ModelViewProjectionNv";
			case All::IdentityNv:return "IdentityNv";
			case All::InverseNv:return "InverseNv";
			case All::TransposeNv:return "TransposeNv";
			case All::InverseTransposeNv:return "InverseTransposeNv";
			case All::MaxProgramMatrixStackDepthArb:return "MaxProgramMatrixStackDepthArb";
			case All::MaxProgramMatricesArb:return "MaxProgramMatricesArb";
			case All::Matrix0Nv:return "Matrix0Nv";
			case All::Matrix1Nv:return "Matrix1Nv";
			case All::Matrix2Nv:return "Matrix2Nv";
			case All::Matrix3Nv:return "Matrix3Nv";
			case All::Matrix4Nv:return "Matrix4Nv";
			case All::Matrix5Nv:return "Matrix5Nv";
			case All::Matrix6Nv:return "Matrix6Nv";
			case All::Matrix7Nv:return "Matrix7Nv";
			case All::CurrentMatrixStackDepthArb:return "CurrentMatrixStackDepthArb";
			case All::CurrentMatrixArb:return "CurrentMatrixArb";
			case All::VertexProgramPointSize:return "VertexProgramPointSize";
			case All::VertexProgramTwoSide:return "VertexProgramTwoSide";
			case All::AttribArrayPointerNv:return "AttribArrayPointerNv";
			case All::ProgramTargetNv:return "ProgramTargetNv";
			case All::ProgramResidentNv:return "ProgramResidentNv";
			case All::TrackMatrixNv:return "TrackMatrixNv";
			case All::TrackMatrixTransformNv:return "TrackMatrixTransformNv";
			case All::VertexProgramBindingNv:return "VertexProgramBindingNv";
			case All::ProgramErrorPositionArb:return "ProgramErrorPositionArb";
			case All::OffsetTextureRectangleNv:return "OffsetTextureRectangleNv";
			case All::OffsetTextureRectangleScaleNv:return "OffsetTextureRectangleScaleNv";
			case All::DotProductTextureRectangleNv:return "DotProductTextureRectangleNv";
			case All::DepthClampNv:return "DepthClampNv";
			case All::VertexAttribArray0Nv:return "VertexAttribArray0Nv";
			case All::VertexAttribArray1Nv:return "VertexAttribArray1Nv";
			case All::VertexAttribArray2Nv:return "VertexAttribArray2Nv";
			case All::VertexAttribArray3Nv:return "VertexAttribArray3Nv";
			case All::VertexAttribArray4Nv:return "VertexAttribArray4Nv";
			case All::VertexAttribArray5Nv:return "VertexAttribArray5Nv";
			case All::VertexAttribArray6Nv:return "VertexAttribArray6Nv";
			case All::VertexAttribArray7Nv:return "VertexAttribArray7Nv";
			case All::VertexAttribArray8Nv:return "VertexAttribArray8Nv";
			case All::VertexAttribArray9Nv:return "VertexAttribArray9Nv";
			case All::VertexAttribArray10Nv:return "VertexAttribArray10Nv";
			case All::VertexAttribArray11Nv:return "VertexAttribArray11Nv";
			case All::VertexAttribArray12Nv:return "VertexAttribArray12Nv";
			case All::VertexAttribArray13Nv:return "VertexAttribArray13Nv";
			case All::VertexAttribArray14Nv:return "VertexAttribArray14Nv";
			case All::VertexAttribArray15Nv:return "VertexAttribArray15Nv";
			case All::Map1VertexAttrib04Nv:return "Map1VertexAttrib04Nv";
			case All::Map1VertexAttrib14Nv:return "Map1VertexAttrib14Nv";
			case All::Map1VertexAttrib24Nv:return "Map1VertexAttrib24Nv";
			case All::Map1VertexAttrib34Nv:return "Map1VertexAttrib34Nv";
			case All::Map1VertexAttrib44Nv:return "Map1VertexAttrib44Nv";
			case All::Map1VertexAttrib54Nv:return "Map1VertexAttrib54Nv";
			case All::Map1VertexAttrib64Nv:return "Map1VertexAttrib64Nv";
			case All::Map1VertexAttrib74Nv:return "Map1VertexAttrib74Nv";
			case All::Map1VertexAttrib84Nv:return "Map1VertexAttrib84Nv";
			case All::Map1VertexAttrib94Nv:return "Map1VertexAttrib94Nv";
			case All::Map1VertexAttrib104Nv:return "Map1VertexAttrib104Nv";
			case All::Map1VertexAttrib114Nv:return "Map1VertexAttrib114Nv";
			case All::Map1VertexAttrib124Nv:return "Map1VertexAttrib124Nv";
			case All::Map1VertexAttrib134Nv:return "Map1VertexAttrib134Nv";
			case All::Map1VertexAttrib144Nv:return "Map1VertexAttrib144Nv";
			case All::Map1VertexAttrib154Nv:return "Map1VertexAttrib154Nv";
			case All::Map2VertexAttrib04Nv:return "Map2VertexAttrib04Nv";
			case All::Map2VertexAttrib14Nv:return "Map2VertexAttrib14Nv";
			case All::Map2VertexAttrib24Nv:return "Map2VertexAttrib24Nv";
			case All::Map2VertexAttrib34Nv:return "Map2VertexAttrib34Nv";
			case All::Map2VertexAttrib44Nv:return "Map2VertexAttrib44Nv";
			case All::Map2VertexAttrib54Nv:return "Map2VertexAttrib54Nv";
			case All::Map2VertexAttrib64Nv:return "Map2VertexAttrib64Nv";
			case All::Map2VertexAttrib74Nv:return "Map2VertexAttrib74Nv";
			case All::Map2VertexAttrib84Nv:return "Map2VertexAttrib84Nv";
			case All::Map2VertexAttrib94Nv:return "Map2VertexAttrib94Nv";
			case All::Map2VertexAttrib104Nv:return "Map2VertexAttrib104Nv";
			case All::Map2VertexAttrib114Nv:return "Map2VertexAttrib114Nv";
			case All::Map2VertexAttrib124Nv:return "Map2VertexAttrib124Nv";
			case All::Map2VertexAttrib134Nv:return "Map2VertexAttrib134Nv";
			case All::Map2VertexAttrib144Nv:return "Map2VertexAttrib144Nv";
			case All::Map2VertexAttrib154Nv:return "Map2VertexAttrib154Nv";
			case All::TextureCompressedImageSize:return "TextureCompressedImageSize";
			case All::TextureCompressedArb:return "TextureCompressedArb";
			case All::NumCompressedTextureFormatsArb:return "NumCompressedTextureFormatsArb";
			case All::CompressedTextureFormatsArb:return "CompressedTextureFormatsArb";
			case All::MaxVertexUnitsArb:return "MaxVertexUnitsArb";
			case All::ActiveVertexUnitsArb:return "ActiveVertexUnitsArb";
			case All::WeightSumUnityArb:return "WeightSumUnityArb";
			case All::VertexBlendArb:return "VertexBlendArb";
			case All::CurrentWeightArb:return "CurrentWeightArb";
			case All::WeightArrayTypeArb:return "WeightArrayTypeArb";
			case All::WeightArrayStrideArb:return "WeightArrayStrideArb";
			case All::WeightArraySizeArb:return "WeightArraySizeArb";
			case All::WeightArrayPointerArb:return "WeightArrayPointerArb";
			case All::WeightArrayArb:return "WeightArrayArb";
			case All::Dot3Rgb:return "Dot3Rgb";
			case All::Dot3Rgba:return "Dot3Rgba";
			case All::Multisample3Dfx:return "Multisample3Dfx";
			case All::SampleBuffers3Dfx:return "SampleBuffers3Dfx";
			case All::Samples3Dfx:return "Samples3Dfx";
			case All::EvalFractionalTessellationNv:return "EvalFractionalTessellationNv";
			case All::EvalVertexAttrib0Nv:return "EvalVertexAttrib0Nv";
			case All::EvalVertexAttrib1Nv:return "EvalVertexAttrib1Nv";
			case All::EvalVertexAttrib2Nv:return "EvalVertexAttrib2Nv";
			case All::EvalVertexAttrib3Nv:return "EvalVertexAttrib3Nv";
			case All::EvalVertexAttrib4Nv:return "EvalVertexAttrib4Nv";
			case All::EvalVertexAttrib5Nv:return "EvalVertexAttrib5Nv";
			case All::EvalVertexAttrib6Nv:return "EvalVertexAttrib6Nv";
			case All::EvalVertexAttrib7Nv:return "EvalVertexAttrib7Nv";
			case All::EvalVertexAttrib8Nv:return "EvalVertexAttrib8Nv";
			case All::EvalVertexAttrib9Nv:return "EvalVertexAttrib9Nv";
			case All::EvalVertexAttrib10Nv:return "EvalVertexAttrib10Nv";
			case All::EvalVertexAttrib11Nv:return "EvalVertexAttrib11Nv";
			case All::EvalVertexAttrib12Nv:return "EvalVertexAttrib12Nv";
			case All::EvalVertexAttrib13Nv:return "EvalVertexAttrib13Nv";
			case All::EvalVertexAttrib14Nv:return "EvalVertexAttrib14Nv";
			case All::EvalVertexAttrib15Nv:return "EvalVertexAttrib15Nv";
			case All::MaxMapTessellationNv:return "MaxMapTessellationNv";
			case All::MaxRationalEvalOrderNv:return "MaxRationalEvalOrderNv";
			case All::MaxProgramPatchAttribsNv:return "MaxProgramPatchAttribsNv";
			case All::RgbaUnsignedDotProductMappingNv:return "RgbaUnsignedDotProductMappingNv";
			case All::UnsignedIntS8S888Nv:return "UnsignedIntS8S888Nv";
			case All::UnsignedInt88S8S8RevNv:return "UnsignedInt88S8S8RevNv";
			case All::DsdtMagIntensityNv:return "DsdtMagIntensityNv";
			case All::ShaderConsistentNv:return "ShaderConsistentNv";
			case All::TextureShaderNv:return "TextureShaderNv";
			case All::ShaderOperationNv:return "ShaderOperationNv";
			case All::CullModesNv:return "CullModesNv";
			case All::OffsetTextureMatrixNv:return "OffsetTextureMatrixNv";
			case All::OffsetTextureScaleNv:return "OffsetTextureScaleNv";
			case All::OffsetTextureBiasNv:return "OffsetTextureBiasNv";
			case All::PreviousTextureInputNv:return "PreviousTextureInputNv";
			case All::ConstEyeNv:return "ConstEyeNv";
			case All::PassThroughNv:return "PassThroughNv";
			case All::CullFragmentNv:return "CullFragmentNv";
			case All::OffsetTexture2DNv:return "OffsetTexture2DNv";
			case All::DependentArTexture2DNv:return "DependentArTexture2DNv";
			case All::DependentGbTexture2DNv:return "DependentGbTexture2DNv";
			case All::SurfaceStateNv:return "SurfaceStateNv";
			case All::DotProductNv:return "DotProductNv";
			case All::DotProductDepthReplaceNv:return "DotProductDepthReplaceNv";
			case All::DotProductTexture2DNv:return "DotProductTexture2DNv";
			case All::DotProductTexture3DNv:return "DotProductTexture3DNv";
			case All::DotProductTextureCubeMapNv:return "DotProductTextureCubeMapNv";
			case All::DotProductDiffuseCubeMapNv:return "DotProductDiffuseCubeMapNv";
			case All::DotProductReflectCubeMapNv:return "DotProductReflectCubeMapNv";
			case All::DotProductConstEyeReflectCubeMapNv:return "DotProductConstEyeReflectCubeMapNv";
			case All::HiloNv:return "HiloNv";
			case All::DsdtNv:return "DsdtNv";
			case All::DsdtMagNv:return "DsdtMagNv";
			case All::DsdtMagVibNv:return "DsdtMagVibNv";
			case All::Hilo16Nv:return "Hilo16Nv";
			case All::SignedHiloNv:return "SignedHiloNv";
			case All::SignedHilo16Nv:return "SignedHilo16Nv";
			case All::SignedRgbaNv:return "SignedRgbaNv";
			case All::SignedRgba8Nv:return "SignedRgba8Nv";
			case All::SurfaceRegisteredNv:return "SurfaceRegisteredNv";
			case All::SignedRgbNv:return "SignedRgbNv";
			case All::SignedRgb8Nv:return "SignedRgb8Nv";
			case All::SurfaceMappedNv:return "SurfaceMappedNv";
			case All::SignedLuminanceNv:return "SignedLuminanceNv";
			case All::SignedLuminance8Nv:return "SignedLuminance8Nv";
			case All::SignedLuminanceAlphaNv:return "SignedLuminanceAlphaNv";
			case All::SignedLuminance8Alpha8Nv:return "SignedLuminance8Alpha8Nv";
			case All::SignedAlphaNv:return "SignedAlphaNv";
			case All::SignedAlpha8Nv:return "SignedAlpha8Nv";
			case All::SignedIntensityNv:return "SignedIntensityNv";
			case All::SignedIntensity8Nv:return "SignedIntensity8Nv";
			case All::Dsdt8Nv:return "Dsdt8Nv";
			case All::Dsdt8Mag8Nv:return "Dsdt8Mag8Nv";
			case All::Dsdt8Mag8Intensity8Nv:return "Dsdt8Mag8Intensity8Nv";
			case All::SignedRgbUnsignedAlphaNv:return "SignedRgbUnsignedAlphaNv";
			case All::SignedRgb8UnsignedAlpha8Nv:return "SignedRgb8UnsignedAlpha8Nv";
			case All::HiScaleNv:return "HiScaleNv";
			case All::LoScaleNv:return "LoScaleNv";
			case All::DsScaleNv:return "DsScaleNv";
			case All::DtScaleNv:return "DtScaleNv";
			case All::MagnitudeScaleNv:return "MagnitudeScaleNv";
			case All::VibranceScaleNv:return "VibranceScaleNv";
			case All::HiBiasNv:return "HiBiasNv";
			case All::LoBiasNv:return "LoBiasNv";
			case All::DsBiasNv:return "DsBiasNv";
			case All::DtBiasNv:return "DtBiasNv";
			case All::MagnitudeBiasNv:return "MagnitudeBiasNv";
			case All::VibranceBiasNv:return "VibranceBiasNv";
			case All::TextureBorderValuesNv:return "TextureBorderValuesNv";
			case All::TextureHiSizeNv:return "TextureHiSizeNv";
			case All::TextureLoSizeNv:return "TextureLoSizeNv";
			case All::TextureDsSizeNv:return "TextureDsSizeNv";
			case All::TextureDtSizeNv:return "TextureDtSizeNv";
			case All::TextureMagSizeNv:return "TextureMagSizeNv";
			case All::ModelView2Arb:return "ModelView2Arb";
			case All::ModelView3Arb:return "ModelView3Arb";
			case All::ModelView4Arb:return "ModelView4Arb";
			case All::ModelView5Arb:return "ModelView5Arb";
			case All::ModelView6Arb:return "ModelView6Arb";
			case All::ModelView7Arb:return "ModelView7Arb";
			case All::ModelView8Arb:return "ModelView8Arb";
			case All::ModelView9Arb:return "ModelView9Arb";
			case All::ModelView10Arb:return "ModelView10Arb";
			case All::ModelView11Arb:return "ModelView11Arb";
			case All::ModelView12Arb:return "ModelView12Arb";
			case All::ModelView13Arb:return "ModelView13Arb";
			case All::ModelView14Arb:return "ModelView14Arb";
			case All::ModelView15Arb:return "ModelView15Arb";
			case All::ModelView16Arb:return "ModelView16Arb";
			case All::ModelView17Arb:return "ModelView17Arb";
			case All::ModelView18Arb:return "ModelView18Arb";
			case All::ModelView19Arb:return "ModelView19Arb";
			case All::ModelView20Arb:return "ModelView20Arb";
			case All::ModelView21Arb:return "ModelView21Arb";
			case All::ModelView22Arb:return "ModelView22Arb";
			case All::ModelView23Arb:return "ModelView23Arb";
			case All::ModelView24Arb:return "ModelView24Arb";
			case All::ModelView25Arb:return "ModelView25Arb";
			case All::ModelView26Arb:return "ModelView26Arb";
			case All::ModelView27Arb:return "ModelView27Arb";
			case All::ModelView28Arb:return "ModelView28Arb";
			case All::ModelView29Arb:return "ModelView29Arb";
			case All::ModelView30Arb:return "ModelView30Arb";
			case All::ModelView31Arb:return "ModelView31Arb";
			case All::Dot3RgbExt:return "Dot3RgbExt";
			case All::Dot3RgbaExt:return "Dot3RgbaExt";
			case All::ModulateAddAti:return "ModulateAddAti";
			case All::ModulateSignedAddAti:return "ModulateSignedAddAti";
			case All::ModulateSubtractAti:return "ModulateSubtractAti";
			case All::SetAmd:return "SetAmd";
			case All::ReplaceValueAmd:return "ReplaceValueAmd";
			case All::StencilOpValueAmd:return "StencilOpValueAmd";
			case All::StencilBackOpValueAmd:return "StencilBackOpValueAmd";
			case All::OcclusionQueryEventMaskAmd:return "OcclusionQueryEventMaskAmd";
			case All::UnsignedInt248Mesa:return "UnsignedInt248Mesa";
			case All::UnsignedInt824RevMesa:return "UnsignedInt824RevMesa";
			case All::UnsignedShort151Mesa:return "UnsignedShort151Mesa";
			case All::UnsignedShort115RevMesa:return "UnsignedShort115RevMesa";
			case All::TraceMaskMesa:return "TraceMaskMesa";
			case All::TraceNameMesa:return "TraceNameMesa";
			case All::YcbcrMesa:return "YcbcrMesa";
			case All::PackInvertMesa:return "PackInvertMesa";
			case All::/* NOT an alias. Accidental reuse of GL_TEXTURE_1D_STACK_MESAX */ DebugObjectMesa:return "/* NOT an alias. Accidental reuse of GL_TEXTURE_1D_STACK_MESAX */ DebugObjectMesa";
			case All::/* NOT an alias. Accidental reuse of GL_TEXTURE_2D_STACK_MESAX */ DebugPrintMesa:return "/* NOT an alias. Accidental reuse of GL_TEXTURE_2D_STACK_MESAX */ DebugPrintMesa";
			case All::/* NOT an alias. Accidental reuse of GL_PROXY_TEXTURE_1D_STACK_MESAX */ DebugAssertMesa:return "/* NOT an alias. Accidental reuse of GL_PROXY_TEXTURE_1D_STACK_MESAX */ DebugAssertMesa";
			case All::ProxyTexture2DStackMesax:return "ProxyTexture2DStackMesax";
			case All::Texture1DStackBindingMesax:return "Texture1DStackBindingMesax";
			case All::Texture2DStackBindingMesax:return "Texture2DStackBindingMesax";
			case All::ProgramBinaryFormatMesa:return "ProgramBinaryFormatMesa";
			case All::ArrayObjectBufferAti:return "ArrayObjectBufferAti";
			case All::ArrayObjectOffsetAti:return "ArrayObjectOffsetAti";
			case All::ElementArrayAti:return "ElementArrayAti";
			case All::ElementArrayTypeAti:return "ElementArrayTypeAti";
			case All::ElementArrayPointerAti:return "ElementArrayPointerAti";
			case All::MaxVertexStreamsAti:return "MaxVertexStreamsAti";
			case All::VertexSourceAti:return "VertexSourceAti";
			case All::DudvAti:return "DudvAti";
			case All::Du8dv8Ati:return "Du8dv8Ati";
			case All::BumpEnvmapAti:return "BumpEnvmapAti";
			case All::BumpTargetAti:return "BumpTargetAti";
			case All::VertexShaderExt:return "VertexShaderExt";
			case All::VertexShaderBindingExt:return "VertexShaderBindingExt";
			case All::OutputVertexExt:return "OutputVertexExt";
			case All::OutputColor0Ext:return "OutputColor0Ext";
			case All::OutputColor1Ext:return "OutputColor1Ext";
			case All::OutputTextureCoord0Ext:return "OutputTextureCoord0Ext";
			case All::OutputTextureCoord1Ext:return "OutputTextureCoord1Ext";
			case All::OutputTextureCoord2Ext:return "OutputTextureCoord2Ext";
			case All::OutputTextureCoord3Ext:return "OutputTextureCoord3Ext";
			case All::OutputTextureCoord4Ext:return "OutputTextureCoord4Ext";
			case All::OutputTextureCoord5Ext:return "OutputTextureCoord5Ext";
			case All::OutputTextureCoord6Ext:return "OutputTextureCoord6Ext";
			case All::OutputTextureCoord7Ext:return "OutputTextureCoord7Ext";
			case All::OutputTextureCoord8Ext:return "OutputTextureCoord8Ext";
			case All::OutputTextureCoord9Ext:return "OutputTextureCoord9Ext";
			case All::OutputTextureCoord10Ext:return "OutputTextureCoord10Ext";
			case All::OutputTextureCoord11Ext:return "OutputTextureCoord11Ext";
			case All::OutputTextureCoord12Ext:return "OutputTextureCoord12Ext";
			case All::OutputTextureCoord13Ext:return "OutputTextureCoord13Ext";
			case All::OutputTextureCoord14Ext:return "OutputTextureCoord14Ext";
			case All::OutputTextureCoord15Ext:return "OutputTextureCoord15Ext";
			case All::OutputTextureCoord16Ext:return "OutputTextureCoord16Ext";
			case All::OutputTextureCoord17Ext:return "OutputTextureCoord17Ext";
			case All::OutputTextureCoord18Ext:return "OutputTextureCoord18Ext";
			case All::OutputTextureCoord19Ext:return "OutputTextureCoord19Ext";
			case All::OutputTextureCoord20Ext:return "OutputTextureCoord20Ext";
			case All::OutputTextureCoord21Ext:return "OutputTextureCoord21Ext";
			case All::OutputTextureCoord22Ext:return "OutputTextureCoord22Ext";
			case All::OutputTextureCoord23Ext:return "OutputTextureCoord23Ext";
			case All::OutputTextureCoord24Ext:return "OutputTextureCoord24Ext";
			case All::OutputTextureCoord25Ext:return "OutputTextureCoord25Ext";
			case All::OutputTextureCoord26Ext:return "OutputTextureCoord26Ext";
			case All::OutputTextureCoord27Ext:return "OutputTextureCoord27Ext";
			case All::OutputTextureCoord28Ext:return "OutputTextureCoord28Ext";
			case All::OutputTextureCoord29Ext:return "OutputTextureCoord29Ext";
			case All::OutputTextureCoord30Ext:return "OutputTextureCoord30Ext";
			case All::OutputTextureCoord31Ext:return "OutputTextureCoord31Ext";
			case All::OutputFogExt:return "OutputFogExt";
			case All::MaxVertexShaderInstructionsExt:return "MaxVertexShaderInstructionsExt";
			case All::MaxVertexShaderVariantsExt:return "MaxVertexShaderVariantsExt";
			case All::MaxVertexShaderInvariantsExt:return "MaxVertexShaderInvariantsExt";
			case All::MaxVertexShaderLocalConstantsExt:return "MaxVertexShaderLocalConstantsExt";
			case All::MaxVertexShaderLocalsExt:return "MaxVertexShaderLocalsExt";
			case All::MaxOptimizedVertexShaderInstructionsExt:return "MaxOptimizedVertexShaderInstructionsExt";
			case All::MaxOptimizedVertexShaderVariantsExt:return "MaxOptimizedVertexShaderVariantsExt";
			case All::MaxOptimizedVertexShaderLocalConstantsExt:return "MaxOptimizedVertexShaderLocalConstantsExt";
			case All::MaxOptimizedVertexShaderInvariantsExt:return "MaxOptimizedVertexShaderInvariantsExt";
			case All::MaxOptimizedVertexShaderLocalsExt:return "MaxOptimizedVertexShaderLocalsExt";
			case All::VertexShaderInstructionsExt:return "VertexShaderInstructionsExt";
			case All::VertexShaderVariantsExt:return "VertexShaderVariantsExt";
			case All::VertexShaderInvariantsExt:return "VertexShaderInvariantsExt";
			case All::VertexShaderLocalConstantsExt:return "VertexShaderLocalConstantsExt";
			case All::VertexShaderLocalsExt:return "VertexShaderLocalsExt";
			case All::VertexShaderOptimizedExt:return "VertexShaderOptimizedExt";
			case All::VariantArrayPointerExt:return "VariantArrayPointerExt";
			case All::InvariantValueExt:return "InvariantValueExt";
			case All::InvariantDatatypeExt:return "InvariantDatatypeExt";
			case All::LocalConstantValueExt:return "LocalConstantValueExt";
			case All::LocalConstantDatatypeExt:return "LocalConstantDatatypeExt";
			case All::PnTrianglesAti:return "PnTrianglesAti";
			case All::MaxPnTrianglesTesselationLevelAti:return "MaxPnTrianglesTesselationLevelAti";
			case All::PnTrianglesPointModeLinearAti:return "PnTrianglesPointModeLinearAti";
			case All::PnTrianglesPointModeCubicAti:return "PnTrianglesPointModeCubicAti";
			case All::PnTrianglesNormalModeLinearAti:return "PnTrianglesNormalModeLinearAti";
			case All::PnTrianglesNormalModeQuadraticAti:return "PnTrianglesNormalModeQuadraticAti";
			case All::VboFreeMemoryAti:return "VboFreeMemoryAti";
			case All::TextureFreeMemoryAti:return "TextureFreeMemoryAti";
			case All::RenderbufferFreeMemoryAti:return "RenderbufferFreeMemoryAti";
			case All::NumProgramBinaryFormatsOes:return "NumProgramBinaryFormatsOes";
			case All::ProgramBinaryFormatsOes:return "ProgramBinaryFormatsOes";
			case All::StencilBackFuncAti:return "StencilBackFuncAti";
			case All::StencilBackFailAti:return "StencilBackFailAti";
			case All::StencilBackPassDepthFailAti:return "StencilBackPassDepthFailAti";
			case All::StencilBackPassDepthPassAti:return "StencilBackPassDepthPassAti";
			case All::ProgramAluInstructionsArb:return "ProgramAluInstructionsArb";
			case All::ProgramTexInstructionsArb:return "ProgramTexInstructionsArb";
			case All::ProgramTexIndirectionsArb:return "ProgramTexIndirectionsArb";
			case All::ProgramNativeAluInstructionsArb:return "ProgramNativeAluInstructionsArb";
			case All::ProgramNativeTexInstructionsArb:return "ProgramNativeTexInstructionsArb";
			case All::ProgramNativeTexIndirectionsArb:return "ProgramNativeTexIndirectionsArb";
			case All::MaxProgramAluInstructionsArb:return "MaxProgramAluInstructionsArb";
			case All::MaxProgramTexInstructionsArb:return "MaxProgramTexInstructionsArb";
			case All::MaxProgramTexIndirectionsArb:return "MaxProgramTexIndirectionsArb";
			case All::MaxProgramNativeAluInstructionsArb:return "MaxProgramNativeAluInstructionsArb";
			case All::MaxProgramNativeTexInstructionsArb:return "MaxProgramNativeTexInstructionsArb";
			case All::MaxProgramNativeTexIndirectionsArb:return "MaxProgramNativeTexIndirectionsArb";
			case All::RgbaFloat32Apple:return "RgbaFloat32Apple";
			case All::RgbFloat32Apple:return "RgbFloat32Apple";
			case All::Alpha32fArb:return "Alpha32fArb";
			case All::Intensity32fArb:return "Intensity32fArb";
			case All::Luminance32fArb:return "Luminance32fArb";
			case All::LuminanceAlpha32fArb:return "LuminanceAlpha32fArb";
			case All::RgbaFloat16Apple:return "RgbaFloat16Apple";
			case All::RgbFloat16Apple:return "RgbFloat16Apple";
			case All::Alpha16fArb:return "Alpha16fArb";
			case All::Intensity16fArb:return "Intensity16fArb";
			case All::Luminance16fArb:return "Luminance16fArb";
			case All::LuminanceAlpha16fArb:return "LuminanceAlpha16fArb";
			case All::RgbaFloatModeArb:return "RgbaFloatModeArb";
			case All::WriteonlyRenderingQcom:return "WriteonlyRenderingQcom";
			case All::MaxDrawBuffersArb:return "MaxDrawBuffersArb";
			case All::DrawBuffer0:return "DrawBuffer0";
			case All::DrawBuffer1:return "DrawBuffer1";
			case All::DrawBuffer2:return "DrawBuffer2";
			case All::DrawBuffer3:return "DrawBuffer3";
			case All::DrawBuffer4:return "DrawBuffer4";
			case All::DrawBuffer5:return "DrawBuffer5";
			case All::DrawBuffer6:return "DrawBuffer6";
			case All::DrawBuffer7:return "DrawBuffer7";
			case All::DrawBuffer8:return "DrawBuffer8";
			case All::DrawBuffer9:return "DrawBuffer9";
			case All::DrawBuffer10:return "DrawBuffer10";
			case All::DrawBuffer11:return "DrawBuffer11";
			case All::DrawBuffer12:return "DrawBuffer12";
			case All::DrawBuffer13:return "DrawBuffer13";
			case All::DrawBuffer14:return "DrawBuffer14";
			case All::DrawBuffer15:return "DrawBuffer15";
			case All::ColorClearUnclampedValueAti:return "ColorClearUnclampedValueAti";
			case All::/* Defined by Mesa but not ATI */ CompressedLuminanceAlpha3DcAti:return "/* Defined by Mesa but not ATI */ CompressedLuminanceAlpha3DcAti";
			case All::BlendEquationAlphaExt:return "BlendEquationAlphaExt";
			case All::SubsampleDistanceAmd:return "SubsampleDistanceAmd";
			case All::MatrixPaletteArb:return "MatrixPaletteArb";
			case All::MaxMatrixPaletteStackDepthArb:return "MaxMatrixPaletteStackDepthArb";
			case All::MaxPaletteMatricesArb:return "MaxPaletteMatricesArb";
			case All::CurrentPaletteMatrixArb:return "CurrentPaletteMatrixArb";
			case All::MatrixIndexArrayArb:return "MatrixIndexArrayArb";
			case All::CurrentMatrixIndexArb:return "CurrentMatrixIndexArb";
			case All::MatrixIndexArraySizeArb:return "MatrixIndexArraySizeArb";
			case All::MatrixIndexArrayTypeArb:return "MatrixIndexArrayTypeArb";
			case All::MatrixIndexArrayStrideArb:return "MatrixIndexArrayStrideArb";
			case All::MatrixIndexArrayPointerArb:return "MatrixIndexArrayPointerArb";
			case All::TextureDepthSize:return "TextureDepthSize";
			case All::DepthTextureMode:return "DepthTextureMode";
			case All::TextureCompareModeArb:return "TextureCompareModeArb";
			case All::TextureCompareFuncArb:return "TextureCompareFuncArb";
			case All::CompareRToTextureArb:return "CompareRToTextureArb";
			case All::OffsetProjectiveTexture2DNv:return "OffsetProjectiveTexture2DNv";
			case All::OffsetProjectiveTexture2DScaleNv:return "OffsetProjectiveTexture2DScaleNv";
			case All::OffsetProjectiveTextureRectangleNv:return "OffsetProjectiveTextureRectangleNv";
			case All::OffsetProjectiveTextureRectangleScaleNv:return "OffsetProjectiveTextureRectangleScaleNv";
			case All::OffsetHiloTexture2DNv:return "OffsetHiloTexture2DNv";
			case All::OffsetHiloTextureRectangleNv:return "OffsetHiloTextureRectangleNv";
			case All::OffsetHiloProjectiveTexture2DNv:return "OffsetHiloProjectiveTexture2DNv";
			case All::OffsetHiloProjectiveTextureRectangleNv:return "OffsetHiloProjectiveTextureRectangleNv";
			case All::DependentHiloTexture2DNv:return "DependentHiloTexture2DNv";
			case All::DependentRgbTexture3DNv:return "DependentRgbTexture3DNv";
			case All::DependentRgbTextureCubeMapNv:return "DependentRgbTextureCubeMapNv";
			case All::DotProductPassThroughNv:return "DotProductPassThroughNv";
			case All::DotProductTexture1DNv:return "DotProductTexture1DNv";
			case All::DotProductAffineDepthReplaceNv:return "DotProductAffineDepthReplaceNv";
			case All::Hilo8Nv:return "Hilo8Nv";
			case All::SignedHilo8Nv:return "SignedHilo8Nv";
			case All::ForceBlueToOneNv:return "ForceBlueToOneNv";
			case All::PointSpriteArb:return "PointSpriteArb";
			case All::CoordReplaceArb:return "CoordReplaceArb";
			case All::PointSpriteRModeNv:return "PointSpriteRModeNv";
			case All::PixelCounterBitsNv:return "PixelCounterBitsNv";
			case All::CurrentOcclusionQueryIdNv:return "CurrentOcclusionQueryIdNv";
			case All::QueryResultArb:return "QueryResultArb";
			case All::QueryResultAvailableArb:return "QueryResultAvailableArb";
			case All::MaxFragmentProgramLocalParametersNv:return "MaxFragmentProgramLocalParametersNv";
			case All::MaxVertexAttribsArb:return "MaxVertexAttribsArb";
			case All::VertexAttribArrayNormalizedArb:return "VertexAttribArrayNormalizedArb";
			case All::MaxTessControlInputComponents:return "MaxTessControlInputComponents";
			case All::MaxTessEvaluationInputComponents:return "MaxTessEvaluationInputComponents";
			case All::DepthStencilToRgbaNv:return "DepthStencilToRgbaNv";
			case All::DepthStencilToBgraNv:return "DepthStencilToBgraNv";
			case All::FragmentProgramNv:return "FragmentProgramNv";
			case All::MaxTextureCoords:return "MaxTextureCoords";
			case All::MaxTextureImageUnitsArb:return "MaxTextureImageUnitsArb";
			case All::FragmentProgramBindingNv:return "FragmentProgramBindingNv";
			case All::ProgramErrorStringArb:return "ProgramErrorStringArb";
			case All::ProgramFormatArb:return "ProgramFormatArb";
			case All::WritePixelDataRangeLengthNv:return "WritePixelDataRangeLengthNv";
			case All::ReadPixelDataRangeLengthNv:return "ReadPixelDataRangeLengthNv";
			case All::WritePixelDataRangePointerNv:return "WritePixelDataRangePointerNv";
			case All::ReadPixelDataRangePointerNv:return "ReadPixelDataRangePointerNv";
			case All::GeometryShaderInvocations:return "GeometryShaderInvocations";
			case All::FloatRNv:return "FloatRNv";
			case All::FloatRgNv:return "FloatRgNv";
			case All::FloatRgbNv:return "FloatRgbNv";
			case All::FloatRgbaNv:return "FloatRgbaNv";
			case All::FloatR16Nv:return "FloatR16Nv";
			case All::FloatR32Nv:return "FloatR32Nv";
			case All::FloatRg16Nv:return "FloatRg16Nv";
			case All::FloatRg32Nv:return "FloatRg32Nv";
			case All::FloatRgb16Nv:return "FloatRgb16Nv";
			case All::FloatRgb32Nv:return "FloatRgb32Nv";
			case All::FloatRgba16Nv:return "FloatRgba16Nv";
			case All::FloatRgba32Nv:return "FloatRgba32Nv";
			case All::TextureFloatComponentsNv:return "TextureFloatComponentsNv";
			case All::FloatClearColorValueNv:return "FloatClearColorValueNv";
			case All::FloatRgbaModeNv:return "FloatRgbaModeNv";
			case All::TextureUnsignedRemapModeNv:return "TextureUnsignedRemapModeNv";
			case All::DepthBoundsTestExt:return "DepthBoundsTestExt";
			case All::DepthBoundsExt:return "DepthBoundsExt";
			case All::ArrayBufferArb:return "ArrayBufferArb";
			case All::ElementArrayBufferArb:return "ElementArrayBufferArb";
			case All::ArrayBufferBindingArb:return "ArrayBufferBindingArb";
			case All::ElementArrayBufferBindingArb:return "ElementArrayBufferBindingArb";
			case All::VertexArrayBufferBinding:return "VertexArrayBufferBinding";
			case All::NormalArrayBufferBinding:return "NormalArrayBufferBinding";
			case All::ColorArrayBufferBinding:return "ColorArrayBufferBinding";
			case All::IndexArrayBufferBinding:return "IndexArrayBufferBinding";
			case All::TextureCoordArrayBufferBinding:return "TextureCoordArrayBufferBinding";
			case All::EdgeFlagArrayBufferBinding:return "EdgeFlagArrayBufferBinding";
			case All::SecondaryColorArrayBufferBinding:return "SecondaryColorArrayBufferBinding";
			case All::FogCoordinateArrayBufferBindingArb:return "FogCoordinateArrayBufferBindingArb";
			case All::WeightArrayBufferBinding:return "WeightArrayBufferBinding";
			case All::VertexAttribArrayBufferBindingArb:return "VertexAttribArrayBufferBindingArb";
			case All::ProgramInstructionsArb:return "ProgramInstructionsArb";
			case All::MaxProgramInstructionsArb:return "MaxProgramInstructionsArb";
			case All::ProgramNativeInstructionsArb:return "ProgramNativeInstructionsArb";
			case All::MaxProgramNativeInstructionsArb:return "MaxProgramNativeInstructionsArb";
			case All::ProgramTemporariesArb:return "ProgramTemporariesArb";
			case All::MaxProgramTemporariesArb:return "MaxProgramTemporariesArb";
			case All::ProgramNativeTemporariesArb:return "ProgramNativeTemporariesArb";
			case All::MaxProgramNativeTemporariesArb:return "MaxProgramNativeTemporariesArb";
			case All::ProgramParametersArb:return "ProgramParametersArb";
			case All::MaxProgramParametersArb:return "MaxProgramParametersArb";
			case All::ProgramNativeParametersArb:return "ProgramNativeParametersArb";
			case All::MaxProgramNativeParametersArb:return "MaxProgramNativeParametersArb";
			case All::ProgramAttribsArb:return "ProgramAttribsArb";
			case All::MaxProgramAttribsArb:return "MaxProgramAttribsArb";
			case All::ProgramNativeAttribsArb:return "ProgramNativeAttribsArb";
			case All::MaxProgramNativeAttribsArb:return "MaxProgramNativeAttribsArb";
			case All::ProgramAddressRegistersArb:return "ProgramAddressRegistersArb";
			case All::MaxProgramAddressRegistersArb:return "MaxProgramAddressRegistersArb";
			case All::ProgramNativeAddressRegistersArb:return "ProgramNativeAddressRegistersArb";
			case All::MaxProgramNativeAddressRegistersArb:return "MaxProgramNativeAddressRegistersArb";
			case All::MaxProgramLocalParametersArb:return "MaxProgramLocalParametersArb";
			case All::MaxProgramEnvParametersArb:return "MaxProgramEnvParametersArb";
			case All::ProgramUnderNativeLimitsArb:return "ProgramUnderNativeLimitsArb";
			case All::TransposeCurrentMatrixArb:return "TransposeCurrentMatrixArb";
			case All::ReadOnlyArb:return "ReadOnlyArb";
			case All::WriteOnlyArb:return "WriteOnlyArb";
			case All::ReadWriteArb:return "ReadWriteArb";
			case All::BufferAccessOes:return "BufferAccessOes";
			case All::BufferMappedOes:return "BufferMappedOes";
			case All::BufferMapPointerOes:return "BufferMapPointerOes";
			case All::WriteDiscardNv:return "WriteDiscardNv";
			case All::TimeElapsedExt:return "TimeElapsedExt";
			case All::Matrix0Arb:return "Matrix0Arb";
			case All::Matrix1Arb:return "Matrix1Arb";
			case All::Matrix2Arb:return "Matrix2Arb";
			case All::Matrix3Arb:return "Matrix3Arb";
			case All::Matrix4Arb:return "Matrix4Arb";
			case All::Matrix5Arb:return "Matrix5Arb";
			case All::Matrix6Arb:return "Matrix6Arb";
			case All::Matrix7Arb:return "Matrix7Arb";
			case All::Matrix8Arb:return "Matrix8Arb";
			case All::Matrix9Arb:return "Matrix9Arb";
			case All::Matrix10Arb:return "Matrix10Arb";
			case All::Matrix11Arb:return "Matrix11Arb";
			case All::Matrix12Arb:return "Matrix12Arb";
			case All::Matrix13Arb:return "Matrix13Arb";
			case All::Matrix14Arb:return "Matrix14Arb";
			case All::Matrix15Arb:return "Matrix15Arb";
			case All::Matrix16Arb:return "Matrix16Arb";
			case All::Matrix17Arb:return "Matrix17Arb";
			case All::Matrix18Arb:return "Matrix18Arb";
			case All::Matrix19Arb:return "Matrix19Arb";
			case All::Matrix20Arb:return "Matrix20Arb";
			case All::Matrix21Arb:return "Matrix21Arb";
			case All::Matrix22Arb:return "Matrix22Arb";
			case All::Matrix23Arb:return "Matrix23Arb";
			case All::Matrix24Arb:return "Matrix24Arb";
			case All::Matrix25Arb:return "Matrix25Arb";
			case All::Matrix26Arb:return "Matrix26Arb";
			case All::Matrix27Arb:return "Matrix27Arb";
			case All::Matrix28Arb:return "Matrix28Arb";
			case All::Matrix29Arb:return "Matrix29Arb";
			case All::Matrix30Arb:return "Matrix30Arb";
			case All::Matrix31Arb:return "Matrix31Arb";
			case All::StreamDrawArb:return "StreamDrawArb";
			case All::StreamReadArb:return "StreamReadArb";
			case All::StreamCopyArb:return "StreamCopyArb";
			case All::StaticDrawArb:return "StaticDrawArb";
			case All::StaticReadArb:return "StaticReadArb";
			case All::StaticCopyArb:return "StaticCopyArb";
			case All::DynamicDrawArb:return "DynamicDrawArb";
			case All::DynamicReadArb:return "DynamicReadArb";
			case All::DynamicCopyArb:return "DynamicCopyArb";
			case All::PixelPackBufferArb:return "PixelPackBufferArb";
			case All::PixelUnpackBufferArb:return "PixelUnpackBufferArb";
			case All::PixelPackBufferBindingArb:return "PixelPackBufferBindingArb";
			case All::PixelUnpackBufferBindingArb:return "PixelUnpackBufferBindingArb";
			case All::TextureStencilSize:return "TextureStencilSize";
			case All::StencilTagBitsExt:return "StencilTagBitsExt";
			case All::StencilClearTagValueExt:return "StencilClearTagValueExt";
			case All::MaxProgramExecInstructionsNv:return "MaxProgramExecInstructionsNv";
			case All::MaxProgramCallDepthNv:return "MaxProgramCallDepthNv";
			case All::MaxProgramIfDepthNv:return "MaxProgramIfDepthNv";
			case All::MaxProgramLoopDepthNv:return "MaxProgramLoopDepthNv";
			case All::MaxProgramLoopCountNv:return "MaxProgramLoopCountNv";
			case All::Src1ColorExt:return "Src1ColorExt";
			case All::OneMinusSrc1ColorExt:return "OneMinusSrc1ColorExt";
			case All::OneMinusSrc1AlphaExt:return "OneMinusSrc1AlphaExt";
			case All::MaxDualSourceDrawBuffersExt:return "MaxDualSourceDrawBuffersExt";
			case All::VertexAttribArrayIntegerNv:return "VertexAttribArrayIntegerNv";
			case All::VertexAttribArrayDivisorAngle:return "VertexAttribArrayDivisorAngle";
			case All::MaxArrayTextureLayersExt:return "MaxArrayTextureLayersExt";
			case All::MinProgramTexelOffsetExt:return "MinProgramTexelOffsetExt";
			case All::MaxProgramTexelOffsetExt:return "MaxProgramTexelOffsetExt";
			case All::ProgramAttribComponentsNv:return "ProgramAttribComponentsNv";
			case All::ProgramResultComponentsNv:return "ProgramResultComponentsNv";
			case All::MaxProgramAttribComponentsNv:return "MaxProgramAttribComponentsNv";
			case All::MaxProgramResultComponentsNv:return "MaxProgramResultComponentsNv";
			case All::StencilTestTwoSideExt:return "StencilTestTwoSideExt";
			case All::ActiveStencilFaceExt:return "ActiveStencilFaceExt";
			case All::SamplesPassedArb:return "SamplesPassedArb";
			case All::GeometryLinkedVerticesOutExt:return "GeometryLinkedVerticesOutExt";
			case All::GeometryLinkedInputTypeExt:return "GeometryLinkedInputTypeExt";
			case All::GeometryLinkedOutputTypeExt:return "GeometryLinkedOutputTypeExt";
			case All::ClampVertexColor:return "ClampVertexColor";
			case All::ClampFragmentColor:return "ClampFragmentColor";
			case All::NumFragmentRegistersAti:return "NumFragmentRegistersAti";
			case All::NumFragmentConstantsAti:return "NumFragmentConstantsAti";
			case All::NumPassesAti:return "NumPassesAti";
			case All::NumInstructionsPerPassAti:return "NumInstructionsPerPassAti";
			case All::NumInstructionsTotalAti:return "NumInstructionsTotalAti";
			case All::NumInputInterpolatorComponentsAti:return "NumInputInterpolatorComponentsAti";
			case All::NumLoopbackComponentsAti:return "NumLoopbackComponentsAti";
			case All::ColorAlphaPairingAti:return "ColorAlphaPairingAti";
			case All::SwizzleStrqAti:return "SwizzleStrqAti";
			case All::SwizzleStrqDqAti:return "SwizzleStrqDqAti";
			case All::InterlaceOml:return "InterlaceOml";
			case All::InterlaceReadOml:return "InterlaceReadOml";
			case All::FormatSubsample2424Oml:return "FormatSubsample2424Oml";
			case All::FormatSubsample244244Oml:return "FormatSubsample244244Oml";
			case All::ResampleReplicateOml:return "ResampleReplicateOml";
			case All::ResampleZeroFillOml:return "ResampleZeroFillOml";
			case All::ResampleAverageOml:return "ResampleAverageOml";
			case All::ResampleDecimateOml:return "ResampleDecimateOml";
			case All::PointSizeArrayTypeOes:return "PointSizeArrayTypeOes";
			case All::PointSizeArrayStrideOes:return "PointSizeArrayStrideOes";
			case All::PointSizeArrayPointerOes:return "PointSizeArrayPointerOes";
			case All::ModelViewMatrixFloatAsIntBitsOes:return "ModelViewMatrixFloatAsIntBitsOes";
			case All::ProjectionMatrixFloatAsIntBitsOes:return "ProjectionMatrixFloatAsIntBitsOes";
			case All::TextureMatrixFloatAsIntBitsOes:return "TextureMatrixFloatAsIntBitsOes";
			case All::VertexAttribMap1Apple:return "VertexAttribMap1Apple";
			case All::VertexAttribMap2Apple:return "VertexAttribMap2Apple";
			case All::VertexAttribMap1SizeApple:return "VertexAttribMap1SizeApple";
			case All::VertexAttribMap1CoeffApple:return "VertexAttribMap1CoeffApple";
			case All::VertexAttribMap1OrderApple:return "VertexAttribMap1OrderApple";
			case All::VertexAttribMap1DomainApple:return "VertexAttribMap1DomainApple";
			case All::VertexAttribMap2SizeApple:return "VertexAttribMap2SizeApple";
			case All::VertexAttribMap2CoeffApple:return "VertexAttribMap2CoeffApple";
			case All::VertexAttribMap2OrderApple:return "VertexAttribMap2OrderApple";
			case All::VertexAttribMap2DomainApple:return "VertexAttribMap2DomainApple";
			case All::ElementArrayApple:return "ElementArrayApple";
			case All::ElementArrayTypeApple:return "ElementArrayTypeApple";
			case All::ElementArrayPointerApple:return "ElementArrayPointerApple";
			case All::ColorFloatApple:return "ColorFloatApple";
			case All::BufferSerializedModifyApple:return "BufferSerializedModifyApple";
			case All::BufferFlushingUnmapApple:return "BufferFlushingUnmapApple";
			case All::AuxDepthStencilApple:return "AuxDepthStencilApple";
			case All::PackRowBytesApple:return "PackRowBytesApple";
			case All::UnpackRowBytesApple:return "UnpackRowBytesApple";
			case All::ReleasedApple:return "ReleasedApple";
			case All::VolatileApple:return "VolatileApple";
			case All::RetainedApple:return "RetainedApple";
			case All::UndefinedApple:return "UndefinedApple";
			case All::PurgeableApple:return "PurgeableApple";
			case All::Rgb422Apple:return "Rgb422Apple";
			case All::MaxGeometryUniformBlocksExt:return "MaxGeometryUniformBlocksExt";
			case All::MaxCombinedGeometryUniformComponentsExt:return "MaxCombinedGeometryUniformComponentsExt";
			case All::TextureSrgbDecodeExt:return "TextureSrgbDecodeExt";
			case All::DecodeExt:return "DecodeExt";
			case All::SkipDecodeExt:return "SkipDecodeExt";
			case All::ProgramPipelineObjectExt:return "ProgramPipelineObjectExt";
			case All::RgbRaw422Apple:return "RgbRaw422Apple";
			case All::FragmentShaderDiscardsSamplesExt:return "FragmentShaderDiscardsSamplesExt";
			case All::SyncObjectApple:return "SyncObjectApple";
			case All::ShaderObjectArb:return "ShaderObjectArb";
			case All::MaxFragmentUniformComponentsArb:return "MaxFragmentUniformComponentsArb";
			case All::MaxVertexUniformComponentsArb:return "MaxVertexUniformComponentsArb";
			case All::MaxVaryingComponentsExt:return "MaxVaryingComponentsExt";
			case All::MaxVertexTextureImageUnitsArb:return "MaxVertexTextureImageUnitsArb";
			case All::MaxCombinedTextureImageUnitsArb:return "MaxCombinedTextureImageUnitsArb";
			case All::ObjectTypeArb:return "ObjectTypeArb";
			case All::ObjectSubtypeArb:return "ObjectSubtypeArb";
			case All::ObjectDeleteStatusArb:return "ObjectDeleteStatusArb";
			case All::ObjectCompileStatusArb:return "ObjectCompileStatusArb";
			case All::ObjectLinkStatusArb:return "ObjectLinkStatusArb";
			case All::ObjectValidateStatusArb:return "ObjectValidateStatusArb";
			case All::ObjectInfoLogLengthArb:return "ObjectInfoLogLengthArb";
			case All::ObjectAttachedObjectsArb:return "ObjectAttachedObjectsArb";
			case All::ObjectActiveUniformsArb:return "ObjectActiveUniformsArb";
			case All::ObjectActiveUniformMaxLengthArb:return "ObjectActiveUniformMaxLengthArb";
			case All::ObjectShaderSourceLengthArb:return "ObjectShaderSourceLengthArb";
			case All::ObjectActiveAttributesArb:return "ObjectActiveAttributesArb";
			case All::ObjectActiveAttributeMaxLengthArb:return "ObjectActiveAttributeMaxLengthArb";
			case All::ShadingLanguageVersionArb:return "ShadingLanguageVersionArb";
			case All::Palette4Rgb8Oes:return "Palette4Rgb8Oes";
			case All::Palette4Rgba8Oes:return "Palette4Rgba8Oes";
			case All::Palette4R5G6B5Oes:return "Palette4R5G6B5Oes";
			case All::Palette4Rgba4Oes:return "Palette4Rgba4Oes";
			case All::Palette4Rgb5A1Oes:return "Palette4Rgb5A1Oes";
			case All::Palette8Rgb8Oes:return "Palette8Rgb8Oes";
			case All::Palette8Rgba8Oes:return "Palette8Rgba8Oes";
			case All::Palette8R5G6B5Oes:return "Palette8R5G6B5Oes";
			case All::Palette8Rgba4Oes:return "Palette8Rgba4Oes";
			case All::Palette8Rgb5A1Oes:return "Palette8Rgb5A1Oes";
			case All::ImplementationColorReadTypeOes:return "ImplementationColorReadTypeOes";
			case All::ImplementationColorReadFormatOes:return "ImplementationColorReadFormatOes";
			case All::PointSizeArrayOes:return "PointSizeArrayOes";
			case All::TextureCropRectOes:return "TextureCropRectOes";
			case All::MatrixIndexArrayBufferBindingOes:return "MatrixIndexArrayBufferBindingOes";
			case All::PointSizeArrayBufferBindingOes:return "PointSizeArrayBufferBindingOes";
			case All::FragmentProgramPositionMesa:return "FragmentProgramPositionMesa";
			case All::FragmentProgramCallbackMesa:return "FragmentProgramCallbackMesa";
			case All::FragmentProgramCallbackFuncMesa:return "FragmentProgramCallbackFuncMesa";
			case All::FragmentProgramCallbackDataMesa:return "FragmentProgramCallbackDataMesa";
			case All::VertexProgramPositionMesa:return "VertexProgramPositionMesa";
			case All::VertexProgramCallbackMesa:return "VertexProgramCallbackMesa";
			case All::VertexProgramCallbackFuncMesa:return "VertexProgramCallbackFuncMesa";
			case All::VertexProgramCallbackDataMesa:return "VertexProgramCallbackDataMesa";
			case All::TileRasterOrderFixedMesa:return "TileRasterOrderFixedMesa";
			case All::TileRasterOrderIncreasingXMesa:return "TileRasterOrderIncreasingXMesa";
			case All::TileRasterOrderIncreasingYMesa:return "TileRasterOrderIncreasingYMesa";
			case All::FramebufferFlipYMesa:return "FramebufferFlipYMesa";
			case All::FramebufferFlipXMesa:return "FramebufferFlipXMesa";
			case All::FramebufferSwapXyMesa:return "FramebufferSwapXyMesa";
			case All::ConstBwTilingMesa:return "ConstBwTilingMesa";
			case All::CounterTypeAmd:return "CounterTypeAmd";
			case All::CounterRangeAmd:return "CounterRangeAmd";
			case All::UnsignedInt64Amd:return "UnsignedInt64Amd";
			case All::PercentageAmd:return "PercentageAmd";
			case All::PerfmonResultAvailableAmd:return "PerfmonResultAvailableAmd";
			case All::PerfmonResultSizeAmd:return "PerfmonResultSizeAmd";
			case All::PerfmonResultAmd:return "PerfmonResultAmd";
			case All::TextureWidthQcom:return "TextureWidthQcom";
			case All::TextureHeightQcom:return "TextureHeightQcom";
			case All::TextureDepthQcom:return "TextureDepthQcom";
			case All::TextureInternalFormatQcom:return "TextureInternalFormatQcom";
			case All::TextureFormatQcom:return "TextureFormatQcom";
			case All::TextureTypeQcom:return "TextureTypeQcom";
			case All::TextureImageValidQcom:return "TextureImageValidQcom";
			case All::TextureNumLevelsQcom:return "TextureNumLevelsQcom";
			case All::TextureTargetQcom:return "TextureTargetQcom";
			case All::TextureObjectValidQcom:return "TextureObjectValidQcom";
			case All::StateRestore:return "StateRestore";
			case All::SamplerExternal2DY2yExt:return "SamplerExternal2DY2yExt";
			case All::TextureProtectedExt:return "TextureProtectedExt";
			case All::TextureFoveatedFeatureBitsQcom:return "TextureFoveatedFeatureBitsQcom";
			case All::TextureFoveatedMinPixelDensityQcom:return "TextureFoveatedMinPixelDensityQcom";
			case All::TextureFoveatedFeatureQueryQcom:return "TextureFoveatedFeatureQueryQcom";
			case All::TextureFoveatedNumFocalPointsQueryQcom:return "TextureFoveatedNumFocalPointsQueryQcom";
			case All::FramebufferIncompleteFoveationQcom:return "FramebufferIncompleteFoveationQcom";
			case All::ModulateColorImg:return "ModulateColorImg";
			case All::RecipAddSignedAlphaImg:return "RecipAddSignedAlphaImg";
			case All::TextureAlphaModulateImg:return "TextureAlphaModulateImg";
			case All::FactorAlphaModulateImg:return "FactorAlphaModulateImg";
			case All::FragmentAlphaModulateImg:return "FragmentAlphaModulateImg";
			case All::AddBlendImg:return "AddBlendImg";
			case All::TextureRedType:return "TextureRedType";
			case All::TextureGreenType:return "TextureGreenType";
			case All::TextureBlueType:return "TextureBlueType";
			case All::TextureAlphaType:return "TextureAlphaType";
			case All::TextureLuminanceType:return "TextureLuminanceType";
			case All::TextureIntensityType:return "TextureIntensityType";
			case All::TextureDepthType:return "TextureDepthType";
			case All::UnsignedNormalized:return "UnsignedNormalized";
			case All::Texture1DArrayExt:return "Texture1DArrayExt";
			case All::Texture2DArrayExt:return "Texture2DArrayExt";
			case All::TextureBinding1DArrayExt:return "TextureBinding1DArrayExt";
			case All::TextureBinding2DArrayExt:return "TextureBinding2DArrayExt";
			case All::MaxProgramOutputVerticesNv:return "MaxProgramOutputVerticesNv";
			case All::MaxProgramTotalOutputComponentsNv:return "MaxProgramTotalOutputComponentsNv";
			case All::MaxGeometryTextureImageUnitsArb:return "MaxGeometryTextureImageUnitsArb";
			case All::TextureBufferArb:return "TextureBufferArb";
			case All::MaxTextureBufferSizeArb:return "MaxTextureBufferSizeArb";
			case All::TextureBindingBufferArb:return "TextureBindingBufferArb";
			case All::TextureBufferDataStoreBinding:return "TextureBufferDataStoreBinding";
			case All::TextureBufferFormatArb:return "TextureBufferFormatArb";
			case All::AnySamplesPassedExt:return "AnySamplesPassedExt";
			case All::SampleShadingArb:return "SampleShadingArb";
			case All::MinSampleShadingValue:return "MinSampleShadingValue";
			case All::RgbaSignedComponentsExt:return "RgbaSignedComponentsExt";
			case All::TextureSharedSize:return "TextureSharedSize";
			case All::SluminanceAlpha:return "SluminanceAlpha";
			case All::Sluminance8Alpha8:return "Sluminance8Alpha8";
			case All::Sluminance:return "Sluminance";
			case All::Sluminance8:return "Sluminance8";
			case All::CompressedSrgbExt:return "CompressedSrgbExt";
			case All::CompressedSrgbAlphaExt:return "CompressedSrgbAlphaExt";
			case All::CompressedSluminance:return "CompressedSluminance";
			case All::CompressedSluminanceAlpha:return "CompressedSluminanceAlpha";
			case All::TessControlProgramParameterBufferNv:return "TessControlProgramParameterBufferNv";
			case All::TessEvaluationProgramParameterBufferNv:return "TessEvaluationProgramParameterBufferNv";
			case All::TransformFeedbackVaryingMaxLengthExt:return "TransformFeedbackVaryingMaxLengthExt";
			case All::BackPrimaryColorNv:return "BackPrimaryColorNv";
			case All::BackSecondaryColorNv:return "BackSecondaryColorNv";
			case All::TextureCoordNv:return "TextureCoordNv";
			case All::ClipDistanceNv:return "ClipDistanceNv";
			case All::VertexIdNv:return "VertexIdNv";
			case All::PrimitiveIdNv:return "PrimitiveIdNv";
			case All::GenericAttribNv:return "GenericAttribNv";
			case All::TransformFeedbackAttribsNv:return "TransformFeedbackAttribsNv";
			case All::TransformFeedbackBufferModeExt:return "TransformFeedbackBufferModeExt";
			case All::MaxTransformFeedbackSeparateComponents:return "MaxTransformFeedbackSeparateComponents";
			case All::ActiveVaryingsNv:return "ActiveVaryingsNv";
			case All::ActiveVaryingMaxLengthNv:return "ActiveVaryingMaxLengthNv";
			case All::TransformFeedbackVaryingsExt:return "TransformFeedbackVaryingsExt";
			case All::TransformFeedbackBufferStartExt:return "TransformFeedbackBufferStartExt";
			case All::TransformFeedbackBufferSizeExt:return "TransformFeedbackBufferSizeExt";
			case All::TransformFeedbackRecordNv:return "TransformFeedbackRecordNv";
			case All::PrimitivesGeneratedExt:return "PrimitivesGeneratedExt";
			case All::TransformFeedbackPrimitivesWrittenExt:return "TransformFeedbackPrimitivesWrittenExt";
			case All::RasterizerDiscardExt:return "RasterizerDiscardExt";
			case All::MaxTransformFeedbackInterleavedComponents:return "MaxTransformFeedbackInterleavedComponents";
			case All::MaxTransformFeedbackSeparateAttribs:return "MaxTransformFeedbackSeparateAttribs";
			case All::InterleavedAttribsExt:return "InterleavedAttribsExt";
			case All::SeparateAttribsExt:return "SeparateAttribsExt";
			case All::TransformFeedbackBufferExt:return "TransformFeedbackBufferExt";
			case All::TransformFeedbackBufferBindingExt:return "TransformFeedbackBufferBindingExt";
			case All::TextureLodBiasQcom:return "TextureLodBiasQcom";
			case All::PointSpriteCoordOrigin:return "PointSpriteCoordOrigin";
			case All::LowerLeftExt:return "LowerLeftExt";
			case All::UpperLeftExt:return "UpperLeftExt";
			case All::DrawFramebufferBindingAngle:return "DrawFramebufferBindingAngle";
			case All::RenderbufferBindingAngle:return "RenderbufferBindingAngle";
			case All::ReadFramebufferAngle:return "ReadFramebufferAngle";
			case All::DrawFramebufferAngle:return "DrawFramebufferAngle";
			case All::ReadFramebufferBindingAngle:return "ReadFramebufferBindingAngle";
			case All::FramebufferCompleteExt:return "FramebufferCompleteExt";
			case All::FramebufferIncompleteAttachmentExt:return "FramebufferIncompleteAttachmentExt";
			case All::FramebufferIncompleteMissingAttachmentExt:return "FramebufferIncompleteMissingAttachmentExt";
			case All::FramebufferIncompleteDimensions:return "FramebufferIncompleteDimensions";
			case All::FramebufferIncompleteFormatsExt:return "FramebufferIncompleteFormatsExt";
			case All::FramebufferIncompleteDrawBufferExt:return "FramebufferIncompleteDrawBufferExt";
			case All::FramebufferIncompleteReadBufferExt:return "FramebufferIncompleteReadBufferExt";
			case All::FramebufferUnsupportedExt:return "FramebufferUnsupportedExt";
			case All::FramebufferExt:return "FramebufferExt";
			case All::RenderbufferExt:return "RenderbufferExt";
			case All::FramebufferIncompleteMultisampleAngle:return "FramebufferIncompleteMultisampleAngle";
			case All::MaxSamples:return "MaxSamples";
			case All::HalfFloatOes:return "HalfFloatOes";
			case All::TextureExternalOes:return "TextureExternalOes";
			case All::SamplerExternalOes:return "SamplerExternalOes";
			case All::TextureBindingExternalOes:return "TextureBindingExternalOes";
			case All::RequiredTextureImageUnitsOes:return "RequiredTextureImageUnitsOes";
			case All::AnySamplesPassedConservativeExt:return "AnySamplesPassedConservativeExt";
			case All::RedIntegerExt:return "RedIntegerExt";
			case All::GreenIntegerExt:return "GreenIntegerExt";
			case All::BlueIntegerExt:return "BlueIntegerExt";
			case All::AlphaInteger:return "AlphaInteger";
			case All::RgbIntegerExt:return "RgbIntegerExt";
			case All::RgbaIntegerExt:return "RgbaIntegerExt";
			case All::BgrIntegerExt:return "BgrIntegerExt";
			case All::BgraIntegerExt:return "BgraIntegerExt";
			case All::LuminanceIntegerExt:return "LuminanceIntegerExt";
			case All::LuminanceAlphaIntegerExt:return "LuminanceAlphaIntegerExt";
			case All::RgbaIntegerModeExt:return "RgbaIntegerModeExt";
			case All::MaxProgramParameterBufferBindingsNv:return "MaxProgramParameterBufferBindingsNv";
			case All::MaxProgramParameterBufferSizeNv:return "MaxProgramParameterBufferSizeNv";
			case All::VertexProgramParameterBufferNv:return "VertexProgramParameterBufferNv";
			case All::GeometryProgramParameterBufferNv:return "GeometryProgramParameterBufferNv";
			case All::FragmentProgramParameterBufferNv:return "FragmentProgramParameterBufferNv";
			case All::MaxProgramGenericAttribsNv:return "MaxProgramGenericAttribsNv";
			case All::MaxProgramGenericResultsNv:return "MaxProgramGenericResultsNv";
			case All::FramebufferIncompleteLayerTargetsArb:return "FramebufferIncompleteLayerTargetsArb";
			case All::FramebufferIncompleteLayerCountArb:return "FramebufferIncompleteLayerCountArb";
			case All::LayerNv:return "LayerNv";
			case All::ShaderIncludeArb:return "ShaderIncludeArb";
			case All::DepthBufferFloatModeNv:return "DepthBufferFloatModeNv";
			case All::FramebufferSrgbExt:return "FramebufferSrgbExt";
			case All::FramebufferSrgbCapableExt:return "FramebufferSrgbCapableExt";
			case All::Sampler1DArrayExt:return "Sampler1DArrayExt";
			case All::Sampler2DArrayExt:return "Sampler2DArrayExt";
			case All::SamplerBufferExt:return "SamplerBufferExt";
			case All::Sampler1DArrayShadowExt:return "Sampler1DArrayShadowExt";
			case All::Sampler2DArrayShadowExt:return "Sampler2DArrayShadowExt";
			case All::SamplerCubeShadowExt:return "SamplerCubeShadowExt";
			case All::UnsignedIntVec2Ext:return "UnsignedIntVec2Ext";
			case All::UnsignedIntVec3Ext:return "UnsignedIntVec3Ext";
			case All::UnsignedIntVec4Ext:return "UnsignedIntVec4Ext";
			case All::IntSampler1DExt:return "IntSampler1DExt";
			case All::IntSampler2DExt:return "IntSampler2DExt";
			case All::IntSampler3DExt:return "IntSampler3DExt";
			case All::IntSamplerCubeExt:return "IntSamplerCubeExt";
			case All::IntSampler2DRectExt:return "IntSampler2DRectExt";
			case All::IntSampler1DArrayExt:return "IntSampler1DArrayExt";
			case All::IntSampler2DArrayExt:return "IntSampler2DArrayExt";
			case All::IntSamplerBufferExt:return "IntSamplerBufferExt";
			case All::UnsignedIntSampler1DExt:return "UnsignedIntSampler1DExt";
			case All::UnsignedIntSampler2DExt:return "UnsignedIntSampler2DExt";
			case All::UnsignedIntSampler3DExt:return "UnsignedIntSampler3DExt";
			case All::UnsignedIntSamplerCubeExt:return "UnsignedIntSamplerCubeExt";
			case All::UnsignedIntSampler2DRectExt:return "UnsignedIntSampler2DRectExt";
			case All::UnsignedIntSampler1DArrayExt:return "UnsignedIntSampler1DArrayExt";
			case All::UnsignedIntSampler2DArrayExt:return "UnsignedIntSampler2DArrayExt";
			case All::UnsignedIntSamplerBufferExt:return "UnsignedIntSamplerBufferExt";
			case All::GeometryShaderArb:return "GeometryShaderArb";
			case All::GeometryVerticesOutArb:return "GeometryVerticesOutArb";
			case All::GeometryInputTypeArb:return "GeometryInputTypeArb";
			case All::GeometryOutputTypeArb:return "GeometryOutputTypeArb";
			case All::MaxGeometryVaryingComponentsArb:return "MaxGeometryVaryingComponentsArb";
			case All::MaxVertexVaryingComponentsArb:return "MaxVertexVaryingComponentsArb";
			case All::MaxGeometryUniformComponentsArb:return "MaxGeometryUniformComponentsArb";
			case All::MaxGeometryOutputVertices:return "MaxGeometryOutputVertices";
			case All::MaxGeometryTotalOutputComponents:return "MaxGeometryTotalOutputComponents";
			case All::MaxVertexBindableUniformsExt:return "MaxVertexBindableUniformsExt";
			case All::MaxFragmentBindableUniformsExt:return "MaxFragmentBindableUniformsExt";
			case All::MaxGeometryBindableUniformsExt:return "MaxGeometryBindableUniformsExt";
			case All::MaxSubroutines:return "MaxSubroutines";
			case All::MaxSubroutineUniformLocations:return "MaxSubroutineUniformLocations";
			case All::NamedStringLengthArb:return "NamedStringLengthArb";
			case All::NamedStringTypeArb:return "NamedStringTypeArb";
			case All::MaxBindableUniformSizeExt:return "MaxBindableUniformSizeExt";
			case All::UniformBufferExt:return "UniformBufferExt";
			case All::UniformBufferBindingExt:return "UniformBufferBindingExt";
			case All::UnsignedInt1010102Oes:return "UnsignedInt1010102Oes";
			case All::Int1010102Oes:return "Int1010102Oes";
			case All::MaxMultisampleCoverageModesNv:return "MaxMultisampleCoverageModesNv";
			case All::MultisampleCoverageModesNv:return "MultisampleCoverageModesNv";
			case All::QueryWaitNv:return "QueryWaitNv";
			case All::QueryNoWaitNv:return "QueryNoWaitNv";
			case All::QueryByRegionWaitNv:return "QueryByRegionWaitNv";
			case All::QueryByRegionNoWaitNv:return "QueryByRegionNoWaitNv";
			case All::PolygonOffsetClamp:return "PolygonOffsetClamp";
			case All::MaxCombinedTessControlUniformComponents:return "MaxCombinedTessControlUniformComponents";
			case All::MaxCombinedTessEvaluationUniformComponents:return "MaxCombinedTessEvaluationUniformComponents";
			case All::ColorSamplesNv:return "ColorSamplesNv";
			case All::TransformFeedbackNv:return "TransformFeedbackNv";
			case All::TransformFeedbackBufferPaused:return "TransformFeedbackBufferPaused";
			case All::TransformFeedbackBufferActive:return "TransformFeedbackBufferActive";
			case All::TransformFeedbackBinding:return "TransformFeedbackBinding";
			case All::FrameNv:return "FrameNv";
			case All::FieldsNv:return "FieldsNv";
			case All::CurrentTimeNv:return "CurrentTimeNv";
			case All::NumFillStreamsNv:return "NumFillStreamsNv";
			case All::PresentTimeNv:return "PresentTimeNv";
			case All::PresentDurationNv:return "PresentDurationNv";
			case All::DepthComponent16NonlinearNv:return "DepthComponent16NonlinearNv";
			case All::ProgramMatrixExt:return "ProgramMatrixExt";
			case All::TransposeProgramMatrixExt:return "TransposeProgramMatrixExt";
			case All::ProgramMatrixStackDepthExt:return "ProgramMatrixStackDepthExt";
			case All::TextureSwizzleRExt:return "TextureSwizzleRExt";
			case All::TextureSwizzleGExt:return "TextureSwizzleGExt";
			case All::TextureSwizzleBExt:return "TextureSwizzleBExt";
			case All::TextureSwizzleAExt:return "TextureSwizzleAExt";
			case All::TextureSwizzleRgbaExt:return "TextureSwizzleRgbaExt";
			case All::QuadsFollowProvokingVertexConvention:return "QuadsFollowProvokingVertexConvention";
			case All::FirstVertexConventionExt:return "FirstVertexConventionExt";
			case All::LastVertexConventionExt:return "LastVertexConventionExt";
			case All::ProvokingVertexExt:return "ProvokingVertexExt";
			case All::SamplePositionNv:return "SamplePositionNv";
			case All::SampleMaskNv:return "SampleMaskNv";
			case All::SampleMaskValue:return "SampleMaskValue";
			case All::TextureBindingRenderbufferNv:return "TextureBindingRenderbufferNv";
			case All::TextureRenderbufferDataStoreBindingNv:return "TextureRenderbufferDataStoreBindingNv";
			case All::TextureRenderbufferNv:return "TextureRenderbufferNv";
			case All::SamplerRenderbufferNv:return "SamplerRenderbufferNv";
			case All::IntSamplerRenderbufferNv:return "IntSamplerRenderbufferNv";
			case All::UnsignedIntSamplerRenderbufferNv:return "UnsignedIntSamplerRenderbufferNv";
			case All::MaxSampleMaskWordsNv:return "MaxSampleMaskWordsNv";
			case All::MaxGeometryProgramInvocationsNv:return "MaxGeometryProgramInvocationsNv";
			case All::MinFragmentInterpolationOffset:return "MinFragmentInterpolationOffset";
			case All::MaxFragmentInterpolationOffset:return "MaxFragmentInterpolationOffset";
			case All::FragmentInterpolationOffsetBits:return "FragmentInterpolationOffsetBits";
			case All::MinProgramTextureGatherOffset:return "MinProgramTextureGatherOffset";
			case All::MaxProgramTextureGatherOffset:return "MaxProgramTextureGatherOffset";
			case All::MaxMeshUniformBlocksExt:return "MaxMeshUniformBlocksExt";
			case All::MaxMeshTextureImageUnitsExt:return "MaxMeshTextureImageUnitsExt";
			case All::MaxMeshImageUniformsExt:return "MaxMeshImageUniformsExt";
			case All::MaxMeshUniformComponentsExt:return "MaxMeshUniformComponentsExt";
			case All::MaxMeshAtomicCounterBuffersExt:return "MaxMeshAtomicCounterBuffersExt";
			case All::MaxMeshAtomicCountersExt:return "MaxMeshAtomicCountersExt";
			case All::MaxMeshShaderStorageBlocksExt:return "MaxMeshShaderStorageBlocksExt";
			case All::MaxCombinedMeshUniformComponentsExt:return "MaxCombinedMeshUniformComponentsExt";
			case All::MaxTaskUniformBlocksExt:return "MaxTaskUniformBlocksExt";
			case All::MaxTaskTextureImageUnitsExt:return "MaxTaskTextureImageUnitsExt";
			case All::MaxTaskImageUniformsExt:return "MaxTaskImageUniformsExt";
			case All::MaxTaskUniformComponentsExt:return "MaxTaskUniformComponentsExt";
			case All::MaxTaskAtomicCounterBuffersExt:return "MaxTaskAtomicCounterBuffersExt";
			case All::MaxTaskAtomicCountersExt:return "MaxTaskAtomicCountersExt";
			case All::MaxTaskShaderStorageBlocksExt:return "MaxTaskShaderStorageBlocksExt";
			case All::MaxCombinedTaskUniformComponentsExt:return "MaxCombinedTaskUniformComponentsExt";
			case All::MaxTransformFeedbackBuffers:return "MaxTransformFeedbackBuffers";
			case All::MaxVertexStreams:return "MaxVertexStreams";
			case All::PatchVerticesExt:return "PatchVerticesExt";
			case All::PatchDefaultInnerLevelExt:return "PatchDefaultInnerLevelExt";
			case All::PatchDefaultOuterLevelExt:return "PatchDefaultOuterLevelExt";
			case All::TessControlOutputVertices:return "TessControlOutputVertices";
			case All::TessGenMode:return "TessGenMode";
			case All::TessGenSpacing:return "TessGenSpacing";
			case All::TessGenVertexOrder:return "TessGenVertexOrder";
			case All::TessGenPointMode:return "TessGenPointMode";
			case All::Isolines:return "Isolines";
			case All::FractionalOdd:return "FractionalOdd";
			case All::FractionalEven:return "FractionalEven";
			case All::MaxPatchVertices:return "MaxPatchVertices";
			case All::MaxTessGenLevel:return "MaxTessGenLevel";
			case All::MaxTessControlUniformComponents:return "MaxTessControlUniformComponents";
			case All::MaxTessEvaluationUniformComponents:return "MaxTessEvaluationUniformComponents";
			case All::MaxTessControlTextureImageUnits:return "MaxTessControlTextureImageUnits";
			case All::MaxTessEvaluationTextureImageUnits:return "MaxTessEvaluationTextureImageUnits";
			case All::MaxTessControlOutputComponents:return "MaxTessControlOutputComponents";
			case All::MaxTessPatchComponents:return "MaxTessPatchComponents";
			case All::MaxTessControlTotalOutputComponents:return "MaxTessControlTotalOutputComponents";
			case All::MaxTessEvaluationOutputComponents:return "MaxTessEvaluationOutputComponents";
			case All::TessEvaluationShaderExt:return "TessEvaluationShaderExt";
			case All::TessControlShaderExt:return "TessControlShaderExt";
			case All::MaxTessControlUniformBlocksExt:return "MaxTessControlUniformBlocksExt";
			case All::MaxTessEvaluationUniformBlocksExt:return "MaxTessEvaluationUniformBlocksExt";
			case All::TrpImg:return "TrpImg";
			case All::TrpErrorContextResetImg:return "TrpErrorContextResetImg";
			case All::TrpUnsupportedContextImg:return "TrpUnsupportedContextImg";
			case All::PvricSignatureMismatchImg:return "PvricSignatureMismatchImg";
			case All::CoverageComponentNv:return "CoverageComponentNv";
			case All::CoverageComponent4Nv:return "CoverageComponent4Nv";
			case All::CoverageAttachmentNv:return "CoverageAttachmentNv";
			case All::CoverageBuffersNv:return "CoverageBuffersNv";
			case All::CoverageSamplesNv:return "CoverageSamplesNv";
			case All::CoverageAllFragmentsNv:return "CoverageAllFragmentsNv";
			case All::CoverageEdgeFragmentsNv:return "CoverageEdgeFragmentsNv";
			case All::CoverageAutomaticNv:return "CoverageAutomaticNv";
			case All::InclusiveExt:return "InclusiveExt";
			case All::ExclusiveExt:return "ExclusiveExt";
			case All::WindowRectangleExt:return "WindowRectangleExt";
			case All::WindowRectangleModeExt:return "WindowRectangleModeExt";
			case All::MaxWindowRectanglesExt:return "MaxWindowRectanglesExt";
			case All::NumWindowRectanglesExt:return "NumWindowRectanglesExt";
			case All::BufferGpuAddressNv:return "BufferGpuAddressNv";
			case All::VertexAttribArrayUnifiedNv:return "VertexAttribArrayUnifiedNv";
			case All::ElementArrayUnifiedNv:return "ElementArrayUnifiedNv";
			case All::VertexAttribArrayAddressNv:return "VertexAttribArrayAddressNv";
			case All::VertexArrayAddressNv:return "VertexArrayAddressNv";
			case All::NormalArrayAddressNv:return "NormalArrayAddressNv";
			case All::ColorArrayAddressNv:return "ColorArrayAddressNv";
			case All::IndexArrayAddressNv:return "IndexArrayAddressNv";
			case All::TextureCoordArrayAddressNv:return "TextureCoordArrayAddressNv";
			case All::EdgeFlagArrayAddressNv:return "EdgeFlagArrayAddressNv";
			case All::SecondaryColorArrayAddressNv:return "SecondaryColorArrayAddressNv";
			case All::FogCoordArrayAddressNv:return "FogCoordArrayAddressNv";
			case All::ElementArrayAddressNv:return "ElementArrayAddressNv";
			case All::VertexAttribArrayLengthNv:return "VertexAttribArrayLengthNv";
			case All::VertexArrayLengthNv:return "VertexArrayLengthNv";
			case All::NormalArrayLengthNv:return "NormalArrayLengthNv";
			case All::ColorArrayLengthNv:return "ColorArrayLengthNv";
			case All::IndexArrayLengthNv:return "IndexArrayLengthNv";
			case All::TextureCoordArrayLengthNv:return "TextureCoordArrayLengthNv";
			case All::EdgeFlagArrayLengthNv:return "EdgeFlagArrayLengthNv";
			case All::SecondaryColorArrayLengthNv:return "SecondaryColorArrayLengthNv";
			case All::FogCoordArrayLengthNv:return "FogCoordArrayLengthNv";
			case All::ElementArrayLengthNv:return "ElementArrayLengthNv";
			case All::GpuAddressNv:return "GpuAddressNv";
			case All::MaxShaderBufferAddressNv:return "MaxShaderBufferAddressNv";
			case All::CopyReadBufferNv:return "CopyReadBufferNv";
			case All::CopyWriteBufferNv:return "CopyWriteBufferNv";
			case All::MaxImageUnits:return "MaxImageUnits";
			case All::MaxCombinedImageUnitsAndFragmentOutputs:return "MaxCombinedImageUnitsAndFragmentOutputs";
			case All::ImageBindingName:return "ImageBindingName";
			case All::ImageBindingLevel:return "ImageBindingLevel";
			case All::ImageBindingLayered:return "ImageBindingLayered";
			case All::ImageBindingLayer:return "ImageBindingLayer";
			case All::ImageBindingAccess:return "ImageBindingAccess";
			case All::DrawIndirectUnifiedNv:return "DrawIndirectUnifiedNv";
			case All::DrawIndirectAddressNv:return "DrawIndirectAddressNv";
			case All::DrawIndirectLengthNv:return "DrawIndirectLengthNv";
			case All::DrawIndirectBufferBinding:return "DrawIndirectBufferBinding";
			case All::MaxProgramSubroutineParametersNv:return "MaxProgramSubroutineParametersNv";
			case All::MaxProgramSubroutineNumNv:return "MaxProgramSubroutineNumNv";
			case All::DoubleMat2Ext:return "DoubleMat2Ext";
			case All::DoubleMat3Ext:return "DoubleMat3Ext";
			case All::DoubleMat4Ext:return "DoubleMat4Ext";
			case All::DoubleMat2x3Ext:return "DoubleMat2x3Ext";
			case All::DoubleMat2x4Ext:return "DoubleMat2x4Ext";
			case All::DoubleMat3x2Ext:return "DoubleMat3x2Ext";
			case All::DoubleMat3x4Ext:return "DoubleMat3x4Ext";
			case All::DoubleMat4x2Ext:return "DoubleMat4x2Ext";
			case All::DoubleMat4x3Ext:return "DoubleMat4x3Ext";
			case All::VertexBindingBuffer:return "VertexBindingBuffer";
			case All::MaliProgramBinaryArm:return "MaliProgramBinaryArm";
			case All::MaxShaderPixelLocalStorageFastSizeExt:return "MaxShaderPixelLocalStorageFastSizeExt";
			case All::ShaderPixelLocalStorageExt:return "ShaderPixelLocalStorageExt";
			case All::MaxShaderPixelLocalStorageSizeExt:return "MaxShaderPixelLocalStorageSizeExt";
			case All::TextureAstcDecodePrecisionExt:return "TextureAstcDecodePrecisionExt";
			case All::RedSnorm:return "RedSnorm";
			case All::RgSnorm:return "RgSnorm";
			case All::RgbSnorm:return "RgbSnorm";
			case All::RgbaSnorm:return "RgbaSnorm";
			case All::SignedNormalized:return "SignedNormalized";
			case All::MaxProgramTextureGatherComponentsArb:return "MaxProgramTextureGatherComponentsArb";
			case All::PerfmonGlobalModeQcom:return "PerfmonGlobalModeQcom";
			case All::MaxShaderSubsampledImageUnitsQcom:return "MaxShaderSubsampledImageUnitsQcom";
			case All::CpuOptimizedQcom:return "CpuOptimizedQcom";
			case All::GpuOptimizedQcom:return "GpuOptimizedQcom";
			case All::RenderDirectToFramebufferQcom:return "RenderDirectToFramebufferQcom";
			case All::GpuDisjointExt:return "GpuDisjointExt";
			case All::TextureFormatSrgbOverrideExt:return "TextureFormatSrgbOverrideExt";
			case All::Int8Nv:return "Int8Nv";
			case All::Int8Vec2Nv:return "Int8Vec2Nv";
			case All::Int8Vec3Nv:return "Int8Vec3Nv";
			case All::Int8Vec4Nv:return "Int8Vec4Nv";
			case All::Int16Nv:return "Int16Nv";
			case All::Int16Vec2Nv:return "Int16Vec2Nv";
			case All::Int16Vec3Nv:return "Int16Vec3Nv";
			case All::Int16Vec4Nv:return "Int16Vec4Nv";
			case All::Int64Vec2Nv:return "Int64Vec2Nv";
			case All::Int64Vec3Nv:return "Int64Vec3Nv";
			case All::Int64Vec4Nv:return "Int64Vec4Nv";
			case All::UnsignedInt8Nv:return "UnsignedInt8Nv";
			case All::UnsignedInt8Vec2Nv:return "UnsignedInt8Vec2Nv";
			case All::UnsignedInt8Vec3Nv:return "UnsignedInt8Vec3Nv";
			case All::UnsignedInt8Vec4Nv:return "UnsignedInt8Vec4Nv";
			case All::UnsignedInt16Nv:return "UnsignedInt16Nv";
			case All::UnsignedInt16Vec2Nv:return "UnsignedInt16Vec2Nv";
			case All::UnsignedInt16Vec3Nv:return "UnsignedInt16Vec3Nv";
			case All::UnsignedInt16Vec4Nv:return "UnsignedInt16Vec4Nv";
			case All::UnsignedInt64Vec2Nv:return "UnsignedInt64Vec2Nv";
			case All::UnsignedInt64Vec3Nv:return "UnsignedInt64Vec3Nv";
			case All::UnsignedInt64Vec4Nv:return "UnsignedInt64Vec4Nv";
			case All::Float16Nv:return "Float16Nv";
			case All::Float16Vec2Nv:return "Float16Vec2Nv";
			case All::Float16Vec3Nv:return "Float16Vec3Nv";
			case All::Float16Vec4Nv:return "Float16Vec4Nv";
			case All::DoubleVec2Ext:return "DoubleVec2Ext";
			case All::DoubleVec3Ext:return "DoubleVec3Ext";
			case All::DoubleVec4Ext:return "DoubleVec4Ext";
			case All::SamplerBufferAmd:return "SamplerBufferAmd";
			case All::IntSamplerBufferAmd:return "IntSamplerBufferAmd";
			case All::UnsignedIntSamplerBufferAmd:return "UnsignedIntSamplerBufferAmd";
			case All::TessellationModeAmd:return "TessellationModeAmd";
			case All::TessellationFactorAmd:return "TessellationFactorAmd";
			case All::DiscreteAmd:return "DiscreteAmd";
			case All::ContinuousAmd:return "ContinuousAmd";
			case All::TextureBindingCubeMapArray:return "TextureBindingCubeMapArray";
			case All::SamplerCubeMapArrayArb:return "SamplerCubeMapArrayArb";
			case All::SamplerCubeMapArrayShadowArb:return "SamplerCubeMapArrayShadowArb";
			case All::IntSamplerCubeMapArrayArb:return "IntSamplerCubeMapArrayArb";
			case All::UnsignedIntSamplerCubeMapArrayArb:return "UnsignedIntSamplerCubeMapArrayArb";
			case All::AlphaSnorm:return "AlphaSnorm";
			case All::LuminanceSnorm:return "LuminanceSnorm";
			case All::LuminanceAlphaSnorm:return "LuminanceAlphaSnorm";
			case All::IntensitySnorm:return "IntensitySnorm";
			case All::Alpha8Snorm:return "Alpha8Snorm";
			case All::Luminance8Snorm:return "Luminance8Snorm";
			case All::Luminance8Alpha8Snorm:return "Luminance8Alpha8Snorm";
			case All::Intensity8Snorm:return "Intensity8Snorm";
			case All::Alpha16Snorm:return "Alpha16Snorm";
			case All::Luminance16Snorm:return "Luminance16Snorm";
			case All::Luminance16Alpha16Snorm:return "Luminance16Alpha16Snorm";
			case All::Intensity16Snorm:return "Intensity16Snorm";
			case All::FactorMinAmd:return "FactorMinAmd";
			case All::FactorMaxAmd:return "FactorMaxAmd";
			case All::DepthClampNearAmd:return "DepthClampNearAmd";
			case All::DepthClampFarAmd:return "DepthClampFarAmd";
			case All::VideoBufferNv:return "VideoBufferNv";
			case All::VideoBufferBindingNv:return "VideoBufferBindingNv";
			case All::FieldUpperNv:return "FieldUpperNv";
			case All::FieldLowerNv:return "FieldLowerNv";
			case All::NumVideoCaptureStreamsNv:return "NumVideoCaptureStreamsNv";
			case All::NextVideoCaptureBufferStatusNv:return "NextVideoCaptureBufferStatusNv";
			case All::VideoCaptureTo422SupportedNv:return "VideoCaptureTo422SupportedNv";
			case All::LastVideoCaptureStatusNv:return "LastVideoCaptureStatusNv";
			case All::VideoBufferPitchNv:return "VideoBufferPitchNv";
			case All::VideoColorConversionMatrixNv:return "VideoColorConversionMatrixNv";
			case All::VideoColorConversionMaxNv:return "VideoColorConversionMaxNv";
			case All::VideoColorConversionMinNv:return "VideoColorConversionMinNv";
			case All::VideoColorConversionOffsetNv:return "VideoColorConversionOffsetNv";
			case All::VideoBufferInternalFormatNv:return "VideoBufferInternalFormatNv";
			case All::PartialSuccessNv:return "PartialSuccessNv";
			case All::SuccessNv:return "SuccessNv";
			case All::FailureNv:return "FailureNv";
			case All::Ycbycr8422Nv:return "Ycbycr8422Nv";
			case All::Ycbaycr8a4224Nv:return "Ycbaycr8a4224Nv";
			case All::Z6y10z6cb10z6y10z6cr10422Nv:return "Z6y10z6cb10z6y10z6cr10422Nv";
			case All::Z6y10z6cb10z6a10z6y10z6cr10z6a104224Nv:return "Z6y10z6cb10z6a10z6y10z6cr10z6a104224Nv";
			case All::Z4y12z4cb12z4y12z4cr12422Nv:return "Z4y12z4cb12z4y12z4cr12422Nv";
			case All::Z4y12z4cb12z4a12z4y12z4cr12z4a124224Nv:return "Z4y12z4cb12z4a12z4y12z4cr12z4a124224Nv";
			case All::Z4y12z4cb12z4cr12444Nv:return "Z4y12z4cb12z4cr12444Nv";
			case All::VideoCaptureFrameWidthNv:return "VideoCaptureFrameWidthNv";
			case All::VideoCaptureFrameHeightNv:return "VideoCaptureFrameHeightNv";
			case All::VideoCaptureFieldUpperHeightNv:return "VideoCaptureFieldUpperHeightNv";
			case All::VideoCaptureFieldLowerHeightNv:return "VideoCaptureFieldLowerHeightNv";
			case All::VideoCaptureSurfaceOriginNv:return "VideoCaptureSurfaceOriginNv";
			case All::TextureCoverageSamplesNv:return "TextureCoverageSamplesNv";
			case All::TextureColorSamplesNv:return "TextureColorSamplesNv";
			case All::GpuMemoryInfoDedicatedVidmemNvx:return "GpuMemoryInfoDedicatedVidmemNvx";
			case All::GpuMemoryInfoTotalAvailableMemoryNvx:return "GpuMemoryInfoTotalAvailableMemoryNvx";
			case All::GpuMemoryInfoCurrentAvailableVidmemNvx:return "GpuMemoryInfoCurrentAvailableVidmemNvx";
			case All::GpuMemoryInfoEvictionCountNvx:return "GpuMemoryInfoEvictionCountNvx";
			case All::GpuMemoryInfoEvictedMemoryNvx:return "GpuMemoryInfoEvictedMemoryNvx";
			case All::Image1DExt:return "Image1DExt";
			case All::Image2DExt:return "Image2DExt";
			case All::Image3DExt:return "Image3DExt";
			case All::Image2DRectExt:return "Image2DRectExt";
			case All::ImageCubeExt:return "ImageCubeExt";
			case All::ImageBufferExt:return "ImageBufferExt";
			case All::Image1DArrayExt:return "Image1DArrayExt";
			case All::Image2DArrayExt:return "Image2DArrayExt";
			case All::ImageCubeMapArrayExt:return "ImageCubeMapArrayExt";
			case All::Image2DMultisampleExt:return "Image2DMultisampleExt";
			case All::Image2DMultisampleArrayExt:return "Image2DMultisampleArrayExt";
			case All::IntImage1DExt:return "IntImage1DExt";
			case All::IntImage2DExt:return "IntImage2DExt";
			case All::IntImage3DExt:return "IntImage3DExt";
			case All::IntImage2DRectExt:return "IntImage2DRectExt";
			case All::IntImageCubeExt:return "IntImageCubeExt";
			case All::IntImageBufferExt:return "IntImageBufferExt";
			case All::IntImage1DArrayExt:return "IntImage1DArrayExt";
			case All::IntImage2DArrayExt:return "IntImage2DArrayExt";
			case All::IntImageCubeMapArrayExt:return "IntImageCubeMapArrayExt";
			case All::IntImage2DMultisampleExt:return "IntImage2DMultisampleExt";
			case All::IntImage2DMultisampleArrayExt:return "IntImage2DMultisampleArrayExt";
			case All::UnsignedIntImage1DExt:return "UnsignedIntImage1DExt";
			case All::UnsignedIntImage2DExt:return "UnsignedIntImage2DExt";
			case All::UnsignedIntImage3DExt:return "UnsignedIntImage3DExt";
			case All::UnsignedIntImage2DRectExt:return "UnsignedIntImage2DRectExt";
			case All::UnsignedIntImageCubeExt:return "UnsignedIntImageCubeExt";
			case All::UnsignedIntImageBufferExt:return "UnsignedIntImageBufferExt";
			case All::UnsignedIntImage1DArrayExt:return "UnsignedIntImage1DArrayExt";
			case All::UnsignedIntImage2DArrayExt:return "UnsignedIntImage2DArrayExt";
			case All::UnsignedIntImageCubeMapArrayExt:return "UnsignedIntImageCubeMapArrayExt";
			case All::UnsignedIntImage2DMultisampleExt:return "UnsignedIntImage2DMultisampleExt";
			case All::UnsignedIntImage2DMultisampleArrayExt:return "UnsignedIntImage2DMultisampleArrayExt";
			case All::MaxImageSamples:return "MaxImageSamples";
			case All::ImageBindingFormat:return "ImageBindingFormat";
			case All::SquareNv:return "SquareNv";
			case All::RoundNv:return "RoundNv";
			case All::TriangularNv:return "TriangularNv";
			case All::BevelNv:return "BevelNv";
			case All::MiterRevertNv:return "MiterRevertNv";
			case All::MiterTruncateNv:return "MiterTruncateNv";
			case All::PathErrorPositionNv:return "PathErrorPositionNv";
			case All::PathFogGenModeNv:return "PathFogGenModeNv";
			case All::PathGenModeNv:return "PathGenModeNv";
			case All::PathGenCoeffNv:return "PathGenCoeffNv";
			case All::PathGenColorFormatNv:return "PathGenColorFormatNv";
			case All::PathGenComponentsNv:return "PathGenComponentsNv";
			case All::MoveToResetsNv:return "MoveToResetsNv";
			case All::MoveToContinuesNv:return "MoveToContinuesNv";
			case All::PathStencilFuncNv:return "PathStencilFuncNv";
			case All::PathStencilRefNv:return "PathStencilRefNv";
			case All::PathStencilValueMaskNv:return "PathStencilValueMaskNv";
			case All::ScaledResolveFastestExt:return "ScaledResolveFastestExt";
			case All::ScaledResolveNicestExt:return "ScaledResolveNicestExt";
			case All::PathStencilDepthOffsetFactorNv:return "PathStencilDepthOffsetFactorNv";
			case All::PathStencilDepthOffsetUnitsNv:return "PathStencilDepthOffsetUnitsNv";
			case All::PathCoverDepthFuncNv:return "PathCoverDepthFuncNv";
			case All::ImageFormatCompatibilityBySize:return "ImageFormatCompatibilityBySize";
			case All::ImageFormatCompatibilityByClass:return "ImageFormatCompatibilityByClass";
			case All::MaxVertexImageUniforms:return "MaxVertexImageUniforms";
			case All::MaxTessControlImageUniforms:return "MaxTessControlImageUniforms";
			case All::MaxTessEvaluationImageUniforms:return "MaxTessEvaluationImageUniforms";
			case All::MaxGeometryImageUniforms:return "MaxGeometryImageUniforms";
			case All::MaxFragmentImageUniforms:return "MaxFragmentImageUniforms";
			case All::MaxCombinedImageUniforms:return "MaxCombinedImageUniforms";
			case All::MaxDeep3DTextureWidthHeightNv:return "MaxDeep3DTextureWidthHeightNv";
			case All::MaxDeep3DTextureDepthNv:return "MaxDeep3DTextureDepthNv";
			case All::MaxGeometryShaderStorageBlocksExt:return "MaxGeometryShaderStorageBlocksExt";
			case All::MaxTessControlShaderStorageBlocksExt:return "MaxTessControlShaderStorageBlocksExt";
			case All::MaxTessEvaluationShaderStorageBlocksExt:return "MaxTessEvaluationShaderStorageBlocksExt";
			case All::MaxShaderStorageBlockSize:return "MaxShaderStorageBlockSize";
			case All::SyncX11FenceExt:return "SyncX11FenceExt";
			case All::MaxComputeFixedGroupInvocationsArb:return "MaxComputeFixedGroupInvocationsArb";
			case All::ColorAttachmentExt:return "ColorAttachmentExt";
			case All::MultiviewExt:return "MultiviewExt";
			case All::MaxMultiviewBuffersExt:return "MaxMultiviewBuffersExt";
			case All::ContextRobustAccess:return "ContextRobustAccess";
			case All::ComputeProgramParameterBufferNv:return "ComputeProgramParameterBufferNv";
			case All::Texture2DMultisampleArrayOes:return "Texture2DMultisampleArrayOes";
			case All::TextureBinding2DMultisampleArrayOes:return "TextureBinding2DMultisampleArrayOes";
			case All::TextureSamples:return "TextureSamples";
			case All::TextureFixedSampleLocations:return "TextureFixedSampleLocations";
			case All::Sampler2DMultisampleArrayOes:return "Sampler2DMultisampleArrayOes";
			case All::IntSampler2DMultisampleArrayOes:return "IntSampler2DMultisampleArrayOes";
			case All::UnsignedIntSampler2DMultisampleArrayOes:return "UnsignedIntSampler2DMultisampleArrayOes";
			case All::MaxServerWaitTimeoutApple:return "MaxServerWaitTimeoutApple";
			case All::ObjectTypeApple:return "ObjectTypeApple";
			case All::SyncConditionApple:return "SyncConditionApple";
			case All::SyncStatusApple:return "SyncStatusApple";
			case All::SyncFlagsApple:return "SyncFlagsApple";
			case All::SyncFence:return "SyncFence";
			case All::SyncGpuCommandsCompleteApple:return "SyncGpuCommandsCompleteApple";
			case All::Unsignaled:return "Unsignaled";
			case All::Signaled:return "Signaled";
			case All::AlreadySignaledApple:return "AlreadySignaledApple";
			case All::TimeoutExpiredApple:return "TimeoutExpiredApple";
			case All::ConditionSatisfiedApple:return "ConditionSatisfiedApple";
			case All::WaitFailedApple:return "WaitFailedApple";
			case All::MaxGeometryInputComponentsExt:return "MaxGeometryInputComponentsExt";
			case All::MaxGeometryOutputComponentsExt:return "MaxGeometryOutputComponentsExt";
			case All::UnpackCompressedBlockWidth:return "UnpackCompressedBlockWidth";
			case All::UnpackCompressedBlockHeight:return "UnpackCompressedBlockHeight";
			case All::UnpackCompressedBlockDepth:return "UnpackCompressedBlockDepth";
			case All::UnpackCompressedBlockSize:return "UnpackCompressedBlockSize";
			case All::PackCompressedBlockWidth:return "PackCompressedBlockWidth";
			case All::PackCompressedBlockHeight:return "PackCompressedBlockHeight";
			case All::PackCompressedBlockDepth:return "PackCompressedBlockDepth";
			case All::PackCompressedBlockSize:return "PackCompressedBlockSize";
			case All::TextureImmutableFormat:return "TextureImmutableFormat";
			case All::SgxProgramBinaryImg:return "SgxProgramBinaryImg";
			case All::FramebufferIncompleteMultisampleImg:return "FramebufferIncompleteMultisampleImg";
			case All::MaxSamplesImg:return "MaxSamplesImg";
			case All::TextureSamplesImg:return "TextureSamplesImg";
			case All::CubicImg:return "CubicImg";
			case All::CubicMipmapNearestImg:return "CubicMipmapNearestImg";
			case All::CubicMipmapLinearImg:return "CubicMipmapLinearImg";
			case All::FramebufferIncompleteMultisampleAndDownsampleImg:return "FramebufferIncompleteMultisampleAndDownsampleImg";
			case All::NumDownsampleScalesImg:return "NumDownsampleScalesImg";
			case All::DownsampleScalesImg:return "DownsampleScalesImg";
			case All::MaxDebugMessageLength:return "MaxDebugMessageLength";
			case All::MaxDebugLoggedMessages:return "MaxDebugLoggedMessages";
			case All::DebugLoggedMessages:return "DebugLoggedMessages";
			case All::DebugSeverityHighAmd:return "DebugSeverityHighAmd";
			case All::DebugSeverityMediumAmd:return "DebugSeverityMediumAmd";
			case All::DebugSeverityLowAmd:return "DebugSeverityLowAmd";
			case All::DebugCategoryApiErrorAmd:return "DebugCategoryApiErrorAmd";
			case All::DebugCategoryWindowSystemAmd:return "DebugCategoryWindowSystemAmd";
			case All::DebugCategoryDeprecationAmd:return "DebugCategoryDeprecationAmd";
			case All::DebugCategoryUndefinedBehaviorAmd:return "DebugCategoryUndefinedBehaviorAmd";
			case All::DebugCategoryPerformanceAmd:return "DebugCategoryPerformanceAmd";
			case All::DebugCategoryShaderCompilerAmd:return "DebugCategoryShaderCompilerAmd";
			case All::DebugCategoryApplicationAmd:return "DebugCategoryApplicationAmd";
			case All::DebugCategoryOtherAmd:return "DebugCategoryOtherAmd";
			case All::BufferObjectExt:return "BufferObjectExt";
			case All::PerformanceMonitorAmd:return "PerformanceMonitorAmd";
			case All::QueryObjectAmd:return "QueryObjectAmd";
			case All::VertexArrayObjectAmd:return "VertexArrayObjectAmd";
			case All::SamplerObjectAmd:return "SamplerObjectAmd";
			case All::ExternalVirtualMemoryBufferAmd:return "ExternalVirtualMemoryBufferAmd";
			case All::QueryBufferAmd:return "QueryBufferAmd";
			case All::QueryBufferBinding:return "QueryBufferBinding";
			case All::QueryResultNoWaitAmd:return "QueryResultNoWaitAmd";
			case All::VirtualPageSizeXArb:return "VirtualPageSizeXArb";
			case All::VirtualPageSizeYArb:return "VirtualPageSizeYArb";
			case All::VirtualPageSizeZArb:return "VirtualPageSizeZArb";
			case All::MaxSparseTextureSizeArb:return "MaxSparseTextureSizeArb";
			case All::MaxSparse3DTextureSizeArb:return "MaxSparse3DTextureSizeArb";
			case All::MaxSparseArrayTextureLayers:return "MaxSparseArrayTextureLayers";
			case All::MinSparseLevelAmd:return "MinSparseLevelAmd";
			case All::MinLodWarningAmd:return "MinLodWarningAmd";
			case All::TextureBufferOffset:return "TextureBufferOffset";
			case All::TextureBufferSize:return "TextureBufferSize";
			case All::TextureBufferOffsetAlignmentExt:return "TextureBufferOffsetAlignmentExt";
			case All::StreamRasterizationAmd:return "StreamRasterizationAmd";
			case All::VertexElementSwizzleAmd:return "VertexElementSwizzleAmd";
			case All::VertexIdSwizzleAmd:return "VertexIdSwizzleAmd";
			case All::TextureSparseArb:return "TextureSparseArb";
			case All::VirtualPageSizeIndexArb:return "VirtualPageSizeIndexArb";
			case All::NumVirtualPageSizesArb:return "NumVirtualPageSizesArb";
			case All::SparseTextureFullArrayCubeMipmapsArb:return "SparseTextureFullArrayCubeMipmapsArb";
			case All::NumSparseLevelsArb:return "NumSparseLevelsArb";
			case All::PixelsPerSamplePatternXAmd:return "PixelsPerSamplePatternXAmd";
			case All::PixelsPerSamplePatternYAmd:return "PixelsPerSamplePatternYAmd";
			case All::MaxShaderCompilerThreadsKhr:return "MaxShaderCompilerThreadsKhr";
			case All::CompletionStatusKhr:return "CompletionStatusKhr";
			case All::MaxColorFramebufferSamplesAmd:return "MaxColorFramebufferSamplesAmd";
			case All::MaxColorFramebufferStorageSamplesAmd:return "MaxColorFramebufferStorageSamplesAmd";
			case All::MaxDepthStencilFramebufferSamplesAmd:return "MaxDepthStencilFramebufferSamplesAmd";
			case All::NumSupportedMultisampleModesAmd:return "NumSupportedMultisampleModesAmd";
			case All::SupportedMultisampleModesAmd:return "SupportedMultisampleModesAmd";
			case All::MaxComputeImageUniforms:return "MaxComputeImageUniforms";
			case All::MaxComputeFixedGroupSizeArb:return "MaxComputeFixedGroupSizeArb";
			case All::Float16Mat2Amd:return "Float16Mat2Amd";
			case All::Float16Mat3Amd:return "Float16Mat3Amd";
			case All::Float16Mat4Amd:return "Float16Mat4Amd";
			case All::Float16Mat2x3Amd:return "Float16Mat2x3Amd";
			case All::Float16Mat2x4Amd:return "Float16Mat2x4Amd";
			case All::Float16Mat3x2Amd:return "Float16Mat3x2Amd";
			case All::Float16Mat3x4Amd:return "Float16Mat3x4Amd";
			case All::Float16Mat4x2Amd:return "Float16Mat4x2Amd";
			case All::Float16Mat4x3Amd:return "Float16Mat4x3Amd";
			case All::UnpackFlipYWebgl:return "UnpackFlipYWebgl";
			case All::UnpackPremultiplyAlphaWebgl:return "UnpackPremultiplyAlphaWebgl";
			case All::ContextLostWebgl:return "ContextLostWebgl";
			case All::UnpackColorspaceConversionWebgl:return "UnpackColorspaceConversionWebgl";
			case All::BrowserDefaultWebgl:return "BrowserDefaultWebgl";
			case All::/* From the WEBGL_debug_renderer_info extension */ UnmaskedVendorWebgl:return "/* From the WEBGL_debug_renderer_info extension */ UnmaskedVendorWebgl";
			case All::/* From the WEBGL_debug_renderer_info extension */ UnmaskedRendererWebgl:return "/* From the WEBGL_debug_renderer_info extension */ UnmaskedRendererWebgl";
			case All::MaxClientWaitTimeoutWebgl:return "MaxClientWaitTimeoutWebgl";
			case All::/* From the WEBGL_video_texture extension */ TextureVideoImageWebgl:return "/* From the WEBGL_video_texture extension */ TextureVideoImageWebgl";
			case All::/* From the WEBGL_video_texture extension */ SamplerVideoImageWebgl:return "/* From the WEBGL_video_texture extension */ SamplerVideoImageWebgl";
			case All::Smaphs30ProgramBinaryDmp:return "Smaphs30ProgramBinaryDmp";
			case All::SmaphsProgramBinaryDmp:return "SmaphsProgramBinaryDmp";
			case All::DmpProgramBinaryDmp:return "DmpProgramBinaryDmp";
			case All::BlendPremultipliedSrcNv:return "BlendPremultipliedSrcNv";
			case All::BlendOverlapNv:return "BlendOverlapNv";
			case All::UncorrelatedNv:return "UncorrelatedNv";
			case All::DisjointNv:return "DisjointNv";
			case All::ConjointNv:return "ConjointNv";
			case All::BlendAdvancedCoherentKhr:return "BlendAdvancedCoherentKhr";
			case All::SrcNv:return "SrcNv";
			case All::DstNv:return "DstNv";
			case All::SrcOverNv:return "SrcOverNv";
			case All::DstOverNv:return "DstOverNv";
			case All::SrcInNv:return "SrcInNv";
			case All::DstInNv:return "DstInNv";
			case All::SrcOutNv:return "SrcOutNv";
			case All::DstOutNv:return "DstOutNv";
			case All::SrcAtopNv:return "SrcAtopNv";
			case All::DstAtopNv:return "DstAtopNv";
			case All::PlusNv:return "PlusNv";
			case All::PlusDarkerNv:return "PlusDarkerNv";
			case All::Multiply:return "Multiply";
			case All::Screen:return "Screen";
			case All::Overlay:return "Overlay";
			case All::Darken:return "Darken";
			case All::Lighten:return "Lighten";
			case All::Colordodge:return "Colordodge";
			case All::Colorburn:return "Colorburn";
			case All::Hardlight:return "Hardlight";
			case All::Softlight:return "Softlight";
			case All::Difference:return "Difference";
			case All::MinusNv:return "MinusNv";
			case All::Exclusion:return "Exclusion";
			case All::ContrastNv:return "ContrastNv";
			case All::InvertRgbNv:return "InvertRgbNv";
			case All::LineardodgeNv:return "LineardodgeNv";
			case All::LinearburnNv:return "LinearburnNv";
			case All::VividlightNv:return "VividlightNv";
			case All::LinearlightNv:return "LinearlightNv";
			case All::PinlightNv:return "PinlightNv";
			case All::HardmixNv:return "HardmixNv";
			case All::HslHue:return "HslHue";
			case All::HslSaturation:return "HslSaturation";
			case All::HslColor:return "HslColor";
			case All::HslLuminosity:return "HslLuminosity";
			case All::PlusClampedNv:return "PlusClampedNv";
			case All::PlusClampedAlphaNv:return "PlusClampedAlphaNv";
			case All::MinusClampedNv:return "MinusClampedNv";
			case All::InvertOvgNv:return "InvertOvgNv";
			case All::MaxLgpuGpusNvx:return "MaxLgpuGpusNvx";
			case All::PurgedContextResetNv:return "PurgedContextResetNv";
			case All::PrimitiveBoundingBoxArb:return "PrimitiveBoundingBoxArb";
			case All::AlphaToCoverageDitherModeNv:return "AlphaToCoverageDitherModeNv";
			case All::AtomicCounterBufferStart:return "AtomicCounterBufferStart";
			case All::AtomicCounterBufferSize:return "AtomicCounterBufferSize";
			case All::MaxVertexAtomicCounterBuffers:return "MaxVertexAtomicCounterBuffers";
			case All::MaxTessControlAtomicCounterBuffers:return "MaxTessControlAtomicCounterBuffers";
			case All::MaxTessEvaluationAtomicCounterBuffers:return "MaxTessEvaluationAtomicCounterBuffers";
			case All::MaxGeometryAtomicCounterBuffers:return "MaxGeometryAtomicCounterBuffers";
			case All::MaxFragmentAtomicCounterBuffers:return "MaxFragmentAtomicCounterBuffers";
			case All::MaxCombinedAtomicCounterBuffers:return "MaxCombinedAtomicCounterBuffers";
			case All::MaxTessControlAtomicCountersExt:return "MaxTessControlAtomicCountersExt";
			case All::MaxTessEvaluationAtomicCountersExt:return "MaxTessEvaluationAtomicCountersExt";
			case All::MaxGeometryAtomicCountersExt:return "MaxGeometryAtomicCountersExt";
			case All::MaxAtomicCounterBufferSize:return "MaxAtomicCounterBufferSize";
			case All::UnsignedIntAtomicCounter:return "UnsignedIntAtomicCounter";
			case All::MaxAtomicCounterBufferBindings:return "MaxAtomicCounterBufferBindings";
			case All::FragmentCoverageToColorNv:return "FragmentCoverageToColorNv";
			case All::FragmentCoverageColorNv:return "FragmentCoverageColorNv";
			case All::MeshOutputPerVertexGranularityExt:return "MeshOutputPerVertexGranularityExt";
			case All::DebugOutputKhr:return "DebugOutputKhr";
			case All::IsPerPatchExt:return "IsPerPatchExt";
			case All::ReferencedByTessControlShaderExt:return "ReferencedByTessControlShaderExt";
			case All::ReferencedByTessEvaluationShaderExt:return "ReferencedByTessEvaluationShaderExt";
			case All::ReferencedByGeometryShaderExt:return "ReferencedByGeometryShaderExt";
			case All::LocationIndexExt:return "LocationIndexExt";
			case All::FramebufferDefaultLayersExt:return "FramebufferDefaultLayersExt";
			case All::MaxFramebufferLayersExt:return "MaxFramebufferLayersExt";
			case All::RasterMultisampleExt:return "RasterMultisampleExt";
			case All::RasterSamplesExt:return "RasterSamplesExt";
			case All::MaxRasterSamplesExt:return "MaxRasterSamplesExt";
			case All::RasterFixedSampleLocationsExt:return "RasterFixedSampleLocationsExt";
			case All::MultisampleRasterizationAllowedExt:return "MultisampleRasterizationAllowedExt";
			case All::EffectiveRasterSamplesExt:return "EffectiveRasterSamplesExt";
			case All::DepthSamplesNv:return "DepthSamplesNv";
			case All::StencilSamplesNv:return "StencilSamplesNv";
			case All::MixedDepthSamplesSupportedNv:return "MixedDepthSamplesSupportedNv";
			case All::MixedStencilSamplesSupportedNv:return "MixedStencilSamplesSupportedNv";
			case All::CoverageModulationTableNv:return "CoverageModulationTableNv";
			case All::CoverageModulationNv:return "CoverageModulationNv";
			case All::CoverageModulationTableSizeNv:return "CoverageModulationTableSizeNv";
			case All::WarpSizeNv:return "WarpSizeNv";
			case All::WarpsPerSmNv:return "WarpsPerSmNv";
			case All::SmCountNv:return "SmCountNv";
			case All::FillRectangleNv:return "FillRectangleNv";
			case All::SampleLocationSubpixelBitsArb:return "SampleLocationSubpixelBitsArb";
			case All::SampleLocationPixelGridWidthArb:return "SampleLocationPixelGridWidthArb";
			case All::SampleLocationPixelGridHeightArb:return "SampleLocationPixelGridHeightArb";
			case All::ProgrammableSampleLocationTableSizeArb:return "ProgrammableSampleLocationTableSizeArb";
			case All::ProgrammableSampleLocationNv:return "ProgrammableSampleLocationNv";
			case All::FramebufferProgrammableSampleLocationsArb:return "FramebufferProgrammableSampleLocationsArb";
			case All::FramebufferSampleLocationPixelGridArb:return "FramebufferSampleLocationPixelGridArb";
			case All::MaxComputeVariableGroupInvocationsArb:return "MaxComputeVariableGroupInvocationsArb";
			case All::MaxComputeVariableGroupSizeArb:return "MaxComputeVariableGroupSizeArb";
			case All::ConservativeRasterizationNv:return "ConservativeRasterizationNv";
			case All::SubpixelPrecisionBiasXBitsNv:return "SubpixelPrecisionBiasXBitsNv";
			case All::SubpixelPrecisionBiasYBitsNv:return "SubpixelPrecisionBiasYBitsNv";
			case All::MaxSubpixelPrecisionBiasBitsNv:return "MaxSubpixelPrecisionBiasBitsNv";
			case All::AlphaToCoverageDitherDefaultNv:return "AlphaToCoverageDitherDefaultNv";
			case All::AlphaToCoverageDitherEnableNv:return "AlphaToCoverageDitherEnableNv";
			case All::AlphaToCoverageDitherDisableNv:return "AlphaToCoverageDitherDisableNv";
			case All::ViewportSwizzlePositiveXNv:return "ViewportSwizzlePositiveXNv";
			case All::ViewportSwizzleNegativeXNv:return "ViewportSwizzleNegativeXNv";
			case All::ViewportSwizzlePositiveYNv:return "ViewportSwizzlePositiveYNv";
			case All::ViewportSwizzleNegativeYNv:return "ViewportSwizzleNegativeYNv";
			case All::ViewportSwizzlePositiveZNv:return "ViewportSwizzlePositiveZNv";
			case All::ViewportSwizzleNegativeZNv:return "ViewportSwizzleNegativeZNv";
			case All::ViewportSwizzlePositiveWNv:return "ViewportSwizzlePositiveWNv";
			case All::ViewportSwizzleNegativeWNv:return "ViewportSwizzleNegativeWNv";
			case All::ViewportSwizzleXNv:return "ViewportSwizzleXNv";
			case All::ViewportSwizzleYNv:return "ViewportSwizzleYNv";
			case All::ViewportSwizzleZNv:return "ViewportSwizzleZNv";
			case All::ViewportSwizzleWNv:return "ViewportSwizzleWNv";
			case All::ClipOrigin:return "ClipOrigin";
			case All::ClipDepthMode:return "ClipDepthMode";
			case All::NegativeOneToOneExt:return "NegativeOneToOneExt";
			case All::ZeroToOneExt:return "ZeroToOneExt";
			case All::TextureReductionModeArb:return "TextureReductionModeArb";
			case All::WeightedAverageArb:return "WeightedAverageArb";
			case All::FontGlyphsAvailableNv:return "FontGlyphsAvailableNv";
			case All::FontTargetUnavailableNv:return "FontTargetUnavailableNv";
			case All::FontUnavailableNv:return "FontUnavailableNv";
			case All::FontUnintelligibleNv:return "FontUnintelligibleNv";
			case All::StandardFontFormatNv:return "StandardFontFormatNv";
			case All::FragmentInputNv:return "FragmentInputNv";
			case All::UniformBufferUnifiedNv:return "UniformBufferUnifiedNv";
			case All::UniformBufferAddressNv:return "UniformBufferAddressNv";
			case All::UniformBufferLengthNv:return "UniformBufferLengthNv";
			case All::MultisamplesNv:return "MultisamplesNv";
			case All::SupersampleScaleXNv:return "SupersampleScaleXNv";
			case All::SupersampleScaleYNv:return "SupersampleScaleYNv";
			case All::ConformantNv:return "ConformantNv";
			case All::ConservativeRasterDilateNv:return "ConservativeRasterDilateNv";
			case All::ConservativeRasterDilateRangeNv:return "ConservativeRasterDilateRangeNv";
			case All::ConservativeRasterDilateGranularityNv:return "ConservativeRasterDilateGranularityNv";
			case All::ViewportPositionWScaleNv:return "ViewportPositionWScaleNv";
			case All::ViewportPositionWScaleXCoeffNv:return "ViewportPositionWScaleXCoeffNv";
			case All::ViewportPositionWScaleYCoeffNv:return "ViewportPositionWScaleYCoeffNv";
			case All::RepresentativeFragmentTestNv:return "RepresentativeFragmentTestNv";
			case All::MultisampleLineWidthRangeArb:return "MultisampleLineWidthRangeArb";
			case All::MultisampleLineWidthGranularityArb:return "MultisampleLineWidthGranularityArb";
			case All::ViewClassEacR11:return "ViewClassEacR11";
			case All::ViewClassEacRg11:return "ViewClassEacRg11";
			case All::ViewClassEtc2Rgb:return "ViewClassEtc2Rgb";
			case All::ViewClassEtc2Rgba:return "ViewClassEtc2Rgba";
			case All::ViewClassEtc2EacRgba:return "ViewClassEtc2EacRgba";
			case All::ViewClassAstc4X4Rgba:return "ViewClassAstc4X4Rgba";
			case All::ViewClassAstc5X4Rgba:return "ViewClassAstc5X4Rgba";
			case All::ViewClassAstc5X5Rgba:return "ViewClassAstc5X5Rgba";
			case All::ViewClassAstc6X5Rgba:return "ViewClassAstc6X5Rgba";
			case All::ViewClassAstc6X6Rgba:return "ViewClassAstc6X6Rgba";
			case All::ViewClassAstc8X5Rgba:return "ViewClassAstc8X5Rgba";
			case All::ViewClassAstc8X6Rgba:return "ViewClassAstc8X6Rgba";
			case All::ViewClassAstc8X8Rgba:return "ViewClassAstc8X8Rgba";
			case All::ViewClassAstc10X5Rgba:return "ViewClassAstc10X5Rgba";
			case All::ViewClassAstc10X6Rgba:return "ViewClassAstc10X6Rgba";
			case All::ViewClassAstc10X8Rgba:return "ViewClassAstc10X8Rgba";
			case All::ViewClassAstc10X10Rgba:return "ViewClassAstc10X10Rgba";
			case All::ViewClassAstc12X10Rgba:return "ViewClassAstc12X10Rgba";
			case All::ViewClassAstc12X12Rgba:return "ViewClassAstc12X12Rgba";
			case All::TranslatedShaderSourceLengthAngle:return "TranslatedShaderSourceLengthAngle";
			case All::Bgra8Ext:return "Bgra8Ext";
			case All::TextureUsageAngle:return "TextureUsageAngle";
			case All::FramebufferAttachmentAngle:return "FramebufferAttachmentAngle";
			case All::PackReverseRowOrderAngle:return "PackReverseRowOrderAngle";
			case All::ProgramBinaryAngle:return "ProgramBinaryAngle";
			case All::PerfqueryCounterEventIntel:return "PerfqueryCounterEventIntel";
			case All::PerfqueryCounterDurationNormIntel:return "PerfqueryCounterDurationNormIntel";
			case All::PerfqueryCounterDurationRawIntel:return "PerfqueryCounterDurationRawIntel";
			case All::PerfqueryCounterThroughputIntel:return "PerfqueryCounterThroughputIntel";
			case All::PerfqueryCounterRawIntel:return "PerfqueryCounterRawIntel";
			case All::PerfqueryCounterTimestampIntel:return "PerfqueryCounterTimestampIntel";
			case All::PerfqueryCounterDataUint32Intel:return "PerfqueryCounterDataUint32Intel";
			case All::PerfqueryCounterDataUint64Intel:return "PerfqueryCounterDataUint64Intel";
			case All::PerfqueryCounterDataFloatIntel:return "PerfqueryCounterDataFloatIntel";
			case All::PerfqueryCounterDataDoubleIntel:return "PerfqueryCounterDataDoubleIntel";
			case All::PerfqueryCounterDataBool32Intel:return "PerfqueryCounterDataBool32Intel";
			case All::PerfqueryQueryNameLengthMaxIntel:return "PerfqueryQueryNameLengthMaxIntel";
			case All::PerfqueryCounterNameLengthMaxIntel:return "PerfqueryCounterNameLengthMaxIntel";
			case All::PerfqueryCounterDescLengthMaxIntel:return "PerfqueryCounterDescLengthMaxIntel";
			case All::PerfqueryGpaExtendedCountersIntel:return "PerfqueryGpaExtendedCountersIntel";
			case All::SubgroupSizeKhr:return "SubgroupSizeKhr";
			case All::SubgroupSupportedStagesKhr:return "SubgroupSupportedStagesKhr";
			case All::SubgroupSupportedFeaturesKhr:return "SubgroupSupportedFeaturesKhr";
			case All::SubgroupQuadAllStagesKhr:return "SubgroupQuadAllStagesKhr";
			case All::MaxMeshTotalMemorySizeNv:return "MaxMeshTotalMemorySizeNv";
			case All::MaxTaskTotalMemorySizeNv:return "MaxTaskTotalMemorySizeNv";
			case All::MaxMeshOutputVerticesExt:return "MaxMeshOutputVerticesExt";
			case All::MaxMeshOutputPrimitivesNv:return "MaxMeshOutputPrimitivesNv";
			case All::MaxTaskOutputCountNv:return "MaxTaskOutputCountNv";
			case All::MaxMeshWorkGroupSizeNv:return "MaxMeshWorkGroupSizeNv";
			case All::MaxTaskWorkGroupSizeNv:return "MaxTaskWorkGroupSizeNv";
			case All::MaxDrawMeshTasksCountNv:return "MaxDrawMeshTasksCountNv";
			case All::MeshWorkGroupSizeExt:return "MeshWorkGroupSizeExt";
			case All::TaskWorkGroupSizeExt:return "TaskWorkGroupSizeExt";
			case All::QueryResourceTypeVidmemAllocNv:return "QueryResourceTypeVidmemAllocNv";
			case All::QueryResourceMemtypeVidmemNv:return "QueryResourceMemtypeVidmemNv";
			case All::MeshOutputPerPrimitiveGranularityExt:return "MeshOutputPerPrimitiveGranularityExt";
			case All::QueryResourceSysReservedNv:return "QueryResourceSysReservedNv";
			case All::QueryResourceTextureNv:return "QueryResourceTextureNv";
			case All::QueryResourceRenderbufferNv:return "QueryResourceRenderbufferNv";
			case All::QueryResourceBufferobjectNv:return "QueryResourceBufferobjectNv";
			case All::PerGpuStorageNv:return "PerGpuStorageNv";
			case All::MulticastProgrammableSampleLocationNv:return "MulticastProgrammableSampleLocationNv";
			case All::UploadGpuMaskNvx:return "UploadGpuMaskNvx";
			case All::ConservativeRasterModeNv:return "ConservativeRasterModeNv";
			case All::ConservativeRasterModePostSnapNv:return "ConservativeRasterModePostSnapNv";
			case All::ConservativeRasterModePreSnapTrianglesNv:return "ConservativeRasterModePreSnapTrianglesNv";
			case All::ConservativeRasterModePreSnapNv:return "ConservativeRasterModePreSnapNv";
			case All::ShaderBinaryFormatSpirVArb:return "ShaderBinaryFormatSpirVArb";
			case All::SpirVBinary:return "SpirVBinary";
			case All::SpirVExtensions:return "SpirVExtensions";
			case All::NumSpirVExtensions:return "NumSpirVExtensions";
			case All::ScissorTestExclusiveNv:return "ScissorTestExclusiveNv";
			case All::ScissorBoxExclusiveNv:return "ScissorBoxExclusiveNv";
			case All::MaxMeshMultiviewViewCountExt:return "MaxMeshMultiviewViewCountExt";
			case All::RenderGpuMaskNv:return "RenderGpuMaskNv";
			case All::MeshShaderExt:return "MeshShaderExt";
			case All::TaskShaderExt:return "TaskShaderExt";
			case All::ShadingRateImageBindingNv:return "ShadingRateImageBindingNv";
			case All::ShadingRateImageTexelWidthNv:return "ShadingRateImageTexelWidthNv";
			case All::ShadingRateImageTexelHeightNv:return "ShadingRateImageTexelHeightNv";
			case All::ShadingRateImagePaletteSizeNv:return "ShadingRateImagePaletteSizeNv";
			case All::MaxCoarseFragmentSamplesNv:return "MaxCoarseFragmentSamplesNv";
			case All::ShadingRateImageNv:return "ShadingRateImageNv";
			case All::ShadingRateNoInvocationsNv:return "ShadingRateNoInvocationsNv";
			case All::ShadingRate1InvocationPerPixelNv:return "ShadingRate1InvocationPerPixelNv";
			case All::ShadingRate1InvocationPer1X2PixelsNv:return "ShadingRate1InvocationPer1X2PixelsNv";
			case All::ShadingRate1InvocationPer2X1PixelsNv:return "ShadingRate1InvocationPer2X1PixelsNv";
			case All::ShadingRate1InvocationPer2X2PixelsNv:return "ShadingRate1InvocationPer2X2PixelsNv";
			case All::ShadingRate1InvocationPer2X4PixelsNv:return "ShadingRate1InvocationPer2X4PixelsNv";
			case All::ShadingRate1InvocationPer4X2PixelsNv:return "ShadingRate1InvocationPer4X2PixelsNv";
			case All::ShadingRate1InvocationPer4X4PixelsNv:return "ShadingRate1InvocationPer4X4PixelsNv";
			case All::ShadingRate2InvocationsPerPixelNv:return "ShadingRate2InvocationsPerPixelNv";
			case All::ShadingRate4InvocationsPerPixelNv:return "ShadingRate4InvocationsPerPixelNv";
			case All::ShadingRate8InvocationsPerPixelNv:return "ShadingRate8InvocationsPerPixelNv";
			case All::ShadingRate16InvocationsPerPixelNv:return "ShadingRate16InvocationsPerPixelNv";
			case All::MeshVerticesOutExt:return "MeshVerticesOutExt";
			case All::MeshPrimitivesOutExt:return "MeshPrimitivesOutExt";
			case All::MeshOutputTypeExt:return "MeshOutputTypeExt";
			case All::MeshSubroutineExt:return "MeshSubroutineExt";
			case All::TaskSubroutineExt:return "TaskSubroutineExt";
			case All::MeshSubroutineUniformExt:return "MeshSubroutineUniformExt";
			case All::TaskSubroutineUniformExt:return "TaskSubroutineUniformExt";
			case All::NumTilingTypesExt:return "NumTilingTypesExt";
			case All::TilingTypesExt:return "TilingTypesExt";
			case All::OptimalTilingExt:return "OptimalTilingExt";
			case All::LinearTilingExt:return "LinearTilingExt";
			case All::UniformBlockReferencedByMeshShaderExt:return "UniformBlockReferencedByMeshShaderExt";
			case All::UniformBlockReferencedByTaskShaderExt:return "UniformBlockReferencedByTaskShaderExt";
			case All::AtomicCounterBufferReferencedByMeshShaderExt:return "AtomicCounterBufferReferencedByMeshShaderExt";
			case All::AtomicCounterBufferReferencedByTaskShaderExt:return "AtomicCounterBufferReferencedByTaskShaderExt";
			case All::ReferencedByMeshShaderExt:return "ReferencedByMeshShaderExt";
			case All::ReferencedByTaskShaderExt:return "ReferencedByTaskShaderExt";
			case All::MaxMeshWorkGroupInvocationsNv:return "MaxMeshWorkGroupInvocationsNv";
			case All::MaxTaskWorkGroupInvocationsNv:return "MaxTaskWorkGroupInvocationsNv";
			case All::AttachedMemoryObjectNv:return "AttachedMemoryObjectNv";
			case All::AttachedMemoryOffsetNv:return "AttachedMemoryOffsetNv";
			case All::MemoryAttachableAlignmentNv:return "MemoryAttachableAlignmentNv";
			case All::MemoryAttachableSizeNv:return "MemoryAttachableSizeNv";
			case All::MemoryAttachableNv:return "MemoryAttachableNv";
			case All::DetachedMemoryIncarnationNv:return "DetachedMemoryIncarnationNv";
			case All::DetachedTexturesNv:return "DetachedTexturesNv";
			case All::DetachedBuffersNv:return "DetachedBuffersNv";
			case All::MaxDetachedTexturesNv:return "MaxDetachedTexturesNv";
			case All::MaxDetachedBuffersNv:return "MaxDetachedBuffersNv";
			case All::ShadingRateSampleOrderDefaultNv:return "ShadingRateSampleOrderDefaultNv";
			case All::ShadingRateSampleOrderPixelMajorNv:return "ShadingRateSampleOrderPixelMajorNv";
			case All::ShadingRateSampleOrderSampleMajorNv:return "ShadingRateSampleOrderSampleMajorNv";
			case All::MaxViewsOvr:return "MaxViewsOvr";
			case All::FramebufferIncompleteViewTargetsOvr:return "FramebufferIncompleteViewTargetsOvr";
			case All::GsShaderBinaryMtk:return "GsShaderBinaryMtk";
			case All::GsProgramBinaryMtk:return "GsProgramBinaryMtk";
			case All::MaxShaderCombinedLocalStorageFastSizeExt:return "MaxShaderCombinedLocalStorageFastSizeExt";
			case All::MaxShaderCombinedLocalStorageSizeExt:return "MaxShaderCombinedLocalStorageSizeExt";
			case All::FramebufferIncompleteInsufficientShaderCombinedLocalStorageExt:return "FramebufferIncompleteInsufficientShaderCombinedLocalStorageExt";
			case All::ValidateShaderBinaryQcom:return "ValidateShaderBinaryQcom";
			case All::HuaweiProgramBinary:return "ProgramBinaryHuawei";
			case All::RasterPositionUnclippedIbm:return "RasterPositionUnclippedIbm";
			case All::CullVertexIbm:return "CullVertexIbm";
			case All::AllStaticDataIbm:return "AllStaticDataIbm";
			case All::StaticVertexArrayIbm:return "StaticVertexArrayIbm";
			case All::VertexArrayListIbm:return "VertexArrayListIbm";
			case All::NormalArrayListIbm:return "NormalArrayListIbm";
			case All::ColorArrayListIbm:return "ColorArrayListIbm";
			case All::IndexArrayListIbm:return "IndexArrayListIbm";
			case All::TextureCoordArrayListIbm:return "TextureCoordArrayListIbm";
			case All::EdgeFlagArrayListIbm:return "EdgeFlagArrayListIbm";
			case All::FogCoordinateArrayListIbm:return "FogCoordinateArrayListIbm";
			case All::SecondaryColorArrayListIbm:return "SecondaryColorArrayListIbm";
			case All::VertexArrayListStrideIbm:return "VertexArrayListStrideIbm";
			case All::NormalArrayListStrideIbm:return "NormalArrayListStrideIbm";
			case All::ColorArrayListStrideIbm:return "ColorArrayListStrideIbm";
			case All::IndexArrayListStrideIbm:return "IndexArrayListStrideIbm";
			case All::TextureCoordArrayListStrideIbm:return "TextureCoordArrayListStrideIbm";
			case All::EdgeFlagArrayListStrideIbm:return "EdgeFlagArrayListStrideIbm";
			case All::FogCoordinateArrayListStrideIbm:return "FogCoordinateArrayListStrideIbm";
			case All::SecondaryColorArrayListStrideIbm:return "SecondaryColorArrayListStrideIbm";
			case All::NativeGraphicsHandlePgi:return "NativeGraphicsHandlePgi";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TransformFeedbackTokenNV value) {
		switch(value){
			case TransformFeedbackTokenNV::NextBufferNv:return "NextBufferNv";
			case TransformFeedbackTokenNV::SkipComponents4Nv:return "SkipComponents4Nv";
			case TransformFeedbackTokenNV::SkipComponents3Nv:return "SkipComponents3Nv";
			case TransformFeedbackTokenNV::SkipComponents2Nv:return "SkipComponents2Nv";
			case TransformFeedbackTokenNV::SkipComponents1Nv:return "SkipComponents1Nv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TriangleListSUN value) {
		switch(value){
			case TriangleListSUN::RestartSun:return "RestartSun";
			case TriangleListSUN::ReplaceMiddleSun:return "ReplaceMiddleSun";
			case TriangleListSUN::ReplaceOldestSun:return "ReplaceOldestSun";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SpecialNumbers value) {
		switch(value){
			case SpecialNumbers::False:return "False";
			case SpecialNumbers::True:return "True";
			case SpecialNumbers::/* Tagged as uint */ InvalidIndex:return "/* Tagged as uint */ InvalidIndex";
			case SpecialNumbers::/* Tagged as uint64 */ TimeoutIgnored:return "/* Tagged as uint64 */ TimeoutIgnored";
			case SpecialNumbers::UuidSizeExt:return "UuidSizeExt";
			case SpecialNumbers::LuidSizeExt:return "LuidSizeExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(Boolean value) {
		switch(value){
			case Boolean::False:return "False";
			case Boolean::True:return "True";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderWriteMaskEXT value) {
		switch(value){
			case VertexShaderWriteMaskEXT::False:return "False";
			case VertexShaderWriteMaskEXT::True:return "True";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClampColorModeARB value) {
		switch(value){
			case ClampColorModeARB::False:return "False";
			case ClampColorModeARB::True:return "True";
			case ClampColorModeARB::FixedOnly:return "FixedOnly";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GraphicsResetStatus value) {
		switch(value){
			case GraphicsResetStatus::NoError:return "NoError";
			case GraphicsResetStatus::GuiltyContextReset:return "GuiltyContextReset";
			case GraphicsResetStatus::InnocentContextReset:return "InnocentContextReset";
			case GraphicsResetStatus::UnknownContextReset:return "UnknownContextReset";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ErrorCode value) {
		switch(value){
			case ErrorCode::NoError:return "NoError";
			case ErrorCode::InvalidEnum:return "InvalidEnum";
			case ErrorCode::InvalidValue:return "InvalidValue";
			case ErrorCode::InvalidOperation:return "InvalidOperation";
			case ErrorCode::StackOverflow:return "StackOverflow";
			case ErrorCode::StackUnderflow:return "StackUnderflow";
			case ErrorCode::OutOfMemory:return "OutOfMemory";
			case ErrorCode::InvalidFramebufferOperation:return "InvalidFramebufferOperation";
			case ErrorCode::TableTooLargeExt:return "TableTooLargeExt";
			case ErrorCode::TextureTooLargeExt:return "TextureTooLargeExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureSwizzle value) {
		switch(value){
			case TextureSwizzle::Zero:return "Zero";
			case TextureSwizzle::One:return "One";
			case TextureSwizzle::Red:return "Red";
			case TextureSwizzle::Green:return "Green";
			case TextureSwizzle::Blue:return "Blue";
			case TextureSwizzle::Alpha:return "Alpha";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(StencilOp value) {
		switch(value){
			case StencilOp::Zero:return "Zero";
			case StencilOp::Invert:return "Invert";
			case StencilOp::Keep:return "Keep";
			case StencilOp::Replace:return "Replace";
			case StencilOp::Incr:return "Incr";
			case StencilOp::Decr:return "Decr";
			case StencilOp::IncrWrap:return "IncrWrap";
			case StencilOp::DecrWrap:return "DecrWrap";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BlendingFactor value) {
		switch(value){
			case BlendingFactor::Zero:return "Zero";
			case BlendingFactor::One:return "One";
			case BlendingFactor::SrcColor:return "SrcColor";
			case BlendingFactor::OneMinusSrcColor:return "OneMinusSrcColor";
			case BlendingFactor::SrcAlpha:return "SrcAlpha";
			case BlendingFactor::OneMinusSrcAlpha:return "OneMinusSrcAlpha";
			case BlendingFactor::DstAlpha:return "DstAlpha";
			case BlendingFactor::OneMinusDstAlpha:return "OneMinusDstAlpha";
			case BlendingFactor::DstColor:return "DstColor";
			case BlendingFactor::OneMinusDstColor:return "OneMinusDstColor";
			case BlendingFactor::SrcAlphaSaturate:return "SrcAlphaSaturate";
			case BlendingFactor::ConstantColor:return "ConstantColor";
			case BlendingFactor::OneMinusConstantColor:return "OneMinusConstantColor";
			case BlendingFactor::ConstantAlpha:return "ConstantAlpha";
			case BlendingFactor::OneMinusConstantAlpha:return "OneMinusConstantAlpha";
			case BlendingFactor::Src1Alpha:return "Src1Alpha";
			case BlendingFactor::Src1Color:return "Src1Color";
			case BlendingFactor::OneMinusSrc1Color:return "OneMinusSrc1Color";
			case BlendingFactor::OneMinusSrc1Alpha:return "OneMinusSrc1Alpha";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderGenericSourceATI value) {
		switch(value){
			case FragmentShaderGenericSourceATI::Zero:return "Zero";
			case FragmentShaderGenericSourceATI::One:return "One";
			case FragmentShaderGenericSourceATI::PrimaryColor:return "PrimaryColor";
			case FragmentShaderGenericSourceATI::Reg0Ati:return "Reg0Ati";
			case FragmentShaderGenericSourceATI::Reg1Ati:return "Reg1Ati";
			case FragmentShaderGenericSourceATI::Reg2Ati:return "Reg2Ati";
			case FragmentShaderGenericSourceATI::Reg3Ati:return "Reg3Ati";
			case FragmentShaderGenericSourceATI::Reg4Ati:return "Reg4Ati";
			case FragmentShaderGenericSourceATI::Reg5Ati:return "Reg5Ati";
			case FragmentShaderGenericSourceATI::Reg6Ati:return "Reg6Ati";
			case FragmentShaderGenericSourceATI::Reg7Ati:return "Reg7Ati";
			case FragmentShaderGenericSourceATI::Reg8Ati:return "Reg8Ati";
			case FragmentShaderGenericSourceATI::Reg9Ati:return "Reg9Ati";
			case FragmentShaderGenericSourceATI::Reg10Ati:return "Reg10Ati";
			case FragmentShaderGenericSourceATI::Reg11Ati:return "Reg11Ati";
			case FragmentShaderGenericSourceATI::Reg12Ati:return "Reg12Ati";
			case FragmentShaderGenericSourceATI::Reg13Ati:return "Reg13Ati";
			case FragmentShaderGenericSourceATI::Reg14Ati:return "Reg14Ati";
			case FragmentShaderGenericSourceATI::Reg15Ati:return "Reg15Ati";
			case FragmentShaderGenericSourceATI::Reg16Ati:return "Reg16Ati";
			case FragmentShaderGenericSourceATI::Reg17Ati:return "Reg17Ati";
			case FragmentShaderGenericSourceATI::Reg18Ati:return "Reg18Ati";
			case FragmentShaderGenericSourceATI::Reg19Ati:return "Reg19Ati";
			case FragmentShaderGenericSourceATI::Reg20Ati:return "Reg20Ati";
			case FragmentShaderGenericSourceATI::Reg21Ati:return "Reg21Ati";
			case FragmentShaderGenericSourceATI::Reg22Ati:return "Reg22Ati";
			case FragmentShaderGenericSourceATI::Reg23Ati:return "Reg23Ati";
			case FragmentShaderGenericSourceATI::Reg24Ati:return "Reg24Ati";
			case FragmentShaderGenericSourceATI::Reg25Ati:return "Reg25Ati";
			case FragmentShaderGenericSourceATI::Reg26Ati:return "Reg26Ati";
			case FragmentShaderGenericSourceATI::Reg27Ati:return "Reg27Ati";
			case FragmentShaderGenericSourceATI::Reg28Ati:return "Reg28Ati";
			case FragmentShaderGenericSourceATI::Reg29Ati:return "Reg29Ati";
			case FragmentShaderGenericSourceATI::Reg30Ati:return "Reg30Ati";
			case FragmentShaderGenericSourceATI::Reg31Ati:return "Reg31Ati";
			case FragmentShaderGenericSourceATI::Con0Ati:return "Con0Ati";
			case FragmentShaderGenericSourceATI::Con1Ati:return "Con1Ati";
			case FragmentShaderGenericSourceATI::Con2Ati:return "Con2Ati";
			case FragmentShaderGenericSourceATI::Con3Ati:return "Con3Ati";
			case FragmentShaderGenericSourceATI::Con4Ati:return "Con4Ati";
			case FragmentShaderGenericSourceATI::Con5Ati:return "Con5Ati";
			case FragmentShaderGenericSourceATI::Con6Ati:return "Con6Ati";
			case FragmentShaderGenericSourceATI::Con7Ati:return "Con7Ati";
			case FragmentShaderGenericSourceATI::Con8Ati:return "Con8Ati";
			case FragmentShaderGenericSourceATI::Con9Ati:return "Con9Ati";
			case FragmentShaderGenericSourceATI::Con10Ati:return "Con10Ati";
			case FragmentShaderGenericSourceATI::Con11Ati:return "Con11Ati";
			case FragmentShaderGenericSourceATI::Con12Ati:return "Con12Ati";
			case FragmentShaderGenericSourceATI::Con13Ati:return "Con13Ati";
			case FragmentShaderGenericSourceATI::Con14Ati:return "Con14Ati";
			case FragmentShaderGenericSourceATI::Con15Ati:return "Con15Ati";
			case FragmentShaderGenericSourceATI::Con16Ati:return "Con16Ati";
			case FragmentShaderGenericSourceATI::Con17Ati:return "Con17Ati";
			case FragmentShaderGenericSourceATI::Con18Ati:return "Con18Ati";
			case FragmentShaderGenericSourceATI::Con19Ati:return "Con19Ati";
			case FragmentShaderGenericSourceATI::Con20Ati:return "Con20Ati";
			case FragmentShaderGenericSourceATI::Con21Ati:return "Con21Ati";
			case FragmentShaderGenericSourceATI::Con22Ati:return "Con22Ati";
			case FragmentShaderGenericSourceATI::Con23Ati:return "Con23Ati";
			case FragmentShaderGenericSourceATI::Con24Ati:return "Con24Ati";
			case FragmentShaderGenericSourceATI::Con25Ati:return "Con25Ati";
			case FragmentShaderGenericSourceATI::Con26Ati:return "Con26Ati";
			case FragmentShaderGenericSourceATI::Con27Ati:return "Con27Ati";
			case FragmentShaderGenericSourceATI::Con28Ati:return "Con28Ati";
			case FragmentShaderGenericSourceATI::Con29Ati:return "Con29Ati";
			case FragmentShaderGenericSourceATI::Con30Ati:return "Con30Ati";
			case FragmentShaderGenericSourceATI::Con31Ati:return "Con31Ati";
			case FragmentShaderGenericSourceATI::SecondaryInterpolatorAti:return "SecondaryInterpolatorAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderValueRepATI value) {
		switch(value){
			case FragmentShaderValueRepATI::None:return "None";
			case FragmentShaderValueRepATI::Red:return "Red";
			case FragmentShaderValueRepATI::Green:return "Green";
			case FragmentShaderValueRepATI::Blue:return "Blue";
			case FragmentShaderValueRepATI::Alpha:return "Alpha";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SyncBehaviorFlags value) {
		switch(value){
			case SyncBehaviorFlags::None:return "None";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureCompareMode value) {
		switch(value){
			case TextureCompareMode::None:return "None";
			case TextureCompareMode::CompareRToTexture:return "CompareRToTexture";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathColorFormat value) {
		switch(value){
			case PathColorFormat::None:return "None";
			case PathColorFormat::Alpha:return "Alpha";
			case PathColorFormat::Rgb:return "Rgb";
			case PathColorFormat::Rgba:return "Rgba";
			case PathColorFormat::Luminance:return "Luminance";
			case PathColorFormat::LuminanceAlpha:return "LuminanceAlpha";
			case PathColorFormat::Intensity:return "Intensity";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerBiasNV value) {
		switch(value){
			case CombinerBiasNV::None:return "None";
			case CombinerBiasNV::BiasByNegativeOneHalfNv:return "BiasByNegativeOneHalfNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerScaleNV value) {
		switch(value){
			case CombinerScaleNV::None:return "None";
			case CombinerScaleNV::ScaleByTwoNv:return "ScaleByTwoNv";
			case CombinerScaleNV::ScaleByFourNv:return "ScaleByFourNv";
			case CombinerScaleNV::ScaleByOneHalfNv:return "ScaleByOneHalfNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DrawBufferMode value) {
		switch(value){
			case DrawBufferMode::None:return "None";
			case DrawBufferMode::FrontLeft:return "FrontLeft";
			case DrawBufferMode::FrontRight:return "FrontRight";
			case DrawBufferMode::BackLeft:return "BackLeft";
			case DrawBufferMode::BackRight:return "BackRight";
			case DrawBufferMode::Front:return "Front";
			case DrawBufferMode::Back:return "Back";
			case DrawBufferMode::Left:return "Left";
			case DrawBufferMode::Right:return "Right";
			case DrawBufferMode::FrontAndBack:return "FrontAndBack";
			case DrawBufferMode::Aux0:return "Aux0";
			case DrawBufferMode::Aux1:return "Aux1";
			case DrawBufferMode::Aux2:return "Aux2";
			case DrawBufferMode::Aux3:return "Aux3";
			case DrawBufferMode::ColorAttachment0:return "ColorAttachment0";
			case DrawBufferMode::ColorAttachment1:return "ColorAttachment1";
			case DrawBufferMode::ColorAttachment2:return "ColorAttachment2";
			case DrawBufferMode::ColorAttachment3:return "ColorAttachment3";
			case DrawBufferMode::ColorAttachment4:return "ColorAttachment4";
			case DrawBufferMode::ColorAttachment5:return "ColorAttachment5";
			case DrawBufferMode::ColorAttachment6:return "ColorAttachment6";
			case DrawBufferMode::ColorAttachment7:return "ColorAttachment7";
			case DrawBufferMode::ColorAttachment8:return "ColorAttachment8";
			case DrawBufferMode::ColorAttachment9:return "ColorAttachment9";
			case DrawBufferMode::ColorAttachment10:return "ColorAttachment10";
			case DrawBufferMode::ColorAttachment11:return "ColorAttachment11";
			case DrawBufferMode::ColorAttachment12:return "ColorAttachment12";
			case DrawBufferMode::ColorAttachment13:return "ColorAttachment13";
			case DrawBufferMode::ColorAttachment14:return "ColorAttachment14";
			case DrawBufferMode::ColorAttachment15:return "ColorAttachment15";
			case DrawBufferMode::ColorAttachment16:return "ColorAttachment16";
			case DrawBufferMode::ColorAttachment17:return "ColorAttachment17";
			case DrawBufferMode::ColorAttachment18:return "ColorAttachment18";
			case DrawBufferMode::ColorAttachment19:return "ColorAttachment19";
			case DrawBufferMode::ColorAttachment20:return "ColorAttachment20";
			case DrawBufferMode::ColorAttachment21:return "ColorAttachment21";
			case DrawBufferMode::ColorAttachment22:return "ColorAttachment22";
			case DrawBufferMode::ColorAttachment23:return "ColorAttachment23";
			case DrawBufferMode::ColorAttachment24:return "ColorAttachment24";
			case DrawBufferMode::ColorAttachment25:return "ColorAttachment25";
			case DrawBufferMode::ColorAttachment26:return "ColorAttachment26";
			case DrawBufferMode::ColorAttachment27:return "ColorAttachment27";
			case DrawBufferMode::ColorAttachment28:return "ColorAttachment28";
			case DrawBufferMode::ColorAttachment29:return "ColorAttachment29";
			case DrawBufferMode::ColorAttachment30:return "ColorAttachment30";
			case DrawBufferMode::ColorAttachment31:return "ColorAttachment31";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelTexGenModeSGIX value) {
		switch(value){
			case PixelTexGenModeSGIX::None:return "None";
			case PixelTexGenModeSGIX::Alpha:return "Alpha";
			case PixelTexGenModeSGIX::Rgb:return "Rgb";
			case PixelTexGenModeSGIX::Rgba:return "Rgba";
			case PixelTexGenModeSGIX::PixelTexGenQCeilingSgix:return "PixelTexGenQCeilingSgix";
			case PixelTexGenModeSGIX::PixelTexGenQRoundSgix:return "PixelTexGenQRoundSgix";
			case PixelTexGenModeSGIX::PixelTexGenQFloorSgix:return "PixelTexGenQFloorSgix";
			case PixelTexGenModeSGIX::PixelTexGenAlphaLsSgix:return "PixelTexGenAlphaLsSgix";
			case PixelTexGenModeSGIX::PixelTexGenAlphaMsSgix:return "PixelTexGenAlphaMsSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ReadBufferMode value) {
		switch(value){
			case ReadBufferMode::None:return "None";
			case ReadBufferMode::FrontLeft:return "FrontLeft";
			case ReadBufferMode::FrontRight:return "FrontRight";
			case ReadBufferMode::BackLeft:return "BackLeft";
			case ReadBufferMode::BackRight:return "BackRight";
			case ReadBufferMode::Front:return "Front";
			case ReadBufferMode::Back:return "Back";
			case ReadBufferMode::Left:return "Left";
			case ReadBufferMode::Right:return "Right";
			case ReadBufferMode::Aux0:return "Aux0";
			case ReadBufferMode::Aux1:return "Aux1";
			case ReadBufferMode::Aux2:return "Aux2";
			case ReadBufferMode::Aux3:return "Aux3";
			case ReadBufferMode::ColorAttachment0:return "ColorAttachment0";
			case ReadBufferMode::ColorAttachment1:return "ColorAttachment1";
			case ReadBufferMode::ColorAttachment2:return "ColorAttachment2";
			case ReadBufferMode::ColorAttachment3:return "ColorAttachment3";
			case ReadBufferMode::ColorAttachment4:return "ColorAttachment4";
			case ReadBufferMode::ColorAttachment5:return "ColorAttachment5";
			case ReadBufferMode::ColorAttachment6:return "ColorAttachment6";
			case ReadBufferMode::ColorAttachment7:return "ColorAttachment7";
			case ReadBufferMode::ColorAttachment8:return "ColorAttachment8";
			case ReadBufferMode::ColorAttachment9:return "ColorAttachment9";
			case ReadBufferMode::ColorAttachment10:return "ColorAttachment10";
			case ReadBufferMode::ColorAttachment11:return "ColorAttachment11";
			case ReadBufferMode::ColorAttachment12:return "ColorAttachment12";
			case ReadBufferMode::ColorAttachment13:return "ColorAttachment13";
			case ReadBufferMode::ColorAttachment14:return "ColorAttachment14";
			case ReadBufferMode::ColorAttachment15:return "ColorAttachment15";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorBuffer value) {
		switch(value){
			case ColorBuffer::None:return "None";
			case ColorBuffer::FrontLeft:return "FrontLeft";
			case ColorBuffer::FrontRight:return "FrontRight";
			case ColorBuffer::BackLeft:return "BackLeft";
			case ColorBuffer::BackRight:return "BackRight";
			case ColorBuffer::Front:return "Front";
			case ColorBuffer::Back:return "Back";
			case ColorBuffer::Left:return "Left";
			case ColorBuffer::Right:return "Right";
			case ColorBuffer::FrontAndBack:return "FrontAndBack";
			case ColorBuffer::ColorAttachment0:return "ColorAttachment0";
			case ColorBuffer::ColorAttachment1:return "ColorAttachment1";
			case ColorBuffer::ColorAttachment2:return "ColorAttachment2";
			case ColorBuffer::ColorAttachment3:return "ColorAttachment3";
			case ColorBuffer::ColorAttachment4:return "ColorAttachment4";
			case ColorBuffer::ColorAttachment5:return "ColorAttachment5";
			case ColorBuffer::ColorAttachment6:return "ColorAttachment6";
			case ColorBuffer::ColorAttachment7:return "ColorAttachment7";
			case ColorBuffer::ColorAttachment8:return "ColorAttachment8";
			case ColorBuffer::ColorAttachment9:return "ColorAttachment9";
			case ColorBuffer::ColorAttachment10:return "ColorAttachment10";
			case ColorBuffer::ColorAttachment11:return "ColorAttachment11";
			case ColorBuffer::ColorAttachment12:return "ColorAttachment12";
			case ColorBuffer::ColorAttachment13:return "ColorAttachment13";
			case ColorBuffer::ColorAttachment14:return "ColorAttachment14";
			case ColorBuffer::ColorAttachment15:return "ColorAttachment15";
			case ColorBuffer::ColorAttachment16:return "ColorAttachment16";
			case ColorBuffer::ColorAttachment17:return "ColorAttachment17";
			case ColorBuffer::ColorAttachment18:return "ColorAttachment18";
			case ColorBuffer::ColorAttachment19:return "ColorAttachment19";
			case ColorBuffer::ColorAttachment20:return "ColorAttachment20";
			case ColorBuffer::ColorAttachment21:return "ColorAttachment21";
			case ColorBuffer::ColorAttachment22:return "ColorAttachment22";
			case ColorBuffer::ColorAttachment23:return "ColorAttachment23";
			case ColorBuffer::ColorAttachment24:return "ColorAttachment24";
			case ColorBuffer::ColorAttachment25:return "ColorAttachment25";
			case ColorBuffer::ColorAttachment26:return "ColorAttachment26";
			case ColorBuffer::ColorAttachment27:return "ColorAttachment27";
			case ColorBuffer::ColorAttachment28:return "ColorAttachment28";
			case ColorBuffer::ColorAttachment29:return "ColorAttachment29";
			case ColorBuffer::ColorAttachment30:return "ColorAttachment30";
			case ColorBuffer::ColorAttachment31:return "ColorAttachment31";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathGenMode value) {
		switch(value){
			case PathGenMode::None:return "None";
			case PathGenMode::EyeLinear:return "EyeLinear";
			case PathGenMode::ObjectLinear:return "ObjectLinear";
			case PathGenMode::Constant:return "Constant";
			case PathGenMode::PathObjectBoundingBoxNv:return "PathObjectBoundingBoxNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathTransformType value) {
		switch(value){
			case PathTransformType::None:return "None";
			case PathTransformType::TranslateXNv:return "TranslateXNv";
			case PathTransformType::TranslateYNv:return "TranslateYNv";
			case PathTransformType::Translate2DNv:return "Translate2DNv";
			case PathTransformType::Translate3DNv:return "Translate3DNv";
			case PathTransformType::Affine2DNv:return "Affine2DNv";
			case PathTransformType::Affine3DNv:return "Affine3DNv";
			case PathTransformType::TransposeAffine2DNv:return "TransposeAffine2DNv";
			case PathTransformType::TransposeAffine3DNv:return "TransposeAffine3DNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PrimitiveType value) {
		switch(value){
			case PrimitiveType::Points:return "Points";
			case PrimitiveType::Lines:return "Lines";
			case PrimitiveType::LineLoop:return "LineLoop";
			case PrimitiveType::LineStrip:return "LineStrip";
			case PrimitiveType::Triangles:return "Triangles";
			case PrimitiveType::TriangleStrip:return "TriangleStrip";
			case PrimitiveType::TriangleFan:return "TriangleFan";
			case PrimitiveType::Quads:return "Quads";
			case PrimitiveType::QuadStrip:return "QuadStrip";
			case PrimitiveType::Polygon:return "Polygon";
			case PrimitiveType::LinesAdjacency:return "LinesAdjacency";
			case PrimitiveType::LineStripAdjacency:return "LineStripAdjacency";
			case PrimitiveType::TrianglesAdjacency:return "TrianglesAdjacency";
			case PrimitiveType::TriangleStripAdjacency:return "TriangleStripAdjacency";
			case PrimitiveType::Patches:return "Patches";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(AccumOp value) {
		switch(value){
			case AccumOp::Accum:return "Accum";
			case AccumOp::Load:return "Load";
			case AccumOp::Return:return "Return";
			case AccumOp::Mult:return "Mult";
			case AccumOp::Add:return "Add";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureEnvMode value) {
		switch(value){
			case TextureEnvMode::Add:return "Add";
			case TextureEnvMode::Blend:return "Blend";
			case TextureEnvMode::Replace:return "Replace";
			case TextureEnvMode::Modulate:return "Modulate";
			case TextureEnvMode::Decal:return "Decal";
			case TextureEnvMode::ReplaceExt:return "ReplaceExt";
			case TextureEnvMode::TextureEnvBiasSgix:return "TextureEnvBiasSgix";
			case TextureEnvMode::Combine:return "Combine";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightEnvModeSGIX value) {
		switch(value){
			case LightEnvModeSGIX::Add:return "Add";
			case LightEnvModeSGIX::Replace:return "Replace";
			case LightEnvModeSGIX::Modulate:return "Modulate";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(StencilFunction value) {
		switch(value){
			case StencilFunction::Never:return "Never";
			case StencilFunction::Less:return "Less";
			case StencilFunction::Equal:return "Equal";
			case StencilFunction::Lequal:return "Lequal";
			case StencilFunction::Greater:return "Greater";
			case StencilFunction::Notequal:return "Notequal";
			case StencilFunction::Gequal:return "Gequal";
			case StencilFunction::Always:return "Always";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(IndexFunctionEXT value) {
		switch(value){
			case IndexFunctionEXT::Never:return "Never";
			case IndexFunctionEXT::Less:return "Less";
			case IndexFunctionEXT::Equal:return "Equal";
			case IndexFunctionEXT::Lequal:return "Lequal";
			case IndexFunctionEXT::Greater:return "Greater";
			case IndexFunctionEXT::Notequal:return "Notequal";
			case IndexFunctionEXT::Gequal:return "Gequal";
			case IndexFunctionEXT::Always:return "Always";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(AlphaFunction value) {
		switch(value){
			case AlphaFunction::Never:return "Never";
			case AlphaFunction::Less:return "Less";
			case AlphaFunction::Equal:return "Equal";
			case AlphaFunction::Lequal:return "Lequal";
			case AlphaFunction::Greater:return "Greater";
			case AlphaFunction::Notequal:return "Notequal";
			case AlphaFunction::Gequal:return "Gequal";
			case AlphaFunction::Always:return "Always";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DepthFunction value) {
		switch(value){
			case DepthFunction::Never:return "Never";
			case DepthFunction::Less:return "Less";
			case DepthFunction::Equal:return "Equal";
			case DepthFunction::Lequal:return "Lequal";
			case DepthFunction::Greater:return "Greater";
			case DepthFunction::Notequal:return "Notequal";
			case DepthFunction::Gequal:return "Gequal";
			case DepthFunction::Always:return "Always";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TriangleFace value) {
		switch(value){
			case TriangleFace::Front:return "Front";
			case TriangleFace::Back:return "Back";
			case TriangleFace::FrontAndBack:return "FrontAndBack";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FeedbackType value) {
		switch(value){
			case FeedbackType::_2D:return "_2D";
			case FeedbackType::_3D:return "_3D";
			case FeedbackType::_3DColor:return "_3DColor";
			case FeedbackType::_3DColorTexture:return "_3DColorTexture";
			case FeedbackType::_4DColorTexture:return "_4DColorTexture";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FeedBackToken value) {
		switch(value){
			case FeedBackToken::PassThroughToken:return "PassThroughToken";
			case FeedBackToken::PointToken:return "PointToken";
			case FeedBackToken::LineToken:return "LineToken";
			case FeedBackToken::PolygonToken:return "PolygonToken";
			case FeedBackToken::BitmapToken:return "BitmapToken";
			case FeedBackToken::DrawPixelToken:return "DrawPixelToken";
			case FeedBackToken::CopyPixelToken:return "CopyPixelToken";
			case FeedBackToken::LineResetToken:return "LineResetToken";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogMode value) {
		switch(value){
			case FogMode::Exp:return "Exp";
			case FogMode::Exp2:return "Exp2";
			case FogMode::Linear:return "Linear";
			case FogMode::FogFuncSgis:return "FogFuncSgis";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FrontFaceDirection value) {
		switch(value){
			case FrontFaceDirection::Cw:return "Cw";
			case FrontFaceDirection::Ccw:return "Ccw";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapQuery value) {
		switch(value){
			case MapQuery::Coeff:return "Coeff";
			case MapQuery::Order:return "Order";
			case MapQuery::Domain:return "Domain";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetMapQuery value) {
		switch(value){
			case GetMapQuery::Coeff:return "Coeff";
			case GetMapQuery::Order:return "Order";
			case GetMapQuery::Domain:return "Domain";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetPName value) {
		switch(value){
			case GetPName::CurrentColor:return "CurrentColor";
			case GetPName::CurrentIndex:return "CurrentIndex";
			case GetPName::CurrentNormal:return "CurrentNormal";
			case GetPName::CurrentTextureCoords:return "CurrentTextureCoords";
			case GetPName::CurrentRasterColor:return "CurrentRasterColor";
			case GetPName::CurrentRasterIndex:return "CurrentRasterIndex";
			case GetPName::CurrentRasterTextureCoords:return "CurrentRasterTextureCoords";
			case GetPName::CurrentRasterPosition:return "CurrentRasterPosition";
			case GetPName::CurrentRasterPositionValid:return "CurrentRasterPositionValid";
			case GetPName::CurrentRasterDistance:return "CurrentRasterDistance";
			case GetPName::PointSmooth:return "PointSmooth";
			case GetPName::PointSize:return "PointSize";
			case GetPName::PointSizeRange:return "PointSizeRange";
			case GetPName::PointSizeGranularity:return "PointSizeGranularity";
			case GetPName::LineSmooth:return "LineSmooth";
			case GetPName::LineWidth:return "LineWidth";
			case GetPName::LineWidthRange:return "LineWidthRange";
			case GetPName::LineWidthGranularity:return "LineWidthGranularity";
			case GetPName::LineStipple:return "LineStipple";
			case GetPName::LineStipplePattern:return "LineStipplePattern";
			case GetPName::LineStippleRepeat:return "LineStippleRepeat";
			case GetPName::ListMode:return "ListMode";
			case GetPName::MaxListNesting:return "MaxListNesting";
			case GetPName::ListBase:return "ListBase";
			case GetPName::ListIndex:return "ListIndex";
			case GetPName::PolygonMode:return "PolygonMode";
			case GetPName::PolygonSmooth:return "PolygonSmooth";
			case GetPName::PolygonStipple:return "PolygonStipple";
			case GetPName::EdgeFlag:return "EdgeFlag";
			case GetPName::CullFace:return "CullFace";
			case GetPName::CullFaceMode:return "CullFaceMode";
			case GetPName::FrontFace:return "FrontFace";
			case GetPName::Lighting:return "Lighting";
			case GetPName::LightModelLocalViewer:return "LightModelLocalViewer";
			case GetPName::LightModelTwoSide:return "LightModelTwoSide";
			case GetPName::LightModelAmbient:return "LightModelAmbient";
			case GetPName::ShadeModel:return "ShadeModel";
			case GetPName::ColorMaterialFace:return "ColorMaterialFace";
			case GetPName::ColorMaterialParameter:return "ColorMaterialParameter";
			case GetPName::ColorMaterial:return "ColorMaterial";
			case GetPName::Fog:return "Fog";
			case GetPName::FogIndex:return "FogIndex";
			case GetPName::FogDensity:return "FogDensity";
			case GetPName::FogStart:return "FogStart";
			case GetPName::FogEnd:return "FogEnd";
			case GetPName::FogMode:return "FogMode";
			case GetPName::FogColor:return "FogColor";
			case GetPName::DepthRange:return "DepthRange";
			case GetPName::DepthTest:return "DepthTest";
			case GetPName::DepthWritemask:return "DepthWritemask";
			case GetPName::DepthClearValue:return "DepthClearValue";
			case GetPName::DepthFunc:return "DepthFunc";
			case GetPName::AccumClearValue:return "AccumClearValue";
			case GetPName::StencilTest:return "StencilTest";
			case GetPName::StencilClearValue:return "StencilClearValue";
			case GetPName::StencilFunc:return "StencilFunc";
			case GetPName::StencilValueMask:return "StencilValueMask";
			case GetPName::StencilFail:return "StencilFail";
			case GetPName::StencilPassDepthFail:return "StencilPassDepthFail";
			case GetPName::StencilPassDepthPass:return "StencilPassDepthPass";
			case GetPName::StencilRef:return "StencilRef";
			case GetPName::StencilWritemask:return "StencilWritemask";
			case GetPName::MatrixMode:return "MatrixMode";
			case GetPName::Normalize:return "Normalize";
			case GetPName::Viewport:return "Viewport";
			case GetPName::ModelViewStackDepth:return "ModelViewStackDepth";
			case GetPName::ProjectionStackDepth:return "ProjectionStackDepth";
			case GetPName::TextureStackDepth:return "TextureStackDepth";
			case GetPName::ModelViewMatrix:return "ModelViewMatrix";
			case GetPName::ProjectionMatrix:return "ProjectionMatrix";
			case GetPName::TextureMatrix:return "TextureMatrix";
			case GetPName::AttribStackDepth:return "AttribStackDepth";
			case GetPName::ClientAttribStackDepth:return "ClientAttribStackDepth";
			case GetPName::AlphaTest:return "AlphaTest";
			case GetPName::AlphaTestFunc:return "AlphaTestFunc";
			case GetPName::AlphaTestRef:return "AlphaTestRef";
			case GetPName::Dither:return "Dither";
			case GetPName::BlendDst:return "BlendDst";
			case GetPName::BlendSrc:return "BlendSrc";
			case GetPName::Blend:return "Blend";
			case GetPName::LogicOpMode:return "LogicOpMode";
			case GetPName::IndexLogicOp:return "IndexLogicOp";
			case GetPName::ColorLogicOp:return "ColorLogicOp";
			case GetPName::AuxBuffers:return "AuxBuffers";
			case GetPName::DrawBuffer:return "DrawBuffer";
			case GetPName::ReadBuffer:return "ReadBuffer";
			case GetPName::ScissorBox:return "ScissorBox";
			case GetPName::ScissorTest:return "ScissorTest";
			case GetPName::IndexClearValue:return "IndexClearValue";
			case GetPName::IndexWritemask:return "IndexWritemask";
			case GetPName::ColorClearValue:return "ColorClearValue";
			case GetPName::ColorWritemask:return "ColorWritemask";
			case GetPName::IndexMode:return "IndexMode";
			case GetPName::RgbaMode:return "RgbaMode";
			case GetPName::Doublebuffer:return "Doublebuffer";
			case GetPName::Stereo:return "Stereo";
			case GetPName::RenderMode:return "RenderMode";
			case GetPName::PerspectiveCorrectionHint:return "PerspectiveCorrectionHint";
			case GetPName::PointSmoothHint:return "PointSmoothHint";
			case GetPName::LineSmoothHint:return "LineSmoothHint";
			case GetPName::PolygonSmoothHint:return "PolygonSmoothHint";
			case GetPName::FogHint:return "FogHint";
			case GetPName::TextureGenS:return "TextureGenS";
			case GetPName::TextureGenT:return "TextureGenT";
			case GetPName::TextureGenR:return "TextureGenR";
			case GetPName::TextureGenQ:return "TextureGenQ";
			case GetPName::PixelMapIToISize:return "PixelMapIToISize";
			case GetPName::PixelMapSToSSize:return "PixelMapSToSSize";
			case GetPName::PixelMapIToRSize:return "PixelMapIToRSize";
			case GetPName::PixelMapIToGSize:return "PixelMapIToGSize";
			case GetPName::PixelMapIToBSize:return "PixelMapIToBSize";
			case GetPName::PixelMapIToASize:return "PixelMapIToASize";
			case GetPName::PixelMapRToRSize:return "PixelMapRToRSize";
			case GetPName::PixelMapGToGSize:return "PixelMapGToGSize";
			case GetPName::PixelMapBToBSize:return "PixelMapBToBSize";
			case GetPName::PixelMapAToASize:return "PixelMapAToASize";
			case GetPName::UnpackSwapBytes:return "UnpackSwapBytes";
			case GetPName::UnpackLsbFirst:return "UnpackLsbFirst";
			case GetPName::UnpackRowLength:return "UnpackRowLength";
			case GetPName::UnpackSkipRows:return "UnpackSkipRows";
			case GetPName::UnpackSkipPixels:return "UnpackSkipPixels";
			case GetPName::UnpackAlignment:return "UnpackAlignment";
			case GetPName::PackSwapBytes:return "PackSwapBytes";
			case GetPName::PackLsbFirst:return "PackLsbFirst";
			case GetPName::PackRowLength:return "PackRowLength";
			case GetPName::PackSkipRows:return "PackSkipRows";
			case GetPName::PackSkipPixels:return "PackSkipPixels";
			case GetPName::PackAlignment:return "PackAlignment";
			case GetPName::MapColor:return "MapColor";
			case GetPName::MapStencil:return "MapStencil";
			case GetPName::IndexShift:return "IndexShift";
			case GetPName::IndexOffset:return "IndexOffset";
			case GetPName::RedScale:return "RedScale";
			case GetPName::RedBias:return "RedBias";
			case GetPName::ZoomX:return "ZoomX";
			case GetPName::ZoomY:return "ZoomY";
			case GetPName::GreenScale:return "GreenScale";
			case GetPName::GreenBias:return "GreenBias";
			case GetPName::BlueScale:return "BlueScale";
			case GetPName::BlueBias:return "BlueBias";
			case GetPName::AlphaScale:return "AlphaScale";
			case GetPName::AlphaBias:return "AlphaBias";
			case GetPName::DepthScale:return "DepthScale";
			case GetPName::DepthBias:return "DepthBias";
			case GetPName::MaxEvalOrder:return "MaxEvalOrder";
			case GetPName::MaxLights:return "MaxLights";
			case GetPName::MaxClipPlanes:return "MaxClipPlanes";
			case GetPName::MaxTextureSize:return "MaxTextureSize";
			case GetPName::MaxPixelMapTable:return "MaxPixelMapTable";
			case GetPName::MaxAttribStackDepth:return "MaxAttribStackDepth";
			case GetPName::MaxModelViewStackDepth:return "MaxModelViewStackDepth";
			case GetPName::MaxNameStackDepth:return "MaxNameStackDepth";
			case GetPName::MaxProjectionStackDepth:return "MaxProjectionStackDepth";
			case GetPName::MaxTextureStackDepth:return "MaxTextureStackDepth";
			case GetPName::MaxViewportDims:return "MaxViewportDims";
			case GetPName::MaxClientAttribStackDepth:return "MaxClientAttribStackDepth";
			case GetPName::SubpixelBits:return "SubpixelBits";
			case GetPName::IndexBits:return "IndexBits";
			case GetPName::RedBits:return "RedBits";
			case GetPName::GreenBits:return "GreenBits";
			case GetPName::BlueBits:return "BlueBits";
			case GetPName::AlphaBits:return "AlphaBits";
			case GetPName::DepthBits:return "DepthBits";
			case GetPName::StencilBits:return "StencilBits";
			case GetPName::AccumRedBits:return "AccumRedBits";
			case GetPName::AccumGreenBits:return "AccumGreenBits";
			case GetPName::AccumBlueBits:return "AccumBlueBits";
			case GetPName::AccumAlphaBits:return "AccumAlphaBits";
			case GetPName::NameStackDepth:return "NameStackDepth";
			case GetPName::AutoNormal:return "AutoNormal";
			case GetPName::Map1Color4:return "Map1Color4";
			case GetPName::Map1Index:return "Map1Index";
			case GetPName::Map1Normal:return "Map1Normal";
			case GetPName::Map1TextureCoord1:return "Map1TextureCoord1";
			case GetPName::Map1TextureCoord2:return "Map1TextureCoord2";
			case GetPName::Map1TextureCoord3:return "Map1TextureCoord3";
			case GetPName::Map1TextureCoord4:return "Map1TextureCoord4";
			case GetPName::Map1Vertex3:return "Map1Vertex3";
			case GetPName::Map1Vertex4:return "Map1Vertex4";
			case GetPName::Map2Color4:return "Map2Color4";
			case GetPName::Map2Index:return "Map2Index";
			case GetPName::Map2Normal:return "Map2Normal";
			case GetPName::Map2TextureCoord1:return "Map2TextureCoord1";
			case GetPName::Map2TextureCoord2:return "Map2TextureCoord2";
			case GetPName::Map2TextureCoord3:return "Map2TextureCoord3";
			case GetPName::Map2TextureCoord4:return "Map2TextureCoord4";
			case GetPName::Map2Vertex3:return "Map2Vertex3";
			case GetPName::Map2Vertex4:return "Map2Vertex4";
			case GetPName::Map1GridDomain:return "Map1GridDomain";
			case GetPName::Map1GridSegments:return "Map1GridSegments";
			case GetPName::Map2GridDomain:return "Map2GridDomain";
			case GetPName::Map2GridSegments:return "Map2GridSegments";
			case GetPName::Texture1D:return "Texture1D";
			case GetPName::Texture2D:return "Texture2D";
			case GetPName::FeedbackBufferSize:return "FeedbackBufferSize";
			case GetPName::FeedbackBufferType:return "FeedbackBufferType";
			case GetPName::SelectionBufferSize:return "SelectionBufferSize";
			case GetPName::PolygonOffsetUnits:return "PolygonOffsetUnits";
			case GetPName::PolygonOffsetPoint:return "PolygonOffsetPoint";
			case GetPName::PolygonOffsetLine:return "PolygonOffsetLine";
			case GetPName::ClipPlane0:return "ClipPlane0";
			case GetPName::ClipPlane1:return "ClipPlane1";
			case GetPName::ClipPlane2:return "ClipPlane2";
			case GetPName::ClipPlane3:return "ClipPlane3";
			case GetPName::ClipPlane4:return "ClipPlane4";
			case GetPName::ClipPlane5:return "ClipPlane5";
			case GetPName::Light0:return "Light0";
			case GetPName::Light1:return "Light1";
			case GetPName::Light2:return "Light2";
			case GetPName::Light3:return "Light3";
			case GetPName::Light4:return "Light4";
			case GetPName::Light5:return "Light5";
			case GetPName::Light6:return "Light6";
			case GetPName::Light7:return "Light7";
			case GetPName::BlendColor:return "BlendColor";
			case GetPName::BlendEquation:return "BlendEquation";
			case GetPName::PackCmykHintExt:return "PackCmykHintExt";
			case GetPName::UnpackCmykHintExt:return "UnpackCmykHintExt";
			case GetPName::Convolution1DExt:return "Convolution1DExt";
			case GetPName::Convolution2DExt:return "Convolution2DExt";
			case GetPName::Separable2DExt:return "Separable2DExt";
			case GetPName::PostConvolutionRedScaleExt:return "PostConvolutionRedScaleExt";
			case GetPName::PostConvolutionGreenScaleExt:return "PostConvolutionGreenScaleExt";
			case GetPName::PostConvolutionBlueScaleExt:return "PostConvolutionBlueScaleExt";
			case GetPName::PostConvolutionAlphaScaleExt:return "PostConvolutionAlphaScaleExt";
			case GetPName::PostConvolutionRedBiasExt:return "PostConvolutionRedBiasExt";
			case GetPName::PostConvolutionGreenBiasExt:return "PostConvolutionGreenBiasExt";
			case GetPName::PostConvolutionBlueBiasExt:return "PostConvolutionBlueBiasExt";
			case GetPName::PostConvolutionAlphaBiasExt:return "PostConvolutionAlphaBiasExt";
			case GetPName::HistogramExt:return "HistogramExt";
			case GetPName::MinmaxExt:return "MinmaxExt";
			case GetPName::PolygonOffsetFill:return "PolygonOffsetFill";
			case GetPName::PolygonOffsetFactor:return "PolygonOffsetFactor";
			case GetPName::PolygonOffsetBiasExt:return "PolygonOffsetBiasExt";
			case GetPName::RescaleNormalExt:return "RescaleNormalExt";
			case GetPName::TextureBinding1D:return "TextureBinding1D";
			case GetPName::TextureBinding2D:return "TextureBinding2D";
			case GetPName::Texture3DBindingExt:return "Texture3DBindingExt";
			case GetPName::PackSkipImages:return "PackSkipImages";
			case GetPName::PackImageHeight:return "PackImageHeight";
			case GetPName::UnpackSkipImages:return "UnpackSkipImages";
			case GetPName::UnpackImageHeight:return "UnpackImageHeight";
			case GetPName::Texture3DExt:return "Texture3DExt";
			case GetPName::Max3DTextureSize:return "Max3DTextureSize";
			case GetPName::VertexArray:return "VertexArray";
			case GetPName::NormalArray:return "NormalArray";
			case GetPName::ColorArray:return "ColorArray";
			case GetPName::IndexArray:return "IndexArray";
			case GetPName::TextureCoordArray:return "TextureCoordArray";
			case GetPName::EdgeFlagArray:return "EdgeFlagArray";
			case GetPName::VertexArraySize:return "VertexArraySize";
			case GetPName::VertexArrayType:return "VertexArrayType";
			case GetPName::VertexArrayStride:return "VertexArrayStride";
			case GetPName::VertexArrayCountExt:return "VertexArrayCountExt";
			case GetPName::NormalArrayType:return "NormalArrayType";
			case GetPName::NormalArrayStride:return "NormalArrayStride";
			case GetPName::NormalArrayCountExt:return "NormalArrayCountExt";
			case GetPName::ColorArraySize:return "ColorArraySize";
			case GetPName::ColorArrayType:return "ColorArrayType";
			case GetPName::ColorArrayStride:return "ColorArrayStride";
			case GetPName::ColorArrayCountExt:return "ColorArrayCountExt";
			case GetPName::IndexArrayType:return "IndexArrayType";
			case GetPName::IndexArrayStride:return "IndexArrayStride";
			case GetPName::IndexArrayCountExt:return "IndexArrayCountExt";
			case GetPName::TextureCoordArraySize:return "TextureCoordArraySize";
			case GetPName::TextureCoordArrayType:return "TextureCoordArrayType";
			case GetPName::TextureCoordArrayStride:return "TextureCoordArrayStride";
			case GetPName::TextureCoordArrayCountExt:return "TextureCoordArrayCountExt";
			case GetPName::EdgeFlagArrayStride:return "EdgeFlagArrayStride";
			case GetPName::EdgeFlagArrayCountExt:return "EdgeFlagArrayCountExt";
			case GetPName::InterlaceSgix:return "InterlaceSgix";
			case GetPName::DetailTexture2DBindingSgis:return "DetailTexture2DBindingSgis";
			case GetPName::MultisampleSgis:return "MultisampleSgis";
			case GetPName::SampleAlphaToMaskSgis:return "SampleAlphaToMaskSgis";
			case GetPName::SampleAlphaToOneSgis:return "SampleAlphaToOneSgis";
			case GetPName::SampleMaskSgis:return "SampleMaskSgis";
			case GetPName::SampleBuffers:return "SampleBuffers";
			case GetPName::Samples:return "Samples";
			case GetPName::SampleCoverageValue:return "SampleCoverageValue";
			case GetPName::SampleCoverageInvert:return "SampleCoverageInvert";
			case GetPName::SamplePatternSgis:return "SamplePatternSgis";
			case GetPName::ColorMatrixSgi:return "ColorMatrixSgi";
			case GetPName::ColorMatrixStackDepthSgi:return "ColorMatrixStackDepthSgi";
			case GetPName::MaxColorMatrixStackDepthSgi:return "MaxColorMatrixStackDepthSgi";
			case GetPName::PostColorMatrixRedScaleSgi:return "PostColorMatrixRedScaleSgi";
			case GetPName::PostColorMatrixGreenScaleSgi:return "PostColorMatrixGreenScaleSgi";
			case GetPName::PostColorMatrixBlueScaleSgi:return "PostColorMatrixBlueScaleSgi";
			case GetPName::PostColorMatrixAlphaScaleSgi:return "PostColorMatrixAlphaScaleSgi";
			case GetPName::PostColorMatrixRedBiasSgi:return "PostColorMatrixRedBiasSgi";
			case GetPName::PostColorMatrixGreenBiasSgi:return "PostColorMatrixGreenBiasSgi";
			case GetPName::PostColorMatrixBlueBiasSgi:return "PostColorMatrixBlueBiasSgi";
			case GetPName::PostColorMatrixAlphaBiasSgi:return "PostColorMatrixAlphaBiasSgi";
			case GetPName::TextureColorTableSgi:return "TextureColorTableSgi";
			case GetPName::BlendDstRgb:return "BlendDstRgb";
			case GetPName::BlendSrcRgb:return "BlendSrcRgb";
			case GetPName::BlendDstAlpha:return "BlendDstAlpha";
			case GetPName::BlendSrcAlpha:return "BlendSrcAlpha";
			case GetPName::ColorTableSgi:return "ColorTableSgi";
			case GetPName::PostConvolutionColorTableSgi:return "PostConvolutionColorTableSgi";
			case GetPName::PostColorMatrixColorTableSgi:return "PostColorMatrixColorTableSgi";
			case GetPName::MaxElementsVertices:return "MaxElementsVertices";
			case GetPName::MaxElementsIndices:return "MaxElementsIndices";
			case GetPName::PointSizeMin:return "PointSizeMin";
			case GetPName::PointSizeMax:return "PointSizeMax";
			case GetPName::PointFadeThresholdSize:return "PointFadeThresholdSize";
			case GetPName::DistanceAttenuationExt:return "DistanceAttenuationExt";
			case GetPName::FogFuncPointsSgis:return "FogFuncPointsSgis";
			case GetPName::MaxFogFuncPointsSgis:return "MaxFogFuncPointsSgis";
			case GetPName::PackSkipVolumesSgis:return "PackSkipVolumesSgis";
			case GetPName::PackImageDepthSgis:return "PackImageDepthSgis";
			case GetPName::UnpackSkipVolumesSgis:return "UnpackSkipVolumesSgis";
			case GetPName::UnpackImageDepthSgis:return "UnpackImageDepthSgis";
			case GetPName::Texture4DSgis:return "Texture4DSgis";
			case GetPName::Max4DTextureSizeSgis:return "Max4DTextureSizeSgis";
			case GetPName::PixelTexGenSgix:return "PixelTexGenSgix";
			case GetPName::PixelTileBestAlignmentSgix:return "PixelTileBestAlignmentSgix";
			case GetPName::PixelTileCacheIncrementSgix:return "PixelTileCacheIncrementSgix";
			case GetPName::PixelTileWidthSgix:return "PixelTileWidthSgix";
			case GetPName::PixelTileHeightSgix:return "PixelTileHeightSgix";
			case GetPName::PixelTileGridWidthSgix:return "PixelTileGridWidthSgix";
			case GetPName::PixelTileGridHeightSgix:return "PixelTileGridHeightSgix";
			case GetPName::PixelTileGridDepthSgix:return "PixelTileGridDepthSgix";
			case GetPName::PixelTileCacheSizeSgix:return "PixelTileCacheSizeSgix";
			case GetPName::SpriteSgix:return "SpriteSgix";
			case GetPName::SpriteModeSgix:return "SpriteModeSgix";
			case GetPName::SpriteAxisSgix:return "SpriteAxisSgix";
			case GetPName::SpriteTranslationSgix:return "SpriteTranslationSgix";
			case GetPName::Texture4DBindingSgis:return "Texture4DBindingSgis";
			case GetPName::MaxClipmapDepthSgix:return "MaxClipmapDepthSgix";
			case GetPName::MaxClipmapVirtualDepthSgix:return "MaxClipmapVirtualDepthSgix";
			case GetPName::PostTextureFilterBiasRangeSgix:return "PostTextureFilterBiasRangeSgix";
			case GetPName::PostTextureFilterScaleRangeSgix:return "PostTextureFilterScaleRangeSgix";
			case GetPName::ReferencePlaneSgix:return "ReferencePlaneSgix";
			case GetPName::ReferencePlaneEquationSgix:return "ReferencePlaneEquationSgix";
			case GetPName::IrInstrument1Sgix:return "IrInstrument1Sgix";
			case GetPName::InstrumentMeasurementsSgix:return "InstrumentMeasurementsSgix";
			case GetPName::CalligraphicFragmentSgix:return "CalligraphicFragmentSgix";
			case GetPName::FramezoomSgix:return "FramezoomSgix";
			case GetPName::FramezoomFactorSgix:return "FramezoomFactorSgix";
			case GetPName::MaxFramezoomFactorSgix:return "MaxFramezoomFactorSgix";
			case GetPName::GenerateMipmapHintSgis:return "GenerateMipmapHintSgis";
			case GetPName::DeformationsMaskSgix:return "DeformationsMaskSgix";
			case GetPName::FogOffsetSgix:return "FogOffsetSgix";
			case GetPName::FogOffsetValueSgix:return "FogOffsetValueSgix";
			case GetPName::LightModelColorControl:return "LightModelColorControl";
			case GetPName::SharedTexturePaletteExt:return "SharedTexturePaletteExt";
			case GetPName::MajorVersion:return "MajorVersion";
			case GetPName::MinorVersion:return "MinorVersion";
			case GetPName::NumExtensions:return "NumExtensions";
			case GetPName::ContextFlags:return "ContextFlags";
			case GetPName::ProgramPipelineBinding:return "ProgramPipelineBinding";
			case GetPName::MaxViewports:return "MaxViewports";
			case GetPName::ViewportSubpixelBits:return "ViewportSubpixelBits";
			case GetPName::ViewportBoundsRange:return "ViewportBoundsRange";
			case GetPName::LayerProvokingVertex:return "LayerProvokingVertex";
			case GetPName::ViewportIndexProvokingVertex:return "ViewportIndexProvokingVertex";
			case GetPName::MaxComputeUniformComponents:return "MaxComputeUniformComponents";
			case GetPName::MaxComputeAtomicCounterBuffers:return "MaxComputeAtomicCounterBuffers";
			case GetPName::MaxComputeAtomicCounters:return "MaxComputeAtomicCounters";
			case GetPName::MaxCombinedComputeUniformComponents:return "MaxCombinedComputeUniformComponents";
			case GetPName::MaxDebugGroupStackDepth:return "MaxDebugGroupStackDepth";
			case GetPName::DebugGroupStackDepth:return "DebugGroupStackDepth";
			case GetPName::MaxUniformLocations:return "MaxUniformLocations";
			case GetPName::VertexBindingDivisor:return "VertexBindingDivisor";
			case GetPName::VertexBindingOffset:return "VertexBindingOffset";
			case GetPName::VertexBindingStride:return "VertexBindingStride";
			case GetPName::MaxVertexAttribRelativeOffset:return "MaxVertexAttribRelativeOffset";
			case GetPName::MaxVertexAttribBindings:return "MaxVertexAttribBindings";
			case GetPName::MaxLabelLength:return "MaxLabelLength";
			case GetPName::ConvolutionHintSgix:return "ConvolutionHintSgix";
			case GetPName::AsyncMarkerSgix:return "AsyncMarkerSgix";
			case GetPName::PixelTexGenModeSgix:return "PixelTexGenModeSgix";
			case GetPName::AsyncHistogramSgix:return "AsyncHistogramSgix";
			case GetPName::MaxAsyncHistogramSgix:return "MaxAsyncHistogramSgix";
			case GetPName::PixelTextureSgis:return "PixelTextureSgis";
			case GetPName::AsyncTexImageSgix:return "AsyncTexImageSgix";
			case GetPName::AsyncDrawPixelsSgix:return "AsyncDrawPixelsSgix";
			case GetPName::AsyncReadPixelsSgix:return "AsyncReadPixelsSgix";
			case GetPName::MaxAsyncTexImageSgix:return "MaxAsyncTexImageSgix";
			case GetPName::MaxAsyncDrawPixelsSgix:return "MaxAsyncDrawPixelsSgix";
			case GetPName::MaxAsyncReadPixelsSgix:return "MaxAsyncReadPixelsSgix";
			case GetPName::VertexPreclipSgix:return "VertexPreclipSgix";
			case GetPName::VertexPreclipHintSgix:return "VertexPreclipHintSgix";
			case GetPName::FragmentLightingSgix:return "FragmentLightingSgix";
			case GetPName::FragmentColorMaterialSgix:return "FragmentColorMaterialSgix";
			case GetPName::FragmentColorMaterialFaceSgix:return "FragmentColorMaterialFaceSgix";
			case GetPName::FragmentColorMaterialParameterSgix:return "FragmentColorMaterialParameterSgix";
			case GetPName::MaxFragmentLightsSgix:return "MaxFragmentLightsSgix";
			case GetPName::MaxActiveLightsSgix:return "MaxActiveLightsSgix";
			case GetPName::LightEnvModeSgix:return "LightEnvModeSgix";
			case GetPName::FragmentLightModelLocalViewerSgix:return "FragmentLightModelLocalViewerSgix";
			case GetPName::FragmentLightModelTwoSideSgix:return "FragmentLightModelTwoSideSgix";
			case GetPName::FragmentLightModelAmbientSgix:return "FragmentLightModelAmbientSgix";
			case GetPName::FragmentLightModelNormalInterpolationSgix:return "FragmentLightModelNormalInterpolationSgix";
			case GetPName::FragmentLight0Sgix:return "FragmentLight0Sgix";
			case GetPName::/* Formerly 0x842C in SGI specfile */ PackResampleSgix:return "/* Formerly 0x842C in SGI specfile */ PackResampleSgix";
			case GetPName::/* Formerly 0x842D in SGI specfile */ UnpackResampleSgix:return "/* Formerly 0x842D in SGI specfile */ UnpackResampleSgix";
			case GetPName::AliasedPointSizeRange:return "AliasedPointSizeRange";
			case GetPName::AliasedLineWidthRange:return "AliasedLineWidthRange";
			case GetPName::ActiveTexture:return "ActiveTexture";
			case GetPName::MaxRenderbufferSize:return "MaxRenderbufferSize";
			case GetPName::TextureCompressionHint:return "TextureCompressionHint";
			case GetPName::TextureBindingRectangle:return "TextureBindingRectangle";
			case GetPName::MaxRectangleTextureSize:return "MaxRectangleTextureSize";
			case GetPName::MaxTextureLodBias:return "MaxTextureLodBias";
			case GetPName::TextureBindingCubeMap:return "TextureBindingCubeMap";
			case GetPName::MaxCubeMapTextureSize:return "MaxCubeMapTextureSize";
			case GetPName::PackSubsampleRateSgix:return "PackSubsampleRateSgix";
			case GetPName::UnpackSubsampleRateSgix:return "UnpackSubsampleRateSgix";
			case GetPName::VertexArrayBinding:return "VertexArrayBinding";
			case GetPName::ProgramPointSize:return "ProgramPointSize";
			case GetPName::NumCompressedTextureFormats:return "NumCompressedTextureFormats";
			case GetPName::CompressedTextureFormats:return "CompressedTextureFormats";
			case GetPName::NumProgramBinaryFormats:return "NumProgramBinaryFormats";
			case GetPName::ProgramBinaryFormats:return "ProgramBinaryFormats";
			case GetPName::StencilBackFunc:return "StencilBackFunc";
			case GetPName::StencilBackFail:return "StencilBackFail";
			case GetPName::StencilBackPassDepthFail:return "StencilBackPassDepthFail";
			case GetPName::StencilBackPassDepthPass:return "StencilBackPassDepthPass";
			case GetPName::MaxDrawBuffers:return "MaxDrawBuffers";
			case GetPName::BlendEquationAlpha:return "BlendEquationAlpha";
			case GetPName::MaxVertexAttribs:return "MaxVertexAttribs";
			case GetPName::MaxTextureImageUnits:return "MaxTextureImageUnits";
			case GetPName::ArrayBufferBinding:return "ArrayBufferBinding";
			case GetPName::ElementArrayBufferBinding:return "ElementArrayBufferBinding";
			case GetPName::PixelPackBufferBinding:return "PixelPackBufferBinding";
			case GetPName::PixelUnpackBufferBinding:return "PixelUnpackBufferBinding";
			case GetPName::MaxDualSourceDrawBuffers:return "MaxDualSourceDrawBuffers";
			case GetPName::MaxArrayTextureLayers:return "MaxArrayTextureLayers";
			case GetPName::MinProgramTexelOffset:return "MinProgramTexelOffset";
			case GetPName::MaxProgramTexelOffset:return "MaxProgramTexelOffset";
			case GetPName::SamplerBinding:return "SamplerBinding";
			case GetPName::FragmentShaderAti:return "FragmentShaderAti";
			case GetPName::UniformBufferBinding:return "UniformBufferBinding";
			case GetPName::UniformBufferStart:return "UniformBufferStart";
			case GetPName::UniformBufferSize:return "UniformBufferSize";
			case GetPName::MaxVertexUniformBlocks:return "MaxVertexUniformBlocks";
			case GetPName::MaxGeometryUniformBlocks:return "MaxGeometryUniformBlocks";
			case GetPName::MaxFragmentUniformBlocks:return "MaxFragmentUniformBlocks";
			case GetPName::MaxCombinedUniformBlocks:return "MaxCombinedUniformBlocks";
			case GetPName::MaxUniformBufferBindings:return "MaxUniformBufferBindings";
			case GetPName::MaxUniformBlockSize:return "MaxUniformBlockSize";
			case GetPName::MaxCombinedVertexUniformComponents:return "MaxCombinedVertexUniformComponents";
			case GetPName::MaxCombinedGeometryUniformComponents:return "MaxCombinedGeometryUniformComponents";
			case GetPName::MaxCombinedFragmentUniformComponents:return "MaxCombinedFragmentUniformComponents";
			case GetPName::UniformBufferOffsetAlignment:return "UniformBufferOffsetAlignment";
			case GetPName::MaxFragmentUniformComponents:return "MaxFragmentUniformComponents";
			case GetPName::MaxVertexUniformComponents:return "MaxVertexUniformComponents";
			case GetPName::MaxVaryingFloats:return "MaxVaryingFloats";
			case GetPName::MaxVertexTextureImageUnits:return "MaxVertexTextureImageUnits";
			case GetPName::MaxCombinedTextureImageUnits:return "MaxCombinedTextureImageUnits";
			case GetPName::FragmentShaderDerivativeHint:return "FragmentShaderDerivativeHint";
			case GetPName::CurrentProgram:return "CurrentProgram";
			case GetPName::ImplementationColorReadType:return "ImplementationColorReadType";
			case GetPName::ImplementationColorReadFormat:return "ImplementationColorReadFormat";
			case GetPName::TextureBinding1DArray:return "TextureBinding1DArray";
			case GetPName::TextureBinding2DArray:return "TextureBinding2DArray";
			case GetPName::MaxGeometryTextureImageUnits:return "MaxGeometryTextureImageUnits";
			case GetPName::MaxTextureBufferSize:return "MaxTextureBufferSize";
			case GetPName::TextureBindingBuffer:return "TextureBindingBuffer";
			case GetPName::TransformFeedbackBufferStart:return "TransformFeedbackBufferStart";
			case GetPName::TransformFeedbackBufferSize:return "TransformFeedbackBufferSize";
			case GetPName::TransformFeedbackBufferBinding:return "TransformFeedbackBufferBinding";
			case GetPName::MotionEstimationSearchBlockXQcom:return "MotionEstimationSearchBlockXQcom";
			case GetPName::MotionEstimationSearchBlockYQcom:return "MotionEstimationSearchBlockYQcom";
			case GetPName::StencilBackRef:return "StencilBackRef";
			case GetPName::StencilBackValueMask:return "StencilBackValueMask";
			case GetPName::StencilBackWritemask:return "StencilBackWritemask";
			case GetPName::DrawFramebufferBinding:return "DrawFramebufferBinding";
			case GetPName::RenderbufferBinding:return "RenderbufferBinding";
			case GetPName::ReadFramebufferBinding:return "ReadFramebufferBinding";
			case GetPName::MaxColorAttachments:return "MaxColorAttachments";
			case GetPName::TextureGenStrOes:return "TextureGenStrOes";
			case GetPName::MaxElementIndex:return "MaxElementIndex";
			case GetPName::MaxGeometryUniformComponents:return "MaxGeometryUniformComponents";
			case GetPName::ShaderBinaryFormats:return "ShaderBinaryFormats";
			case GetPName::NumShaderBinaryFormats:return "NumShaderBinaryFormats";
			case GetPName::ShaderCompiler:return "ShaderCompiler";
			case GetPName::MaxVertexUniformVectors:return "MaxVertexUniformVectors";
			case GetPName::MaxVaryingVectors:return "MaxVaryingVectors";
			case GetPName::MaxFragmentUniformVectors:return "MaxFragmentUniformVectors";
			case GetPName::Timestamp:return "Timestamp";
			case GetPName::ProvokingVertex:return "ProvokingVertex";
			case GetPName::MaxSampleMaskWords:return "MaxSampleMaskWords";
			case GetPName::MaxTessControlUniformBlocks:return "MaxTessControlUniformBlocks";
			case GetPName::MaxTessEvaluationUniformBlocks:return "MaxTessEvaluationUniformBlocks";
			case GetPName::FetchPerSampleArm:return "FetchPerSampleArm";
			case GetPName::FragmentShaderFramebufferFetchMrtArm:return "FragmentShaderFramebufferFetchMrtArm";
			case GetPName::FragmentShadingRateNonTrivialCombinersSupportedExt:return "FragmentShadingRateNonTrivialCombinersSupportedExt";
			case GetPName::PrimitiveRestartIndex:return "PrimitiveRestartIndex";
			case GetPName::MinMapBufferAlignment:return "MinMapBufferAlignment";
			case GetPName::ShaderStorageBufferBinding:return "ShaderStorageBufferBinding";
			case GetPName::ShaderStorageBufferStart:return "ShaderStorageBufferStart";
			case GetPName::ShaderStorageBufferSize:return "ShaderStorageBufferSize";
			case GetPName::MaxVertexShaderStorageBlocks:return "MaxVertexShaderStorageBlocks";
			case GetPName::MaxGeometryShaderStorageBlocks:return "MaxGeometryShaderStorageBlocks";
			case GetPName::MaxTessControlShaderStorageBlocks:return "MaxTessControlShaderStorageBlocks";
			case GetPName::MaxTessEvaluationShaderStorageBlocks:return "MaxTessEvaluationShaderStorageBlocks";
			case GetPName::MaxFragmentShaderStorageBlocks:return "MaxFragmentShaderStorageBlocks";
			case GetPName::MaxComputeShaderStorageBlocks:return "MaxComputeShaderStorageBlocks";
			case GetPName::MaxCombinedShaderStorageBlocks:return "MaxCombinedShaderStorageBlocks";
			case GetPName::MaxShaderStorageBufferBindings:return "MaxShaderStorageBufferBindings";
			case GetPName::ShaderStorageBufferOffsetAlignment:return "ShaderStorageBufferOffsetAlignment";
			case GetPName::MaxComputeWorkGroupInvocations:return "MaxComputeWorkGroupInvocations";
			case GetPName::DispatchIndirectBufferBinding:return "DispatchIndirectBufferBinding";
			case GetPName::TextureBinding2DMultisample:return "TextureBinding2DMultisample";
			case GetPName::TextureBinding2DMultisampleArray:return "TextureBinding2DMultisampleArray";
			case GetPName::MaxColorTextureSamples:return "MaxColorTextureSamples";
			case GetPName::MaxDepthTextureSamples:return "MaxDepthTextureSamples";
			case GetPName::MaxIntegerSamples:return "MaxIntegerSamples";
			case GetPName::MaxServerWaitTimeout:return "MaxServerWaitTimeout";
			case GetPName::MaxVertexOutputComponents:return "MaxVertexOutputComponents";
			case GetPName::MaxGeometryInputComponents:return "MaxGeometryInputComponents";
			case GetPName::MaxGeometryOutputComponents:return "MaxGeometryOutputComponents";
			case GetPName::MaxFragmentInputComponents:return "MaxFragmentInputComponents";
			case GetPName::ContextProfileMask:return "ContextProfileMask";
			case GetPName::TextureBufferOffsetAlignment:return "TextureBufferOffsetAlignment";
			case GetPName::MaxComputeUniformBlocks:return "MaxComputeUniformBlocks";
			case GetPName::MaxComputeTextureImageUnits:return "MaxComputeTextureImageUnits";
			case GetPName::MaxComputeWorkGroupCount:return "MaxComputeWorkGroupCount";
			case GetPName::MaxComputeWorkGroupSize:return "MaxComputeWorkGroupSize";
			case GetPName::MaxVertexAtomicCounters:return "MaxVertexAtomicCounters";
			case GetPName::MaxTessControlAtomicCounters:return "MaxTessControlAtomicCounters";
			case GetPName::MaxTessEvaluationAtomicCounters:return "MaxTessEvaluationAtomicCounters";
			case GetPName::MaxGeometryAtomicCounters:return "MaxGeometryAtomicCounters";
			case GetPName::MaxFragmentAtomicCounters:return "MaxFragmentAtomicCounters";
			case GetPName::MaxCombinedAtomicCounters:return "MaxCombinedAtomicCounters";
			case GetPName::MaxFramebufferWidth:return "MaxFramebufferWidth";
			case GetPName::MaxFramebufferHeight:return "MaxFramebufferHeight";
			case GetPName::MaxFramebufferLayers:return "MaxFramebufferLayers";
			case GetPName::MaxFramebufferSamples:return "MaxFramebufferSamples";
			case GetPName::NumDeviceUuidsExt:return "NumDeviceUuidsExt";
			case GetPName::DeviceUuidExt:return "DeviceUuidExt";
			case GetPName::DriverUuidExt:return "DriverUuidExt";
			case GetPName::DeviceLuidExt:return "DeviceLuidExt";
			case GetPName::DeviceNodeMaskExt:return "DeviceNodeMaskExt";
			case GetPName::ShadingRateImagePerPrimitiveNv:return "ShadingRateImagePerPrimitiveNv";
			case GetPName::ShadingRateImagePaletteCountNv:return "ShadingRateImagePaletteCountNv";
			case GetPName::MaxTimelineSemaphoreValueDifferenceNv:return "MaxTimelineSemaphoreValueDifferenceNv";
			case GetPName::FramebufferFetchNoncoherentQcom:return "FramebufferFetchNoncoherentQcom";
			case GetPName::ShadingRateQcom:return "ShadingRateQcom";
			case GetPName::ShadingRateExt:return "ShadingRateExt";
			case GetPName::MinFragmentShadingRateAttachmentTexelWidthExt:return "MinFragmentShadingRateAttachmentTexelWidthExt";
			case GetPName::MaxFragmentShadingRateAttachmentTexelWidthExt:return "MaxFragmentShadingRateAttachmentTexelWidthExt";
			case GetPName::MinFragmentShadingRateAttachmentTexelHeightExt:return "MinFragmentShadingRateAttachmentTexelHeightExt";
			case GetPName::MaxFragmentShadingRateAttachmentTexelHeightExt:return "MaxFragmentShadingRateAttachmentTexelHeightExt";
			case GetPName::MaxFragmentShadingRateAttachmentTexelAspectRatioExt:return "MaxFragmentShadingRateAttachmentTexelAspectRatioExt";
			case GetPName::MaxFragmentShadingRateAttachmentLayersExt:return "MaxFragmentShadingRateAttachmentLayersExt";
			case GetPName::FragmentShadingRateWithShaderDepthStencilWritesSupportedExt:return "FragmentShadingRateWithShaderDepthStencilWritesSupportedExt";
			case GetPName::FragmentShadingRateWithSampleMaskSupportedExt:return "FragmentShadingRateWithSampleMaskSupportedExt";
			case GetPName::FragmentShadingRateAttachmentWithDefaultFramebufferSupportedExt:return "FragmentShadingRateAttachmentWithDefaultFramebufferSupportedExt";
			case GetPName::ShaderCoreCountArm:return "ShaderCoreCountArm";
			case GetPName::ShaderCoreActiveCountArm:return "ShaderCoreActiveCountArm";
			case GetPName::ShaderCorePresentMaskArm:return "ShaderCorePresentMaskArm";
			case GetPName::ShaderCoreMaxWarpCountArm:return "ShaderCoreMaxWarpCountArm";
			case GetPName::ShaderCorePixelRateArm:return "ShaderCorePixelRateArm";
			case GetPName::ShaderCoreTexelRateArm:return "ShaderCoreTexelRateArm";
			case GetPName::ShaderCoreFmaRateArm:return "ShaderCoreFmaRateArm";
			case GetPName::MaxTaskWorkGroupTotalCountExt:return "MaxTaskWorkGroupTotalCountExt";
			case GetPName::MaxMeshWorkGroupTotalCountExt:return "MaxMeshWorkGroupTotalCountExt";
			case GetPName::MaxTaskPayloadSizeExt:return "MaxTaskPayloadSizeExt";
			case GetPName::MaxTaskSharedMemorySizeExt:return "MaxTaskSharedMemorySizeExt";
			case GetPName::MaxMeshSharedMemorySizeExt:return "MaxMeshSharedMemorySizeExt";
			case GetPName::MaxTaskPayloadAndSharedMemorySizeExt:return "MaxTaskPayloadAndSharedMemorySizeExt";
			case GetPName::MaxMeshPayloadAndSharedMemorySizeExt:return "MaxMeshPayloadAndSharedMemorySizeExt";
			case GetPName::MaxMeshOutputMemorySizeExt:return "MaxMeshOutputMemorySizeExt";
			case GetPName::MaxMeshPayloadAndOutputMemorySizeExt:return "MaxMeshPayloadAndOutputMemorySizeExt";
			case GetPName::MaxMeshOutputComponentsExt:return "MaxMeshOutputComponentsExt";
			case GetPName::MaxMeshOutputLayersExt:return "MaxMeshOutputLayersExt";
			case GetPName::MaxPreferredTaskWorkGroupInvocationsExt:return "MaxPreferredTaskWorkGroupInvocationsExt";
			case GetPName::MaxPreferredMeshWorkGroupInvocationsExt:return "MaxPreferredMeshWorkGroupInvocationsExt";
			case GetPName::MeshPrefersLocalInvocationVertexOutputExt:return "MeshPrefersLocalInvocationVertexOutputExt";
			case GetPName::MeshPrefersLocalInvocationPrimitiveOutputExt:return "MeshPrefersLocalInvocationPrimitiveOutputExt";
			case GetPName::MeshPrefersCompactVertexOutputExt:return "MeshPrefersCompactVertexOutputExt";
			case GetPName::MeshPrefersCompactPrimitiveOutputExt:return "MeshPrefersCompactPrimitiveOutputExt";
			case GetPName::MaxTaskWorkGroupCountExt:return "MaxTaskWorkGroupCountExt";
			case GetPName::MaxMeshWorkGroupCountExt:return "MaxMeshWorkGroupCountExt";
			case GetPName::MaxMeshOutputPrimitivesExt:return "MaxMeshOutputPrimitivesExt";
			case GetPName::MaxMeshWorkGroupInvocationsExt:return "MaxMeshWorkGroupInvocationsExt";
			case GetPName::MaxMeshWorkGroupSizeExt:return "MaxMeshWorkGroupSizeExt";
			case GetPName::MaxTaskWorkGroupInvocationsExt:return "MaxTaskWorkGroupInvocationsExt";
			case GetPName::MaxTaskWorkGroupSizeExt:return "MaxTaskWorkGroupSizeExt";
			case GetPName::FragmentShadingRatePrimitiveRateWithMultiViewportSupportedExt:return "FragmentShadingRatePrimitiveRateWithMultiViewportSupportedExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderTextureUnitParameter value) {
		switch(value){
			case VertexShaderTextureUnitParameter::CurrentTextureCoords:return "CurrentTextureCoords";
			case VertexShaderTextureUnitParameter::TextureMatrix:return "TextureMatrix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(EnableCap value) {
		switch(value){
			case EnableCap::PointSmooth:return "PointSmooth";
			case EnableCap::LineSmooth:return "LineSmooth";
			case EnableCap::LineStipple:return "LineStipple";
			case EnableCap::PolygonSmooth:return "PolygonSmooth";
			case EnableCap::PolygonStipple:return "PolygonStipple";
			case EnableCap::CullFace:return "CullFace";
			case EnableCap::Lighting:return "Lighting";
			case EnableCap::ColorMaterial:return "ColorMaterial";
			case EnableCap::Fog:return "Fog";
			case EnableCap::DepthTest:return "DepthTest";
			case EnableCap::StencilTest:return "StencilTest";
			case EnableCap::Normalize:return "Normalize";
			case EnableCap::AlphaTest:return "AlphaTest";
			case EnableCap::Dither:return "Dither";
			case EnableCap::Blend:return "Blend";
			case EnableCap::IndexLogicOp:return "IndexLogicOp";
			case EnableCap::ColorLogicOp:return "ColorLogicOp";
			case EnableCap::ScissorTest:return "ScissorTest";
			case EnableCap::TextureGenS:return "TextureGenS";
			case EnableCap::TextureGenT:return "TextureGenT";
			case EnableCap::TextureGenR:return "TextureGenR";
			case EnableCap::TextureGenQ:return "TextureGenQ";
			case EnableCap::AutoNormal:return "AutoNormal";
			case EnableCap::Map1Color4:return "Map1Color4";
			case EnableCap::Map1Index:return "Map1Index";
			case EnableCap::Map1Normal:return "Map1Normal";
			case EnableCap::Map1TextureCoord1:return "Map1TextureCoord1";
			case EnableCap::Map1TextureCoord2:return "Map1TextureCoord2";
			case EnableCap::Map1TextureCoord3:return "Map1TextureCoord3";
			case EnableCap::Map1TextureCoord4:return "Map1TextureCoord4";
			case EnableCap::Map1Vertex3:return "Map1Vertex3";
			case EnableCap::Map1Vertex4:return "Map1Vertex4";
			case EnableCap::Map2Color4:return "Map2Color4";
			case EnableCap::Map2Index:return "Map2Index";
			case EnableCap::Map2Normal:return "Map2Normal";
			case EnableCap::Map2TextureCoord1:return "Map2TextureCoord1";
			case EnableCap::Map2TextureCoord2:return "Map2TextureCoord2";
			case EnableCap::Map2TextureCoord3:return "Map2TextureCoord3";
			case EnableCap::Map2TextureCoord4:return "Map2TextureCoord4";
			case EnableCap::Map2Vertex3:return "Map2Vertex3";
			case EnableCap::Map2Vertex4:return "Map2Vertex4";
			case EnableCap::Texture1D:return "Texture1D";
			case EnableCap::Texture2D:return "Texture2D";
			case EnableCap::PolygonOffsetPoint:return "PolygonOffsetPoint";
			case EnableCap::PolygonOffsetLine:return "PolygonOffsetLine";
			case EnableCap::ClipPlane0:return "ClipPlane0";
			case EnableCap::ClipPlane1:return "ClipPlane1";
			case EnableCap::ClipPlane2:return "ClipPlane2";
			case EnableCap::ClipPlane3:return "ClipPlane3";
			case EnableCap::ClipPlane4:return "ClipPlane4";
			case EnableCap::ClipPlane5:return "ClipPlane5";
			case EnableCap::ClipDistance6:return "ClipDistance6";
			case EnableCap::ClipDistance7:return "ClipDistance7";
			case EnableCap::Light0:return "Light0";
			case EnableCap::Light1:return "Light1";
			case EnableCap::Light2:return "Light2";
			case EnableCap::Light3:return "Light3";
			case EnableCap::Light4:return "Light4";
			case EnableCap::Light5:return "Light5";
			case EnableCap::Light6:return "Light6";
			case EnableCap::Light7:return "Light7";
			case EnableCap::Convolution1DExt:return "Convolution1DExt";
			case EnableCap::Convolution2DExt:return "Convolution2DExt";
			case EnableCap::Separable2DExt:return "Separable2DExt";
			case EnableCap::HistogramExt:return "HistogramExt";
			case EnableCap::MinmaxExt:return "MinmaxExt";
			case EnableCap::PolygonOffsetFill:return "PolygonOffsetFill";
			case EnableCap::RescaleNormalExt:return "RescaleNormalExt";
			case EnableCap::Texture3DExt:return "Texture3DExt";
			case EnableCap::VertexArray:return "VertexArray";
			case EnableCap::NormalArray:return "NormalArray";
			case EnableCap::ColorArray:return "ColorArray";
			case EnableCap::IndexArray:return "IndexArray";
			case EnableCap::TextureCoordArray:return "TextureCoordArray";
			case EnableCap::EdgeFlagArray:return "EdgeFlagArray";
			case EnableCap::InterlaceSgix:return "InterlaceSgix";
			case EnableCap::Multisample:return "Multisample";
			case EnableCap::SampleAlphaToCoverage:return "SampleAlphaToCoverage";
			case EnableCap::SampleAlphaToOne:return "SampleAlphaToOne";
			case EnableCap::SampleCoverage:return "SampleCoverage";
			case EnableCap::TextureColorTableSgi:return "TextureColorTableSgi";
			case EnableCap::ColorTable:return "ColorTable";
			case EnableCap::PostConvolutionColorTable:return "PostConvolutionColorTable";
			case EnableCap::PostColorMatrixColorTable:return "PostColorMatrixColorTable";
			case EnableCap::Texture4DSgis:return "Texture4DSgis";
			case EnableCap::PixelTexGenSgix:return "PixelTexGenSgix";
			case EnableCap::SpriteSgix:return "SpriteSgix";
			case EnableCap::ReferencePlaneSgix:return "ReferencePlaneSgix";
			case EnableCap::IrInstrument1Sgix:return "IrInstrument1Sgix";
			case EnableCap::CalligraphicFragmentSgix:return "CalligraphicFragmentSgix";
			case EnableCap::FramezoomSgix:return "FramezoomSgix";
			case EnableCap::FogOffsetSgix:return "FogOffsetSgix";
			case EnableCap::SharedTexturePaletteExt:return "SharedTexturePaletteExt";
			case EnableCap::DebugOutputSynchronous:return "DebugOutputSynchronous";
			case EnableCap::AsyncHistogramSgix:return "AsyncHistogramSgix";
			case EnableCap::PixelTextureSgis:return "PixelTextureSgis";
			case EnableCap::AsyncTexImageSgix:return "AsyncTexImageSgix";
			case EnableCap::AsyncDrawPixelsSgix:return "AsyncDrawPixelsSgix";
			case EnableCap::AsyncReadPixelsSgix:return "AsyncReadPixelsSgix";
			case EnableCap::FragmentLightingSgix:return "FragmentLightingSgix";
			case EnableCap::FragmentColorMaterialSgix:return "FragmentColorMaterialSgix";
			case EnableCap::FragmentLight0Sgix:return "FragmentLight0Sgix";
			case EnableCap::FragmentLight1Sgix:return "FragmentLight1Sgix";
			case EnableCap::FragmentLight2Sgix:return "FragmentLight2Sgix";
			case EnableCap::FragmentLight3Sgix:return "FragmentLight3Sgix";
			case EnableCap::FragmentLight4Sgix:return "FragmentLight4Sgix";
			case EnableCap::FragmentLight5Sgix:return "FragmentLight5Sgix";
			case EnableCap::FragmentLight6Sgix:return "FragmentLight6Sgix";
			case EnableCap::FragmentLight7Sgix:return "FragmentLight7Sgix";
			case EnableCap::TextureRectangle:return "TextureRectangle";
			case EnableCap::TextureCubeMap:return "TextureCubeMap";
			case EnableCap::ProgramPointSize:return "ProgramPointSize";
			case EnableCap::DepthClamp:return "DepthClamp";
			case EnableCap::TextureCubeMapSeamless:return "TextureCubeMapSeamless";
			case EnableCap::SampleShading:return "SampleShading";
			case EnableCap::RasterizerDiscard:return "RasterizerDiscard";
			case EnableCap::TextureGenStrOes:return "TextureGenStrOes";
			case EnableCap::PrimitiveRestartFixedIndex:return "PrimitiveRestartFixedIndex";
			case EnableCap::FramebufferSrgb:return "FramebufferSrgb";
			case EnableCap::SampleMask:return "SampleMask";
			case EnableCap::FetchPerSampleArm:return "FetchPerSampleArm";
			case EnableCap::PrimitiveRestart:return "PrimitiveRestart";
			case EnableCap::DebugOutput:return "DebugOutput";
			case EnableCap::ShadingRateImagePerPrimitiveNv:return "ShadingRateImagePerPrimitiveNv";
			case EnableCap::FramebufferFetchNoncoherentQcom:return "FramebufferFetchNoncoherentQcom";
			case EnableCap::ShadingRatePreserveAspectRatioQcom:return "ShadingRatePreserveAspectRatioQcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightModelParameter value) {
		switch(value){
			case LightModelParameter::LightModelLocalViewer:return "LightModelLocalViewer";
			case LightModelParameter::LightModelTwoSide:return "LightModelTwoSide";
			case LightModelParameter::LightModelAmbient:return "LightModelAmbient";
			case LightModelParameter::LightModelColorControl:return "LightModelColorControl";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogPName value) {
		switch(value){
			case FogPName::FogIndex:return "FogIndex";
			case FogPName::FogDensity:return "FogDensity";
			case FogPName::FogStart:return "FogStart";
			case FogPName::FogEnd:return "FogEnd";
			case FogPName::FogMode:return "FogMode";
			case FogPName::FogCoordSrc:return "FogCoordSrc";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogParameter value) {
		switch(value){
			case FogParameter::FogIndex:return "FogIndex";
			case FogParameter::FogDensity:return "FogDensity";
			case FogParameter::FogStart:return "FogStart";
			case FogParameter::FogEnd:return "FogEnd";
			case FogParameter::FogMode:return "FogMode";
			case FogParameter::FogColor:return "FogColor";
			case FogParameter::FogOffsetValueSgix:return "FogOffsetValueSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetFramebufferParameter value) {
		switch(value){
			case GetFramebufferParameter::Doublebuffer:return "Doublebuffer";
			case GetFramebufferParameter::Stereo:return "Stereo";
			case GetFramebufferParameter::SampleBuffers:return "SampleBuffers";
			case GetFramebufferParameter::Samples:return "Samples";
			case GetFramebufferParameter::ImplementationColorReadType:return "ImplementationColorReadType";
			case GetFramebufferParameter::ImplementationColorReadFormat:return "ImplementationColorReadFormat";
			case GetFramebufferParameter::FramebufferDefaultWidth:return "FramebufferDefaultWidth";
			case GetFramebufferParameter::FramebufferDefaultHeight:return "FramebufferDefaultHeight";
			case GetFramebufferParameter::FramebufferDefaultLayers:return "FramebufferDefaultLayers";
			case GetFramebufferParameter::FramebufferDefaultSamples:return "FramebufferDefaultSamples";
			case GetFramebufferParameter::FramebufferDefaultFixedSampleLocations:return "FramebufferDefaultFixedSampleLocations";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(HintTarget value) {
		switch(value){
			case HintTarget::PerspectiveCorrectionHint:return "PerspectiveCorrectionHint";
			case HintTarget::PointSmoothHint:return "PointSmoothHint";
			case HintTarget::LineSmoothHint:return "LineSmoothHint";
			case HintTarget::PolygonSmoothHint:return "PolygonSmoothHint";
			case HintTarget::FogHint:return "FogHint";
			case HintTarget::PackCmykHintExt:return "PackCmykHintExt";
			case HintTarget::UnpackCmykHintExt:return "UnpackCmykHintExt";
			case HintTarget::PhongHintWin:return "PhongHintWin";
			case HintTarget::ClipVolumeClippingHintExt:return "ClipVolumeClippingHintExt";
			case HintTarget::TextureMultiBufferHintSgix:return "TextureMultiBufferHintSgix";
			case HintTarget::GenerateMipmapHint:return "GenerateMipmapHint";
			case HintTarget::ProgramBinaryRetrievableHint:return "ProgramBinaryRetrievableHint";
			case HintTarget::ConvolutionHintSgix:return "ConvolutionHintSgix";
			case HintTarget::ScalebiasHintSgix:return "ScalebiasHintSgix";
			case HintTarget::LineQualityHintSgix:return "LineQualityHintSgix";
			case HintTarget::VertexPreclipSgix:return "VertexPreclipSgix";
			case HintTarget::VertexPreclipHintSgix:return "VertexPreclipHintSgix";
			case HintTarget::TextureCompressionHint:return "TextureCompressionHint";
			case HintTarget::VertexArrayStorageHintApple:return "VertexArrayStorageHintApple";
			case HintTarget::MultisampleFilterHintNv:return "MultisampleFilterHintNv";
			case HintTarget::TransformHintApple:return "TransformHintApple";
			case HintTarget::TextureStorageHintApple:return "TextureStorageHintApple";
			case HintTarget::FragmentShaderDerivativeHint:return "FragmentShaderDerivativeHint";
			case HintTarget::BinningControlHintQcom:return "BinningControlHintQcom";
			case HintTarget::PreferDoublebufferHintPgi:return "PreferDoublebufferHintPgi";
			case HintTarget::ConserveMemoryHintPgi:return "ConserveMemoryHintPgi";
			case HintTarget::ReclaimMemoryHintPgi:return "ReclaimMemoryHintPgi";
			case HintTarget::NativeGraphicsBeginHintPgi:return "NativeGraphicsBeginHintPgi";
			case HintTarget::NativeGraphicsEndHintPgi:return "NativeGraphicsEndHintPgi";
			case HintTarget::AlwaysFastHintPgi:return "AlwaysFastHintPgi";
			case HintTarget::AlwaysSoftHintPgi:return "AlwaysSoftHintPgi";
			case HintTarget::AllowDrawObjHintPgi:return "AllowDrawObjHintPgi";
			case HintTarget::AllowDrawWinHintPgi:return "AllowDrawWinHintPgi";
			case HintTarget::AllowDrawFrgHintPgi:return "AllowDrawFrgHintPgi";
			case HintTarget::AllowDrawMemHintPgi:return "AllowDrawMemHintPgi";
			case HintTarget::StrictDepthfuncHintPgi:return "StrictDepthfuncHintPgi";
			case HintTarget::StrictLightingHintPgi:return "StrictLightingHintPgi";
			case HintTarget::StrictScissorHintPgi:return "StrictScissorHintPgi";
			case HintTarget::FullStippleHintPgi:return "FullStippleHintPgi";
			case HintTarget::ClipNearHintPgi:return "ClipNearHintPgi";
			case HintTarget::ClipFarHintPgi:return "ClipFarHintPgi";
			case HintTarget::WideLineHintPgi:return "WideLineHintPgi";
			case HintTarget::BackNormalsHintPgi:return "BackNormalsHintPgi";
			case HintTarget::VertexDataHintPgi:return "VertexDataHintPgi";
			case HintTarget::VertexConsistentHintPgi:return "VertexConsistentHintPgi";
			case HintTarget::MaterialSideHintPgi:return "MaterialSideHintPgi";
			case HintTarget::MaxVertexHintPgi:return "MaxVertexHintPgi";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelMap value) {
		switch(value){
			case PixelMap::PixelMapIToI:return "PixelMapIToI";
			case PixelMap::PixelMapSToS:return "PixelMapSToS";
			case PixelMap::PixelMapIToR:return "PixelMapIToR";
			case PixelMap::PixelMapIToG:return "PixelMapIToG";
			case PixelMap::PixelMapIToB:return "PixelMapIToB";
			case PixelMap::PixelMapIToA:return "PixelMapIToA";
			case PixelMap::PixelMapRToR:return "PixelMapRToR";
			case PixelMap::PixelMapGToG:return "PixelMapGToG";
			case PixelMap::PixelMapBToB:return "PixelMapBToB";
			case PixelMap::PixelMapAToA:return "PixelMapAToA";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelStoreParameter value) {
		switch(value){
			case PixelStoreParameter::UnpackSwapBytes:return "UnpackSwapBytes";
			case PixelStoreParameter::UnpackLsbFirst:return "UnpackLsbFirst";
			case PixelStoreParameter::UnpackRowLength:return "UnpackRowLength";
			case PixelStoreParameter::UnpackSkipRows:return "UnpackSkipRows";
			case PixelStoreParameter::UnpackSkipPixels:return "UnpackSkipPixels";
			case PixelStoreParameter::UnpackAlignment:return "UnpackAlignment";
			case PixelStoreParameter::PackSwapBytes:return "PackSwapBytes";
			case PixelStoreParameter::PackLsbFirst:return "PackLsbFirst";
			case PixelStoreParameter::PackRowLength:return "PackRowLength";
			case PixelStoreParameter::PackSkipRows:return "PackSkipRows";
			case PixelStoreParameter::PackSkipPixels:return "PackSkipPixels";
			case PixelStoreParameter::PackAlignment:return "PackAlignment";
			case PixelStoreParameter::PackSkipImages:return "PackSkipImages";
			case PixelStoreParameter::PackImageHeight:return "PackImageHeight";
			case PixelStoreParameter::UnpackSkipImages:return "UnpackSkipImages";
			case PixelStoreParameter::UnpackImageHeight:return "UnpackImageHeight";
			case PixelStoreParameter::PackSkipVolumesSgis:return "PackSkipVolumesSgis";
			case PixelStoreParameter::PackImageDepthSgis:return "PackImageDepthSgis";
			case PixelStoreParameter::UnpackSkipVolumesSgis:return "UnpackSkipVolumesSgis";
			case PixelStoreParameter::UnpackImageDepthSgis:return "UnpackImageDepthSgis";
			case PixelStoreParameter::PixelTileWidthSgix:return "PixelTileWidthSgix";
			case PixelStoreParameter::PixelTileHeightSgix:return "PixelTileHeightSgix";
			case PixelStoreParameter::PixelTileGridWidthSgix:return "PixelTileGridWidthSgix";
			case PixelStoreParameter::PixelTileGridHeightSgix:return "PixelTileGridHeightSgix";
			case PixelStoreParameter::PixelTileGridDepthSgix:return "PixelTileGridDepthSgix";
			case PixelStoreParameter::PixelTileCacheSizeSgix:return "PixelTileCacheSizeSgix";
			case PixelStoreParameter::/* Formerly 0x842C in SGI specfile */ PackResampleSgix:return "/* Formerly 0x842C in SGI specfile */ PackResampleSgix";
			case PixelStoreParameter::/* Formerly 0x842D in SGI specfile */ UnpackResampleSgix:return "/* Formerly 0x842D in SGI specfile */ UnpackResampleSgix";
			case PixelStoreParameter::PackSubsampleRateSgix:return "PackSubsampleRateSgix";
			case PixelStoreParameter::UnpackSubsampleRateSgix:return "UnpackSubsampleRateSgix";
			case PixelStoreParameter::PackResampleOml:return "PackResampleOml";
			case PixelStoreParameter::UnpackResampleOml:return "UnpackResampleOml";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelTransferParameter value) {
		switch(value){
			case PixelTransferParameter::MapColor:return "MapColor";
			case PixelTransferParameter::MapStencil:return "MapStencil";
			case PixelTransferParameter::IndexShift:return "IndexShift";
			case PixelTransferParameter::IndexOffset:return "IndexOffset";
			case PixelTransferParameter::RedScale:return "RedScale";
			case PixelTransferParameter::RedBias:return "RedBias";
			case PixelTransferParameter::GreenScale:return "GreenScale";
			case PixelTransferParameter::GreenBias:return "GreenBias";
			case PixelTransferParameter::BlueScale:return "BlueScale";
			case PixelTransferParameter::BlueBias:return "BlueBias";
			case PixelTransferParameter::AlphaScale:return "AlphaScale";
			case PixelTransferParameter::AlphaBias:return "AlphaBias";
			case PixelTransferParameter::DepthScale:return "DepthScale";
			case PixelTransferParameter::DepthBias:return "DepthBias";
			case PixelTransferParameter::PostConvolutionRedScale:return "PostConvolutionRedScale";
			case PixelTransferParameter::PostConvolutionGreenScale:return "PostConvolutionGreenScale";
			case PixelTransferParameter::PostConvolutionBlueScale:return "PostConvolutionBlueScale";
			case PixelTransferParameter::PostConvolutionAlphaScale:return "PostConvolutionAlphaScale";
			case PixelTransferParameter::PostConvolutionRedBias:return "PostConvolutionRedBias";
			case PixelTransferParameter::PostConvolutionGreenBias:return "PostConvolutionGreenBias";
			case PixelTransferParameter::PostConvolutionBlueBias:return "PostConvolutionBlueBias";
			case PixelTransferParameter::PostConvolutionAlphaBias:return "PostConvolutionAlphaBias";
			case PixelTransferParameter::PostColorMatrixRedScale:return "PostColorMatrixRedScale";
			case PixelTransferParameter::PostColorMatrixGreenScale:return "PostColorMatrixGreenScale";
			case PixelTransferParameter::PostColorMatrixBlueScale:return "PostColorMatrixBlueScale";
			case PixelTransferParameter::PostColorMatrixAlphaScale:return "PostColorMatrixAlphaScale";
			case PixelTransferParameter::PostColorMatrixRedBias:return "PostColorMatrixRedBias";
			case PixelTransferParameter::PostColorMatrixGreenBias:return "PostColorMatrixGreenBias";
			case PixelTransferParameter::PostColorMatrixBlueBias:return "PostColorMatrixBlueBias";
			case PixelTransferParameter::PostColorMatrixAlphaBias:return "PostColorMatrixAlphaBias";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(IndexMaterialParameterEXT value) {
		switch(value){
			case IndexMaterialParameterEXT::IndexOffset:return "IndexOffset";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureEnvParameter value) {
		switch(value){
			case TextureEnvParameter::AlphaScale:return "AlphaScale";
			case TextureEnvParameter::TextureEnvMode:return "TextureEnvMode";
			case TextureEnvParameter::TextureEnvColor:return "TextureEnvColor";
			case TextureEnvParameter::TextureLodBias:return "TextureLodBias";
			case TextureEnvParameter::Combine:return "Combine";
			case TextureEnvParameter::CombineRgb:return "CombineRgb";
			case TextureEnvParameter::CombineAlpha:return "CombineAlpha";
			case TextureEnvParameter::RgbScale:return "RgbScale";
			case TextureEnvParameter::AddSigned:return "AddSigned";
			case TextureEnvParameter::Interpolate:return "Interpolate";
			case TextureEnvParameter::Constant:return "Constant";
			case TextureEnvParameter::PrimaryColor:return "PrimaryColor";
			case TextureEnvParameter::Previous:return "Previous";
			case TextureEnvParameter::Source0Rgb:return "Source0Rgb";
			case TextureEnvParameter::Source1Rgb:return "Source1Rgb";
			case TextureEnvParameter::Source2Rgb:return "Source2Rgb";
			case TextureEnvParameter::Source3RgbNv:return "Source3RgbNv";
			case TextureEnvParameter::Source0Alpha:return "Source0Alpha";
			case TextureEnvParameter::Source1Alpha:return "Source1Alpha";
			case TextureEnvParameter::Source2Alpha:return "Source2Alpha";
			case TextureEnvParameter::Source3AlphaNv:return "Source3AlphaNv";
			case TextureEnvParameter::Operand0Rgb:return "Operand0Rgb";
			case TextureEnvParameter::Operand1Rgb:return "Operand1Rgb";
			case TextureEnvParameter::Operand2Rgb:return "Operand2Rgb";
			case TextureEnvParameter::Operand3RgbNv:return "Operand3RgbNv";
			case TextureEnvParameter::Operand0Alpha:return "Operand0Alpha";
			case TextureEnvParameter::Operand1Alpha:return "Operand1Alpha";
			case TextureEnvParameter::Operand2Alpha:return "Operand2Alpha";
			case TextureEnvParameter::Operand3AlphaNv:return "Operand3AlphaNv";
			case TextureEnvParameter::CoordReplace:return "CoordReplace";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapTarget value) {
		switch(value){
			case MapTarget::Map1Color4:return "Map1Color4";
			case MapTarget::Map1Index:return "Map1Index";
			case MapTarget::Map1Normal:return "Map1Normal";
			case MapTarget::Map1TextureCoord1:return "Map1TextureCoord1";
			case MapTarget::Map1TextureCoord2:return "Map1TextureCoord2";
			case MapTarget::Map1TextureCoord3:return "Map1TextureCoord3";
			case MapTarget::Map1TextureCoord4:return "Map1TextureCoord4";
			case MapTarget::Map1Vertex3:return "Map1Vertex3";
			case MapTarget::Map1Vertex4:return "Map1Vertex4";
			case MapTarget::Map2Color4:return "Map2Color4";
			case MapTarget::Map2Index:return "Map2Index";
			case MapTarget::Map2Normal:return "Map2Normal";
			case MapTarget::Map2TextureCoord1:return "Map2TextureCoord1";
			case MapTarget::Map2TextureCoord2:return "Map2TextureCoord2";
			case MapTarget::Map2TextureCoord3:return "Map2TextureCoord3";
			case MapTarget::Map2TextureCoord4:return "Map2TextureCoord4";
			case MapTarget::Map2Vertex3:return "Map2Vertex3";
			case MapTarget::Map2Vertex4:return "Map2Vertex4";
			case MapTarget::GeometryDeformationSgix:return "GeometryDeformationSgix";
			case MapTarget::TextureDeformationSgix:return "TextureDeformationSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CopyImageSubDataTarget value) {
		switch(value){
			case CopyImageSubDataTarget::Texture1D:return "Texture1D";
			case CopyImageSubDataTarget::Texture2D:return "Texture2D";
			case CopyImageSubDataTarget::Texture3D:return "Texture3D";
			case CopyImageSubDataTarget::TextureRectangle:return "TextureRectangle";
			case CopyImageSubDataTarget::TextureCubeMap:return "TextureCubeMap";
			case CopyImageSubDataTarget::Texture1DArray:return "Texture1DArray";
			case CopyImageSubDataTarget::Texture2DArray:return "Texture2DArray";
			case CopyImageSubDataTarget::Renderbuffer:return "Renderbuffer";
			case CopyImageSubDataTarget::TextureCubeMapArray:return "TextureCubeMapArray";
			case CopyImageSubDataTarget::Texture2DMultisample:return "Texture2DMultisample";
			case CopyImageSubDataTarget::Texture2DMultisampleArray:return "Texture2DMultisampleArray";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureTarget value) {
		switch(value){
			case TextureTarget::Texture1D:return "Texture1D";
			case TextureTarget::Texture2D:return "Texture2D";
			case TextureTarget::ProxyTexture1D:return "ProxyTexture1D";
			case TextureTarget::ProxyTexture2D:return "ProxyTexture2D";
			case TextureTarget::Texture3D:return "Texture3D";
			case TextureTarget::ProxyTexture3D:return "ProxyTexture3D";
			case TextureTarget::DetailTexture2DSgis:return "DetailTexture2DSgis";
			case TextureTarget::Texture4DSgis:return "Texture4DSgis";
			case TextureTarget::ProxyTexture4DSgis:return "ProxyTexture4DSgis";
			case TextureTarget::TextureRectangle:return "TextureRectangle";
			case TextureTarget::ProxyTextureRectangle:return "ProxyTextureRectangle";
			case TextureTarget::TextureCubeMap:return "TextureCubeMap";
			case TextureTarget::TextureCubeMapPositiveX:return "TextureCubeMapPositiveX";
			case TextureTarget::TextureCubeMapNegativeX:return "TextureCubeMapNegativeX";
			case TextureTarget::TextureCubeMapPositiveY:return "TextureCubeMapPositiveY";
			case TextureTarget::TextureCubeMapNegativeY:return "TextureCubeMapNegativeY";
			case TextureTarget::TextureCubeMapPositiveZ:return "TextureCubeMapPositiveZ";
			case TextureTarget::TextureCubeMapNegativeZ:return "TextureCubeMapNegativeZ";
			case TextureTarget::ProxyTextureCubeMap:return "ProxyTextureCubeMap";
			case TextureTarget::Texture1DArray:return "Texture1DArray";
			case TextureTarget::ProxyTexture1DArray:return "ProxyTexture1DArray";
			case TextureTarget::Texture2DArray:return "Texture2DArray";
			case TextureTarget::ProxyTexture2DArray:return "ProxyTexture2DArray";
			case TextureTarget::TextureBuffer:return "TextureBuffer";
			case TextureTarget::Renderbuffer:return "Renderbuffer";
			case TextureTarget::TextureCubeMapArray:return "TextureCubeMapArray";
			case TextureTarget::ProxyTextureCubeMapArray:return "ProxyTextureCubeMapArray";
			case TextureTarget::Texture2DMultisample:return "Texture2DMultisample";
			case TextureTarget::ProxyTexture2DMultisample:return "ProxyTexture2DMultisample";
			case TextureTarget::Texture2DMultisampleArray:return "Texture2DMultisampleArray";
			case TextureTarget::ProxyTexture2DMultisampleArray:return "ProxyTexture2DMultisampleArray";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetPointervPName value) {
		switch(value){
			case GetPointervPName::FeedbackBufferPointer:return "FeedbackBufferPointer";
			case GetPointervPName::SelectionBufferPointer:return "SelectionBufferPointer";
			case GetPointervPName::VertexArrayPointer:return "VertexArrayPointer";
			case GetPointervPName::NormalArrayPointer:return "NormalArrayPointer";
			case GetPointervPName::ColorArrayPointer:return "ColorArrayPointer";
			case GetPointervPName::IndexArrayPointer:return "IndexArrayPointer";
			case GetPointervPName::TextureCoordArrayPointer:return "TextureCoordArrayPointer";
			case GetPointervPName::EdgeFlagArrayPointer:return "EdgeFlagArrayPointer";
			case GetPointervPName::InstrumentBufferPointerSgix:return "InstrumentBufferPointerSgix";
			case GetPointervPName::DebugCallbackFunction:return "DebugCallbackFunction";
			case GetPointervPName::DebugCallbackUserParam:return "DebugCallbackUserParam";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureParameterName value) {
		switch(value){
			case TextureParameterName::TextureWidth:return "TextureWidth";
			case TextureParameterName::TextureHeight:return "TextureHeight";
			case TextureParameterName::TextureInternalFormat:return "TextureInternalFormat";
			case TextureParameterName::TextureBorderColor:return "TextureBorderColor";
			case TextureParameterName::TextureBorder:return "TextureBorder";
			case TextureParameterName::TextureMagFilter:return "TextureMagFilter";
			case TextureParameterName::TextureMinFilter:return "TextureMinFilter";
			case TextureParameterName::TextureWrapS:return "TextureWrapS";
			case TextureParameterName::TextureWrapT:return "TextureWrapT";
			case TextureParameterName::TextureRedSize:return "TextureRedSize";
			case TextureParameterName::TextureGreenSize:return "TextureGreenSize";
			case TextureParameterName::TextureBlueSize:return "TextureBlueSize";
			case TextureParameterName::TextureAlphaSize:return "TextureAlphaSize";
			case TextureParameterName::TextureLuminanceSize:return "TextureLuminanceSize";
			case TextureParameterName::TextureIntensitySize:return "TextureIntensitySize";
			case TextureParameterName::TexturePriority:return "TexturePriority";
			case TextureParameterName::TextureResident:return "TextureResident";
			case TextureParameterName::TextureDepthExt:return "TextureDepthExt";
			case TextureParameterName::TextureWrapR:return "TextureWrapR";
			case TextureParameterName::DetailTextureLevelSgis:return "DetailTextureLevelSgis";
			case TextureParameterName::DetailTextureModeSgis:return "DetailTextureModeSgis";
			case TextureParameterName::DetailTextureFuncPointsSgis:return "DetailTextureFuncPointsSgis";
			case TextureParameterName::SharpenTextureFuncPointsSgis:return "SharpenTextureFuncPointsSgis";
			case TextureParameterName::ShadowAmbientSgix:return "ShadowAmbientSgix";
			case TextureParameterName::DualTextureSelectSgis:return "DualTextureSelectSgis";
			case TextureParameterName::QuadTextureSelectSgis:return "QuadTextureSelectSgis";
			case TextureParameterName::Texture4DsizeSgis:return "Texture4DsizeSgis";
			case TextureParameterName::TextureWrapQSgis:return "TextureWrapQSgis";
			case TextureParameterName::TextureMinLod:return "TextureMinLod";
			case TextureParameterName::TextureMaxLod:return "TextureMaxLod";
			case TextureParameterName::TextureBaseLevel:return "TextureBaseLevel";
			case TextureParameterName::TextureMaxLevel:return "TextureMaxLevel";
			case TextureParameterName::TextureFilter4SizeSgis:return "TextureFilter4SizeSgis";
			case TextureParameterName::TextureClipmapCenterSgix:return "TextureClipmapCenterSgix";
			case TextureParameterName::TextureClipmapFrameSgix:return "TextureClipmapFrameSgix";
			case TextureParameterName::TextureClipmapOffsetSgix:return "TextureClipmapOffsetSgix";
			case TextureParameterName::TextureClipmapVirtualDepthSgix:return "TextureClipmapVirtualDepthSgix";
			case TextureParameterName::TextureClipmapLodOffsetSgix:return "TextureClipmapLodOffsetSgix";
			case TextureParameterName::TextureClipmapDepthSgix:return "TextureClipmapDepthSgix";
			case TextureParameterName::PostTextureFilterBiasSgix:return "PostTextureFilterBiasSgix";
			case TextureParameterName::PostTextureFilterScaleSgix:return "PostTextureFilterScaleSgix";
			case TextureParameterName::TextureLodBiasSSgix:return "TextureLodBiasSSgix";
			case TextureParameterName::TextureLodBiasTSgix:return "TextureLodBiasTSgix";
			case TextureParameterName::TextureLodBiasRSgix:return "TextureLodBiasRSgix";
			case TextureParameterName::GenerateMipmap:return "GenerateMipmap";
			case TextureParameterName::TextureCompareSgix:return "TextureCompareSgix";
			case TextureParameterName::TextureCompareOperatorSgix:return "TextureCompareOperatorSgix";
			case TextureParameterName::TextureLequalRSgix:return "TextureLequalRSgix";
			case TextureParameterName::TextureGequalRSgix:return "TextureGequalRSgix";
			case TextureParameterName::TextureMaxClampSSgix:return "TextureMaxClampSSgix";
			case TextureParameterName::TextureMaxClampTSgix:return "TextureMaxClampTSgix";
			case TextureParameterName::TextureMaxClampRSgix:return "TextureMaxClampRSgix";
			case TextureParameterName::TextureMemoryLayoutIntel:return "TextureMemoryLayoutIntel";
			case TextureParameterName::TextureMaxAnisotropy:return "TextureMaxAnisotropy";
			case TextureParameterName::TextureLodBias:return "TextureLodBias";
			case TextureParameterName::TextureCompareMode:return "TextureCompareMode";
			case TextureParameterName::TextureCompareFunc:return "TextureCompareFunc";
			case TextureParameterName::TextureSwizzleR:return "TextureSwizzleR";
			case TextureParameterName::TextureSwizzleG:return "TextureSwizzleG";
			case TextureParameterName::TextureSwizzleB:return "TextureSwizzleB";
			case TextureParameterName::TextureSwizzleA:return "TextureSwizzleA";
			case TextureParameterName::TextureSwizzleRgba:return "TextureSwizzleRgba";
			case TextureParameterName::TextureUnnormalizedCoordinatesArm:return "TextureUnnormalizedCoordinatesArm";
			case TextureParameterName::DepthStencilTextureMode:return "DepthStencilTextureMode";
			case TextureParameterName::TextureTilingExt:return "TextureTilingExt";
			case TextureParameterName::TextureFoveatedCutoffDensityQcom:return "TextureFoveatedCutoffDensityQcom";
			case TextureParameterName::TextureYDegammaQcom:return "TextureYDegammaQcom";
			case TextureParameterName::TextureCbcrDegammaQcom:return "TextureCbcrDegammaQcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetTextureParameter value) {
		switch(value){
			case GetTextureParameter::TextureWidth:return "TextureWidth";
			case GetTextureParameter::TextureHeight:return "TextureHeight";
			case GetTextureParameter::TextureInternalFormat:return "TextureInternalFormat";
			case GetTextureParameter::TextureBorderColor:return "TextureBorderColor";
			case GetTextureParameter::TextureBorder:return "TextureBorder";
			case GetTextureParameter::TextureMagFilter:return "TextureMagFilter";
			case GetTextureParameter::TextureMinFilter:return "TextureMinFilter";
			case GetTextureParameter::TextureWrapS:return "TextureWrapS";
			case GetTextureParameter::TextureWrapT:return "TextureWrapT";
			case GetTextureParameter::TextureRedSize:return "TextureRedSize";
			case GetTextureParameter::TextureGreenSize:return "TextureGreenSize";
			case GetTextureParameter::TextureBlueSize:return "TextureBlueSize";
			case GetTextureParameter::TextureAlphaSize:return "TextureAlphaSize";
			case GetTextureParameter::TextureLuminanceSize:return "TextureLuminanceSize";
			case GetTextureParameter::TextureIntensitySize:return "TextureIntensitySize";
			case GetTextureParameter::TexturePriority:return "TexturePriority";
			case GetTextureParameter::TextureResident:return "TextureResident";
			case GetTextureParameter::TextureDepthExt:return "TextureDepthExt";
			case GetTextureParameter::TextureWrapRExt:return "TextureWrapRExt";
			case GetTextureParameter::DetailTextureLevelSgis:return "DetailTextureLevelSgis";
			case GetTextureParameter::DetailTextureModeSgis:return "DetailTextureModeSgis";
			case GetTextureParameter::DetailTextureFuncPointsSgis:return "DetailTextureFuncPointsSgis";
			case GetTextureParameter::SharpenTextureFuncPointsSgis:return "SharpenTextureFuncPointsSgis";
			case GetTextureParameter::ShadowAmbientSgix:return "ShadowAmbientSgix";
			case GetTextureParameter::DualTextureSelectSgis:return "DualTextureSelectSgis";
			case GetTextureParameter::QuadTextureSelectSgis:return "QuadTextureSelectSgis";
			case GetTextureParameter::Texture4DsizeSgis:return "Texture4DsizeSgis";
			case GetTextureParameter::TextureWrapQSgis:return "TextureWrapQSgis";
			case GetTextureParameter::TextureMinLodSgis:return "TextureMinLodSgis";
			case GetTextureParameter::TextureMaxLodSgis:return "TextureMaxLodSgis";
			case GetTextureParameter::TextureBaseLevelSgis:return "TextureBaseLevelSgis";
			case GetTextureParameter::TextureMaxLevelSgis:return "TextureMaxLevelSgis";
			case GetTextureParameter::TextureFilter4SizeSgis:return "TextureFilter4SizeSgis";
			case GetTextureParameter::TextureClipmapCenterSgix:return "TextureClipmapCenterSgix";
			case GetTextureParameter::TextureClipmapFrameSgix:return "TextureClipmapFrameSgix";
			case GetTextureParameter::TextureClipmapOffsetSgix:return "TextureClipmapOffsetSgix";
			case GetTextureParameter::TextureClipmapVirtualDepthSgix:return "TextureClipmapVirtualDepthSgix";
			case GetTextureParameter::TextureClipmapLodOffsetSgix:return "TextureClipmapLodOffsetSgix";
			case GetTextureParameter::TextureClipmapDepthSgix:return "TextureClipmapDepthSgix";
			case GetTextureParameter::PostTextureFilterBiasSgix:return "PostTextureFilterBiasSgix";
			case GetTextureParameter::PostTextureFilterScaleSgix:return "PostTextureFilterScaleSgix";
			case GetTextureParameter::TextureLodBiasSSgix:return "TextureLodBiasSSgix";
			case GetTextureParameter::TextureLodBiasTSgix:return "TextureLodBiasTSgix";
			case GetTextureParameter::TextureLodBiasRSgix:return "TextureLodBiasRSgix";
			case GetTextureParameter::GenerateMipmapSgis:return "GenerateMipmapSgis";
			case GetTextureParameter::TextureCompareSgix:return "TextureCompareSgix";
			case GetTextureParameter::TextureCompareOperatorSgix:return "TextureCompareOperatorSgix";
			case GetTextureParameter::TextureLequalRSgix:return "TextureLequalRSgix";
			case GetTextureParameter::TextureGequalRSgix:return "TextureGequalRSgix";
			case GetTextureParameter::TextureMaxClampSSgix:return "TextureMaxClampSSgix";
			case GetTextureParameter::TextureMaxClampTSgix:return "TextureMaxClampTSgix";
			case GetTextureParameter::TextureMaxClampRSgix:return "TextureMaxClampRSgix";
			case GetTextureParameter::NormalMap:return "NormalMap";
			case GetTextureParameter::ReflectionMap:return "ReflectionMap";
			case GetTextureParameter::TextureUnnormalizedCoordinatesArm:return "TextureUnnormalizedCoordinatesArm";
			case GetTextureParameter::SurfaceCompressionExt:return "SurfaceCompressionExt";
			case GetTextureParameter::TextureYDegammaQcom:return "TextureYDegammaQcom";
			case GetTextureParameter::TextureCbcrDegammaQcom:return "TextureCbcrDegammaQcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SamplerParameterF value) {
		switch(value){
			case SamplerParameterF::TextureBorderColor:return "TextureBorderColor";
			case SamplerParameterF::TextureMinLod:return "TextureMinLod";
			case SamplerParameterF::TextureMaxLod:return "TextureMaxLod";
			case SamplerParameterF::TextureMaxAnisotropy:return "TextureMaxAnisotropy";
			case SamplerParameterF::TextureLodBias:return "TextureLodBias";
			case SamplerParameterF::TextureUnnormalizedCoordinatesArm:return "TextureUnnormalizedCoordinatesArm";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DebugSeverity value) {
		switch(value){
			case DebugSeverity::DontCare:return "DontCare";
			case DebugSeverity::DebugSeverityNotification:return "DebugSeverityNotification";
			case DebugSeverity::DebugSeverityHigh:return "DebugSeverityHigh";
			case DebugSeverity::DebugSeverityMedium:return "DebugSeverityMedium";
			case DebugSeverity::DebugSeverityLow:return "DebugSeverityLow";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(HintMode value) {
		switch(value){
			case HintMode::DontCare:return "DontCare";
			case HintMode::Fastest:return "Fastest";
			case HintMode::Nicest:return "Nicest";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DebugSource value) {
		switch(value){
			case DebugSource::DontCare:return "DontCare";
			case DebugSource::DebugSourceApi:return "DebugSourceApi";
			case DebugSource::DebugSourceWindowSystem:return "DebugSourceWindowSystem";
			case DebugSource::DebugSourceShaderCompiler:return "DebugSourceShaderCompiler";
			case DebugSource::DebugSourceThirdParty:return "DebugSourceThirdParty";
			case DebugSource::DebugSourceApplication:return "DebugSourceApplication";
			case DebugSource::DebugSourceOther:return "DebugSourceOther";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DebugType value) {
		switch(value){
			case DebugType::DontCare:return "DontCare";
			case DebugType::DebugTypeError:return "DebugTypeError";
			case DebugType::DebugTypeDeprecatedBehavior:return "DebugTypeDeprecatedBehavior";
			case DebugType::DebugTypeUndefinedBehavior:return "DebugTypeUndefinedBehavior";
			case DebugType::DebugTypePortability:return "DebugTypePortability";
			case DebugType::DebugTypePerformance:return "DebugTypePerformance";
			case DebugType::DebugTypeOther:return "DebugTypeOther";
			case DebugType::DebugTypeMarker:return "DebugTypeMarker";
			case DebugType::DebugTypePushGroup:return "DebugTypePushGroup";
			case DebugType::DebugTypePopGroup:return "DebugTypePopGroup";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightParameter value) {
		switch(value){
			case LightParameter::Ambient:return "Ambient";
			case LightParameter::Diffuse:return "Diffuse";
			case LightParameter::Specular:return "Specular";
			case LightParameter::Position:return "Position";
			case LightParameter::SpotDirection:return "SpotDirection";
			case LightParameter::SpotExponent:return "SpotExponent";
			case LightParameter::SpotCutoff:return "SpotCutoff";
			case LightParameter::ConstantAttenuation:return "ConstantAttenuation";
			case LightParameter::LinearAttenuation:return "LinearAttenuation";
			case LightParameter::QuadraticAttenuation:return "QuadraticAttenuation";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MaterialParameter value) {
		switch(value){
			case MaterialParameter::Ambient:return "Ambient";
			case MaterialParameter::Diffuse:return "Diffuse";
			case MaterialParameter::Specular:return "Specular";
			case MaterialParameter::Emission:return "Emission";
			case MaterialParameter::Shininess:return "Shininess";
			case MaterialParameter::AmbientAndDiffuse:return "AmbientAndDiffuse";
			case MaterialParameter::ColorIndexes:return "ColorIndexes";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentLightParameterSGIX value) {
		switch(value){
			case FragmentLightParameterSGIX::Ambient:return "Ambient";
			case FragmentLightParameterSGIX::Diffuse:return "Diffuse";
			case FragmentLightParameterSGIX::Specular:return "Specular";
			case FragmentLightParameterSGIX::Position:return "Position";
			case FragmentLightParameterSGIX::SpotDirection:return "SpotDirection";
			case FragmentLightParameterSGIX::SpotExponent:return "SpotExponent";
			case FragmentLightParameterSGIX::SpotCutoff:return "SpotCutoff";
			case FragmentLightParameterSGIX::ConstantAttenuation:return "ConstantAttenuation";
			case FragmentLightParameterSGIX::LinearAttenuation:return "LinearAttenuation";
			case FragmentLightParameterSGIX::QuadraticAttenuation:return "QuadraticAttenuation";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorMaterialParameter value) {
		switch(value){
			case ColorMaterialParameter::Ambient:return "Ambient";
			case ColorMaterialParameter::Diffuse:return "Diffuse";
			case ColorMaterialParameter::Specular:return "Specular";
			case ColorMaterialParameter::Emission:return "Emission";
			case ColorMaterialParameter::AmbientAndDiffuse:return "AmbientAndDiffuse";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ListMode value) {
		switch(value){
			case ListMode::Compile:return "Compile";
			case ListMode::CompileAndExecute:return "CompileAndExecute";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribIType value) {
		switch(value){
			case VertexAttribIType::Byte:return "Byte";
			case VertexAttribIType::UnsignedByte:return "UnsignedByte";
			case VertexAttribIType::Short:return "Short";
			case VertexAttribIType::UnsignedShort:return "UnsignedShort";
			case VertexAttribIType::Int:return "Int";
			case VertexAttribIType::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(WeightPointerTypeARB value) {
		switch(value){
			case WeightPointerTypeARB::Byte:return "Byte";
			case WeightPointerTypeARB::UnsignedByte:return "UnsignedByte";
			case WeightPointerTypeARB::Short:return "Short";
			case WeightPointerTypeARB::UnsignedShort:return "UnsignedShort";
			case WeightPointerTypeARB::Int:return "Int";
			case WeightPointerTypeARB::UnsignedInt:return "UnsignedInt";
			case WeightPointerTypeARB::Float:return "Float";
			case WeightPointerTypeARB::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TangentPointerTypeEXT value) {
		switch(value){
			case TangentPointerTypeEXT::Byte:return "Byte";
			case TangentPointerTypeEXT::Short:return "Short";
			case TangentPointerTypeEXT::Int:return "Int";
			case TangentPointerTypeEXT::Float:return "Float";
			case TangentPointerTypeEXT::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BinormalPointerTypeEXT value) {
		switch(value){
			case BinormalPointerTypeEXT::Byte:return "Byte";
			case BinormalPointerTypeEXT::Short:return "Short";
			case BinormalPointerTypeEXT::Int:return "Int";
			case BinormalPointerTypeEXT::Float:return "Float";
			case BinormalPointerTypeEXT::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorPointerType value) {
		switch(value){
			case ColorPointerType::Byte:return "Byte";
			case ColorPointerType::UnsignedByte:return "UnsignedByte";
			case ColorPointerType::Short:return "Short";
			case ColorPointerType::UnsignedShort:return "UnsignedShort";
			case ColorPointerType::Int:return "Int";
			case ColorPointerType::UnsignedInt:return "UnsignedInt";
			case ColorPointerType::Float:return "Float";
			case ColorPointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ListNameType value) {
		switch(value){
			case ListNameType::Byte:return "Byte";
			case ListNameType::UnsignedByte:return "UnsignedByte";
			case ListNameType::Short:return "Short";
			case ListNameType::UnsignedShort:return "UnsignedShort";
			case ListNameType::Int:return "Int";
			case ListNameType::UnsignedInt:return "UnsignedInt";
			case ListNameType::Float:return "Float";
			case ListNameType::_2Bytes:return "_2Bytes";
			case ListNameType::_3Bytes:return "_3Bytes";
			case ListNameType::_4Bytes:return "_4Bytes";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(NormalPointerType value) {
		switch(value){
			case NormalPointerType::Byte:return "Byte";
			case NormalPointerType::Short:return "Short";
			case NormalPointerType::Int:return "Int";
			case NormalPointerType::Float:return "Float";
			case NormalPointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelType value) {
		switch(value){
			case PixelType::Byte:return "Byte";
			case PixelType::UnsignedByte:return "UnsignedByte";
			case PixelType::Short:return "Short";
			case PixelType::UnsignedShort:return "UnsignedShort";
			case PixelType::Int:return "Int";
			case PixelType::UnsignedInt:return "UnsignedInt";
			case PixelType::Float:return "Float";
			case PixelType::HalfFloat:return "HalfFloat";
			case PixelType::Bitmap:return "Bitmap";
			case PixelType::UnsignedByte332:return "UnsignedByte332";
			case PixelType::UnsignedShort4444:return "UnsignedShort4444";
			case PixelType::UnsignedShort5551:return "UnsignedShort5551";
			case PixelType::UnsignedInt8888:return "UnsignedInt8888";
			case PixelType::UnsignedInt1010102:return "UnsignedInt1010102";
			case PixelType::UnsignedByte233Rev:return "UnsignedByte233Rev";
			case PixelType::UnsignedShort565:return "UnsignedShort565";
			case PixelType::UnsignedShort565Rev:return "UnsignedShort565Rev";
			case PixelType::UnsignedShort4444Rev:return "UnsignedShort4444Rev";
			case PixelType::UnsignedShort1555Rev:return "UnsignedShort1555Rev";
			case PixelType::UnsignedInt8888Rev:return "UnsignedInt8888Rev";
			case PixelType::UnsignedInt2101010Rev:return "UnsignedInt2101010Rev";
			case PixelType::UnsignedInt248:return "UnsignedInt248";
			case PixelType::UnsignedInt10F11F11FRev:return "UnsignedInt10F11F11FRev";
			case PixelType::UnsignedInt5999Rev:return "UnsignedInt5999Rev";
			case PixelType::Float32UnsignedInt248Rev:return "Float32UnsignedInt248Rev";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribType value) {
		switch(value){
			case VertexAttribType::Byte:return "Byte";
			case VertexAttribType::UnsignedByte:return "UnsignedByte";
			case VertexAttribType::Short:return "Short";
			case VertexAttribType::UnsignedShort:return "UnsignedShort";
			case VertexAttribType::Int:return "Int";
			case VertexAttribType::UnsignedInt:return "UnsignedInt";
			case VertexAttribType::Float:return "Float";
			case VertexAttribType::Double:return "Double";
			case VertexAttribType::HalfFloat:return "HalfFloat";
			case VertexAttribType::Fixed:return "Fixed";
			case VertexAttribType::UnsignedInt2101010Rev:return "UnsignedInt2101010Rev";
			case VertexAttribType::UnsignedInt10F11F11FRev:return "UnsignedInt10F11F11FRev";
			case VertexAttribType::Int2101010Rev:return "Int2101010Rev";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribPointerType value) {
		switch(value){
			case VertexAttribPointerType::Byte:return "Byte";
			case VertexAttribPointerType::UnsignedByte:return "UnsignedByte";
			case VertexAttribPointerType::Short:return "Short";
			case VertexAttribPointerType::UnsignedShort:return "UnsignedShort";
			case VertexAttribPointerType::Int:return "Int";
			case VertexAttribPointerType::UnsignedInt:return "UnsignedInt";
			case VertexAttribPointerType::Float:return "Float";
			case VertexAttribPointerType::Double:return "Double";
			case VertexAttribPointerType::HalfFloat:return "HalfFloat";
			case VertexAttribPointerType::Fixed:return "Fixed";
			case VertexAttribPointerType::Int64Arb:return "Int64Arb";
			case VertexAttribPointerType::UnsignedInt64Arb:return "UnsignedInt64Arb";
			case VertexAttribPointerType::UnsignedInt2101010Rev:return "UnsignedInt2101010Rev";
			case VertexAttribPointerType::UnsignedInt10F11F11FRev:return "UnsignedInt10F11F11FRev";
			case VertexAttribPointerType::Int2101010Rev:return "Int2101010Rev";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ScalarType value) {
		switch(value){
			case ScalarType::UnsignedByte:return "UnsignedByte";
			case ScalarType::UnsignedShort:return "UnsignedShort";
			case ScalarType::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ReplacementCodeTypeSUN value) {
		switch(value){
			case ReplacementCodeTypeSUN::UnsignedByte:return "UnsignedByte";
			case ReplacementCodeTypeSUN::UnsignedShort:return "UnsignedShort";
			case ReplacementCodeTypeSUN::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ElementPointerTypeATI value) {
		switch(value){
			case ElementPointerTypeATI::UnsignedByte:return "UnsignedByte";
			case ElementPointerTypeATI::UnsignedShort:return "UnsignedShort";
			case ElementPointerTypeATI::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MatrixIndexPointerTypeARB value) {
		switch(value){
			case MatrixIndexPointerTypeARB::UnsignedByte:return "UnsignedByte";
			case MatrixIndexPointerTypeARB::UnsignedShort:return "UnsignedShort";
			case MatrixIndexPointerTypeARB::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DrawElementsType value) {
		switch(value){
			case DrawElementsType::UnsignedByte:return "UnsignedByte";
			case DrawElementsType::UnsignedShort:return "UnsignedShort";
			case DrawElementsType::UnsignedInt:return "UnsignedInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SecondaryColorPointerTypeIBM value) {
		switch(value){
			case SecondaryColorPointerTypeIBM::Short:return "Short";
			case SecondaryColorPointerTypeIBM::Int:return "Int";
			case SecondaryColorPointerTypeIBM::Float:return "Float";
			case SecondaryColorPointerTypeIBM::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(IndexPointerType value) {
		switch(value){
			case IndexPointerType::Short:return "Short";
			case IndexPointerType::Int:return "Int";
			case IndexPointerType::Float:return "Float";
			case IndexPointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TexCoordPointerType value) {
		switch(value){
			case TexCoordPointerType::Short:return "Short";
			case TexCoordPointerType::Int:return "Int";
			case TexCoordPointerType::Float:return "Float";
			case TexCoordPointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexPointerType value) {
		switch(value){
			case VertexPointerType::Short:return "Short";
			case VertexPointerType::Int:return "Int";
			case VertexPointerType::Float:return "Float";
			case VertexPointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelFormat value) {
		switch(value){
			case PixelFormat::UnsignedShort:return "UnsignedShort";
			case PixelFormat::UnsignedInt:return "UnsignedInt";
			case PixelFormat::ColorIndex:return "ColorIndex";
			case PixelFormat::StencilIndex:return "StencilIndex";
			case PixelFormat::DepthComponent:return "DepthComponent";
			case PixelFormat::Red:return "Red";
			case PixelFormat::Green:return "Green";
			case PixelFormat::Blue:return "Blue";
			case PixelFormat::Alpha:return "Alpha";
			case PixelFormat::Rgb:return "Rgb";
			case PixelFormat::Rgba:return "Rgba";
			case PixelFormat::Luminance:return "Luminance";
			case PixelFormat::LuminanceAlpha:return "LuminanceAlpha";
			case PixelFormat::AbgrExt:return "AbgrExt";
			case PixelFormat::CmykExt:return "CmykExt";
			case PixelFormat::CmykaExt:return "CmykaExt";
			case PixelFormat::Bgr:return "Bgr";
			case PixelFormat::Bgra:return "Bgra";
			case PixelFormat::Ycrcb422Sgix:return "Ycrcb422Sgix";
			case PixelFormat::Ycrcb444Sgix:return "Ycrcb444Sgix";
			case PixelFormat::Rg:return "Rg";
			case PixelFormat::RgInteger:return "RgInteger";
			case PixelFormat::DepthStencil:return "DepthStencil";
			case PixelFormat::RedInteger:return "RedInteger";
			case PixelFormat::GreenInteger:return "GreenInteger";
			case PixelFormat::BlueInteger:return "BlueInteger";
			case PixelFormat::RgbInteger:return "RgbInteger";
			case PixelFormat::RgbaInteger:return "RgbaInteger";
			case PixelFormat::BgrInteger:return "BgrInteger";
			case PixelFormat::BgraInteger:return "BgraInteger";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(AttributeType value) {
		switch(value){
			case AttributeType::Int:return "Int";
			case AttributeType::UnsignedInt:return "UnsignedInt";
			case AttributeType::Float:return "Float";
			case AttributeType::Double:return "Double";
			case AttributeType::Int64Arb:return "Int64Arb";
			case AttributeType::UnsignedInt64Arb:return "UnsignedInt64Arb";
			case AttributeType::FloatVec2:return "FloatVec2";
			case AttributeType::FloatVec3:return "FloatVec3";
			case AttributeType::FloatVec4:return "FloatVec4";
			case AttributeType::IntVec2:return "IntVec2";
			case AttributeType::IntVec3:return "IntVec3";
			case AttributeType::IntVec4:return "IntVec4";
			case AttributeType::Bool:return "Bool";
			case AttributeType::BoolVec2:return "BoolVec2";
			case AttributeType::BoolVec3:return "BoolVec3";
			case AttributeType::BoolVec4:return "BoolVec4";
			case AttributeType::FloatMat2:return "FloatMat2";
			case AttributeType::FloatMat3:return "FloatMat3";
			case AttributeType::FloatMat4:return "FloatMat4";
			case AttributeType::Sampler1D:return "Sampler1D";
			case AttributeType::Sampler2D:return "Sampler2D";
			case AttributeType::Sampler3D:return "Sampler3D";
			case AttributeType::SamplerCube:return "SamplerCube";
			case AttributeType::Sampler1DShadow:return "Sampler1DShadow";
			case AttributeType::Sampler2DShadow:return "Sampler2DShadow";
			case AttributeType::Sampler2DRect:return "Sampler2DRect";
			case AttributeType::Sampler2DRectShadow:return "Sampler2DRectShadow";
			case AttributeType::FloatMat2x3:return "FloatMat2x3";
			case AttributeType::FloatMat2x4:return "FloatMat2x4";
			case AttributeType::FloatMat3x2:return "FloatMat3x2";
			case AttributeType::FloatMat3x4:return "FloatMat3x4";
			case AttributeType::FloatMat4x2:return "FloatMat4x2";
			case AttributeType::FloatMat4x3:return "FloatMat4x3";
			case AttributeType::SamplerBuffer:return "SamplerBuffer";
			case AttributeType::Sampler1DArrayShadow:return "Sampler1DArrayShadow";
			case AttributeType::Sampler2DArrayShadow:return "Sampler2DArrayShadow";
			case AttributeType::SamplerCubeShadow:return "SamplerCubeShadow";
			case AttributeType::UnsignedIntVec2:return "UnsignedIntVec2";
			case AttributeType::UnsignedIntVec3:return "UnsignedIntVec3";
			case AttributeType::UnsignedIntVec4:return "UnsignedIntVec4";
			case AttributeType::IntSampler1D:return "IntSampler1D";
			case AttributeType::IntSampler2D:return "IntSampler2D";
			case AttributeType::IntSampler3D:return "IntSampler3D";
			case AttributeType::IntSamplerCube:return "IntSamplerCube";
			case AttributeType::IntSampler2DRect:return "IntSampler2DRect";
			case AttributeType::IntSampler1DArray:return "IntSampler1DArray";
			case AttributeType::IntSampler2DArray:return "IntSampler2DArray";
			case AttributeType::IntSamplerBuffer:return "IntSamplerBuffer";
			case AttributeType::UnsignedIntSampler1D:return "UnsignedIntSampler1D";
			case AttributeType::UnsignedIntSampler2D:return "UnsignedIntSampler2D";
			case AttributeType::UnsignedIntSampler3D:return "UnsignedIntSampler3D";
			case AttributeType::UnsignedIntSamplerCube:return "UnsignedIntSamplerCube";
			case AttributeType::UnsignedIntSampler2DRect:return "UnsignedIntSampler2DRect";
			case AttributeType::UnsignedIntSampler1DArray:return "UnsignedIntSampler1DArray";
			case AttributeType::UnsignedIntSampler2DArray:return "UnsignedIntSampler2DArray";
			case AttributeType::UnsignedIntSamplerBuffer:return "UnsignedIntSamplerBuffer";
			case AttributeType::DoubleMat2:return "DoubleMat2";
			case AttributeType::DoubleMat3:return "DoubleMat3";
			case AttributeType::DoubleMat4:return "DoubleMat4";
			case AttributeType::DoubleMat2x3:return "DoubleMat2x3";
			case AttributeType::DoubleMat2x4:return "DoubleMat2x4";
			case AttributeType::DoubleMat3x2:return "DoubleMat3x2";
			case AttributeType::DoubleMat3x4:return "DoubleMat3x4";
			case AttributeType::DoubleMat4x2:return "DoubleMat4x2";
			case AttributeType::DoubleMat4x3:return "DoubleMat4x3";
			case AttributeType::Int64Vec2Arb:return "Int64Vec2Arb";
			case AttributeType::Int64Vec3Arb:return "Int64Vec3Arb";
			case AttributeType::Int64Vec4Arb:return "Int64Vec4Arb";
			case AttributeType::UnsignedInt64Vec2Arb:return "UnsignedInt64Vec2Arb";
			case AttributeType::UnsignedInt64Vec3Arb:return "UnsignedInt64Vec3Arb";
			case AttributeType::UnsignedInt64Vec4Arb:return "UnsignedInt64Vec4Arb";
			case AttributeType::DoubleVec2:return "DoubleVec2";
			case AttributeType::DoubleVec3:return "DoubleVec3";
			case AttributeType::DoubleVec4:return "DoubleVec4";
			case AttributeType::SamplerCubeMapArray:return "SamplerCubeMapArray";
			case AttributeType::SamplerCubeMapArrayShadow:return "SamplerCubeMapArrayShadow";
			case AttributeType::IntSamplerCubeMapArray:return "IntSamplerCubeMapArray";
			case AttributeType::UnsignedIntSamplerCubeMapArray:return "UnsignedIntSamplerCubeMapArray";
			case AttributeType::Image1D:return "Image1D";
			case AttributeType::Image2D:return "Image2D";
			case AttributeType::Image3D:return "Image3D";
			case AttributeType::Image2DRect:return "Image2DRect";
			case AttributeType::ImageCube:return "ImageCube";
			case AttributeType::ImageBuffer:return "ImageBuffer";
			case AttributeType::Image1DArray:return "Image1DArray";
			case AttributeType::Image2DArray:return "Image2DArray";
			case AttributeType::ImageCubeMapArray:return "ImageCubeMapArray";
			case AttributeType::Image2DMultisample:return "Image2DMultisample";
			case AttributeType::Image2DMultisampleArray:return "Image2DMultisampleArray";
			case AttributeType::IntImage1D:return "IntImage1D";
			case AttributeType::IntImage2D:return "IntImage2D";
			case AttributeType::IntImage3D:return "IntImage3D";
			case AttributeType::IntImage2DRect:return "IntImage2DRect";
			case AttributeType::IntImageCube:return "IntImageCube";
			case AttributeType::IntImageBuffer:return "IntImageBuffer";
			case AttributeType::IntImage1DArray:return "IntImage1DArray";
			case AttributeType::IntImage2DArray:return "IntImage2DArray";
			case AttributeType::IntImageCubeMapArray:return "IntImageCubeMapArray";
			case AttributeType::IntImage2DMultisample:return "IntImage2DMultisample";
			case AttributeType::IntImage2DMultisampleArray:return "IntImage2DMultisampleArray";
			case AttributeType::UnsignedIntImage1D:return "UnsignedIntImage1D";
			case AttributeType::UnsignedIntImage2D:return "UnsignedIntImage2D";
			case AttributeType::UnsignedIntImage3D:return "UnsignedIntImage3D";
			case AttributeType::UnsignedIntImage2DRect:return "UnsignedIntImage2DRect";
			case AttributeType::UnsignedIntImageCube:return "UnsignedIntImageCube";
			case AttributeType::UnsignedIntImageBuffer:return "UnsignedIntImageBuffer";
			case AttributeType::UnsignedIntImage1DArray:return "UnsignedIntImage1DArray";
			case AttributeType::UnsignedIntImage2DArray:return "UnsignedIntImage2DArray";
			case AttributeType::UnsignedIntImageCubeMapArray:return "UnsignedIntImageCubeMapArray";
			case AttributeType::UnsignedIntImage2DMultisample:return "UnsignedIntImage2DMultisample";
			case AttributeType::UnsignedIntImage2DMultisampleArray:return "UnsignedIntImage2DMultisampleArray";
			case AttributeType::Sampler2DMultisample:return "Sampler2DMultisample";
			case AttributeType::IntSampler2DMultisample:return "IntSampler2DMultisample";
			case AttributeType::UnsignedIntSampler2DMultisample:return "UnsignedIntSampler2DMultisample";
			case AttributeType::Sampler2DMultisampleArray:return "Sampler2DMultisampleArray";
			case AttributeType::IntSampler2DMultisampleArray:return "IntSampler2DMultisampleArray";
			case AttributeType::UnsignedIntSampler2DMultisampleArray:return "UnsignedIntSampler2DMultisampleArray";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(UniformType value) {
		switch(value){
			case UniformType::Int:return "Int";
			case UniformType::UnsignedInt:return "UnsignedInt";
			case UniformType::Float:return "Float";
			case UniformType::Double:return "Double";
			case UniformType::FloatVec2:return "FloatVec2";
			case UniformType::FloatVec3:return "FloatVec3";
			case UniformType::FloatVec4:return "FloatVec4";
			case UniformType::IntVec2:return "IntVec2";
			case UniformType::IntVec3:return "IntVec3";
			case UniformType::IntVec4:return "IntVec4";
			case UniformType::Bool:return "Bool";
			case UniformType::BoolVec2:return "BoolVec2";
			case UniformType::BoolVec3:return "BoolVec3";
			case UniformType::BoolVec4:return "BoolVec4";
			case UniformType::FloatMat2:return "FloatMat2";
			case UniformType::FloatMat3:return "FloatMat3";
			case UniformType::FloatMat4:return "FloatMat4";
			case UniformType::Sampler1D:return "Sampler1D";
			case UniformType::Sampler2D:return "Sampler2D";
			case UniformType::Sampler3D:return "Sampler3D";
			case UniformType::SamplerCube:return "SamplerCube";
			case UniformType::Sampler1DShadow:return "Sampler1DShadow";
			case UniformType::Sampler2DShadow:return "Sampler2DShadow";
			case UniformType::Sampler2DRect:return "Sampler2DRect";
			case UniformType::Sampler2DRectShadow:return "Sampler2DRectShadow";
			case UniformType::FloatMat2x3:return "FloatMat2x3";
			case UniformType::FloatMat2x4:return "FloatMat2x4";
			case UniformType::FloatMat3x2:return "FloatMat3x2";
			case UniformType::FloatMat3x4:return "FloatMat3x4";
			case UniformType::FloatMat4x2:return "FloatMat4x2";
			case UniformType::FloatMat4x3:return "FloatMat4x3";
			case UniformType::Sampler1DArray:return "Sampler1DArray";
			case UniformType::Sampler2DArray:return "Sampler2DArray";
			case UniformType::SamplerBuffer:return "SamplerBuffer";
			case UniformType::Sampler1DArrayShadow:return "Sampler1DArrayShadow";
			case UniformType::Sampler2DArrayShadow:return "Sampler2DArrayShadow";
			case UniformType::SamplerCubeShadow:return "SamplerCubeShadow";
			case UniformType::UnsignedIntVec2:return "UnsignedIntVec2";
			case UniformType::UnsignedIntVec3:return "UnsignedIntVec3";
			case UniformType::UnsignedIntVec4:return "UnsignedIntVec4";
			case UniformType::IntSampler1D:return "IntSampler1D";
			case UniformType::IntSampler2D:return "IntSampler2D";
			case UniformType::IntSampler3D:return "IntSampler3D";
			case UniformType::IntSamplerCube:return "IntSamplerCube";
			case UniformType::IntSampler2DRect:return "IntSampler2DRect";
			case UniformType::IntSampler1DArray:return "IntSampler1DArray";
			case UniformType::IntSampler2DArray:return "IntSampler2DArray";
			case UniformType::IntSamplerBuffer:return "IntSamplerBuffer";
			case UniformType::UnsignedIntSampler1D:return "UnsignedIntSampler1D";
			case UniformType::UnsignedIntSampler2D:return "UnsignedIntSampler2D";
			case UniformType::UnsignedIntSampler3D:return "UnsignedIntSampler3D";
			case UniformType::UnsignedIntSamplerCube:return "UnsignedIntSamplerCube";
			case UniformType::UnsignedIntSampler2DRect:return "UnsignedIntSampler2DRect";
			case UniformType::UnsignedIntSampler1DArray:return "UnsignedIntSampler1DArray";
			case UniformType::UnsignedIntSampler2DArray:return "UnsignedIntSampler2DArray";
			case UniformType::UnsignedIntSamplerBuffer:return "UnsignedIntSamplerBuffer";
			case UniformType::DoubleMat2:return "DoubleMat2";
			case UniformType::DoubleMat3:return "DoubleMat3";
			case UniformType::DoubleMat4:return "DoubleMat4";
			case UniformType::DoubleMat2x3:return "DoubleMat2x3";
			case UniformType::DoubleMat2x4:return "DoubleMat2x4";
			case UniformType::DoubleMat3x2:return "DoubleMat3x2";
			case UniformType::DoubleMat3x4:return "DoubleMat3x4";
			case UniformType::DoubleMat4x2:return "DoubleMat4x2";
			case UniformType::DoubleMat4x3:return "DoubleMat4x3";
			case UniformType::DoubleVec2:return "DoubleVec2";
			case UniformType::DoubleVec3:return "DoubleVec3";
			case UniformType::DoubleVec4:return "DoubleVec4";
			case UniformType::SamplerCubeMapArray:return "SamplerCubeMapArray";
			case UniformType::SamplerCubeMapArrayShadow:return "SamplerCubeMapArrayShadow";
			case UniformType::IntSamplerCubeMapArray:return "IntSamplerCubeMapArray";
			case UniformType::UnsignedIntSamplerCubeMapArray:return "UnsignedIntSamplerCubeMapArray";
			case UniformType::Sampler2DMultisample:return "Sampler2DMultisample";
			case UniformType::IntSampler2DMultisample:return "IntSampler2DMultisample";
			case UniformType::UnsignedIntSampler2DMultisample:return "UnsignedIntSampler2DMultisample";
			case UniformType::Sampler2DMultisampleArray:return "Sampler2DMultisampleArray";
			case UniformType::IntSampler2DMultisampleArray:return "IntSampler2DMultisampleArray";
			case UniformType::UnsignedIntSampler2DMultisampleArray:return "UnsignedIntSampler2DMultisampleArray";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapTypeNV value) {
		switch(value){
			case MapTypeNV::Float:return "Float";
			case MapTypeNV::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexWeightPointerTypeEXT value) {
		switch(value){
			case VertexWeightPointerTypeEXT::Float:return "Float";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogCoordinatePointerType value) {
		switch(value){
			case FogCoordinatePointerType::Float:return "Float";
			case FogCoordinatePointerType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogPointerTypeEXT value) {
		switch(value){
			case FogPointerTypeEXT::Float:return "Float";
			case FogPointerTypeEXT::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogPointerTypeIBM value) {
		switch(value){
			case FogPointerTypeIBM::Float:return "Float";
			case FogPointerTypeIBM::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribLType value) {
		switch(value){
			case VertexAttribLType::Double:return "Double";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LogicOp value) {
		switch(value){
			case LogicOp::Clear:return "Clear";
			case LogicOp::And:return "And";
			case LogicOp::AndReverse:return "AndReverse";
			case LogicOp::Copy:return "Copy";
			case LogicOp::AndInverted:return "AndInverted";
			case LogicOp::Noop:return "Noop";
			case LogicOp::Xor:return "Xor";
			case LogicOp::Or:return "Or";
			case LogicOp::Nor:return "Nor";
			case LogicOp::Equiv:return "Equiv";
			case LogicOp::Invert:return "Invert";
			case LogicOp::OrReverse:return "OrReverse";
			case LogicOp::CopyInverted:return "CopyInverted";
			case LogicOp::OrInverted:return "OrInverted";
			case LogicOp::Nand:return "Nand";
			case LogicOp::Set:return "Set";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathFillMode value) {
		switch(value){
			case PathFillMode::Invert:return "Invert";
			case PathFillMode::PathFillModeNv:return "PathFillModeNv";
			case PathFillMode::CountUpNv:return "CountUpNv";
			case PathFillMode::CountDownNv:return "CountDownNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MatrixMode value) {
		switch(value){
			case MatrixMode::ModelView:return "ModelView";
			case MatrixMode::Projection:return "Projection";
			case MatrixMode::Texture:return "Texture";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ObjectIdentifier value) {
		switch(value){
			case ObjectIdentifier::Texture:return "Texture";
			case ObjectIdentifier::VertexArray:return "VertexArray";
			case ObjectIdentifier::Buffer:return "Buffer";
			case ObjectIdentifier::Shader:return "Shader";
			case ObjectIdentifier::Program:return "Program";
			case ObjectIdentifier::Query:return "Query";
			case ObjectIdentifier::ProgramPipeline:return "ProgramPipeline";
			case ObjectIdentifier::Sampler:return "Sampler";
			case ObjectIdentifier::Framebuffer:return "Framebuffer";
			case ObjectIdentifier::Renderbuffer:return "Renderbuffer";
			case ObjectIdentifier::TransformFeedback:return "TransformFeedback";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(Buffer value) {
		switch(value){
			case Buffer::Color:return "Color";
			case Buffer::Depth:return "Depth";
			case Buffer::Stencil:return "Stencil";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelCopyType value) {
		switch(value){
			case PixelCopyType::Color:return "Color";
			case PixelCopyType::Depth:return "Depth";
			case PixelCopyType::Stencil:return "Stencil";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(InvalidateFramebufferAttachment value) {
		switch(value){
			case InvalidateFramebufferAttachment::Color:return "Color";
			case InvalidateFramebufferAttachment::Depth:return "Depth";
			case InvalidateFramebufferAttachment::Stencil:return "Stencil";
			case InvalidateFramebufferAttachment::DepthStencilAttachment:return "DepthStencilAttachment";
			case InvalidateFramebufferAttachment::ColorAttachment0:return "ColorAttachment0";
			case InvalidateFramebufferAttachment::ColorAttachment1:return "ColorAttachment1";
			case InvalidateFramebufferAttachment::ColorAttachment2:return "ColorAttachment2";
			case InvalidateFramebufferAttachment::ColorAttachment3:return "ColorAttachment3";
			case InvalidateFramebufferAttachment::ColorAttachment4:return "ColorAttachment4";
			case InvalidateFramebufferAttachment::ColorAttachment5:return "ColorAttachment5";
			case InvalidateFramebufferAttachment::ColorAttachment6:return "ColorAttachment6";
			case InvalidateFramebufferAttachment::ColorAttachment7:return "ColorAttachment7";
			case InvalidateFramebufferAttachment::ColorAttachment8:return "ColorAttachment8";
			case InvalidateFramebufferAttachment::ColorAttachment9:return "ColorAttachment9";
			case InvalidateFramebufferAttachment::ColorAttachment10:return "ColorAttachment10";
			case InvalidateFramebufferAttachment::ColorAttachment11:return "ColorAttachment11";
			case InvalidateFramebufferAttachment::ColorAttachment12:return "ColorAttachment12";
			case InvalidateFramebufferAttachment::ColorAttachment13:return "ColorAttachment13";
			case InvalidateFramebufferAttachment::ColorAttachment14:return "ColorAttachment14";
			case InvalidateFramebufferAttachment::ColorAttachment15:return "ColorAttachment15";
			case InvalidateFramebufferAttachment::ColorAttachment16:return "ColorAttachment16";
			case InvalidateFramebufferAttachment::ColorAttachment17:return "ColorAttachment17";
			case InvalidateFramebufferAttachment::ColorAttachment18:return "ColorAttachment18";
			case InvalidateFramebufferAttachment::ColorAttachment19:return "ColorAttachment19";
			case InvalidateFramebufferAttachment::ColorAttachment20:return "ColorAttachment20";
			case InvalidateFramebufferAttachment::ColorAttachment21:return "ColorAttachment21";
			case InvalidateFramebufferAttachment::ColorAttachment22:return "ColorAttachment22";
			case InvalidateFramebufferAttachment::ColorAttachment23:return "ColorAttachment23";
			case InvalidateFramebufferAttachment::ColorAttachment24:return "ColorAttachment24";
			case InvalidateFramebufferAttachment::ColorAttachment25:return "ColorAttachment25";
			case InvalidateFramebufferAttachment::ColorAttachment26:return "ColorAttachment26";
			case InvalidateFramebufferAttachment::ColorAttachment27:return "ColorAttachment27";
			case InvalidateFramebufferAttachment::ColorAttachment28:return "ColorAttachment28";
			case InvalidateFramebufferAttachment::ColorAttachment29:return "ColorAttachment29";
			case InvalidateFramebufferAttachment::ColorAttachment30:return "ColorAttachment30";
			case InvalidateFramebufferAttachment::ColorAttachment31:return "ColorAttachment31";
			case InvalidateFramebufferAttachment::DepthAttachment:return "DepthAttachment";
			case InvalidateFramebufferAttachment::StencilAttachmentExt:return "StencilAttachmentExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(InternalFormat value) {
		switch(value){
			case InternalFormat::StencilIndex:return "StencilIndex";
			case InternalFormat::DepthComponent:return "DepthComponent";
			case InternalFormat::Red:return "Red";
			case InternalFormat::Rgb:return "Rgb";
			case InternalFormat::Rgba:return "Rgba";
			case InternalFormat::R3G3B2:return "R3G3B2";
			case InternalFormat::Alpha4:return "Alpha4";
			case InternalFormat::Alpha8:return "Alpha8";
			case InternalFormat::Alpha12:return "Alpha12";
			case InternalFormat::Alpha16:return "Alpha16";
			case InternalFormat::Luminance4:return "Luminance4";
			case InternalFormat::Luminance8:return "Luminance8";
			case InternalFormat::Luminance12:return "Luminance12";
			case InternalFormat::Luminance16:return "Luminance16";
			case InternalFormat::Luminance4Alpha4:return "Luminance4Alpha4";
			case InternalFormat::Luminance6Alpha2:return "Luminance6Alpha2";
			case InternalFormat::Luminance8Alpha8:return "Luminance8Alpha8";
			case InternalFormat::Luminance12Alpha4:return "Luminance12Alpha4";
			case InternalFormat::Luminance12Alpha12:return "Luminance12Alpha12";
			case InternalFormat::Luminance16Alpha16:return "Luminance16Alpha16";
			case InternalFormat::Intensity:return "Intensity";
			case InternalFormat::Intensity4:return "Intensity4";
			case InternalFormat::Intensity8:return "Intensity8";
			case InternalFormat::Intensity12:return "Intensity12";
			case InternalFormat::Intensity16:return "Intensity16";
			case InternalFormat::Rgb2Ext:return "Rgb2Ext";
			case InternalFormat::Rgb4:return "Rgb4";
			case InternalFormat::Rgb5:return "Rgb5";
			case InternalFormat::Rgb8:return "Rgb8";
			case InternalFormat::Rgb10:return "Rgb10";
			case InternalFormat::Rgb12:return "Rgb12";
			case InternalFormat::Rgb16:return "Rgb16";
			case InternalFormat::Rgba2:return "Rgba2";
			case InternalFormat::Rgba4:return "Rgba4";
			case InternalFormat::Rgb5A1:return "Rgb5A1";
			case InternalFormat::Rgba8:return "Rgba8";
			case InternalFormat::Rgb10A2:return "Rgb10A2";
			case InternalFormat::Rgba12:return "Rgba12";
			case InternalFormat::Rgba16:return "Rgba16";
			case InternalFormat::DualAlpha4Sgis:return "DualAlpha4Sgis";
			case InternalFormat::DualAlpha8Sgis:return "DualAlpha8Sgis";
			case InternalFormat::DualAlpha12Sgis:return "DualAlpha12Sgis";
			case InternalFormat::DualAlpha16Sgis:return "DualAlpha16Sgis";
			case InternalFormat::DualLuminance4Sgis:return "DualLuminance4Sgis";
			case InternalFormat::DualLuminance8Sgis:return "DualLuminance8Sgis";
			case InternalFormat::DualLuminance12Sgis:return "DualLuminance12Sgis";
			case InternalFormat::DualLuminance16Sgis:return "DualLuminance16Sgis";
			case InternalFormat::DualIntensity4Sgis:return "DualIntensity4Sgis";
			case InternalFormat::DualIntensity8Sgis:return "DualIntensity8Sgis";
			case InternalFormat::DualIntensity12Sgis:return "DualIntensity12Sgis";
			case InternalFormat::DualIntensity16Sgis:return "DualIntensity16Sgis";
			case InternalFormat::DualLuminanceAlpha4Sgis:return "DualLuminanceAlpha4Sgis";
			case InternalFormat::DualLuminanceAlpha8Sgis:return "DualLuminanceAlpha8Sgis";
			case InternalFormat::QuadAlpha4Sgis:return "QuadAlpha4Sgis";
			case InternalFormat::QuadAlpha8Sgis:return "QuadAlpha8Sgis";
			case InternalFormat::QuadLuminance4Sgis:return "QuadLuminance4Sgis";
			case InternalFormat::QuadLuminance8Sgis:return "QuadLuminance8Sgis";
			case InternalFormat::QuadIntensity4Sgis:return "QuadIntensity4Sgis";
			case InternalFormat::QuadIntensity8Sgis:return "QuadIntensity8Sgis";
			case InternalFormat::DepthComponent16:return "DepthComponent16";
			case InternalFormat::DepthComponent24:return "DepthComponent24";
			case InternalFormat::DepthComponent32:return "DepthComponent32";
			case InternalFormat::CompressedRed:return "CompressedRed";
			case InternalFormat::CompressedRg:return "CompressedRg";
			case InternalFormat::Rg:return "Rg";
			case InternalFormat::R8:return "R8";
			case InternalFormat::R16:return "R16";
			case InternalFormat::Rg8:return "Rg8";
			case InternalFormat::Rg16:return "Rg16";
			case InternalFormat::R16f:return "R16f";
			case InternalFormat::R32f:return "R32f";
			case InternalFormat::Rg16f:return "Rg16f";
			case InternalFormat::Rg32f:return "Rg32f";
			case InternalFormat::R8i:return "R8i";
			case InternalFormat::R8ui:return "R8ui";
			case InternalFormat::R16i:return "R16i";
			case InternalFormat::R16ui:return "R16ui";
			case InternalFormat::R32i:return "R32i";
			case InternalFormat::R32ui:return "R32ui";
			case InternalFormat::Rg8i:return "Rg8i";
			case InternalFormat::Rg8ui:return "Rg8ui";
			case InternalFormat::Rg16i:return "Rg16i";
			case InternalFormat::Rg16ui:return "Rg16ui";
			case InternalFormat::Rg32i:return "Rg32i";
			case InternalFormat::Rg32ui:return "Rg32ui";
			case InternalFormat::CompressedRgbS3tcDxt1Ext:return "CompressedRgbS3tcDxt1Ext";
			case InternalFormat::CompressedRgbaS3tcDxt1Ext:return "CompressedRgbaS3tcDxt1Ext";
			case InternalFormat::CompressedRgbaS3tcDxt3Angle:return "CompressedRgbaS3tcDxt3Angle";
			case InternalFormat::CompressedRgbaS3tcDxt5Angle:return "CompressedRgbaS3tcDxt5Angle";
			case InternalFormat::CompressedRgb:return "CompressedRgb";
			case InternalFormat::CompressedRgba:return "CompressedRgba";
			case InternalFormat::DepthStencil:return "DepthStencil";
			case InternalFormat::DepthStencilMesa:return "DepthStencilMesa";
			case InternalFormat::Rgba32f:return "Rgba32f";
			case InternalFormat::Rgb32f:return "Rgb32f";
			case InternalFormat::Rgba16f:return "Rgba16f";
			case InternalFormat::Rgb16f:return "Rgb16f";
			case InternalFormat::Depth24Stencil8:return "Depth24Stencil8";
			case InternalFormat::R11fG11fB10f:return "R11fG11fB10f";
			case InternalFormat::Rgb9E5:return "Rgb9E5";
			case InternalFormat::Srgb:return "Srgb";
			case InternalFormat::Srgb8:return "Srgb8";
			case InternalFormat::SrgbAlpha:return "SrgbAlpha";
			case InternalFormat::Srgb8Alpha8:return "Srgb8Alpha8";
			case InternalFormat::CompressedSrgb:return "CompressedSrgb";
			case InternalFormat::CompressedSrgbAlpha:return "CompressedSrgbAlpha";
			case InternalFormat::CompressedSrgbS3tcDxt1Ext:return "CompressedSrgbS3tcDxt1Ext";
			case InternalFormat::CompressedSrgbAlphaS3tcDxt1Ext:return "CompressedSrgbAlphaS3tcDxt1Ext";
			case InternalFormat::CompressedSrgbAlphaS3tcDxt3Ext:return "CompressedSrgbAlphaS3tcDxt3Ext";
			case InternalFormat::CompressedSrgbAlphaS3tcDxt5Ext:return "CompressedSrgbAlphaS3tcDxt5Ext";
			case InternalFormat::DepthComponent32f:return "DepthComponent32f";
			case InternalFormat::Depth32fStencil8:return "Depth32fStencil8";
			case InternalFormat::StencilIndex1:return "StencilIndex1";
			case InternalFormat::StencilIndex4:return "StencilIndex4";
			case InternalFormat::StencilIndex8:return "StencilIndex8";
			case InternalFormat::StencilIndex16:return "StencilIndex16";
			case InternalFormat::Rgb565Oes:return "Rgb565Oes";
			case InternalFormat::Etc1Rgb8Oes:return "Etc1Rgb8Oes";
			case InternalFormat::Rgba32ui:return "Rgba32ui";
			case InternalFormat::Rgb32ui:return "Rgb32ui";
			case InternalFormat::Alpha32uiExt:return "Alpha32uiExt";
			case InternalFormat::Intensity32uiExt:return "Intensity32uiExt";
			case InternalFormat::Luminance32uiExt:return "Luminance32uiExt";
			case InternalFormat::LuminanceAlpha32uiExt:return "LuminanceAlpha32uiExt";
			case InternalFormat::Rgba16ui:return "Rgba16ui";
			case InternalFormat::Rgb16ui:return "Rgb16ui";
			case InternalFormat::Alpha16uiExt:return "Alpha16uiExt";
			case InternalFormat::Intensity16uiExt:return "Intensity16uiExt";
			case InternalFormat::Luminance16uiExt:return "Luminance16uiExt";
			case InternalFormat::LuminanceAlpha16uiExt:return "LuminanceAlpha16uiExt";
			case InternalFormat::Rgba8ui:return "Rgba8ui";
			case InternalFormat::Rgb8ui:return "Rgb8ui";
			case InternalFormat::Alpha8uiExt:return "Alpha8uiExt";
			case InternalFormat::Intensity8uiExt:return "Intensity8uiExt";
			case InternalFormat::Luminance8uiExt:return "Luminance8uiExt";
			case InternalFormat::LuminanceAlpha8uiExt:return "LuminanceAlpha8uiExt";
			case InternalFormat::Rgba32i:return "Rgba32i";
			case InternalFormat::Rgb32i:return "Rgb32i";
			case InternalFormat::Alpha32iExt:return "Alpha32iExt";
			case InternalFormat::Intensity32iExt:return "Intensity32iExt";
			case InternalFormat::Luminance32iExt:return "Luminance32iExt";
			case InternalFormat::LuminanceAlpha32iExt:return "LuminanceAlpha32iExt";
			case InternalFormat::Rgba16i:return "Rgba16i";
			case InternalFormat::Rgb16i:return "Rgb16i";
			case InternalFormat::Alpha16iExt:return "Alpha16iExt";
			case InternalFormat::Intensity16iExt:return "Intensity16iExt";
			case InternalFormat::Luminance16iExt:return "Luminance16iExt";
			case InternalFormat::LuminanceAlpha16iExt:return "LuminanceAlpha16iExt";
			case InternalFormat::Rgba8i:return "Rgba8i";
			case InternalFormat::Rgb8i:return "Rgb8i";
			case InternalFormat::Alpha8iExt:return "Alpha8iExt";
			case InternalFormat::Intensity8iExt:return "Intensity8iExt";
			case InternalFormat::Luminance8iExt:return "Luminance8iExt";
			case InternalFormat::LuminanceAlpha8iExt:return "LuminanceAlpha8iExt";
			case InternalFormat::DepthComponent32fNv:return "DepthComponent32fNv";
			case InternalFormat::Depth32fStencil8Nv:return "Depth32fStencil8Nv";
			case InternalFormat::CompressedRedRgtc1:return "CompressedRedRgtc1";
			case InternalFormat::CompressedSignedRedRgtc1:return "CompressedSignedRedRgtc1";
			case InternalFormat::CompressedRedGreenRgtc2Ext:return "CompressedRedGreenRgtc2Ext";
			case InternalFormat::CompressedSignedRedGreenRgtc2Ext:return "CompressedSignedRedGreenRgtc2Ext";
			case InternalFormat::CompressedRgbaBptcUnorm:return "CompressedRgbaBptcUnorm";
			case InternalFormat::CompressedSrgbAlphaBptcUnorm:return "CompressedSrgbAlphaBptcUnorm";
			case InternalFormat::CompressedRgbBptcSignedFloat:return "CompressedRgbBptcSignedFloat";
			case InternalFormat::CompressedRgbBptcUnsignedFloat:return "CompressedRgbBptcUnsignedFloat";
			case InternalFormat::R8Snorm:return "R8Snorm";
			case InternalFormat::Rg8Snorm:return "Rg8Snorm";
			case InternalFormat::Rgb8Snorm:return "Rgb8Snorm";
			case InternalFormat::Rgba8Snorm:return "Rgba8Snorm";
			case InternalFormat::R16Snorm:return "R16Snorm";
			case InternalFormat::Rg16Snorm:return "Rg16Snorm";
			case InternalFormat::Rgb16Snorm:return "Rgb16Snorm";
			case InternalFormat::Rgba16Snorm:return "Rgba16Snorm";
			case InternalFormat::Sr8Ext:return "Sr8Ext";
			case InternalFormat::Srg8Ext:return "Srg8Ext";
			case InternalFormat::Rgb10A2ui:return "Rgb10A2ui";
			case InternalFormat::CompressedR11Eac:return "CompressedR11Eac";
			case InternalFormat::CompressedSignedR11Eac:return "CompressedSignedR11Eac";
			case InternalFormat::CompressedRg11Eac:return "CompressedRg11Eac";
			case InternalFormat::CompressedSignedRg11Eac:return "CompressedSignedRg11Eac";
			case InternalFormat::CompressedRgb8Etc2:return "CompressedRgb8Etc2";
			case InternalFormat::CompressedSrgb8Etc2:return "CompressedSrgb8Etc2";
			case InternalFormat::CompressedRgb8PunchthroughAlpha1Etc2:return "CompressedRgb8PunchthroughAlpha1Etc2";
			case InternalFormat::CompressedSrgb8PunchthroughAlpha1Etc2:return "CompressedSrgb8PunchthroughAlpha1Etc2";
			case InternalFormat::CompressedRgba8Etc2Eac:return "CompressedRgba8Etc2Eac";
			case InternalFormat::CompressedSrgb8Alpha8Etc2Eac:return "CompressedSrgb8Alpha8Etc2Eac";
			case InternalFormat::CompressedRgbaAstc4X4:return "CompressedRgbaAstc4X4";
			case InternalFormat::CompressedRgbaAstc5X4:return "CompressedRgbaAstc5X4";
			case InternalFormat::CompressedRgbaAstc5X5:return "CompressedRgbaAstc5X5";
			case InternalFormat::CompressedRgbaAstc6X5:return "CompressedRgbaAstc6X5";
			case InternalFormat::CompressedRgbaAstc6X6:return "CompressedRgbaAstc6X6";
			case InternalFormat::CompressedRgbaAstc8X5:return "CompressedRgbaAstc8X5";
			case InternalFormat::CompressedRgbaAstc8X6:return "CompressedRgbaAstc8X6";
			case InternalFormat::CompressedRgbaAstc8X8:return "CompressedRgbaAstc8X8";
			case InternalFormat::CompressedRgbaAstc10X5:return "CompressedRgbaAstc10X5";
			case InternalFormat::CompressedRgbaAstc10X6:return "CompressedRgbaAstc10X6";
			case InternalFormat::CompressedRgbaAstc10X8:return "CompressedRgbaAstc10X8";
			case InternalFormat::CompressedRgbaAstc10X10:return "CompressedRgbaAstc10X10";
			case InternalFormat::CompressedRgbaAstc12X10:return "CompressedRgbaAstc12X10";
			case InternalFormat::CompressedRgbaAstc12X12:return "CompressedRgbaAstc12X12";
			case InternalFormat::CompressedRgbaAstc3X3x3Oes:return "CompressedRgbaAstc3X3x3Oes";
			case InternalFormat::CompressedRgbaAstc4X3x3Oes:return "CompressedRgbaAstc4X3x3Oes";
			case InternalFormat::CompressedRgbaAstc4X4x3Oes:return "CompressedRgbaAstc4X4x3Oes";
			case InternalFormat::CompressedRgbaAstc4X4x4Oes:return "CompressedRgbaAstc4X4x4Oes";
			case InternalFormat::CompressedRgbaAstc5X4x4Oes:return "CompressedRgbaAstc5X4x4Oes";
			case InternalFormat::CompressedRgbaAstc5X5x4Oes:return "CompressedRgbaAstc5X5x4Oes";
			case InternalFormat::CompressedRgbaAstc5X5x5Oes:return "CompressedRgbaAstc5X5x5Oes";
			case InternalFormat::CompressedRgbaAstc6X5x5Oes:return "CompressedRgbaAstc6X5x5Oes";
			case InternalFormat::CompressedRgbaAstc6X6x5Oes:return "CompressedRgbaAstc6X6x5Oes";
			case InternalFormat::CompressedRgbaAstc6X6x6Oes:return "CompressedRgbaAstc6X6x6Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc4X4:return "CompressedSrgb8Alpha8Astc4X4";
			case InternalFormat::CompressedSrgb8Alpha8Astc5X4:return "CompressedSrgb8Alpha8Astc5X4";
			case InternalFormat::CompressedSrgb8Alpha8Astc5X5:return "CompressedSrgb8Alpha8Astc5X5";
			case InternalFormat::CompressedSrgb8Alpha8Astc6X5:return "CompressedSrgb8Alpha8Astc6X5";
			case InternalFormat::CompressedSrgb8Alpha8Astc6X6:return "CompressedSrgb8Alpha8Astc6X6";
			case InternalFormat::CompressedSrgb8Alpha8Astc8X5:return "CompressedSrgb8Alpha8Astc8X5";
			case InternalFormat::CompressedSrgb8Alpha8Astc8X6:return "CompressedSrgb8Alpha8Astc8X6";
			case InternalFormat::CompressedSrgb8Alpha8Astc8X8:return "CompressedSrgb8Alpha8Astc8X8";
			case InternalFormat::CompressedSrgb8Alpha8Astc10X5:return "CompressedSrgb8Alpha8Astc10X5";
			case InternalFormat::CompressedSrgb8Alpha8Astc10X6:return "CompressedSrgb8Alpha8Astc10X6";
			case InternalFormat::CompressedSrgb8Alpha8Astc10X8:return "CompressedSrgb8Alpha8Astc10X8";
			case InternalFormat::CompressedSrgb8Alpha8Astc10X10:return "CompressedSrgb8Alpha8Astc10X10";
			case InternalFormat::CompressedSrgb8Alpha8Astc12X10:return "CompressedSrgb8Alpha8Astc12X10";
			case InternalFormat::CompressedSrgb8Alpha8Astc12X12:return "CompressedSrgb8Alpha8Astc12X12";
			case InternalFormat::CompressedSrgb8Alpha8Astc3X3x3Oes:return "CompressedSrgb8Alpha8Astc3X3x3Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc4X3x3Oes:return "CompressedSrgb8Alpha8Astc4X3x3Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc4X4x3Oes:return "CompressedSrgb8Alpha8Astc4X4x3Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc4X4x4Oes:return "CompressedSrgb8Alpha8Astc4X4x4Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc5X4x4Oes:return "CompressedSrgb8Alpha8Astc5X4x4Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc5X5x4Oes:return "CompressedSrgb8Alpha8Astc5X5x4Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc5X5x5Oes:return "CompressedSrgb8Alpha8Astc5X5x5Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc6X5x5Oes:return "CompressedSrgb8Alpha8Astc6X5x5Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc6X6x5Oes:return "CompressedSrgb8Alpha8Astc6X6x5Oes";
			case InternalFormat::CompressedSrgb8Alpha8Astc6X6x6Oes:return "CompressedSrgb8Alpha8Astc6X6x6Oes";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DepthStencilTextureMode value) {
		switch(value){
			case DepthStencilTextureMode::StencilIndex:return "StencilIndex";
			case DepthStencilTextureMode::DepthComponent:return "DepthComponent";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerComponentUsageNV value) {
		switch(value){
			case CombinerComponentUsageNV::Blue:return "Blue";
			case CombinerComponentUsageNV::Alpha:return "Alpha";
			case CombinerComponentUsageNV::Rgb:return "Rgb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerPortionNV value) {
		switch(value){
			case CombinerPortionNV::Alpha:return "Alpha";
			case CombinerPortionNV::Rgb:return "Rgb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PolygonMode value) {
		switch(value){
			case PolygonMode::Point:return "Point";
			case PolygonMode::Line:return "Line";
			case PolygonMode::Fill:return "Fill";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MeshMode1 value) {
		switch(value){
			case MeshMode1::Point:return "Point";
			case MeshMode1::Line:return "Line";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MeshMode2 value) {
		switch(value){
			case MeshMode2::Point:return "Point";
			case MeshMode2::Line:return "Line";
			case MeshMode2::Fill:return "Fill";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(EvalMapsModeNV value) {
		switch(value){
			case EvalMapsModeNV::FillNv:return "FillNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(RenderingMode value) {
		switch(value){
			case RenderingMode::Render:return "Render";
			case RenderingMode::Feedback:return "Feedback";
			case RenderingMode::Select:return "Select";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShadingModel value) {
		switch(value){
			case ShadingModel::Flat:return "Flat";
			case ShadingModel::Smooth:return "Smooth";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(StringName value) {
		switch(value){
			case StringName::Vendor:return "Vendor";
			case StringName::Renderer:return "Renderer";
			case StringName::Version:return "Version";
			case StringName::Extensions:return "Extensions";
			case StringName::ShadingLanguageVersion:return "ShadingLanguageVersion";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureCoordName value) {
		switch(value){
			case TextureCoordName::S:return "S";
			case TextureCoordName::T:return "T";
			case TextureCoordName::R:return "R";
			case TextureCoordName::Q:return "Q";
			case TextureCoordName::TextureGenStrOes:return "TextureGenStrOes";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureEnvTarget value) {
		switch(value){
			case TextureEnvTarget::TextureEnv:return "TextureEnv";
			case TextureEnvTarget::TextureFilterControl:return "TextureFilterControl";
			case TextureEnvTarget::PointSprite:return "PointSprite";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureGenMode value) {
		switch(value){
			case TextureGenMode::EyeLinear:return "EyeLinear";
			case TextureGenMode::ObjectLinear:return "ObjectLinear";
			case TextureGenMode::SphereMap:return "SphereMap";
			case TextureGenMode::EyeDistanceToPointSgis:return "EyeDistanceToPointSgis";
			case TextureGenMode::ObjectDistanceToPointSgis:return "ObjectDistanceToPointSgis";
			case TextureGenMode::EyeDistanceToLineSgis:return "EyeDistanceToLineSgis";
			case TextureGenMode::ObjectDistanceToLineSgis:return "ObjectDistanceToLineSgis";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureGenParameter value) {
		switch(value){
			case TextureGenParameter::TextureGenMode:return "TextureGenMode";
			case TextureGenParameter::ObjectPlane:return "ObjectPlane";
			case TextureGenParameter::EyePlane:return "EyePlane";
			case TextureGenParameter::EyePointSgis:return "EyePointSgis";
			case TextureGenParameter::ObjectPointSgis:return "ObjectPointSgis";
			case TextureGenParameter::EyeLineSgis:return "EyeLineSgis";
			case TextureGenParameter::ObjectLineSgis:return "ObjectLineSgis";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BlitFramebufferFilter value) {
		switch(value){
			case BlitFramebufferFilter::Nearest:return "Nearest";
			case BlitFramebufferFilter::Linear:return "Linear";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureMagFilter value) {
		switch(value){
			case TextureMagFilter::Nearest:return "Nearest";
			case TextureMagFilter::Linear:return "Linear";
			case TextureMagFilter::LinearDetailSgis:return "LinearDetailSgis";
			case TextureMagFilter::LinearDetailAlphaSgis:return "LinearDetailAlphaSgis";
			case TextureMagFilter::LinearDetailColorSgis:return "LinearDetailColorSgis";
			case TextureMagFilter::LinearSharpenSgis:return "LinearSharpenSgis";
			case TextureMagFilter::LinearSharpenAlphaSgis:return "LinearSharpenAlphaSgis";
			case TextureMagFilter::LinearSharpenColorSgis:return "LinearSharpenColorSgis";
			case TextureMagFilter::Filter4Sgis:return "Filter4Sgis";
			case TextureMagFilter::PixelTexGenQCeilingSgix:return "PixelTexGenQCeilingSgix";
			case TextureMagFilter::PixelTexGenQRoundSgix:return "PixelTexGenQRoundSgix";
			case TextureMagFilter::PixelTexGenQFloorSgix:return "PixelTexGenQFloorSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureMinFilter value) {
		switch(value){
			case TextureMinFilter::Nearest:return "Nearest";
			case TextureMinFilter::Linear:return "Linear";
			case TextureMinFilter::NearestMipmapNearest:return "NearestMipmapNearest";
			case TextureMinFilter::LinearMipmapNearest:return "LinearMipmapNearest";
			case TextureMinFilter::NearestMipmapLinear:return "NearestMipmapLinear";
			case TextureMinFilter::LinearMipmapLinear:return "LinearMipmapLinear";
			case TextureMinFilter::Filter4Sgis:return "Filter4Sgis";
			case TextureMinFilter::LinearClipmapLinearSgix:return "LinearClipmapLinearSgix";
			case TextureMinFilter::PixelTexGenQCeilingSgix:return "PixelTexGenQCeilingSgix";
			case TextureMinFilter::PixelTexGenQRoundSgix:return "PixelTexGenQRoundSgix";
			case TextureMinFilter::PixelTexGenQFloorSgix:return "PixelTexGenQFloorSgix";
			case TextureMinFilter::NearestClipmapNearestSgix:return "NearestClipmapNearestSgix";
			case TextureMinFilter::NearestClipmapLinearSgix:return "NearestClipmapLinearSgix";
			case TextureMinFilter::LinearClipmapNearestSgix:return "LinearClipmapNearestSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SamplerParameterI value) {
		switch(value){
			case SamplerParameterI::TextureMagFilter:return "TextureMagFilter";
			case SamplerParameterI::TextureMinFilter:return "TextureMinFilter";
			case SamplerParameterI::TextureWrapS:return "TextureWrapS";
			case SamplerParameterI::TextureWrapT:return "TextureWrapT";
			case SamplerParameterI::TextureWrapR:return "TextureWrapR";
			case SamplerParameterI::TextureCompareMode:return "TextureCompareMode";
			case SamplerParameterI::TextureCompareFunc:return "TextureCompareFunc";
			case SamplerParameterI::TextureUnnormalizedCoordinatesArm:return "TextureUnnormalizedCoordinatesArm";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureWrapMode value) {
		switch(value){
			case TextureWrapMode::Clamp:return "Clamp";
			case TextureWrapMode::Repeat:return "Repeat";
			case TextureWrapMode::ClampToBorder:return "ClampToBorder";
			case TextureWrapMode::ClampToEdge:return "ClampToEdge";
			case TextureWrapMode::MirroredRepeat:return "MirroredRepeat";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SizedInternalFormat value) {
		switch(value){
			case SizedInternalFormat::R3G3B2:return "R3G3B2";
			case SizedInternalFormat::Alpha4:return "Alpha4";
			case SizedInternalFormat::Alpha8:return "Alpha8";
			case SizedInternalFormat::Alpha12:return "Alpha12";
			case SizedInternalFormat::Alpha16:return "Alpha16";
			case SizedInternalFormat::Luminance4:return "Luminance4";
			case SizedInternalFormat::Luminance8:return "Luminance8";
			case SizedInternalFormat::Luminance12:return "Luminance12";
			case SizedInternalFormat::Luminance16:return "Luminance16";
			case SizedInternalFormat::Luminance4Alpha4:return "Luminance4Alpha4";
			case SizedInternalFormat::Luminance6Alpha2:return "Luminance6Alpha2";
			case SizedInternalFormat::Luminance8Alpha8:return "Luminance8Alpha8";
			case SizedInternalFormat::Luminance12Alpha4:return "Luminance12Alpha4";
			case SizedInternalFormat::Luminance12Alpha12:return "Luminance12Alpha12";
			case SizedInternalFormat::Luminance16Alpha16:return "Luminance16Alpha16";
			case SizedInternalFormat::Intensity4:return "Intensity4";
			case SizedInternalFormat::Intensity8:return "Intensity8";
			case SizedInternalFormat::Intensity12:return "Intensity12";
			case SizedInternalFormat::Intensity16:return "Intensity16";
			case SizedInternalFormat::Rgb2Ext:return "Rgb2Ext";
			case SizedInternalFormat::Rgb4:return "Rgb4";
			case SizedInternalFormat::Rgb5:return "Rgb5";
			case SizedInternalFormat::Rgb8:return "Rgb8";
			case SizedInternalFormat::Rgb10:return "Rgb10";
			case SizedInternalFormat::Rgb12:return "Rgb12";
			case SizedInternalFormat::Rgb16:return "Rgb16";
			case SizedInternalFormat::Rgba2:return "Rgba2";
			case SizedInternalFormat::Rgba4:return "Rgba4";
			case SizedInternalFormat::Rgb5A1:return "Rgb5A1";
			case SizedInternalFormat::Rgba8:return "Rgba8";
			case SizedInternalFormat::Rgb10A2:return "Rgb10A2";
			case SizedInternalFormat::Rgba12:return "Rgba12";
			case SizedInternalFormat::Rgba16:return "Rgba16";
			case SizedInternalFormat::DepthComponent16:return "DepthComponent16";
			case SizedInternalFormat::DepthComponent24:return "DepthComponent24";
			case SizedInternalFormat::DepthComponent32:return "DepthComponent32";
			case SizedInternalFormat::R8:return "R8";
			case SizedInternalFormat::R16:return "R16";
			case SizedInternalFormat::Rg8:return "Rg8";
			case SizedInternalFormat::Rg16:return "Rg16";
			case SizedInternalFormat::R16f:return "R16f";
			case SizedInternalFormat::R32f:return "R32f";
			case SizedInternalFormat::Rg16f:return "Rg16f";
			case SizedInternalFormat::Rg32f:return "Rg32f";
			case SizedInternalFormat::R8i:return "R8i";
			case SizedInternalFormat::R8ui:return "R8ui";
			case SizedInternalFormat::R16i:return "R16i";
			case SizedInternalFormat::R16ui:return "R16ui";
			case SizedInternalFormat::R32i:return "R32i";
			case SizedInternalFormat::R32ui:return "R32ui";
			case SizedInternalFormat::Rg8i:return "Rg8i";
			case SizedInternalFormat::Rg8ui:return "Rg8ui";
			case SizedInternalFormat::Rg16i:return "Rg16i";
			case SizedInternalFormat::Rg16ui:return "Rg16ui";
			case SizedInternalFormat::Rg32i:return "Rg32i";
			case SizedInternalFormat::Rg32ui:return "Rg32ui";
			case SizedInternalFormat::CompressedRgbS3tcDxt1Ext:return "CompressedRgbS3tcDxt1Ext";
			case SizedInternalFormat::CompressedRgbaS3tcDxt1Ext:return "CompressedRgbaS3tcDxt1Ext";
			case SizedInternalFormat::CompressedRgbaS3tcDxt3Angle:return "CompressedRgbaS3tcDxt3Angle";
			case SizedInternalFormat::CompressedRgbaS3tcDxt5Angle:return "CompressedRgbaS3tcDxt5Angle";
			case SizedInternalFormat::Rgba32f:return "Rgba32f";
			case SizedInternalFormat::Rgb32f:return "Rgb32f";
			case SizedInternalFormat::Rgba16f:return "Rgba16f";
			case SizedInternalFormat::Rgb16f:return "Rgb16f";
			case SizedInternalFormat::Depth24Stencil8:return "Depth24Stencil8";
			case SizedInternalFormat::R11fG11fB10f:return "R11fG11fB10f";
			case SizedInternalFormat::Rgb9E5:return "Rgb9E5";
			case SizedInternalFormat::Srgb8:return "Srgb8";
			case SizedInternalFormat::Srgb8Alpha8:return "Srgb8Alpha8";
			case SizedInternalFormat::CompressedSrgbS3tcDxt1Ext:return "CompressedSrgbS3tcDxt1Ext";
			case SizedInternalFormat::CompressedSrgbAlphaS3tcDxt1Ext:return "CompressedSrgbAlphaS3tcDxt1Ext";
			case SizedInternalFormat::CompressedSrgbAlphaS3tcDxt3Ext:return "CompressedSrgbAlphaS3tcDxt3Ext";
			case SizedInternalFormat::CompressedSrgbAlphaS3tcDxt5Ext:return "CompressedSrgbAlphaS3tcDxt5Ext";
			case SizedInternalFormat::DepthComponent32f:return "DepthComponent32f";
			case SizedInternalFormat::Depth32fStencil8:return "Depth32fStencil8";
			case SizedInternalFormat::StencilIndex1:return "StencilIndex1";
			case SizedInternalFormat::StencilIndex4:return "StencilIndex4";
			case SizedInternalFormat::StencilIndex8:return "StencilIndex8";
			case SizedInternalFormat::StencilIndex16:return "StencilIndex16";
			case SizedInternalFormat::Rgb565Oes:return "Rgb565Oes";
			case SizedInternalFormat::Etc1Rgb8Oes:return "Etc1Rgb8Oes";
			case SizedInternalFormat::Rgba32ui:return "Rgba32ui";
			case SizedInternalFormat::Rgb32ui:return "Rgb32ui";
			case SizedInternalFormat::Alpha32uiExt:return "Alpha32uiExt";
			case SizedInternalFormat::Intensity32uiExt:return "Intensity32uiExt";
			case SizedInternalFormat::Luminance32uiExt:return "Luminance32uiExt";
			case SizedInternalFormat::LuminanceAlpha32uiExt:return "LuminanceAlpha32uiExt";
			case SizedInternalFormat::Rgba16ui:return "Rgba16ui";
			case SizedInternalFormat::Rgb16ui:return "Rgb16ui";
			case SizedInternalFormat::Alpha16uiExt:return "Alpha16uiExt";
			case SizedInternalFormat::Intensity16uiExt:return "Intensity16uiExt";
			case SizedInternalFormat::Luminance16uiExt:return "Luminance16uiExt";
			case SizedInternalFormat::LuminanceAlpha16uiExt:return "LuminanceAlpha16uiExt";
			case SizedInternalFormat::Rgba8ui:return "Rgba8ui";
			case SizedInternalFormat::Rgb8ui:return "Rgb8ui";
			case SizedInternalFormat::Alpha8uiExt:return "Alpha8uiExt";
			case SizedInternalFormat::Intensity8uiExt:return "Intensity8uiExt";
			case SizedInternalFormat::Luminance8uiExt:return "Luminance8uiExt";
			case SizedInternalFormat::LuminanceAlpha8uiExt:return "LuminanceAlpha8uiExt";
			case SizedInternalFormat::Rgba32i:return "Rgba32i";
			case SizedInternalFormat::Rgb32i:return "Rgb32i";
			case SizedInternalFormat::Alpha32iExt:return "Alpha32iExt";
			case SizedInternalFormat::Intensity32iExt:return "Intensity32iExt";
			case SizedInternalFormat::Luminance32iExt:return "Luminance32iExt";
			case SizedInternalFormat::LuminanceAlpha32iExt:return "LuminanceAlpha32iExt";
			case SizedInternalFormat::Rgba16i:return "Rgba16i";
			case SizedInternalFormat::Rgb16i:return "Rgb16i";
			case SizedInternalFormat::Alpha16iExt:return "Alpha16iExt";
			case SizedInternalFormat::Intensity16iExt:return "Intensity16iExt";
			case SizedInternalFormat::Luminance16iExt:return "Luminance16iExt";
			case SizedInternalFormat::LuminanceAlpha16iExt:return "LuminanceAlpha16iExt";
			case SizedInternalFormat::Rgba8i:return "Rgba8i";
			case SizedInternalFormat::Rgb8i:return "Rgb8i";
			case SizedInternalFormat::Alpha8iExt:return "Alpha8iExt";
			case SizedInternalFormat::Intensity8iExt:return "Intensity8iExt";
			case SizedInternalFormat::Luminance8iExt:return "Luminance8iExt";
			case SizedInternalFormat::LuminanceAlpha8iExt:return "LuminanceAlpha8iExt";
			case SizedInternalFormat::DepthComponent32fNv:return "DepthComponent32fNv";
			case SizedInternalFormat::Depth32fStencil8Nv:return "Depth32fStencil8Nv";
			case SizedInternalFormat::CompressedRedRgtc1:return "CompressedRedRgtc1";
			case SizedInternalFormat::CompressedSignedRedRgtc1:return "CompressedSignedRedRgtc1";
			case SizedInternalFormat::CompressedRedGreenRgtc2Ext:return "CompressedRedGreenRgtc2Ext";
			case SizedInternalFormat::CompressedSignedRedGreenRgtc2Ext:return "CompressedSignedRedGreenRgtc2Ext";
			case SizedInternalFormat::CompressedRgbaBptcUnorm:return "CompressedRgbaBptcUnorm";
			case SizedInternalFormat::CompressedSrgbAlphaBptcUnorm:return "CompressedSrgbAlphaBptcUnorm";
			case SizedInternalFormat::CompressedRgbBptcSignedFloat:return "CompressedRgbBptcSignedFloat";
			case SizedInternalFormat::CompressedRgbBptcUnsignedFloat:return "CompressedRgbBptcUnsignedFloat";
			case SizedInternalFormat::R8Snorm:return "R8Snorm";
			case SizedInternalFormat::Rg8Snorm:return "Rg8Snorm";
			case SizedInternalFormat::Rgb8Snorm:return "Rgb8Snorm";
			case SizedInternalFormat::Rgba8Snorm:return "Rgba8Snorm";
			case SizedInternalFormat::R16Snorm:return "R16Snorm";
			case SizedInternalFormat::Rg16Snorm:return "Rg16Snorm";
			case SizedInternalFormat::Rgb16Snorm:return "Rgb16Snorm";
			case SizedInternalFormat::Rgba16Snorm:return "Rgba16Snorm";
			case SizedInternalFormat::Rgb10A2ui:return "Rgb10A2ui";
			case SizedInternalFormat::CompressedR11Eac:return "CompressedR11Eac";
			case SizedInternalFormat::CompressedSignedR11Eac:return "CompressedSignedR11Eac";
			case SizedInternalFormat::CompressedRg11Eac:return "CompressedRg11Eac";
			case SizedInternalFormat::CompressedSignedRg11Eac:return "CompressedSignedRg11Eac";
			case SizedInternalFormat::CompressedRgb8Etc2:return "CompressedRgb8Etc2";
			case SizedInternalFormat::CompressedSrgb8Etc2:return "CompressedSrgb8Etc2";
			case SizedInternalFormat::CompressedRgb8PunchthroughAlpha1Etc2:return "CompressedRgb8PunchthroughAlpha1Etc2";
			case SizedInternalFormat::CompressedSrgb8PunchthroughAlpha1Etc2:return "CompressedSrgb8PunchthroughAlpha1Etc2";
			case SizedInternalFormat::CompressedRgba8Etc2Eac:return "CompressedRgba8Etc2Eac";
			case SizedInternalFormat::CompressedSrgb8Alpha8Etc2Eac:return "CompressedSrgb8Alpha8Etc2Eac";
			case SizedInternalFormat::CompressedRgbaAstc4X4:return "CompressedRgbaAstc4X4";
			case SizedInternalFormat::CompressedRgbaAstc5X4:return "CompressedRgbaAstc5X4";
			case SizedInternalFormat::CompressedRgbaAstc5X5:return "CompressedRgbaAstc5X5";
			case SizedInternalFormat::CompressedRgbaAstc6X5:return "CompressedRgbaAstc6X5";
			case SizedInternalFormat::CompressedRgbaAstc6X6:return "CompressedRgbaAstc6X6";
			case SizedInternalFormat::CompressedRgbaAstc8X5:return "CompressedRgbaAstc8X5";
			case SizedInternalFormat::CompressedRgbaAstc8X6:return "CompressedRgbaAstc8X6";
			case SizedInternalFormat::CompressedRgbaAstc8X8:return "CompressedRgbaAstc8X8";
			case SizedInternalFormat::CompressedRgbaAstc10X5:return "CompressedRgbaAstc10X5";
			case SizedInternalFormat::CompressedRgbaAstc10X6:return "CompressedRgbaAstc10X6";
			case SizedInternalFormat::CompressedRgbaAstc10X8:return "CompressedRgbaAstc10X8";
			case SizedInternalFormat::CompressedRgbaAstc10X10:return "CompressedRgbaAstc10X10";
			case SizedInternalFormat::CompressedRgbaAstc12X10:return "CompressedRgbaAstc12X10";
			case SizedInternalFormat::CompressedRgbaAstc12X12:return "CompressedRgbaAstc12X12";
			case SizedInternalFormat::CompressedRgbaAstc3X3x3Oes:return "CompressedRgbaAstc3X3x3Oes";
			case SizedInternalFormat::CompressedRgbaAstc4X3x3Oes:return "CompressedRgbaAstc4X3x3Oes";
			case SizedInternalFormat::CompressedRgbaAstc4X4x3Oes:return "CompressedRgbaAstc4X4x3Oes";
			case SizedInternalFormat::CompressedRgbaAstc4X4x4Oes:return "CompressedRgbaAstc4X4x4Oes";
			case SizedInternalFormat::CompressedRgbaAstc5X4x4Oes:return "CompressedRgbaAstc5X4x4Oes";
			case SizedInternalFormat::CompressedRgbaAstc5X5x4Oes:return "CompressedRgbaAstc5X5x4Oes";
			case SizedInternalFormat::CompressedRgbaAstc5X5x5Oes:return "CompressedRgbaAstc5X5x5Oes";
			case SizedInternalFormat::CompressedRgbaAstc6X5x5Oes:return "CompressedRgbaAstc6X5x5Oes";
			case SizedInternalFormat::CompressedRgbaAstc6X6x5Oes:return "CompressedRgbaAstc6X6x5Oes";
			case SizedInternalFormat::CompressedRgbaAstc6X6x6Oes:return "CompressedRgbaAstc6X6x6Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc4X4:return "CompressedSrgb8Alpha8Astc4X4";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc5X4:return "CompressedSrgb8Alpha8Astc5X4";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc5X5:return "CompressedSrgb8Alpha8Astc5X5";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc6X5:return "CompressedSrgb8Alpha8Astc6X5";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc6X6:return "CompressedSrgb8Alpha8Astc6X6";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc8X5:return "CompressedSrgb8Alpha8Astc8X5";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc8X6:return "CompressedSrgb8Alpha8Astc8X6";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc8X8:return "CompressedSrgb8Alpha8Astc8X8";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc10X5:return "CompressedSrgb8Alpha8Astc10X5";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc10X6:return "CompressedSrgb8Alpha8Astc10X6";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc10X8:return "CompressedSrgb8Alpha8Astc10X8";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc10X10:return "CompressedSrgb8Alpha8Astc10X10";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc12X10:return "CompressedSrgb8Alpha8Astc12X10";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc12X12:return "CompressedSrgb8Alpha8Astc12X12";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc3X3x3Oes:return "CompressedSrgb8Alpha8Astc3X3x3Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc4X3x3Oes:return "CompressedSrgb8Alpha8Astc4X3x3Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc4X4x3Oes:return "CompressedSrgb8Alpha8Astc4X4x3Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc4X4x4Oes:return "CompressedSrgb8Alpha8Astc4X4x4Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc5X4x4Oes:return "CompressedSrgb8Alpha8Astc5X4x4Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc5X5x4Oes:return "CompressedSrgb8Alpha8Astc5X5x4Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc5X5x5Oes:return "CompressedSrgb8Alpha8Astc5X5x5Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc6X5x5Oes:return "CompressedSrgb8Alpha8Astc6X5x5Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc6X6x5Oes:return "CompressedSrgb8Alpha8Astc6X6x5Oes";
			case SizedInternalFormat::CompressedSrgb8Alpha8Astc6X6x6Oes:return "CompressedSrgb8Alpha8Astc6X6x6Oes";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(InterleavedArrayFormat value) {
		switch(value){
			case InterleavedArrayFormat::V2f:return "V2f";
			case InterleavedArrayFormat::V3f:return "V3f";
			case InterleavedArrayFormat::C4ubV2f:return "C4ubV2f";
			case InterleavedArrayFormat::C4ubV3f:return "C4ubV3f";
			case InterleavedArrayFormat::C3fV3f:return "C3fV3f";
			case InterleavedArrayFormat::N3fV3f:return "N3fV3f";
			case InterleavedArrayFormat::C4fN3fV3f:return "C4fN3fV3f";
			case InterleavedArrayFormat::T2fV3f:return "T2fV3f";
			case InterleavedArrayFormat::T4fV4f:return "T4fV4f";
			case InterleavedArrayFormat::T2fC4ubV3f:return "T2fC4ubV3f";
			case InterleavedArrayFormat::T2fC3fV3f:return "T2fC3fV3f";
			case InterleavedArrayFormat::T2fN3fV3f:return "T2fN3fV3f";
			case InterleavedArrayFormat::T2fC4fN3fV3f:return "T2fC4fN3fV3f";
			case InterleavedArrayFormat::T4fC4fN3fV4f:return "T4fC4fN3fV4f";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClipPlaneName value) {
		switch(value){
			case ClipPlaneName::ClipPlane0:return "ClipPlane0";
			case ClipPlaneName::ClipPlane1:return "ClipPlane1";
			case ClipPlaneName::ClipPlane2:return "ClipPlane2";
			case ClipPlaneName::ClipPlane3:return "ClipPlane3";
			case ClipPlaneName::ClipPlane4:return "ClipPlane4";
			case ClipPlaneName::ClipPlane5:return "ClipPlane5";
			case ClipPlaneName::ClipDistance6:return "ClipDistance6";
			case ClipPlaneName::ClipDistance7:return "ClipDistance7";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightName value) {
		switch(value){
			case LightName::Light0:return "Light0";
			case LightName::Light1:return "Light1";
			case LightName::Light2:return "Light2";
			case LightName::Light3:return "Light3";
			case LightName::Light4:return "Light4";
			case LightName::Light5:return "Light5";
			case LightName::Light6:return "Light6";
			case LightName::Light7:return "Light7";
			case LightName::FragmentLight0Sgix:return "FragmentLight0Sgix";
			case LightName::FragmentLight1Sgix:return "FragmentLight1Sgix";
			case LightName::FragmentLight2Sgix:return "FragmentLight2Sgix";
			case LightName::FragmentLight3Sgix:return "FragmentLight3Sgix";
			case LightName::FragmentLight4Sgix:return "FragmentLight4Sgix";
			case LightName::FragmentLight5Sgix:return "FragmentLight5Sgix";
			case LightName::FragmentLight6Sgix:return "FragmentLight6Sgix";
			case LightName::FragmentLight7Sgix:return "FragmentLight7Sgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BlendEquationModeEXT value) {
		switch(value){
			case BlendEquationModeEXT::FuncAdd:return "FuncAdd";
			case BlendEquationModeEXT::Min:return "Min";
			case BlendEquationModeEXT::Max:return "Max";
			case BlendEquationModeEXT::FuncSubtract:return "FuncSubtract";
			case BlendEquationModeEXT::FuncReverseSubtract:return "FuncReverseSubtract";
			case BlendEquationModeEXT::AlphaMinSgix:return "AlphaMinSgix";
			case BlendEquationModeEXT::AlphaMaxSgix:return "AlphaMaxSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ConvolutionTarget value) {
		switch(value){
			case ConvolutionTarget::Convolution1D:return "Convolution1D";
			case ConvolutionTarget::Convolution2D:return "Convolution2D";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ConvolutionTargetEXT value) {
		switch(value){
			case ConvolutionTargetEXT::Convolution1D:return "Convolution1D";
			case ConvolutionTargetEXT::Convolution2D:return "Convolution2D";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SeparableTarget value) {
		switch(value){
			case SeparableTarget::Separable2D:return "Separable2D";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SeparableTargetEXT value) {
		switch(value){
			case SeparableTargetEXT::Separable2D:return "Separable2D";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ConvolutionParameter value) {
		switch(value){
			case ConvolutionParameter::ConvolutionBorderMode:return "ConvolutionBorderMode";
			case ConvolutionParameter::ConvolutionFilterScale:return "ConvolutionFilterScale";
			case ConvolutionParameter::ConvolutionFilterBias:return "ConvolutionFilterBias";
			case ConvolutionParameter::ConvolutionFormat:return "ConvolutionFormat";
			case ConvolutionParameter::ConvolutionWidth:return "ConvolutionWidth";
			case ConvolutionParameter::ConvolutionHeight:return "ConvolutionHeight";
			case ConvolutionParameter::MaxConvolutionWidth:return "MaxConvolutionWidth";
			case ConvolutionParameter::MaxConvolutionHeight:return "MaxConvolutionHeight";
			case ConvolutionParameter::ConvolutionBorderColor:return "ConvolutionBorderColor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ConvolutionBorderModeEXT value) {
		switch(value){
			case ConvolutionBorderModeEXT::Reduce:return "Reduce";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(HistogramTarget value) {
		switch(value){
			case HistogramTarget::Histogram:return "Histogram";
			case HistogramTarget::ProxyHistogram:return "ProxyHistogram";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(HistogramTargetEXT value) {
		switch(value){
			case HistogramTargetEXT::Histogram:return "Histogram";
			case HistogramTargetEXT::ProxyHistogram:return "ProxyHistogram";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetHistogramParameterPNameEXT value) {
		switch(value){
			case GetHistogramParameterPNameEXT::HistogramWidth:return "HistogramWidth";
			case GetHistogramParameterPNameEXT::HistogramFormat:return "HistogramFormat";
			case GetHistogramParameterPNameEXT::HistogramRedSize:return "HistogramRedSize";
			case GetHistogramParameterPNameEXT::HistogramGreenSize:return "HistogramGreenSize";
			case GetHistogramParameterPNameEXT::HistogramBlueSize:return "HistogramBlueSize";
			case GetHistogramParameterPNameEXT::HistogramAlphaSize:return "HistogramAlphaSize";
			case GetHistogramParameterPNameEXT::HistogramLuminanceSize:return "HistogramLuminanceSize";
			case GetHistogramParameterPNameEXT::HistogramSink:return "HistogramSink";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MinmaxTarget value) {
		switch(value){
			case MinmaxTarget::Minmax:return "Minmax";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MinmaxTargetEXT value) {
		switch(value){
			case MinmaxTargetEXT::Minmax:return "Minmax";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetMinmaxParameterPNameEXT value) {
		switch(value){
			case GetMinmaxParameterPNameEXT::MinmaxFormat:return "MinmaxFormat";
			case GetMinmaxParameterPNameEXT::MinmaxSink:return "MinmaxSink";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SamplePatternSGIS value) {
		switch(value){
			case SamplePatternSGIS::_1PassExt:return "_1PassExt";
			case SamplePatternSGIS::_2Pass0Ext:return "_2Pass0Ext";
			case SamplePatternSGIS::_2Pass1Ext:return "_2Pass1Ext";
			case SamplePatternSGIS::_4Pass0Ext:return "_4Pass0Ext";
			case SamplePatternSGIS::_4Pass1Ext:return "_4Pass1Ext";
			case SamplePatternSGIS::_4Pass2Ext:return "_4Pass2Ext";
			case SamplePatternSGIS::_4Pass3Ext:return "_4Pass3Ext";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SamplePatternEXT value) {
		switch(value){
			case SamplePatternEXT::_1PassExt:return "_1PassExt";
			case SamplePatternEXT::_2Pass0Ext:return "_2Pass0Ext";
			case SamplePatternEXT::_2Pass1Ext:return "_2Pass1Ext";
			case SamplePatternEXT::_4Pass0Ext:return "_4Pass0Ext";
			case SamplePatternEXT::_4Pass1Ext:return "_4Pass1Ext";
			case SamplePatternEXT::_4Pass2Ext:return "_4Pass2Ext";
			case SamplePatternEXT::_4Pass3Ext:return "_4Pass3Ext";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(InternalFormatPName value) {
		switch(value){
			case InternalFormatPName::Samples:return "Samples";
			case InternalFormatPName::GenerateMipmap:return "GenerateMipmap";
			case InternalFormatPName::InternalformatSupported:return "InternalformatSupported";
			case InternalFormatPName::InternalformatPreferred:return "InternalformatPreferred";
			case InternalFormatPName::InternalformatRedSize:return "InternalformatRedSize";
			case InternalFormatPName::InternalformatGreenSize:return "InternalformatGreenSize";
			case InternalFormatPName::InternalformatBlueSize:return "InternalformatBlueSize";
			case InternalFormatPName::InternalformatAlphaSize:return "InternalformatAlphaSize";
			case InternalFormatPName::InternalformatDepthSize:return "InternalformatDepthSize";
			case InternalFormatPName::InternalformatStencilSize:return "InternalformatStencilSize";
			case InternalFormatPName::InternalformatSharedSize:return "InternalformatSharedSize";
			case InternalFormatPName::InternalformatRedType:return "InternalformatRedType";
			case InternalFormatPName::InternalformatGreenType:return "InternalformatGreenType";
			case InternalFormatPName::InternalformatBlueType:return "InternalformatBlueType";
			case InternalFormatPName::InternalformatAlphaType:return "InternalformatAlphaType";
			case InternalFormatPName::InternalformatDepthType:return "InternalformatDepthType";
			case InternalFormatPName::InternalformatStencilType:return "InternalformatStencilType";
			case InternalFormatPName::MaxWidth:return "MaxWidth";
			case InternalFormatPName::MaxHeight:return "MaxHeight";
			case InternalFormatPName::MaxDepth:return "MaxDepth";
			case InternalFormatPName::MaxLayers:return "MaxLayers";
			case InternalFormatPName::ColorComponents:return "ColorComponents";
			case InternalFormatPName::ColorRenderable:return "ColorRenderable";
			case InternalFormatPName::DepthRenderable:return "DepthRenderable";
			case InternalFormatPName::StencilRenderable:return "StencilRenderable";
			case InternalFormatPName::FramebufferRenderable:return "FramebufferRenderable";
			case InternalFormatPName::FramebufferRenderableLayered:return "FramebufferRenderableLayered";
			case InternalFormatPName::FramebufferBlend:return "FramebufferBlend";
			case InternalFormatPName::ReadPixels:return "ReadPixels";
			case InternalFormatPName::ReadPixelsFormat:return "ReadPixelsFormat";
			case InternalFormatPName::ReadPixelsType:return "ReadPixelsType";
			case InternalFormatPName::TextureImageFormat:return "TextureImageFormat";
			case InternalFormatPName::TextureImageType:return "TextureImageType";
			case InternalFormatPName::GetTextureImageFormat:return "GetTextureImageFormat";
			case InternalFormatPName::GetTextureImageType:return "GetTextureImageType";
			case InternalFormatPName::Mipmap:return "Mipmap";
			case InternalFormatPName::/* Should be deprecated */ AutoGenerateMipmap:return "/* Should be deprecated */ AutoGenerateMipmap";
			case InternalFormatPName::ColorEncoding:return "ColorEncoding";
			case InternalFormatPName::SrgbRead:return "SrgbRead";
			case InternalFormatPName::SrgbWrite:return "SrgbWrite";
			case InternalFormatPName::Filter:return "Filter";
			case InternalFormatPName::VertexTexture:return "VertexTexture";
			case InternalFormatPName::TessControlTexture:return "TessControlTexture";
			case InternalFormatPName::TessEvaluationTexture:return "TessEvaluationTexture";
			case InternalFormatPName::GeometryTexture:return "GeometryTexture";
			case InternalFormatPName::FragmentTexture:return "FragmentTexture";
			case InternalFormatPName::ComputeTexture:return "ComputeTexture";
			case InternalFormatPName::TextureShadow:return "TextureShadow";
			case InternalFormatPName::TextureGather:return "TextureGather";
			case InternalFormatPName::TextureGatherShadow:return "TextureGatherShadow";
			case InternalFormatPName::ShaderImageLoad:return "ShaderImageLoad";
			case InternalFormatPName::ShaderImageStore:return "ShaderImageStore";
			case InternalFormatPName::ShaderImageAtomic:return "ShaderImageAtomic";
			case InternalFormatPName::ImageTexelSize:return "ImageTexelSize";
			case InternalFormatPName::ImageCompatibilityClass:return "ImageCompatibilityClass";
			case InternalFormatPName::ImagePixelFormat:return "ImagePixelFormat";
			case InternalFormatPName::ImagePixelType:return "ImagePixelType";
			case InternalFormatPName::SimultaneousTextureAndDepthTest:return "SimultaneousTextureAndDepthTest";
			case InternalFormatPName::SimultaneousTextureAndStencilTest:return "SimultaneousTextureAndStencilTest";
			case InternalFormatPName::SimultaneousTextureAndDepthWrite:return "SimultaneousTextureAndDepthWrite";
			case InternalFormatPName::SimultaneousTextureAndStencilWrite:return "SimultaneousTextureAndStencilWrite";
			case InternalFormatPName::TextureCompressedBlockWidth:return "TextureCompressedBlockWidth";
			case InternalFormatPName::TextureCompressedBlockHeight:return "TextureCompressedBlockHeight";
			case InternalFormatPName::TextureCompressedBlockSize:return "TextureCompressedBlockSize";
			case InternalFormatPName::ClearBuffer:return "ClearBuffer";
			case InternalFormatPName::TextureView:return "TextureView";
			case InternalFormatPName::ViewCompatibilityClass:return "ViewCompatibilityClass";
			case InternalFormatPName::TextureCompressed:return "TextureCompressed";
			case InternalFormatPName::NumSurfaceCompressionFixedRatesExt:return "NumSurfaceCompressionFixedRatesExt";
			case InternalFormatPName::ImageFormatCompatibilityType:return "ImageFormatCompatibilityType";
			case InternalFormatPName::ClearTexture:return "ClearTexture";
			case InternalFormatPName::NumSampleCounts:return "NumSampleCounts";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorTableTargetSGI value) {
		switch(value){
			case ColorTableTargetSGI::TextureColorTableSgi:return "TextureColorTableSgi";
			case ColorTableTargetSGI::ProxyTextureColorTableSgi:return "ProxyTextureColorTableSgi";
			case ColorTableTargetSGI::ColorTable:return "ColorTable";
			case ColorTableTargetSGI::PostConvolutionColorTable:return "PostConvolutionColorTable";
			case ColorTableTargetSGI::PostColorMatrixColorTable:return "PostColorMatrixColorTable";
			case ColorTableTargetSGI::ProxyColorTable:return "ProxyColorTable";
			case ColorTableTargetSGI::ProxyPostConvolutionColorTable:return "ProxyPostConvolutionColorTable";
			case ColorTableTargetSGI::ProxyPostColorMatrixColorTable:return "ProxyPostColorMatrixColorTable";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorTableTarget value) {
		switch(value){
			case ColorTableTarget::ColorTable:return "ColorTable";
			case ColorTableTarget::PostConvolutionColorTable:return "PostConvolutionColorTable";
			case ColorTableTarget::PostColorMatrixColorTable:return "PostColorMatrixColorTable";
			case ColorTableTarget::ProxyColorTable:return "ProxyColorTable";
			case ColorTableTarget::ProxyPostConvolutionColorTable:return "ProxyPostConvolutionColorTable";
			case ColorTableTarget::ProxyPostColorMatrixColorTable:return "ProxyPostColorMatrixColorTable";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ColorTableParameterPName value) {
		switch(value){
			case ColorTableParameterPName::ColorTableScale:return "ColorTableScale";
			case ColorTableParameterPName::ColorTableBias:return "ColorTableBias";
			case ColorTableParameterPName::ColorTableFormat:return "ColorTableFormat";
			case ColorTableParameterPName::ColorTableWidth:return "ColorTableWidth";
			case ColorTableParameterPName::ColorTableRedSize:return "ColorTableRedSize";
			case ColorTableParameterPName::ColorTableGreenSize:return "ColorTableGreenSize";
			case ColorTableParameterPName::ColorTableBlueSize:return "ColorTableBlueSize";
			case ColorTableParameterPName::ColorTableAlphaSize:return "ColorTableAlphaSize";
			case ColorTableParameterPName::ColorTableLuminanceSize:return "ColorTableLuminanceSize";
			case ColorTableParameterPName::ColorTableIntensitySize:return "ColorTableIntensitySize";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferTargetARB value) {
		switch(value){
			case BufferTargetARB::ParameterBuffer:return "ParameterBuffer";
			case BufferTargetARB::ArrayBuffer:return "ArrayBuffer";
			case BufferTargetARB::ElementArrayBuffer:return "ElementArrayBuffer";
			case BufferTargetARB::PixelPackBuffer:return "PixelPackBuffer";
			case BufferTargetARB::PixelUnpackBuffer:return "PixelUnpackBuffer";
			case BufferTargetARB::UniformBuffer:return "UniformBuffer";
			case BufferTargetARB::TextureBuffer:return "TextureBuffer";
			case BufferTargetARB::TransformFeedbackBuffer:return "TransformFeedbackBuffer";
			case BufferTargetARB::CopyReadBuffer:return "CopyReadBuffer";
			case BufferTargetARB::CopyWriteBuffer:return "CopyWriteBuffer";
			case BufferTargetARB::DrawIndirectBuffer:return "DrawIndirectBuffer";
			case BufferTargetARB::ShaderStorageBuffer:return "ShaderStorageBuffer";
			case BufferTargetARB::DispatchIndirectBuffer:return "DispatchIndirectBuffer";
			case BufferTargetARB::QueryBuffer:return "QueryBuffer";
			case BufferTargetARB::AtomicCounterBuffer:return "AtomicCounterBuffer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PointParameterNameARB value) {
		switch(value){
			case PointParameterNameARB::PointSizeMin:return "PointSizeMin";
			case PointParameterNameARB::PointSizeMax:return "PointSizeMax";
			case PointParameterNameARB::PointFadeThresholdSize:return "PointFadeThresholdSize";
			case PointParameterNameARB::DistanceAttenuationExt:return "DistanceAttenuationExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureFilterSGIS value) {
		switch(value){
			case TextureFilterSGIS::Filter4Sgis:return "Filter4Sgis";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SpriteParameterNameSGIX value) {
		switch(value){
			case SpriteParameterNameSGIX::SpriteModeSgix:return "SpriteModeSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SpriteModeSGIX value) {
		switch(value){
			case SpriteModeSGIX::SpriteAxialSgix:return "SpriteAxialSgix";
			case SpriteModeSGIX::SpriteObjectAlignedSgix:return "SpriteObjectAlignedSgix";
			case SpriteModeSGIX::SpriteEyeAlignedSgix:return "SpriteEyeAlignedSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ImageTransformPNameHP value) {
		switch(value){
			case ImageTransformPNameHP::ImageScaleXHp:return "ImageScaleXHp";
			case ImageTransformPNameHP::ImageScaleYHp:return "ImageScaleYHp";
			case ImageTransformPNameHP::ImageTranslateXHp:return "ImageTranslateXHp";
			case ImageTransformPNameHP::ImageTranslateYHp:return "ImageTranslateYHp";
			case ImageTransformPNameHP::ImageRotateAngleHp:return "ImageRotateAngleHp";
			case ImageTransformPNameHP::ImageRotateOriginXHp:return "ImageRotateOriginXHp";
			case ImageTransformPNameHP::ImageRotateOriginYHp:return "ImageRotateOriginYHp";
			case ImageTransformPNameHP::ImageMagFilterHp:return "ImageMagFilterHp";
			case ImageTransformPNameHP::ImageMinFilterHp:return "ImageMinFilterHp";
			case ImageTransformPNameHP::ImageCubicWeightHp:return "ImageCubicWeightHp";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ImageTransformTargetHP value) {
		switch(value){
			case ImageTransformTargetHP::ImageTransform2DHp:return "ImageTransform2DHp";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ListParameterName value) {
		switch(value){
			case ListParameterName::ListPrioritySgix:return "ListPrioritySgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FfdTargetSGIX value) {
		switch(value){
			case FfdTargetSGIX::GeometryDeformationSgix:return "GeometryDeformationSgix";
			case FfdTargetSGIX::TextureDeformationSgix:return "TextureDeformationSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CullParameterEXT value) {
		switch(value){
			case CullParameterEXT::CullVertexEyePositionExt:return "CullVertexEyePositionExt";
			case CullParameterEXT::CullVertexObjectPositionExt:return "CullVertexObjectPositionExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightModelColorControl value) {
		switch(value){
			case LightModelColorControl::SingleColor:return "SingleColor";
			case LightModelColorControl::SeparateSpecularColor:return "SeparateSpecularColor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramTarget value) {
		switch(value){
			case ProgramTarget::TextFragmentShaderAti:return "TextFragmentShaderAti";
			case ProgramTarget::VertexProgramArb:return "VertexProgramArb";
			case ProgramTarget::FragmentProgramArb:return "FragmentProgramArb";
			case ProgramTarget::TessControlProgramNv:return "TessControlProgramNv";
			case ProgramTarget::TessEvaluationProgramNv:return "TessEvaluationProgramNv";
			case ProgramTarget::GeometryProgramNv:return "GeometryProgramNv";
			case ProgramTarget::ComputeProgramNv:return "ComputeProgramNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FramebufferAttachmentParameterName value) {
		switch(value){
			case FramebufferAttachmentParameterName::FramebufferAttachmentColorEncoding:return "FramebufferAttachmentColorEncoding";
			case FramebufferAttachmentParameterName::FramebufferAttachmentComponentType:return "FramebufferAttachmentComponentType";
			case FramebufferAttachmentParameterName::FramebufferAttachmentRedSize:return "FramebufferAttachmentRedSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentGreenSize:return "FramebufferAttachmentGreenSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentBlueSize:return "FramebufferAttachmentBlueSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentAlphaSize:return "FramebufferAttachmentAlphaSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentDepthSize:return "FramebufferAttachmentDepthSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentStencilSize:return "FramebufferAttachmentStencilSize";
			case FramebufferAttachmentParameterName::FramebufferAttachmentObjectType:return "FramebufferAttachmentObjectType";
			case FramebufferAttachmentParameterName::FramebufferAttachmentObjectName:return "FramebufferAttachmentObjectName";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureLevel:return "FramebufferAttachmentTextureLevel";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureCubeMapFace:return "FramebufferAttachmentTextureCubeMapFace";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTexture3DZoffsetExt:return "FramebufferAttachmentTexture3DZoffsetExt";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureSamplesExt:return "FramebufferAttachmentTextureSamplesExt";
			case FramebufferAttachmentParameterName::FramebufferAttachmentLayered:return "FramebufferAttachmentLayered";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureScaleImg:return "FramebufferAttachmentTextureScaleImg";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureNumViewsOvr:return "FramebufferAttachmentTextureNumViewsOvr";
			case FramebufferAttachmentParameterName::FramebufferAttachmentTextureBaseViewIndexOvr:return "FramebufferAttachmentTextureBaseViewIndexOvr";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FramebufferStatus value) {
		switch(value){
			case FramebufferStatus::FramebufferUndefined:return "FramebufferUndefined";
			case FramebufferStatus::FramebufferComplete:return "FramebufferComplete";
			case FramebufferStatus::FramebufferIncompleteAttachment:return "FramebufferIncompleteAttachment";
			case FramebufferStatus::FramebufferIncompleteMissingAttachment:return "FramebufferIncompleteMissingAttachment";
			case FramebufferStatus::FramebufferIncompleteDrawBuffer:return "FramebufferIncompleteDrawBuffer";
			case FramebufferStatus::FramebufferIncompleteReadBuffer:return "FramebufferIncompleteReadBuffer";
			case FramebufferStatus::FramebufferUnsupported:return "FramebufferUnsupported";
			case FramebufferStatus::FramebufferIncompleteMultisample:return "FramebufferIncompleteMultisample";
			case FramebufferStatus::FramebufferIncompleteLayerTargets:return "FramebufferIncompleteLayerTargets";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FramebufferAttachment value) {
		switch(value){
			case FramebufferAttachment::DepthStencilAttachment:return "DepthStencilAttachment";
			case FramebufferAttachment::ColorAttachment0:return "ColorAttachment0";
			case FramebufferAttachment::ColorAttachment1:return "ColorAttachment1";
			case FramebufferAttachment::ColorAttachment2:return "ColorAttachment2";
			case FramebufferAttachment::ColorAttachment3:return "ColorAttachment3";
			case FramebufferAttachment::ColorAttachment4:return "ColorAttachment4";
			case FramebufferAttachment::ColorAttachment5:return "ColorAttachment5";
			case FramebufferAttachment::ColorAttachment6:return "ColorAttachment6";
			case FramebufferAttachment::ColorAttachment7:return "ColorAttachment7";
			case FramebufferAttachment::ColorAttachment8:return "ColorAttachment8";
			case FramebufferAttachment::ColorAttachment9:return "ColorAttachment9";
			case FramebufferAttachment::ColorAttachment10:return "ColorAttachment10";
			case FramebufferAttachment::ColorAttachment11:return "ColorAttachment11";
			case FramebufferAttachment::ColorAttachment12:return "ColorAttachment12";
			case FramebufferAttachment::ColorAttachment13:return "ColorAttachment13";
			case FramebufferAttachment::ColorAttachment14:return "ColorAttachment14";
			case FramebufferAttachment::ColorAttachment15:return "ColorAttachment15";
			case FramebufferAttachment::ColorAttachment16:return "ColorAttachment16";
			case FramebufferAttachment::ColorAttachment17:return "ColorAttachment17";
			case FramebufferAttachment::ColorAttachment18:return "ColorAttachment18";
			case FramebufferAttachment::ColorAttachment19:return "ColorAttachment19";
			case FramebufferAttachment::ColorAttachment20:return "ColorAttachment20";
			case FramebufferAttachment::ColorAttachment21:return "ColorAttachment21";
			case FramebufferAttachment::ColorAttachment22:return "ColorAttachment22";
			case FramebufferAttachment::ColorAttachment23:return "ColorAttachment23";
			case FramebufferAttachment::ColorAttachment24:return "ColorAttachment24";
			case FramebufferAttachment::ColorAttachment25:return "ColorAttachment25";
			case FramebufferAttachment::ColorAttachment26:return "ColorAttachment26";
			case FramebufferAttachment::ColorAttachment27:return "ColorAttachment27";
			case FramebufferAttachment::ColorAttachment28:return "ColorAttachment28";
			case FramebufferAttachment::ColorAttachment29:return "ColorAttachment29";
			case FramebufferAttachment::ColorAttachment30:return "ColorAttachment30";
			case FramebufferAttachment::ColorAttachment31:return "ColorAttachment31";
			case FramebufferAttachment::DepthAttachment:return "DepthAttachment";
			case FramebufferAttachment::StencilAttachment:return "StencilAttachment";
			case FramebufferAttachment::ShadingRateAttachmentExt:return "ShadingRateAttachmentExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferPNameARB value) {
		switch(value){
			case BufferPNameARB::BufferImmutableStorage:return "BufferImmutableStorage";
			case BufferPNameARB::BufferStorageFlags:return "BufferStorageFlags";
			case BufferPNameARB::BufferSize:return "BufferSize";
			case BufferPNameARB::BufferUsage:return "BufferUsage";
			case BufferPNameARB::BufferAccess:return "BufferAccess";
			case BufferPNameARB::BufferMapped:return "BufferMapped";
			case BufferPNameARB::BufferAccessFlags:return "BufferAccessFlags";
			case BufferPNameARB::BufferMapLength:return "BufferMapLength";
			case BufferPNameARB::BufferMapOffset:return "BufferMapOffset";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramParameterPName value) {
		switch(value){
			case ProgramParameterPName::ProgramBinaryRetrievableHint:return "ProgramBinaryRetrievableHint";
			case ProgramParameterPName::ProgramSeparable:return "ProgramSeparable";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PipelineParameterName value) {
		switch(value){
			case PipelineParameterName::ActiveProgram:return "ActiveProgram";
			case PipelineParameterName::FragmentShader:return "FragmentShader";
			case PipelineParameterName::VertexShader:return "VertexShader";
			case PipelineParameterName::InfoLogLength:return "InfoLogLength";
			case PipelineParameterName::GeometryShader:return "GeometryShader";
			case PipelineParameterName::TessEvaluationShader:return "TessEvaluationShader";
			case PipelineParameterName::TessControlShader:return "TessControlShader";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramPropertyARB value) {
		switch(value){
			case ProgramPropertyARB::ComputeWorkGroupSize:return "ComputeWorkGroupSize";
			case ProgramPropertyARB::ProgramBinaryLength:return "ProgramBinaryLength";
			case ProgramPropertyARB::GeometryVerticesOut:return "GeometryVerticesOut";
			case ProgramPropertyARB::GeometryInputType:return "GeometryInputType";
			case ProgramPropertyARB::GeometryOutputType:return "GeometryOutputType";
			case ProgramPropertyARB::ActiveUniformBlockMaxNameLength:return "ActiveUniformBlockMaxNameLength";
			case ProgramPropertyARB::ActiveUniformBlocks:return "ActiveUniformBlocks";
			case ProgramPropertyARB::DeleteStatus:return "DeleteStatus";
			case ProgramPropertyARB::LinkStatus:return "LinkStatus";
			case ProgramPropertyARB::ValidateStatus:return "ValidateStatus";
			case ProgramPropertyARB::InfoLogLength:return "InfoLogLength";
			case ProgramPropertyARB::AttachedShaders:return "AttachedShaders";
			case ProgramPropertyARB::ActiveUniforms:return "ActiveUniforms";
			case ProgramPropertyARB::ActiveUniformMaxLength:return "ActiveUniformMaxLength";
			case ProgramPropertyARB::ActiveAttributes:return "ActiveAttributes";
			case ProgramPropertyARB::ActiveAttributeMaxLength:return "ActiveAttributeMaxLength";
			case ProgramPropertyARB::TransformFeedbackVaryingMaxLength:return "TransformFeedbackVaryingMaxLength";
			case ProgramPropertyARB::TransformFeedbackBufferMode:return "TransformFeedbackBufferMode";
			case ProgramPropertyARB::TransformFeedbackVaryings:return "TransformFeedbackVaryings";
			case ProgramPropertyARB::ActiveAtomicCounterBuffers:return "ActiveAtomicCounterBuffers";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribPropertyARB value) {
		switch(value){
			case VertexAttribPropertyARB::VertexAttribBinding:return "VertexAttribBinding";
			case VertexAttribPropertyARB::VertexAttribRelativeOffset:return "VertexAttribRelativeOffset";
			case VertexAttribPropertyARB::VertexAttribArrayEnabled:return "VertexAttribArrayEnabled";
			case VertexAttribPropertyARB::VertexAttribArraySize:return "VertexAttribArraySize";
			case VertexAttribPropertyARB::VertexAttribArrayStride:return "VertexAttribArrayStride";
			case VertexAttribPropertyARB::VertexAttribArrayType:return "VertexAttribArrayType";
			case VertexAttribPropertyARB::CurrentVertexAttrib:return "CurrentVertexAttrib";
			case VertexAttribPropertyARB::VertexAttribArrayLong:return "VertexAttribArrayLong";
			case VertexAttribPropertyARB::VertexAttribArrayNormalized:return "VertexAttribArrayNormalized";
			case VertexAttribPropertyARB::VertexAttribArrayBufferBinding:return "VertexAttribArrayBufferBinding";
			case VertexAttribPropertyARB::VertexAttribArrayInteger:return "VertexAttribArrayInteger";
			case VertexAttribPropertyARB::VertexAttribArrayDivisor:return "VertexAttribArrayDivisor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexArrayPName value) {
		switch(value){
			case VertexArrayPName::VertexAttribRelativeOffset:return "VertexAttribRelativeOffset";
			case VertexArrayPName::VertexAttribArrayEnabled:return "VertexAttribArrayEnabled";
			case VertexArrayPName::VertexAttribArraySize:return "VertexAttribArraySize";
			case VertexArrayPName::VertexAttribArrayStride:return "VertexAttribArrayStride";
			case VertexArrayPName::VertexAttribArrayType:return "VertexAttribArrayType";
			case VertexArrayPName::VertexAttribArrayLong:return "VertexAttribArrayLong";
			case VertexArrayPName::VertexAttribArrayNormalized:return "VertexAttribArrayNormalized";
			case VertexArrayPName::VertexAttribArrayInteger:return "VertexAttribArrayInteger";
			case VertexArrayPName::VertexAttribArrayDivisor:return "VertexAttribArrayDivisor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(QueryObjectParameterName value) {
		switch(value){
			case QueryObjectParameterName::QueryTarget:return "QueryTarget";
			case QueryObjectParameterName::QueryResult:return "QueryResult";
			case QueryObjectParameterName::QueryResultAvailable:return "QueryResultAvailable";
			case QueryObjectParameterName::QueryResultNoWait:return "QueryResultNoWait";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(QueryTarget value) {
		switch(value){
			case QueryTarget::TransformFeedbackOverflow:return "TransformFeedbackOverflow";
			case QueryTarget::VerticesSubmitted:return "VerticesSubmitted";
			case QueryTarget::PrimitivesSubmitted:return "PrimitivesSubmitted";
			case QueryTarget::VertexShaderInvocations:return "VertexShaderInvocations";
			case QueryTarget::TimeElapsed:return "TimeElapsed";
			case QueryTarget::SamplesPassed:return "SamplesPassed";
			case QueryTarget::AnySamplesPassed:return "AnySamplesPassed";
			case QueryTarget::PrimitivesGenerated:return "PrimitivesGenerated";
			case QueryTarget::TransformFeedbackPrimitivesWritten:return "TransformFeedbackPrimitivesWritten";
			case QueryTarget::AnySamplesPassedConservative:return "AnySamplesPassedConservative";
			case QueryTarget::TaskShaderInvocationsExt:return "TaskShaderInvocationsExt";
			case QueryTarget::MeshShaderInvocationsExt:return "MeshShaderInvocationsExt";
			case QueryTarget::MeshPrimitivesGeneratedExt:return "MeshPrimitivesGeneratedExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelTransformTargetEXT value) {
		switch(value){
			case PixelTransformTargetEXT::PixelTransform2DExt:return "PixelTransform2DExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelTransformPNameEXT value) {
		switch(value){
			case PixelTransformPNameEXT::PixelMagFilterExt:return "PixelMagFilterExt";
			case PixelTransformPNameEXT::PixelMinFilterExt:return "PixelMinFilterExt";
			case PixelTransformPNameEXT::PixelCubicWeightExt:return "PixelCubicWeightExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightTextureModeEXT value) {
		switch(value){
			case LightTextureModeEXT::FragmentMaterialExt:return "FragmentMaterialExt";
			case LightTextureModeEXT::FragmentNormalExt:return "FragmentNormalExt";
			case LightTextureModeEXT::FragmentColorExt:return "FragmentColorExt";
			case LightTextureModeEXT::FragmentDepth:return "FragmentDepth";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightTexturePNameEXT value) {
		switch(value){
			case LightTexturePNameEXT::AttenuationExt:return "AttenuationExt";
			case LightTexturePNameEXT::ShadowAttenuationExt:return "ShadowAttenuationExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelTexGenParameterNameSGIS value) {
		switch(value){
			case PixelTexGenParameterNameSGIS::PixelFragmentRgbSourceSgis:return "PixelFragmentRgbSourceSgis";
			case PixelTexGenParameterNameSGIS::PixelFragmentAlphaSourceSgis:return "PixelFragmentAlphaSourceSgis";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PerfQueryDataFlags value) {
		switch(value){
			case PerfQueryDataFlags::PerfqueryDonotFlushIntel:return "PerfqueryDonotFlushIntel";
			case PerfQueryDataFlags::PerfqueryFlushIntel:return "PerfqueryFlushIntel";
			case PerfQueryDataFlags::PerfqueryWaitIntel:return "PerfqueryWaitIntel";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(LightEnvParameterSGIX value) {
		switch(value){
			case LightEnvParameterSGIX::LightEnvModeSgix:return "LightEnvModeSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentLightModelParameterSGIX value) {
		switch(value){
			case FragmentLightModelParameterSGIX::FragmentLightModelLocalViewerSgix:return "FragmentLightModelLocalViewerSgix";
			case FragmentLightModelParameterSGIX::FragmentLightModelTwoSideSgix:return "FragmentLightModelTwoSideSgix";
			case FragmentLightModelParameterSGIX::FragmentLightModelAmbientSgix:return "FragmentLightModelAmbientSgix";
			case FragmentLightModelParameterSGIX::FragmentLightModelNormalInterpolationSgix:return "FragmentLightModelNormalInterpolationSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentLightNameSGIX value) {
		switch(value){
			case FragmentLightNameSGIX::FragmentLight0Sgix:return "FragmentLight0Sgix";
			case FragmentLightNameSGIX::FragmentLight1Sgix:return "FragmentLight1Sgix";
			case FragmentLightNameSGIX::FragmentLight2Sgix:return "FragmentLight2Sgix";
			case FragmentLightNameSGIX::FragmentLight3Sgix:return "FragmentLight3Sgix";
			case FragmentLightNameSGIX::FragmentLight4Sgix:return "FragmentLight4Sgix";
			case FragmentLightNameSGIX::FragmentLight5Sgix:return "FragmentLight5Sgix";
			case FragmentLightNameSGIX::FragmentLight6Sgix:return "FragmentLight6Sgix";
			case FragmentLightNameSGIX::FragmentLight7Sgix:return "FragmentLight7Sgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelStoreResampleMode value) {
		switch(value){
			case PixelStoreResampleMode::/* Formerly 0x8430 in SGI specfile */ ResampleDecimateSgix:return "/* Formerly 0x8430 in SGI specfile */ ResampleDecimateSgix";
			case PixelStoreResampleMode::/* Formerly 0x842E in SGI specfile */ ResampleReplicateSgix:return "/* Formerly 0x842E in SGI specfile */ ResampleReplicateSgix";
			case PixelStoreResampleMode::/* Formerly 0x842F in SGI specfile */ ResampleZeroFillSgix:return "/* Formerly 0x842F in SGI specfile */ ResampleZeroFillSgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FogCoordSrc value) {
		switch(value){
			case FogCoordSrc::FogCoordinate:return "FogCoordinate";
			case FogCoordSrc::FragmentDepth:return "FragmentDepth";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureUnit value) {
		switch(value){
			case TextureUnit::Texture0:return "Texture0";
			case TextureUnit::Texture1:return "Texture1";
			case TextureUnit::Texture2:return "Texture2";
			case TextureUnit::Texture3:return "Texture3";
			case TextureUnit::Texture4:return "Texture4";
			case TextureUnit::Texture5:return "Texture5";
			case TextureUnit::Texture6:return "Texture6";
			case TextureUnit::Texture7:return "Texture7";
			case TextureUnit::Texture8:return "Texture8";
			case TextureUnit::Texture9:return "Texture9";
			case TextureUnit::Texture10:return "Texture10";
			case TextureUnit::Texture11:return "Texture11";
			case TextureUnit::Texture12:return "Texture12";
			case TextureUnit::Texture13:return "Texture13";
			case TextureUnit::Texture14:return "Texture14";
			case TextureUnit::Texture15:return "Texture15";
			case TextureUnit::Texture16:return "Texture16";
			case TextureUnit::Texture17:return "Texture17";
			case TextureUnit::Texture18:return "Texture18";
			case TextureUnit::Texture19:return "Texture19";
			case TextureUnit::Texture20:return "Texture20";
			case TextureUnit::Texture21:return "Texture21";
			case TextureUnit::Texture22:return "Texture22";
			case TextureUnit::Texture23:return "Texture23";
			case TextureUnit::Texture24:return "Texture24";
			case TextureUnit::Texture25:return "Texture25";
			case TextureUnit::Texture26:return "Texture26";
			case TextureUnit::Texture27:return "Texture27";
			case TextureUnit::Texture28:return "Texture28";
			case TextureUnit::Texture29:return "Texture29";
			case TextureUnit::Texture30:return "Texture30";
			case TextureUnit::Texture31:return "Texture31";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderTextureSourceATI value) {
		switch(value){
			case FragmentShaderTextureSourceATI::Texture0:return "Texture0";
			case FragmentShaderTextureSourceATI::Texture1:return "Texture1";
			case FragmentShaderTextureSourceATI::Texture2:return "Texture2";
			case FragmentShaderTextureSourceATI::Texture3:return "Texture3";
			case FragmentShaderTextureSourceATI::Texture4:return "Texture4";
			case FragmentShaderTextureSourceATI::Texture5:return "Texture5";
			case FragmentShaderTextureSourceATI::Texture6:return "Texture6";
			case FragmentShaderTextureSourceATI::Texture7:return "Texture7";
			case FragmentShaderTextureSourceATI::Texture8:return "Texture8";
			case FragmentShaderTextureSourceATI::Texture9:return "Texture9";
			case FragmentShaderTextureSourceATI::Texture10:return "Texture10";
			case FragmentShaderTextureSourceATI::Texture11:return "Texture11";
			case FragmentShaderTextureSourceATI::Texture12:return "Texture12";
			case FragmentShaderTextureSourceATI::Texture13:return "Texture13";
			case FragmentShaderTextureSourceATI::Texture14:return "Texture14";
			case FragmentShaderTextureSourceATI::Texture15:return "Texture15";
			case FragmentShaderTextureSourceATI::Texture16:return "Texture16";
			case FragmentShaderTextureSourceATI::Texture17:return "Texture17";
			case FragmentShaderTextureSourceATI::Texture18:return "Texture18";
			case FragmentShaderTextureSourceATI::Texture19:return "Texture19";
			case FragmentShaderTextureSourceATI::Texture20:return "Texture20";
			case FragmentShaderTextureSourceATI::Texture21:return "Texture21";
			case FragmentShaderTextureSourceATI::Texture22:return "Texture22";
			case FragmentShaderTextureSourceATI::Texture23:return "Texture23";
			case FragmentShaderTextureSourceATI::Texture24:return "Texture24";
			case FragmentShaderTextureSourceATI::Texture25:return "Texture25";
			case FragmentShaderTextureSourceATI::Texture26:return "Texture26";
			case FragmentShaderTextureSourceATI::Texture27:return "Texture27";
			case FragmentShaderTextureSourceATI::Texture28:return "Texture28";
			case FragmentShaderTextureSourceATI::Texture29:return "Texture29";
			case FragmentShaderTextureSourceATI::Texture30:return "Texture30";
			case FragmentShaderTextureSourceATI::Texture31:return "Texture31";
			case FragmentShaderTextureSourceATI::Reg0Ati:return "Reg0Ati";
			case FragmentShaderTextureSourceATI::Reg1Ati:return "Reg1Ati";
			case FragmentShaderTextureSourceATI::Reg2Ati:return "Reg2Ati";
			case FragmentShaderTextureSourceATI::Reg3Ati:return "Reg3Ati";
			case FragmentShaderTextureSourceATI::Reg4Ati:return "Reg4Ati";
			case FragmentShaderTextureSourceATI::Reg5Ati:return "Reg5Ati";
			case FragmentShaderTextureSourceATI::Reg6Ati:return "Reg6Ati";
			case FragmentShaderTextureSourceATI::Reg7Ati:return "Reg7Ati";
			case FragmentShaderTextureSourceATI::Reg8Ati:return "Reg8Ati";
			case FragmentShaderTextureSourceATI::Reg9Ati:return "Reg9Ati";
			case FragmentShaderTextureSourceATI::Reg10Ati:return "Reg10Ati";
			case FragmentShaderTextureSourceATI::Reg11Ati:return "Reg11Ati";
			case FragmentShaderTextureSourceATI::Reg12Ati:return "Reg12Ati";
			case FragmentShaderTextureSourceATI::Reg13Ati:return "Reg13Ati";
			case FragmentShaderTextureSourceATI::Reg14Ati:return "Reg14Ati";
			case FragmentShaderTextureSourceATI::Reg15Ati:return "Reg15Ati";
			case FragmentShaderTextureSourceATI::Reg16Ati:return "Reg16Ati";
			case FragmentShaderTextureSourceATI::Reg17Ati:return "Reg17Ati";
			case FragmentShaderTextureSourceATI::Reg18Ati:return "Reg18Ati";
			case FragmentShaderTextureSourceATI::Reg19Ati:return "Reg19Ati";
			case FragmentShaderTextureSourceATI::Reg20Ati:return "Reg20Ati";
			case FragmentShaderTextureSourceATI::Reg21Ati:return "Reg21Ati";
			case FragmentShaderTextureSourceATI::Reg22Ati:return "Reg22Ati";
			case FragmentShaderTextureSourceATI::Reg23Ati:return "Reg23Ati";
			case FragmentShaderTextureSourceATI::Reg24Ati:return "Reg24Ati";
			case FragmentShaderTextureSourceATI::Reg25Ati:return "Reg25Ati";
			case FragmentShaderTextureSourceATI::Reg26Ati:return "Reg26Ati";
			case FragmentShaderTextureSourceATI::Reg27Ati:return "Reg27Ati";
			case FragmentShaderTextureSourceATI::Reg28Ati:return "Reg28Ati";
			case FragmentShaderTextureSourceATI::Reg29Ati:return "Reg29Ati";
			case FragmentShaderTextureSourceATI::Reg30Ati:return "Reg30Ati";
			case FragmentShaderTextureSourceATI::Reg31Ati:return "Reg31Ati";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerRegisterNV value) {
		switch(value){
			case CombinerRegisterNV::Texture0Arb:return "Texture0Arb";
			case CombinerRegisterNV::Texture1Arb:return "Texture1Arb";
			case CombinerRegisterNV::PrimaryColorNv:return "PrimaryColorNv";
			case CombinerRegisterNV::SecondaryColorNv:return "SecondaryColorNv";
			case CombinerRegisterNV::Spare0Nv:return "Spare0Nv";
			case CombinerRegisterNV::Spare1Nv:return "Spare1Nv";
			case CombinerRegisterNV::DiscardNv:return "DiscardNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(UniformBlockPName value) {
		switch(value){
			case UniformBlockPName::UniformBlockReferencedByTessControlShader:return "UniformBlockReferencedByTessControlShader";
			case UniformBlockPName::UniformBlockReferencedByTessEvaluationShader:return "UniformBlockReferencedByTessEvaluationShader";
			case UniformBlockPName::UniformBlockBinding:return "UniformBlockBinding";
			case UniformBlockPName::UniformBlockDataSize:return "UniformBlockDataSize";
			case UniformBlockPName::UniformBlockNameLength:return "UniformBlockNameLength";
			case UniformBlockPName::UniformBlockActiveUniforms:return "UniformBlockActiveUniforms";
			case UniformBlockPName::UniformBlockActiveUniformIndices:return "UniformBlockActiveUniformIndices";
			case UniformBlockPName::UniformBlockReferencedByVertexShader:return "UniformBlockReferencedByVertexShader";
			case UniformBlockPName::UniformBlockReferencedByGeometryShader:return "UniformBlockReferencedByGeometryShader";
			case UniformBlockPName::UniformBlockReferencedByFragmentShader:return "UniformBlockReferencedByFragmentShader";
			case UniformBlockPName::UniformBlockReferencedByComputeShader:return "UniformBlockReferencedByComputeShader";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FenceConditionNV value) {
		switch(value){
			case FenceConditionNV::AllCompletedNv:return "AllCompletedNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FenceParameterNameNV value) {
		switch(value){
			case FenceParameterNameNV::FenceStatusNv:return "FenceStatusNv";
			case FenceParameterNameNV::FenceConditionNv:return "FenceConditionNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerVariableNV value) {
		switch(value){
			case CombinerVariableNV::VariableANv:return "VariableANv";
			case CombinerVariableNV::VariableBNv:return "VariableBNv";
			case CombinerVariableNV::VariableCNv:return "VariableCNv";
			case CombinerVariableNV::VariableDNv:return "VariableDNv";
			case CombinerVariableNV::VariableENv:return "VariableENv";
			case CombinerVariableNV::VariableFNv:return "VariableFNv";
			case CombinerVariableNV::VariableGNv:return "VariableGNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathColor value) {
		switch(value){
			case PathColor::PrimaryColorNv:return "PrimaryColorNv";
			case PathColor::SecondaryColorNv:return "SecondaryColorNv";
			case PathColor::PrimaryColor:return "PrimaryColor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerMappingNV value) {
		switch(value){
			case CombinerMappingNV::UnsignedIdentityNv:return "UnsignedIdentityNv";
			case CombinerMappingNV::UnsignedInvertNv:return "UnsignedInvertNv";
			case CombinerMappingNV::ExpandNormalNv:return "ExpandNormalNv";
			case CombinerMappingNV::ExpandNegateNv:return "ExpandNegateNv";
			case CombinerMappingNV::HalfBiasNormalNv:return "HalfBiasNormalNv";
			case CombinerMappingNV::HalfBiasNegateNv:return "HalfBiasNegateNv";
			case CombinerMappingNV::SignedIdentityNv:return "SignedIdentityNv";
			case CombinerMappingNV::SignedNegateNv:return "SignedNegateNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerParameterNV value) {
		switch(value){
			case CombinerParameterNV::CombinerInputNv:return "CombinerInputNv";
			case CombinerParameterNV::CombinerMappingNv:return "CombinerMappingNv";
			case CombinerParameterNV::CombinerComponentUsageNv:return "CombinerComponentUsageNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CombinerStageNV value) {
		switch(value){
			case CombinerStageNV::Combiner0Nv:return "Combiner0Nv";
			case CombinerStageNV::Combiner1Nv:return "Combiner1Nv";
			case CombinerStageNV::Combiner2Nv:return "Combiner2Nv";
			case CombinerStageNV::Combiner3Nv:return "Combiner3Nv";
			case CombinerStageNV::Combiner4Nv:return "Combiner4Nv";
			case CombinerStageNV::Combiner5Nv:return "Combiner5Nv";
			case CombinerStageNV::Combiner6Nv:return "Combiner6Nv";
			case CombinerStageNV::Combiner7Nv:return "Combiner7Nv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelStoreSubsampleRate value) {
		switch(value){
			case PixelStoreSubsampleRate::PixelSubsample4444Sgix:return "PixelSubsample4444Sgix";
			case PixelStoreSubsampleRate::PixelSubsample2424Sgix:return "PixelSubsample2424Sgix";
			case PixelStoreSubsampleRate::PixelSubsample4242Sgix:return "PixelSubsample4242Sgix";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureNormalModeEXT value) {
		switch(value){
			case TextureNormalModeEXT::PerturbExt:return "PerturbExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexArrayPNameAPPLE value) {
		switch(value){
			case VertexArrayPNameAPPLE::StorageClientApple:return "StorageClientApple";
			case VertexArrayPNameAPPLE::StorageCachedApple:return "StorageCachedApple";
			case VertexArrayPNameAPPLE::StorageSharedApple:return "StorageSharedApple";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribEnum value) {
		switch(value){
			case VertexAttribEnum::VertexAttribArrayEnabled:return "VertexAttribArrayEnabled";
			case VertexAttribEnum::VertexAttribArraySize:return "VertexAttribArraySize";
			case VertexAttribEnum::VertexAttribArrayStride:return "VertexAttribArrayStride";
			case VertexAttribEnum::VertexAttribArrayType:return "VertexAttribArrayType";
			case VertexAttribEnum::CurrentVertexAttrib:return "CurrentVertexAttrib";
			case VertexAttribEnum::VertexAttribArrayNormalized:return "VertexAttribArrayNormalized";
			case VertexAttribEnum::VertexAttribArrayBufferBinding:return "VertexAttribArrayBufferBinding";
			case VertexAttribEnum::VertexAttribArrayInteger:return "VertexAttribArrayInteger";
			case VertexAttribEnum::VertexAttribArrayDivisor:return "VertexAttribArrayDivisor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramStringProperty value) {
		switch(value){
			case ProgramStringProperty::ProgramStringArb:return "ProgramStringArb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribEnumNV value) {
		switch(value){
			case VertexAttribEnumNV::ProgramParameterNv:return "ProgramParameterNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexAttribPointerPropertyARB value) {
		switch(value){
			case VertexAttribPointerPropertyARB::VertexAttribArrayPointer:return "VertexAttribArrayPointer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(EvalTargetNV value) {
		switch(value){
			case EvalTargetNV::Eval2DNv:return "Eval2DNv";
			case EvalTargetNV::EvalTriangular2DNv:return "EvalTriangular2DNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapParameterNV value) {
		switch(value){
			case MapParameterNV::MapTessellationNv:return "MapTessellationNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MapAttribParameterNV value) {
		switch(value){
			case MapAttribParameterNV::MapAttribUOrderNv:return "MapAttribUOrderNv";
			case MapAttribParameterNV::MapAttribVOrderNv:return "MapAttribVOrderNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ArrayObjectUsageATI value) {
		switch(value){
			case ArrayObjectUsageATI::StaticAti:return "StaticAti";
			case ArrayObjectUsageATI::DynamicAti:return "DynamicAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PreserveModeATI value) {
		switch(value){
			case PreserveModeATI::PreserveAti:return "PreserveAti";
			case PreserveModeATI::DiscardAti:return "DiscardAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ArrayObjectPNameATI value) {
		switch(value){
			case ArrayObjectPNameATI::ObjectBufferSizeAti:return "ObjectBufferSizeAti";
			case ArrayObjectPNameATI::ObjectBufferUsageAti:return "ObjectBufferUsageAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexStreamATI value) {
		switch(value){
			case VertexStreamATI::VertexStream0Ati:return "VertexStream0Ati";
			case VertexStreamATI::VertexStream1Ati:return "VertexStream1Ati";
			case VertexStreamATI::VertexStream2Ati:return "VertexStream2Ati";
			case VertexStreamATI::VertexStream3Ati:return "VertexStream3Ati";
			case VertexStreamATI::VertexStream4Ati:return "VertexStream4Ati";
			case VertexStreamATI::VertexStream5Ati:return "VertexStream5Ati";
			case VertexStreamATI::VertexStream6Ati:return "VertexStream6Ati";
			case VertexStreamATI::VertexStream7Ati:return "VertexStream7Ati";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetTexBumpParameterATI value) {
		switch(value){
			case GetTexBumpParameterATI::BumpRotMatrixAti:return "BumpRotMatrixAti";
			case GetTexBumpParameterATI::BumpRotMatrixSizeAti:return "BumpRotMatrixSizeAti";
			case GetTexBumpParameterATI::BumpNumTexUnitsAti:return "BumpNumTexUnitsAti";
			case GetTexBumpParameterATI::BumpTexUnitsAti:return "BumpTexUnitsAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TexBumpParameterATI value) {
		switch(value){
			case TexBumpParameterATI::BumpRotMatrixAti:return "BumpRotMatrixAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderOpEXT value) {
		switch(value){
			case VertexShaderOpEXT::OpIndexExt:return "OpIndexExt";
			case VertexShaderOpEXT::OpNegateExt:return "OpNegateExt";
			case VertexShaderOpEXT::OpDot3Ext:return "OpDot3Ext";
			case VertexShaderOpEXT::OpDot4Ext:return "OpDot4Ext";
			case VertexShaderOpEXT::OpMulExt:return "OpMulExt";
			case VertexShaderOpEXT::OpAddExt:return "OpAddExt";
			case VertexShaderOpEXT::OpMaddExt:return "OpMaddExt";
			case VertexShaderOpEXT::OpFracExt:return "OpFracExt";
			case VertexShaderOpEXT::OpMaxExt:return "OpMaxExt";
			case VertexShaderOpEXT::OpMinExt:return "OpMinExt";
			case VertexShaderOpEXT::OpSetGeExt:return "OpSetGeExt";
			case VertexShaderOpEXT::OpSetLtExt:return "OpSetLtExt";
			case VertexShaderOpEXT::OpClampExt:return "OpClampExt";
			case VertexShaderOpEXT::OpFloorExt:return "OpFloorExt";
			case VertexShaderOpEXT::OpRoundExt:return "OpRoundExt";
			case VertexShaderOpEXT::OpExpBase2Ext:return "OpExpBase2Ext";
			case VertexShaderOpEXT::OpLogBase2Ext:return "OpLogBase2Ext";
			case VertexShaderOpEXT::OpPowerExt:return "OpPowerExt";
			case VertexShaderOpEXT::OpRecipExt:return "OpRecipExt";
			case VertexShaderOpEXT::OpRecipSqrtExt:return "OpRecipSqrtExt";
			case VertexShaderOpEXT::OpSubExt:return "OpSubExt";
			case VertexShaderOpEXT::OpCrossProductExt:return "OpCrossProductExt";
			case VertexShaderOpEXT::OpMultiplyMatrixExt:return "OpMultiplyMatrixExt";
			case VertexShaderOpEXT::OpMovExt:return "OpMovExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(DataTypeEXT value) {
		switch(value){
			case DataTypeEXT::ScalarExt:return "ScalarExt";
			case DataTypeEXT::VectorExt:return "VectorExt";
			case DataTypeEXT::MatrixExt:return "MatrixExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderStorageTypeEXT value) {
		switch(value){
			case VertexShaderStorageTypeEXT::VariantExt:return "VariantExt";
			case VertexShaderStorageTypeEXT::InvariantExt:return "InvariantExt";
			case VertexShaderStorageTypeEXT::LocalConstantExt:return "LocalConstantExt";
			case VertexShaderStorageTypeEXT::LocalExt:return "LocalExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderCoordOutEXT value) {
		switch(value){
			case VertexShaderCoordOutEXT::XExt:return "XExt";
			case VertexShaderCoordOutEXT::YExt:return "YExt";
			case VertexShaderCoordOutEXT::ZExt:return "ZExt";
			case VertexShaderCoordOutEXT::WExt:return "WExt";
			case VertexShaderCoordOutEXT::NegativeXExt:return "NegativeXExt";
			case VertexShaderCoordOutEXT::NegativeYExt:return "NegativeYExt";
			case VertexShaderCoordOutEXT::NegativeZExt:return "NegativeZExt";
			case VertexShaderCoordOutEXT::NegativeWExt:return "NegativeWExt";
			case VertexShaderCoordOutEXT::ZeroExt:return "ZeroExt";
			case VertexShaderCoordOutEXT::OneExt:return "OneExt";
			case VertexShaderCoordOutEXT::NegativeOneExt:return "NegativeOneExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ParameterRangeEXT value) {
		switch(value){
			case ParameterRangeEXT::NormalizedRangeExt:return "NormalizedRangeExt";
			case ParameterRangeEXT::FullRangeExt:return "FullRangeExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexShaderParameterEXT value) {
		switch(value){
			case VertexShaderParameterEXT::CurrentVertexExt:return "CurrentVertexExt";
			case VertexShaderParameterEXT::MvpMatrixExt:return "MvpMatrixExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetVariantValueEXT value) {
		switch(value){
			case GetVariantValueEXT::VariantValueExt:return "VariantValueExt";
			case GetVariantValueEXT::VariantDatatypeExt:return "VariantDatatypeExt";
			case GetVariantValueEXT::VariantArrayStrideExt:return "VariantArrayStrideExt";
			case GetVariantValueEXT::VariantArrayTypeExt:return "VariantArrayTypeExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VariantCapEXT value) {
		switch(value){
			case VariantCapEXT::VariantArrayExt:return "VariantArrayExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PNTrianglesPNameATI value) {
		switch(value){
			case PNTrianglesPNameATI::PnTrianglesPointModeAti:return "PnTrianglesPointModeAti";
			case PNTrianglesPNameATI::PnTrianglesNormalModeAti:return "PnTrianglesNormalModeAti";
			case PNTrianglesPNameATI::PnTrianglesTesselationLevelAti:return "PnTrianglesTesselationLevelAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(QueryParameterName value) {
		switch(value){
			case QueryParameterName::QueryCounterBits:return "QueryCounterBits";
			case QueryParameterName::CurrentQuery:return "CurrentQuery";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(OcclusionQueryParameterNameNV value) {
		switch(value){
			case OcclusionQueryParameterNameNV::PixelCountNv:return "PixelCountNv";
			case OcclusionQueryParameterNameNV::PixelCountAvailableNv:return "PixelCountAvailableNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramFormat value) {
		switch(value){
			case ProgramFormat::ProgramFormatAsciiArb:return "ProgramFormatAsciiArb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PixelDataRangeTargetNV value) {
		switch(value){
			case PixelDataRangeTargetNV::WritePixelDataRangeNv:return "WritePixelDataRangeNv";
			case PixelDataRangeTargetNV::ReadPixelDataRangeNv:return "ReadPixelDataRangeNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(CopyBufferSubDataTarget value) {
		switch(value){
			case CopyBufferSubDataTarget::ArrayBuffer:return "ArrayBuffer";
			case CopyBufferSubDataTarget::ElementArrayBuffer:return "ElementArrayBuffer";
			case CopyBufferSubDataTarget::PixelPackBuffer:return "PixelPackBuffer";
			case CopyBufferSubDataTarget::PixelUnpackBuffer:return "PixelUnpackBuffer";
			case CopyBufferSubDataTarget::UniformBuffer:return "UniformBuffer";
			case CopyBufferSubDataTarget::TextureBuffer:return "TextureBuffer";
			case CopyBufferSubDataTarget::TransformFeedbackBuffer:return "TransformFeedbackBuffer";
			case CopyBufferSubDataTarget::CopyReadBuffer:return "CopyReadBuffer";
			case CopyBufferSubDataTarget::CopyWriteBuffer:return "CopyWriteBuffer";
			case CopyBufferSubDataTarget::DrawIndirectBuffer:return "DrawIndirectBuffer";
			case CopyBufferSubDataTarget::ShaderStorageBuffer:return "ShaderStorageBuffer";
			case CopyBufferSubDataTarget::DispatchIndirectBuffer:return "DispatchIndirectBuffer";
			case CopyBufferSubDataTarget::QueryBuffer:return "QueryBuffer";
			case CopyBufferSubDataTarget::AtomicCounterBuffer:return "AtomicCounterBuffer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferStorageTarget value) {
		switch(value){
			case BufferStorageTarget::ArrayBuffer:return "ArrayBuffer";
			case BufferStorageTarget::ElementArrayBuffer:return "ElementArrayBuffer";
			case BufferStorageTarget::PixelPackBuffer:return "PixelPackBuffer";
			case BufferStorageTarget::PixelUnpackBuffer:return "PixelUnpackBuffer";
			case BufferStorageTarget::UniformBuffer:return "UniformBuffer";
			case BufferStorageTarget::TextureBuffer:return "TextureBuffer";
			case BufferStorageTarget::TransformFeedbackBuffer:return "TransformFeedbackBuffer";
			case BufferStorageTarget::CopyReadBuffer:return "CopyReadBuffer";
			case BufferStorageTarget::CopyWriteBuffer:return "CopyWriteBuffer";
			case BufferStorageTarget::DrawIndirectBuffer:return "DrawIndirectBuffer";
			case BufferStorageTarget::ShaderStorageBuffer:return "ShaderStorageBuffer";
			case BufferStorageTarget::DispatchIndirectBuffer:return "DispatchIndirectBuffer";
			case BufferStorageTarget::QueryBuffer:return "QueryBuffer";
			case BufferStorageTarget::AtomicCounterBuffer:return "AtomicCounterBuffer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferAccessARB value) {
		switch(value){
			case BufferAccessARB::ReadOnly:return "ReadOnly";
			case BufferAccessARB::WriteOnly:return "WriteOnly";
			case BufferAccessARB::ReadWrite:return "ReadWrite";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferPointerNameARB value) {
		switch(value){
			case BufferPointerNameARB::BufferMapPointer:return "BufferMapPointer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BufferUsageARB value) {
		switch(value){
			case BufferUsageARB::StreamDraw:return "StreamDraw";
			case BufferUsageARB::StreamRead:return "StreamRead";
			case BufferUsageARB::StreamCopy:return "StreamCopy";
			case BufferUsageARB::StaticDraw:return "StaticDraw";
			case BufferUsageARB::StaticRead:return "StaticRead";
			case BufferUsageARB::StaticCopy:return "StaticCopy";
			case BufferUsageARB::DynamicDraw:return "DynamicDraw";
			case BufferUsageARB::DynamicRead:return "DynamicRead";
			case BufferUsageARB::DynamicCopy:return "DynamicCopy";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClampColorTargetARB value) {
		switch(value){
			case ClampColorTargetARB::ClampVertexColorArb:return "ClampVertexColorArb";
			case ClampColorTargetARB::ClampFragmentColorArb:return "ClampFragmentColorArb";
			case ClampColorTargetARB::ClampReadColor:return "ClampReadColor";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderRegATI value) {
		switch(value){
			case FragmentShaderRegATI::Reg0Ati:return "Reg0Ati";
			case FragmentShaderRegATI::Reg1Ati:return "Reg1Ati";
			case FragmentShaderRegATI::Reg2Ati:return "Reg2Ati";
			case FragmentShaderRegATI::Reg3Ati:return "Reg3Ati";
			case FragmentShaderRegATI::Reg4Ati:return "Reg4Ati";
			case FragmentShaderRegATI::Reg5Ati:return "Reg5Ati";
			case FragmentShaderRegATI::Reg6Ati:return "Reg6Ati";
			case FragmentShaderRegATI::Reg7Ati:return "Reg7Ati";
			case FragmentShaderRegATI::Reg8Ati:return "Reg8Ati";
			case FragmentShaderRegATI::Reg9Ati:return "Reg9Ati";
			case FragmentShaderRegATI::Reg10Ati:return "Reg10Ati";
			case FragmentShaderRegATI::Reg11Ati:return "Reg11Ati";
			case FragmentShaderRegATI::Reg12Ati:return "Reg12Ati";
			case FragmentShaderRegATI::Reg13Ati:return "Reg13Ati";
			case FragmentShaderRegATI::Reg14Ati:return "Reg14Ati";
			case FragmentShaderRegATI::Reg15Ati:return "Reg15Ati";
			case FragmentShaderRegATI::Reg16Ati:return "Reg16Ati";
			case FragmentShaderRegATI::Reg17Ati:return "Reg17Ati";
			case FragmentShaderRegATI::Reg18Ati:return "Reg18Ati";
			case FragmentShaderRegATI::Reg19Ati:return "Reg19Ati";
			case FragmentShaderRegATI::Reg20Ati:return "Reg20Ati";
			case FragmentShaderRegATI::Reg21Ati:return "Reg21Ati";
			case FragmentShaderRegATI::Reg22Ati:return "Reg22Ati";
			case FragmentShaderRegATI::Reg23Ati:return "Reg23Ati";
			case FragmentShaderRegATI::Reg24Ati:return "Reg24Ati";
			case FragmentShaderRegATI::Reg25Ati:return "Reg25Ati";
			case FragmentShaderRegATI::Reg26Ati:return "Reg26Ati";
			case FragmentShaderRegATI::Reg27Ati:return "Reg27Ati";
			case FragmentShaderRegATI::Reg28Ati:return "Reg28Ati";
			case FragmentShaderRegATI::Reg29Ati:return "Reg29Ati";
			case FragmentShaderRegATI::Reg30Ati:return "Reg30Ati";
			case FragmentShaderRegATI::Reg31Ati:return "Reg31Ati";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentShaderConATI value) {
		switch(value){
			case FragmentShaderConATI::Con0Ati:return "Con0Ati";
			case FragmentShaderConATI::Con1Ati:return "Con1Ati";
			case FragmentShaderConATI::Con2Ati:return "Con2Ati";
			case FragmentShaderConATI::Con3Ati:return "Con3Ati";
			case FragmentShaderConATI::Con4Ati:return "Con4Ati";
			case FragmentShaderConATI::Con5Ati:return "Con5Ati";
			case FragmentShaderConATI::Con6Ati:return "Con6Ati";
			case FragmentShaderConATI::Con7Ati:return "Con7Ati";
			case FragmentShaderConATI::Con8Ati:return "Con8Ati";
			case FragmentShaderConATI::Con9Ati:return "Con9Ati";
			case FragmentShaderConATI::Con10Ati:return "Con10Ati";
			case FragmentShaderConATI::Con11Ati:return "Con11Ati";
			case FragmentShaderConATI::Con12Ati:return "Con12Ati";
			case FragmentShaderConATI::Con13Ati:return "Con13Ati";
			case FragmentShaderConATI::Con14Ati:return "Con14Ati";
			case FragmentShaderConATI::Con15Ati:return "Con15Ati";
			case FragmentShaderConATI::Con16Ati:return "Con16Ati";
			case FragmentShaderConATI::Con17Ati:return "Con17Ati";
			case FragmentShaderConATI::Con18Ati:return "Con18Ati";
			case FragmentShaderConATI::Con19Ati:return "Con19Ati";
			case FragmentShaderConATI::Con20Ati:return "Con20Ati";
			case FragmentShaderConATI::Con21Ati:return "Con21Ati";
			case FragmentShaderConATI::Con22Ati:return "Con22Ati";
			case FragmentShaderConATI::Con23Ati:return "Con23Ati";
			case FragmentShaderConATI::Con24Ati:return "Con24Ati";
			case FragmentShaderConATI::Con25Ati:return "Con25Ati";
			case FragmentShaderConATI::Con26Ati:return "Con26Ati";
			case FragmentShaderConATI::Con27Ati:return "Con27Ati";
			case FragmentShaderConATI::Con28Ati:return "Con28Ati";
			case FragmentShaderConATI::Con29Ati:return "Con29Ati";
			case FragmentShaderConATI::Con30Ati:return "Con30Ati";
			case FragmentShaderConATI::Con31Ati:return "Con31Ati";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentOp1ATI value) {
		switch(value){
			case FragmentOp1ATI::MovAti:return "MovAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentOp2ATI value) {
		switch(value){
			case FragmentOp2ATI::AddAti:return "AddAti";
			case FragmentOp2ATI::MulAti:return "MulAti";
			case FragmentOp2ATI::SubAti:return "SubAti";
			case FragmentOp2ATI::Dot3Ati:return "Dot3Ati";
			case FragmentOp2ATI::Dot4Ati:return "Dot4Ati";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FragmentOp3ATI value) {
		switch(value){
			case FragmentOp3ATI::MadAti:return "MadAti";
			case FragmentOp3ATI::LerpAti:return "LerpAti";
			case FragmentOp3ATI::CndAti:return "CndAti";
			case FragmentOp3ATI::Cnd0Ati:return "Cnd0Ati";
			case FragmentOp3ATI::Dot2AddAti:return "Dot2AddAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SwizzleOpATI value) {
		switch(value){
			case SwizzleOpATI::SwizzleStrAti:return "SwizzleStrAti";
			case SwizzleOpATI::SwizzleStqAti:return "SwizzleStqAti";
			case SwizzleOpATI::SwizzleStrDrAti:return "SwizzleStrDrAti";
			case SwizzleOpATI::SwizzleStqDqAti:return "SwizzleStqDqAti";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ObjectTypeAPPLE value) {
		switch(value){
			case ObjectTypeAPPLE::DrawPixelsApple:return "DrawPixelsApple";
			case ObjectTypeAPPLE::FenceApple:return "FenceApple";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(UniformPName value) {
		switch(value){
			case UniformPName::UniformType:return "UniformType";
			case UniformPName::UniformSize:return "UniformSize";
			case UniformPName::UniformNameLength:return "UniformNameLength";
			case UniformPName::UniformBlockIndex:return "UniformBlockIndex";
			case UniformPName::UniformOffset:return "UniformOffset";
			case UniformPName::UniformArrayStride:return "UniformArrayStride";
			case UniformPName::UniformMatrixStride:return "UniformMatrixStride";
			case UniformPName::UniformIsRowMajor:return "UniformIsRowMajor";
			case UniformPName::UniformAtomicCounterBufferIndex:return "UniformAtomicCounterBufferIndex";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SubroutineParameterName value) {
		switch(value){
			case SubroutineParameterName::UniformSize:return "UniformSize";
			case SubroutineParameterName::UniformNameLength:return "UniformNameLength";
			case SubroutineParameterName::NumCompatibleSubroutines:return "NumCompatibleSubroutines";
			case SubroutineParameterName::CompatibleSubroutines:return "CompatibleSubroutines";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShaderType value) {
		switch(value){
			case ShaderType::FragmentShader:return "FragmentShader";
			case ShaderType::VertexShader:return "VertexShader";
			case ShaderType::GeometryShader:return "GeometryShader";
			case ShaderType::TessEvaluationShader:return "TessEvaluationShader";
			case ShaderType::TessControlShader:return "TessControlShader";
			case ShaderType::ComputeShader:return "ComputeShader";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ContainerType value) {
		switch(value){
			case ContainerType::ProgramObjectArb:return "ProgramObjectArb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShaderParameterName value) {
		switch(value){
			case ShaderParameterName::ShaderType:return "ShaderType";
			case ShaderParameterName::DeleteStatus:return "DeleteStatus";
			case ShaderParameterName::CompileStatus:return "CompileStatus";
			case ShaderParameterName::InfoLogLength:return "InfoLogLength";
			case ShaderParameterName::ShaderSourceLength:return "ShaderSourceLength";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShaderBinaryFormat value) {
		switch(value){
			case ShaderBinaryFormat::SgxBinaryImg:return "SgxBinaryImg";
			case ShaderBinaryFormat::MaliShaderBinaryArm:return "MaliShaderBinaryArm";
			case ShaderBinaryFormat::ShaderBinaryViv:return "ShaderBinaryViv";
			case ShaderBinaryFormat::ShaderBinaryDmp:return "ShaderBinaryDmp";
			case ShaderBinaryFormat::GccsoShaderBinaryFj:return "GccsoShaderBinaryFj";
			case ShaderBinaryFormat::ShaderBinaryFormatSpirV:return "ShaderBinaryFormatSpirV";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TransformFeedbackPName value) {
		switch(value){
			case TransformFeedbackPName::TransformFeedbackBufferStart:return "TransformFeedbackBufferStart";
			case TransformFeedbackPName::TransformFeedbackBufferSize:return "TransformFeedbackBufferSize";
			case TransformFeedbackPName::TransformFeedbackBufferBinding:return "TransformFeedbackBufferBinding";
			case TransformFeedbackPName::TransformFeedbackPaused:return "TransformFeedbackPaused";
			case TransformFeedbackPName::TransformFeedbackActive:return "TransformFeedbackActive";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TransformFeedbackBufferMode value) {
		switch(value){
			case TransformFeedbackBufferMode::InterleavedAttribs:return "InterleavedAttribs";
			case TransformFeedbackBufferMode::SeparateAttribs:return "SeparateAttribs";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramInterface value) {
		switch(value){
			case ProgramInterface::TransformFeedbackBuffer:return "TransformFeedbackBuffer";
			case ProgramInterface::Uniform:return "Uniform";
			case ProgramInterface::UniformBlock:return "UniformBlock";
			case ProgramInterface::ProgramInput:return "ProgramInput";
			case ProgramInterface::ProgramOutput:return "ProgramOutput";
			case ProgramInterface::BufferVariable:return "BufferVariable";
			case ProgramInterface::ShaderStorageBlock:return "ShaderStorageBlock";
			case ProgramInterface::VertexSubroutine:return "VertexSubroutine";
			case ProgramInterface::TessControlSubroutine:return "TessControlSubroutine";
			case ProgramInterface::TessEvaluationSubroutine:return "TessEvaluationSubroutine";
			case ProgramInterface::GeometrySubroutine:return "GeometrySubroutine";
			case ProgramInterface::FragmentSubroutine:return "FragmentSubroutine";
			case ProgramInterface::ComputeSubroutine:return "ComputeSubroutine";
			case ProgramInterface::VertexSubroutineUniform:return "VertexSubroutineUniform";
			case ProgramInterface::TessControlSubroutineUniform:return "TessControlSubroutineUniform";
			case ProgramInterface::TessEvaluationSubroutineUniform:return "TessEvaluationSubroutineUniform";
			case ProgramInterface::GeometrySubroutineUniform:return "GeometrySubroutineUniform";
			case ProgramInterface::FragmentSubroutineUniform:return "FragmentSubroutineUniform";
			case ProgramInterface::ComputeSubroutineUniform:return "ComputeSubroutineUniform";
			case ProgramInterface::TransformFeedbackVarying:return "TransformFeedbackVarying";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClipControlOrigin value) {
		switch(value){
			case ClipControlOrigin::LowerLeft:return "LowerLeft";
			case ClipControlOrigin::UpperLeft:return "UpperLeft";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FramebufferTarget value) {
		switch(value){
			case FramebufferTarget::ReadFramebuffer:return "ReadFramebuffer";
			case FramebufferTarget::DrawFramebuffer:return "DrawFramebuffer";
			case FramebufferTarget::Framebuffer:return "Framebuffer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(RenderbufferParameterName value) {
		switch(value){
			case RenderbufferParameterName::RenderbufferCoverageSamplesNv:return "RenderbufferCoverageSamplesNv";
			case RenderbufferParameterName::RenderbufferWidth:return "RenderbufferWidth";
			case RenderbufferParameterName::RenderbufferHeight:return "RenderbufferHeight";
			case RenderbufferParameterName::RenderbufferInternalFormat:return "RenderbufferInternalFormat";
			case RenderbufferParameterName::RenderbufferRedSize:return "RenderbufferRedSize";
			case RenderbufferParameterName::RenderbufferGreenSize:return "RenderbufferGreenSize";
			case RenderbufferParameterName::RenderbufferBlueSize:return "RenderbufferBlueSize";
			case RenderbufferParameterName::RenderbufferAlphaSize:return "RenderbufferAlphaSize";
			case RenderbufferParameterName::RenderbufferDepthSize:return "RenderbufferDepthSize";
			case RenderbufferParameterName::RenderbufferStencilSize:return "RenderbufferStencilSize";
			case RenderbufferParameterName::RenderbufferColorSamplesNv:return "RenderbufferColorSamplesNv";
			case RenderbufferParameterName::RenderbufferSamplesImg:return "RenderbufferSamplesImg";
			case RenderbufferParameterName::RenderbufferStorageSamplesAmd:return "RenderbufferStorageSamplesAmd";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(RenderbufferTarget value) {
		switch(value){
			case RenderbufferTarget::Renderbuffer:return "Renderbuffer";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramStagePName value) {
		switch(value){
			case ProgramStagePName::ActiveSubroutines:return "ActiveSubroutines";
			case ProgramStagePName::ActiveSubroutineUniforms:return "ActiveSubroutineUniforms";
			case ProgramStagePName::ActiveSubroutineUniformLocations:return "ActiveSubroutineUniformLocations";
			case ProgramStagePName::ActiveSubroutineMaxLength:return "ActiveSubroutineMaxLength";
			case ProgramStagePName::ActiveSubroutineUniformMaxLength:return "ActiveSubroutineUniformMaxLength";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PrecisionType value) {
		switch(value){
			case PrecisionType::LowFloat:return "LowFloat";
			case PrecisionType::MediumFloat:return "MediumFloat";
			case PrecisionType::HighFloat:return "HighFloat";
			case PrecisionType::LowInt:return "LowInt";
			case PrecisionType::MediumInt:return "MediumInt";
			case PrecisionType::HighInt:return "HighInt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ConditionalRenderMode value) {
		switch(value){
			case ConditionalRenderMode::QueryWait:return "QueryWait";
			case ConditionalRenderMode::QueryNoWait:return "QueryNoWait";
			case ConditionalRenderMode::QueryByRegionWait:return "QueryByRegionWait";
			case ConditionalRenderMode::QueryByRegionNoWait:return "QueryByRegionNoWait";
			case ConditionalRenderMode::QueryWaitInverted:return "QueryWaitInverted";
			case ConditionalRenderMode::QueryNoWaitInverted:return "QueryNoWaitInverted";
			case ConditionalRenderMode::QueryByRegionWaitInverted:return "QueryByRegionWaitInverted";
			case ConditionalRenderMode::QueryByRegionNoWaitInverted:return "QueryByRegionNoWaitInverted";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(BindTransformFeedbackTarget value) {
		switch(value){
			case BindTransformFeedbackTarget::TransformFeedback:return "TransformFeedback";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(QueryCounterTarget value) {
		switch(value){
			case QueryCounterTarget::Timestamp:return "Timestamp";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramResourceProperty value) {
		switch(value){
			case ProgramResourceProperty::NumCompatibleSubroutines:return "NumCompatibleSubroutines";
			case ProgramResourceProperty::CompatibleSubroutines:return "CompatibleSubroutines";
			case ProgramResourceProperty::Uniform:return "Uniform";
			case ProgramResourceProperty::IsPerPatch:return "IsPerPatch";
			case ProgramResourceProperty::NameLength:return "NameLength";
			case ProgramResourceProperty::Type:return "Type";
			case ProgramResourceProperty::ArraySize:return "ArraySize";
			case ProgramResourceProperty::Offset:return "Offset";
			case ProgramResourceProperty::BlockIndex:return "BlockIndex";
			case ProgramResourceProperty::ArrayStride:return "ArrayStride";
			case ProgramResourceProperty::MatrixStride:return "MatrixStride";
			case ProgramResourceProperty::IsRowMajor:return "IsRowMajor";
			case ProgramResourceProperty::AtomicCounterBufferIndex:return "AtomicCounterBufferIndex";
			case ProgramResourceProperty::BufferBinding:return "BufferBinding";
			case ProgramResourceProperty::BufferDataSize:return "BufferDataSize";
			case ProgramResourceProperty::NumActiveVariables:return "NumActiveVariables";
			case ProgramResourceProperty::ActiveVariables:return "ActiveVariables";
			case ProgramResourceProperty::ReferencedByVertexShader:return "ReferencedByVertexShader";
			case ProgramResourceProperty::ReferencedByTessControlShader:return "ReferencedByTessControlShader";
			case ProgramResourceProperty::ReferencedByTessEvaluationShader:return "ReferencedByTessEvaluationShader";
			case ProgramResourceProperty::ReferencedByGeometryShader:return "ReferencedByGeometryShader";
			case ProgramResourceProperty::ReferencedByFragmentShader:return "ReferencedByFragmentShader";
			case ProgramResourceProperty::ReferencedByComputeShader:return "ReferencedByComputeShader";
			case ProgramResourceProperty::TopLevelArraySize:return "TopLevelArraySize";
			case ProgramResourceProperty::TopLevelArrayStride:return "TopLevelArrayStride";
			case ProgramResourceProperty::Location:return "Location";
			case ProgramResourceProperty::LocationIndex:return "LocationIndex";
			case ProgramResourceProperty::LocationComponent:return "LocationComponent";
			case ProgramResourceProperty::TransformFeedbackBufferIndex:return "TransformFeedbackBufferIndex";
			case ProgramResourceProperty::TransformFeedbackBufferStride:return "TransformFeedbackBufferStride";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(VertexProvokingMode value) {
		switch(value){
			case VertexProvokingMode::FirstVertexConvention:return "FirstVertexConvention";
			case VertexProvokingMode::LastVertexConvention:return "LastVertexConvention";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(GetMultisamplePNameNV value) {
		switch(value){
			case GetMultisamplePNameNV::SamplePosition:return "SamplePosition";
			case GetMultisamplePNameNV::ProgrammableSampleLocationArb:return "ProgrammableSampleLocationArb";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PatchParameterName value) {
		switch(value){
			case PatchParameterName::PatchVertices:return "PatchVertices";
			case PatchParameterName::PatchDefaultInnerLevel:return "PatchDefaultInnerLevel";
			case PatchParameterName::PatchDefaultOuterLevel:return "PatchDefaultOuterLevel";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathStringFormat value) {
		switch(value){
			case PathStringFormat::PathFormatSvgNv:return "PathFormatSvgNv";
			case PathStringFormat::PathFormatPsNv:return "PathFormatPsNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathFontTarget value) {
		switch(value){
			case PathFontTarget::StandardFontNameNv:return "StandardFontNameNv";
			case PathFontTarget::SystemFontNameNv:return "SystemFontNameNv";
			case PathFontTarget::FileNameNv:return "FileNameNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathParameter value) {
		switch(value){
			case PathParameter::PathStrokeWidthNv:return "PathStrokeWidthNv";
			case PathParameter::PathEndCapsNv:return "PathEndCapsNv";
			case PathParameter::PathInitialEndCapNv:return "PathInitialEndCapNv";
			case PathParameter::PathTerminalEndCapNv:return "PathTerminalEndCapNv";
			case PathParameter::PathJoinStyleNv:return "PathJoinStyleNv";
			case PathParameter::PathMiterLimitNv:return "PathMiterLimitNv";
			case PathParameter::PathDashCapsNv:return "PathDashCapsNv";
			case PathParameter::PathInitialDashCapNv:return "PathInitialDashCapNv";
			case PathParameter::PathTerminalDashCapNv:return "PathTerminalDashCapNv";
			case PathParameter::PathDashOffsetNv:return "PathDashOffsetNv";
			case PathParameter::PathClientLengthNv:return "PathClientLengthNv";
			case PathParameter::PathFillModeNv:return "PathFillModeNv";
			case PathParameter::PathFillMaskNv:return "PathFillMaskNv";
			case PathParameter::PathFillCoverModeNv:return "PathFillCoverModeNv";
			case PathParameter::PathStrokeCoverModeNv:return "PathStrokeCoverModeNv";
			case PathParameter::PathStrokeMaskNv:return "PathStrokeMaskNv";
			case PathParameter::PathObjectBoundingBoxNv:return "PathObjectBoundingBoxNv";
			case PathParameter::PathCommandCountNv:return "PathCommandCountNv";
			case PathParameter::PathCoordCountNv:return "PathCoordCountNv";
			case PathParameter::PathDashArrayCountNv:return "PathDashArrayCountNv";
			case PathParameter::PathComputedLengthNv:return "PathComputedLengthNv";
			case PathParameter::PathFillBoundingBoxNv:return "PathFillBoundingBoxNv";
			case PathParameter::PathStrokeBoundingBoxNv:return "PathStrokeBoundingBoxNv";
			case PathParameter::PathDashOffsetResetNv:return "PathDashOffsetResetNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathCoverMode value) {
		switch(value){
			case PathCoverMode::PathFillCoverModeNv:return "PathFillCoverModeNv";
			case PathCoverMode::ConvexHullNv:return "ConvexHullNv";
			case PathCoverMode::BoundingBoxNv:return "BoundingBoxNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(InstancedPathCoverMode value) {
		switch(value){
			case InstancedPathCoverMode::PathFillCoverModeNv:return "PathFillCoverModeNv";
			case InstancedPathCoverMode::ConvexHullNv:return "ConvexHullNv";
			case InstancedPathCoverMode::BoundingBoxNv:return "BoundingBoxNv";
			case InstancedPathCoverMode::BoundingBoxOfBoundingBoxesNv:return "BoundingBoxOfBoundingBoxesNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathElementType value) {
		switch(value){
			case PathElementType::Utf8Nv:return "Utf8Nv";
			case PathElementType::Utf16Nv:return "Utf16Nv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathHandleMissingGlyphs value) {
		switch(value){
			case PathHandleMissingGlyphs::SkipMissingGlyphNv:return "SkipMissingGlyphNv";
			case PathHandleMissingGlyphs::UseMissingGlyphNv:return "UseMissingGlyphNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(PathListMode value) {
		switch(value){
			case PathListMode::AccumAdjacentPairsNv:return "AccumAdjacentPairsNv";
			case PathListMode::AdjacentPairsNv:return "AdjacentPairsNv";
			case PathListMode::FirstToRestNv:return "FirstToRestNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(AtomicCounterBufferPName value) {
		switch(value){
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByComputeShader:return "AtomicCounterBufferReferencedByComputeShader";
			case AtomicCounterBufferPName::AtomicCounterBufferBinding:return "AtomicCounterBufferBinding";
			case AtomicCounterBufferPName::AtomicCounterBufferDataSize:return "AtomicCounterBufferDataSize";
			case AtomicCounterBufferPName::AtomicCounterBufferActiveAtomicCounters:return "AtomicCounterBufferActiveAtomicCounters";
			case AtomicCounterBufferPName::AtomicCounterBufferActiveAtomicCounterIndices:return "AtomicCounterBufferActiveAtomicCounterIndices";
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByVertexShader:return "AtomicCounterBufferReferencedByVertexShader";
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByTessControlShader:return "AtomicCounterBufferReferencedByTessControlShader";
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByTessEvaluationShader:return "AtomicCounterBufferReferencedByTessEvaluationShader";
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByGeometryShader:return "AtomicCounterBufferReferencedByGeometryShader";
			case AtomicCounterBufferPName::AtomicCounterBufferReferencedByFragmentShader:return "AtomicCounterBufferReferencedByFragmentShader";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SyncParameterName value) {
		switch(value){
			case SyncParameterName::ObjectType:return "ObjectType";
			case SyncParameterName::SyncCondition:return "SyncCondition";
			case SyncParameterName::SyncStatus:return "SyncStatus";
			case SyncParameterName::SyncFlags:return "SyncFlags";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SyncCondition value) {
		switch(value){
			case SyncCondition::SyncGpuCommandsComplete:return "SyncGpuCommandsComplete";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SyncStatus value) {
		switch(value){
			case SyncStatus::AlreadySignaled:return "AlreadySignaled";
			case SyncStatus::TimeoutExpired:return "TimeoutExpired";
			case SyncStatus::ConditionSatisfied:return "ConditionSatisfied";
			case SyncStatus::WaitFailed:return "WaitFailed";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ProgramInterfacePName value) {
		switch(value){
			case ProgramInterfacePName::ActiveResources:return "ActiveResources";
			case ProgramInterfacePName::MaxNameLength:return "MaxNameLength";
			case ProgramInterfacePName::MaxNumActiveVariables:return "MaxNumActiveVariables";
			case ProgramInterfacePName::MaxNumCompatibleSubroutines:return "MaxNumCompatibleSubroutines";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(FramebufferParameterName value) {
		switch(value){
			case FramebufferParameterName::FramebufferDefaultWidth:return "FramebufferDefaultWidth";
			case FramebufferParameterName::FramebufferDefaultHeight:return "FramebufferDefaultHeight";
			case FramebufferParameterName::FramebufferDefaultLayers:return "FramebufferDefaultLayers";
			case FramebufferParameterName::FramebufferDefaultSamples:return "FramebufferDefaultSamples";
			case FramebufferParameterName::FramebufferDefaultFixedSampleLocations:return "FramebufferDefaultFixedSampleLocations";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ClipControlDepth value) {
		switch(value){
			case ClipControlDepth::NegativeOneToOne:return "NegativeOneToOne";
			case ClipControlDepth::ZeroToOne:return "ZeroToOne";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TextureLayout value) {
		switch(value){
			case TextureLayout::LayoutDepthReadOnlyStencilAttachmentExt:return "LayoutDepthReadOnlyStencilAttachmentExt";
			case TextureLayout::LayoutDepthAttachmentStencilReadOnlyExt:return "LayoutDepthAttachmentStencilReadOnlyExt";
			case TextureLayout::LayoutGeneralExt:return "LayoutGeneralExt";
			case TextureLayout::LayoutColorAttachmentExt:return "LayoutColorAttachmentExt";
			case TextureLayout::LayoutDepthStencilAttachmentExt:return "LayoutDepthStencilAttachmentExt";
			case TextureLayout::LayoutDepthStencilReadOnlyExt:return "LayoutDepthStencilReadOnlyExt";
			case TextureLayout::LayoutShaderReadOnlyExt:return "LayoutShaderReadOnlyExt";
			case TextureLayout::LayoutTransferSrcExt:return "LayoutTransferSrcExt";
			case TextureLayout::LayoutTransferDstExt:return "LayoutTransferDstExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(MemoryObjectParameterName value) {
		switch(value){
			case MemoryObjectParameterName::DedicatedMemoryObjectExt:return "DedicatedMemoryObjectExt";
			case MemoryObjectParameterName::ProtectedMemoryObjectExt:return "ProtectedMemoryObjectExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ExternalHandleType value) {
		switch(value){
			case ExternalHandleType::HandleTypeOpaqueFdExt:return "HandleTypeOpaqueFdExt";
			case ExternalHandleType::HandleTypeOpaqueWin32Ext:return "HandleTypeOpaqueWin32Ext";
			case ExternalHandleType::HandleTypeOpaqueWin32KmtExt:return "HandleTypeOpaqueWin32KmtExt";
			case ExternalHandleType::HandleTypeD3d12TilepoolExt:return "HandleTypeD3d12TilepoolExt";
			case ExternalHandleType::HandleTypeD3d12ResourceExt:return "HandleTypeD3d12ResourceExt";
			case ExternalHandleType::HandleTypeD3d11ImageExt:return "HandleTypeD3d11ImageExt";
			case ExternalHandleType::HandleTypeD3d11ImageKmtExt:return "HandleTypeD3d11ImageKmtExt";
			case ExternalHandleType::HandleTypeD3d12FenceExt:return "HandleTypeD3d12FenceExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(SemaphoreParameterName value) {
		switch(value){
			case SemaphoreParameterName::D3d12FenceValueExt:return "D3d12FenceValueExt";
			case SemaphoreParameterName::SemaphoreTypeNv:return "SemaphoreTypeNv";
			case SemaphoreParameterName::SemaphoreTypeBinaryNv:return "SemaphoreTypeBinaryNv";
			case SemaphoreParameterName::SemaphoreTypeTimelineNv:return "SemaphoreTypeTimelineNv";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShadingRateQCOM value) {
		switch(value){
			case ShadingRateQCOM::ShadingRate1X1PixelsQcom:return "ShadingRate1X1PixelsQcom";
			case ShadingRateQCOM::ShadingRate1X2PixelsQcom:return "ShadingRate1X2PixelsQcom";
			case ShadingRateQCOM::ShadingRate2X1PixelsQcom:return "ShadingRate2X1PixelsQcom";
			case ShadingRateQCOM::ShadingRate2X2PixelsQcom:return "ShadingRate2X2PixelsQcom";
			case ShadingRateQCOM::ShadingRate1X4PixelsQcom:return "ShadingRate1X4PixelsQcom";
			case ShadingRateQCOM::ShadingRate4X1PixelsQcom:return "ShadingRate4X1PixelsQcom";
			case ShadingRateQCOM::ShadingRate4X2PixelsQcom:return "ShadingRate4X2PixelsQcom";
			case ShadingRateQCOM::ShadingRate2X4PixelsQcom:return "ShadingRate2X4PixelsQcom";
			case ShadingRateQCOM::ShadingRate4X4PixelsQcom:return "ShadingRate4X4PixelsQcom";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShadingRate value) {
		switch(value){
			case ShadingRate::ShadingRate1X1PixelsExt:return "ShadingRate1X1PixelsExt";
			case ShadingRate::ShadingRate1X2PixelsExt:return "ShadingRate1X2PixelsExt";
			case ShadingRate::ShadingRate2X1PixelsExt:return "ShadingRate2X1PixelsExt";
			case ShadingRate::ShadingRate2X2PixelsExt:return "ShadingRate2X2PixelsExt";
			case ShadingRate::ShadingRate1X4PixelsExt:return "ShadingRate1X4PixelsExt";
			case ShadingRate::ShadingRate4X1PixelsExt:return "ShadingRate4X1PixelsExt";
			case ShadingRate::ShadingRate4X2PixelsExt:return "ShadingRate4X2PixelsExt";
			case ShadingRate::ShadingRate2X4PixelsExt:return "ShadingRate2X4PixelsExt";
			case ShadingRate::ShadingRate4X4PixelsExt:return "ShadingRate4X4PixelsExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(TexStorageAttribs value) {
		switch(value){
			case TexStorageAttribs::SurfaceCompressionExt:return "SurfaceCompressionExt";
			case TexStorageAttribs::SurfaceCompressionFixedRateNoneExt:return "SurfaceCompressionFixedRateNoneExt";
			case TexStorageAttribs::SurfaceCompressionFixedRateDefaultExt:return "SurfaceCompressionFixedRateDefaultExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate1BpcExt:return "SurfaceCompressionFixedRate1BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate2BpcExt:return "SurfaceCompressionFixedRate2BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate3BpcExt:return "SurfaceCompressionFixedRate3BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate4BpcExt:return "SurfaceCompressionFixedRate4BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate5BpcExt:return "SurfaceCompressionFixedRate5BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate6BpcExt:return "SurfaceCompressionFixedRate6BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate7BpcExt:return "SurfaceCompressionFixedRate7BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate8BpcExt:return "SurfaceCompressionFixedRate8BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate9BpcExt:return "SurfaceCompressionFixedRate9BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate10BpcExt:return "SurfaceCompressionFixedRate10BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate11BpcExt:return "SurfaceCompressionFixedRate11BpcExt";
			case TexStorageAttribs::SurfaceCompressionFixedRate12BpcExt:return "SurfaceCompressionFixedRate12BpcExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(ShadingRateCombinerOp value) {
		switch(value){
			case ShadingRateCombinerOp::FragmentShadingRateCombinerOpKeepExt:return "FragmentShadingRateCombinerOpKeepExt";
			case ShadingRateCombinerOp::FragmentShadingRateCombinerOpReplaceExt:return "FragmentShadingRateCombinerOpReplaceExt";
			case ShadingRateCombinerOp::FragmentShadingRateCombinerOpMinExt:return "FragmentShadingRateCombinerOpMinExt";
			case ShadingRateCombinerOp::FragmentShadingRateCombinerOpMaxExt:return "FragmentShadingRateCombinerOpMaxExt";
			case ShadingRateCombinerOp::FragmentShadingRateCombinerOpMulExt:return "FragmentShadingRateCombinerOpMulExt";
			default: return "Unknown";
		}
	}
	constexpr auto ToString(HintTargetPGI value) {
		switch(value){
			case HintTargetPGI::VertexDataHintPgi:return "VertexDataHintPgi";
			case HintTargetPGI::VertexConsistentHintPgi:return "VertexConsistentHintPgi";
			case HintTargetPGI::MaterialSideHintPgi:return "MaterialSideHintPgi";
			case HintTargetPGI::MaxVertexHintPgi:return "MaxVertexHintPgi";
			default: return "Unknown";
		}
	}
}
