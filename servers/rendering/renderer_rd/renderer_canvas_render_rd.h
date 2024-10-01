/**************************************************************************/
/*  renderer_canvas_render_rd.h                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             Redot ENGINE                               */
/*                        https://Redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Redot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Skibidi Toilet, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef RENDERER_CANVAS_RENDER_RD_H
#define RENDERER_CANVAS_RENDER_RD_H

#include "servers/rendering/renderer_canvas_render.h"
#include "servers/rendering/renderer_compositor.h"
#include "servers/rendering/renderer_rd/pipeline_cache_rd.h"
#include "servers/rendering/renderer_rd/shaders/canvas.glsl.gen.h"
#include "servers/rendering/renderer_rd/shaders/canvas_occlusion.glsl.gen.h"
#include "servers/rendering/renderer_rd/storage_rd/material_storage.h"
#include "servers/rendering/rendering_device.h"
#include "servers/rendering/shader_compiler.h"

class RendererCanvasRenderRD : public RendererCanvasRender {
	enum {
		BASE_UNIFORM_SET = 0,
		MATERIAL_UNIFORM_SET = 1,
		TRANSFORMS_UNIFORM_SET = 2,
		CANVAS_TEXTURE_UNIFORM_SET = 3,
		INSTANCE_DATA_UNIFORM_SET = 4,
	};

	const int SAMPLERS_BINDING_FIRST_INDEX = 10;

	enum ShaderVariant {
		SHADER_VARIANT_QUAD,
		SHADER_VARIANT_NINEPATCH,
		SHADER_VARIANT_PRIMITIVE,
		SHADER_VARIANT_PRIMITIVE_POINTS,
		SHADER_VARIANT_ATTRIBUTES,
		SHADER_VARIANT_ATTRIBUTES_POINTS,
		SHADER_VARIANT_QUAD_LIGHT,
		SHADER_VARIANT_NINEPATCH_LIGHT,
		SHADER_VARIANT_PRIMITIVE_LIGHT,
		SHADER_VARIANT_PRIMITIVE_POINTS_LIGHT,
		SHADER_VARIANT_ATTRIBUTES_LIGHT,
		SHADER_VARIANT_ATTRIBUTES_POINTS_LIGHT,
		SHADER_VARIANT_MAX
	};

	enum {

		FLAGS_INSTANCING_MASK = 0x7F,
		FLAGS_INSTANCING_HAS_COLORS = (1 << 7),
		FLAGS_INSTANCING_HAS_CUSTOM_DATA = (1 << 8),

		FLAGS_CLIP_RECT_UV = (1 << 9),
		FLAGS_TRANSPOSE_RECT = (1 << 10),

		FLAGS_CONVERT_ATTRIBUTES_TO_LINEAR = (1 << 11),

		FLAGS_NINEPACH_DRAW_CENTER = (1 << 12),

		FLAGS_USE_SKELETON = (1 << 15),
		FLAGS_NINEPATCH_H_MODE_SHIFT = 16,
		FLAGS_NINEPATCH_V_MODE_SHIFT = 18,
		FLAGS_LIGHT_COUNT_SHIFT = 20,

		FLAGS_DEFAULT_NORMAL_MAP_USED = (1 << 26),
		FLAGS_DEFAULT_SPECULAR_MAP_USED = (1 << 27),

		FLAGS_USE_MSDF = (1 << 28),
		FLAGS_USE_LCD = (1 << 29),

		FLAGS_FLIP_H = (1 << 30),
		FLAGS_FLIP_V = (1 << 31),
	};

	enum {
		LIGHT_FLAGS_TEXTURE_MASK = 0xFFFF,
		LIGHT_FLAGS_BLEND_SHIFT = 16,
		LIGHT_FLAGS_BLEND_MASK = (3 << 16),
		LIGHT_FLAGS_BLEND_MODE_ADD = (0 << 16),
		LIGHT_FLAGS_BLEND_MODE_SUB = (1 << 16),
		LIGHT_FLAGS_BLEND_MODE_MIX = (2 << 16),
		LIGHT_FLAGS_BLEND_MODE_MASK = (3 << 16),
		LIGHT_FLAGS_HAS_SHADOW = (1 << 20),
		LIGHT_FLAGS_FILTER_SHIFT = 22

	};

	enum {
		MAX_RENDER_ITEMS = 256 * 1024,
		MAX_LIGHT_TEXTURES = 1024,
		MAX_LIGHTS_PER_ITEM = 16,
		DEFAULT_MAX_LIGHTS_PER_RENDER = 256
	};

	/****************/
	/**** SHADER ****/
	/****************/

	enum PipelineVariant {
		PIPELINE_VARIANT_QUAD,
		PIPELINE_VARIANT_NINEPATCH,
		PIPELINE_VARIANT_PRIMITIVE_TRIANGLES,
		PIPELINE_VARIANT_PRIMITIVE_LINES,
		PIPELINE_VARIANT_PRIMITIVE_POINTS,
		PIPELINE_VARIANT_ATTRIBUTE_TRIANGLES,
		PIPELINE_VARIANT_ATTRIBUTE_TRIANGLE_STRIP,
		PIPELINE_VARIANT_ATTRIBUTE_LINES,
		PIPELINE_VARIANT_ATTRIBUTE_LINES_STRIP,
		PIPELINE_VARIANT_ATTRIBUTE_POINTS,
		PIPELINE_VARIANT_QUAD_LCD_BLEND,
		PIPELINE_VARIANT_MAX
	};
	enum PipelineLightMode {
		PIPELINE_LIGHT_MODE_DISABLED,
		PIPELINE_LIGHT_MODE_ENABLED,
		PIPELINE_LIGHT_MODE_MAX
	};

	struct PipelineVariants {
		PipelineCacheRD variants[PIPELINE_LIGHT_MODE_MAX][PIPELINE_VARIANT_MAX];
	};

	struct {
		CanvasShaderRD canvas_shader;
		RID default_version;
		RID default_version_rd_shader;
		RID quad_index_buffer;
		RID quad_index_array;
		PipelineVariants pipeline_variants;

		ShaderCompiler compiler;
	} shader;

	struct CanvasShaderData : public RendererRD::MaterialStorage::ShaderData {
		enum BlendMode { //used internally
			BLEND_MODE_MIX,
			BLEND_MODE_ADD,
			BLEND_MODE_SUB,
			BLEND_MODE_MUL,
			BLEND_MODE_PMALPHA,
			BLEND_MODE_DISABLED,
		};

		bool valid = false;
		RID version;
		PipelineVariants pipeline_variants;

		Vector<ShaderCompiler::GeneratedCode::Texture> texture_uniforms;

		Vector<uint32_t> ubo_offsets;
		uint32_t ubo_size = 0;

		String code;

		bool uses_screen_texture = false;
		bool uses_screen_texture_mipmaps = false;
		bool uses_sdf = false;
		bool uses_time = false;

		virtual void set_code(const String &p_Code);
		virtual bool is_animated() const;
		virtual bool casts_shadows() const;
		virtual RS::ShaderNativeSourceCode get_native_source_code() const;

		CanvasShaderData() {}
		virtual ~CanvasShaderData();
	};

	RendererRD::MaterialStorage::ShaderData *_create_shader_func();
	static RendererRD::MaterialStorage::ShaderData *_create_shader_funcs() {
		return static_cast<RendererCanvasRenderRD *>(singleton)->_create_shader_func();
	}

	struct CanvasMaterialData : public RendererRD::MaterialStorage::MaterialData {
		CanvasShaderData *shader_data = nullptr;
		RID uniform_set;
		RID uniform_set_srgb;

		virtual void set_render_priority(int p_priority) {}
		virtual void set_next_pass(RID p_pass) {}
		virtual bool update_parameters(const HashMap<StringName, Variant> &p_parameters, bool p_uniform_dirty, bool p_textures_dirty);
		virtual ~CanvasMaterialData();
	};

	RendererRD::MaterialStorage::MaterialData *_create_material_func(CanvasShaderData *p_shader);
	static RendererRD::MaterialStorage::MaterialData *_create_material_funcs(RendererRD::MaterialStorage::ShaderData *p_shader) {
		return static_cast<RendererCanvasRenderRD *>(singleton)->_create_material_func(static_cast<CanvasShaderData *>(p_shader));
	}

	/**************************/
	/**** CANVAS TEXTURES *****/
	/**************************/

	struct {
		RS::CanvasItemTextureFilter default_filter;
		RS::CanvasItemTextureRepeat default_repeat;
	} default_samplers;

	/******************/
	/**** POLYGONS ****/
	/******************/

	struct PolygonBuffers {
		RD::VertexFormatID vertex_format_id;
		RID vertex_buffer;
		RID vertex_array;
		RID index_buffer;
		RID indices;
		uint32_t primitive_count = 0;
	};

	struct {
		HashMap<PolygonID, PolygonBuffers> polygons;
		PolygonID last_id;
	} polygon_buffers;

	/********************/
	/**** PRIMITIVES ****/
	/********************/

	struct {
		RID index_array[4];
	} primitive_arrays;

	/*******************/
	/**** MATERIALS ****/
	/*******************/

	/******************/
	/**** LIGHTING ****/
	/******************/

	struct CanvasLight {
		RID texture;
		struct {
			bool enabled = false;
			float z_far;
			float y_offset;
			Transform2D directional_xform;
		} shadow;
	};

	RID_Owner<CanvasLight> canvas_light_owner;

	struct ShadowRenderPushConstant {
		float projection[16];
		float modelview[8];
		float direction[2];
		float z_far;
		float pad;
	};

	struct OccluderPolygon {
		RS::CanvasOccluderPolygonCullMode cull_mode;
		int line_point_count;
		RID vertex_buffer;
		RID vertex_array;
		RID index_buffer;
		RID index_array;

		int sdf_point_count;
		int sdf_index_count;
		RID sdf_vertex_buffer;
		RID sdf_vertex_array;
		RID sdf_index_buffer;
		RID sdf_index_array;
		bool sdf_is_lines;
	};

	struct LightUniform {
		float matrix[8]; //light to texture coordinate matrix
		float shadow_matrix[8]; //light to shadow coordinate matrix
		float color[4];

		uint8_t shadow_color[4];
		uint32_t flags; //index to light texture
		float shadow_pixel_size;
		float height;

		float position[2];
		float shadow_z_far_inv;
		float shadow_y_ofs;

		float atlas_rect[4];
	};

	RID_Owner<OccluderPolygon> occluder_polygon_owner;

	enum ShadowRenderMode {
		SHADOW_RENDER_MODE_SHADOW,
		SHADOW_RENDER_MODE_SDF,
	};

	enum {
		SHADOW_RENDER_SDF_TRIANGLES,
		SHADOW_RENDER_SDF_LINES,
	};

	struct {
		CanvasOcclusionShaderRD shader;
		RID shader_version;
		RID render_pipelines[3];
		RID sdf_render_pipelines[2];
		RD::VertexFormatID vertex_format;
		RD::VertexFormatID sdf_vertex_format;
		RD::FramebufferFormatID framebuffer_format;
		RD::FramebufferFormatID sdf_framebuffer_format;
	} shadow_render;

	/***************/
	/**** STATE ****/
	/***************/

	//state that does not vary across rendering all items

	struct InstanceData {
		float world[6];
		uint32_t flags;
		uint32_t specular_shininess;
		union {
			//rect
			struct {
				float modulation[4];
				union {
					float msdf[4];
					float ninepatch_margins[4];
				};
				float dst_rect[4];
				float src_rect[4];
				float pad[2];
			};
			//primitive
			struct {
				float points[6]; // vec2 points[3]
				float uvs[6]; // vec2 points[3]
				uint32_t colors[6]; // colors encoded as half
			};
		};
		float color_texture_pixel_size[2];
		uint32_t lights[4];
	};

	struct PushConstant {
		uint32_t base_instance_index;
		uint32_t pad1;
		uint32_t pad2;
		uint32_t pad3;
	};

	// TextureState is used to determine when a new batch is required due to a change of texture state.
	struct TextureState {
		static const uint32_t FILTER_SHIFT = 0;
		static const uint32_t FILTER_BITS = 3;
		static const uint32_t FILTER_MASK = (1 << FILTER_BITS) - 1;
		static const uint32_t REPEAT_SHIFT = FILTER_BITS;
		static const uint32_t REPEAT_BITS = 2;
		static const uint32_t REPEAT_MASK = (1 << REPEAT_BITS) - 1;
		static const uint32_t TEXTURE_IS_DATA_SHIFT = REPEAT_SHIFT + REPEAT_BITS;
		static const uint32_t TEXTURE_IS_DATA_BITS = 1;
		static const uint32_t TEXTURE_IS_DATA_MASK = (1 << TEXTURE_IS_DATA_BITS) - 1;
		static const uint32_t LINEAR_COLORS_SHIFT = TEXTURE_IS_DATA_SHIFT + TEXTURE_IS_DATA_BITS;
		static const uint32_t LINEAR_COLORS_BITS = 1;
		static const uint32_t LINEAR_COLORS_MASK = (1 << LINEAR_COLORS_BITS) - 1;

		RID texture;
		uint32_t other = 0;

		TextureState() {}

		TextureState(RID p_texture, RS::CanvasItemTextureFilter p_base_filter, RS::CanvasItemTextureRepeat p_base_repeat, bool p_texture_is_data, bool p_use_linear_colors) {
			texture = p_texture;
			other = (((uint32_t)p_base_filter & FILTER_MASK) << FILTER_SHIFT) |
					(((uint32_t)p_base_repeat & REPEAT_MASK) << REPEAT_SHIFT) |
					(((uint32_t)p_texture_is_data & TEXTURE_IS_DATA_MASK) << TEXTURE_IS_DATA_SHIFT) |
					(((uint32_t)p_use_linear_colors & LINEAR_COLORS_MASK) << LINEAR_COLORS_SHIFT);
		}

		_FORCE_INLINE_ RS::CanvasItemTextureFilter texture_filter() const {
			return (RS::CanvasItemTextureFilter)((other >> FILTER_SHIFT) & FILTER_MASK);
		}

		_FORCE_INLINE_ RS::CanvasItemTextureRepeat texture_repeat() const {
			return (RS::CanvasItemTextureRepeat)((other >> REPEAT_SHIFT) & REPEAT_MASK);
		}

		_FORCE_INLINE_ bool linear_colors() const {
			return (other >> LINEAR_COLORS_SHIFT) & LINEAR_COLORS_MASK;
		}

		_FORCE_INLINE_ bool texture_is_data() const {
			return (other >> TEXTURE_IS_DATA_SHIFT) & TEXTURE_IS_DATA_MASK;
		}

		bool operator==(const TextureState &p_val) const {
			return (texture == p_val.texture) && (other == p_val.other);
		}

		bool operator!=(const TextureState &p_val) const {
			return (texture != p_val.texture) || (other != p_val.other);
		}
	};

	struct Batch {
		// Position in the UBO measured in bytes
		uint32_t start = 0;
		uint32_t instance_count = 0;
		uint32_t instance_buffer_index = 0;

		TextureState tex_state;
		RID tex_uniform_set;

		// The following tex_ prefixed fields are used to cache the texture data for the current batch.
		// These values are applied to new InstanceData for the batch

		// The cached specular shininess derived from the current texture.
		uint32_t tex_specular_shininess = 0;
		// The cached texture flags, such as FLAGS_DEFAULT_SPECULAR_MAP_USED and FLAGS_DEFAULT_NORMAL_MAP_USED
		uint32_t tex_flags = 0;
		// The cached texture pixel size.
		Vector2 tex_texpixel_size;

		Color modulate = Color(1.0, 1.0, 1.0, 1.0);

		Item *clip = nullptr;

		RID material;
		CanvasMaterialData *material_data = nullptr;
		PipelineLightMode light_mode = PipelineLightMode::PIPELINE_LIGHT_MODE_DISABLED;
		PipelineVariant pipeline_variant = PipelineVariant::PIPELINE_VARIANT_QUAD;

		const Item::Command *command = nullptr;
		Item::Command::Type command_type = Item::Command::TYPE_ANIMATION_SLICE; // Can default to any type that doesn't form a batch.

		// batch-specific data
		union {
			// TYPE_PRIMITIVE
			uint32_t primitive_points = 0;
			// TYPE_PARTICLES
			uint32_t mesh_instance_count;
		};
		bool has_blend = false;

		void set_tex_state(TextureState &p_tex_state) {
			tex_state = p_tex_state;
			tex_uniform_set = RID();
			tex_texpixel_size = Size2();
			tex_specular_shininess = 0;
			tex_flags = 0;
		}
	};

	struct DataBuffer {
		LocalVector<RID> instance_buffers;
	};

	struct State {
		//state buffer
		struct Buffer {
			float canvas_transform[16];
			float screen_transform[16];
			float canvas_normal_transform[16];
			float canvas_modulate[4];

			float screen_pixel_size[2];
			float time;
			uint32_t use_pixel_snap;

			float sdf_to_tex[4];
			float sdf_to_screen[2];
			float screen_to_sdf[2];

			uint32_t directional_light_count;
			float tex_to_sdf;
			uint32_t pad1;
			uint32_t pad2;
		};

		LocalVector<DataBuffer> canvas_instance_data_buffers;
		LocalVector<Batch> canvas_instance_batches;
		uint32_t current_data_buffer_index = 0;
		uint32_t current_instance_buffer_index = 0;
		uint32_t current_batch_index = 0;
		uint32_t last_instance_index = 0;
		InstanceData *instance_data_array = nullptr;

		uint32_t max_instances_per_buffer = 16384;
		uint32_t max_instance_buffer_size = 16384 * sizeof(InstanceData);

		RID current_tex_uniform_set;

		LightUniform *light_uniforms = nullptr;

		RID lights_uniform_buffer;
		RID canvas_state_buffer;
		RID shadow_sampler;
		RID shadow_texture;
		RID shadow_depth_texture;
		RID shadow_fb;
		int shadow_texture_size = 2048;

		RID default_transforms_uniform_set;

		uint32_t max_lights_per_render;
		uint32_t max_lights_per_item;

		double time;

	} state;

	Item *items[MAX_RENDER_ITEMS];

	bool using_directional_lights = false;
	RID default_canvas_texture;

	RID default_canvas_group_shader;
	RID default_canvas_group_material;
	RID default_clip_children_material;
	RID default_clip_children_shader;

	RS::CanvasItemTextureFilter default_filter = RS::CANVAS_ITEM_TEXTURE_FILTER_LINEAR;
	RS::CanvasItemTextureRepeat default_repeat = RS::CANVAS_ITEM_TEXTURE_REPEAT_DISABLED;

	RID _create_base_uniform_set(RID p_to_render_target, bool p_backbuffer);

	bool debug_redraw = false;
	Color debug_redraw_color;
	double debug_redraw_time = 1.0;

	// A structure to store cached render target information
	struct RenderTarget {
		// Current render target for the canvas.
		RID render_target;
		// The base flags for each InstanceData, derived from the render target.
		// Either FLAGS_CONVERT_ATTRIBUTES_TO_LINEAR or 0
		uint32_t base_flags = 0;
	};

	void _render_batch_items(RenderTarget p_to_render_target, int p_item_count, const Transform2D &p_canvas_transform_inverse, Light *p_lights, bool &r_sdf_used, bool p_to_backbuffer = false, RenderingMethod::RenderInfo *r_render_info = nullptr);
	void _record_item_commands(const Item *p_item, RenderTarget p_render_target, const Transform2D &p_base_transform, Item *&r_current_clip, Light *p_lights, uint32_t &r_index, bool &r_batch_broken, bool &r_sdf_used, Batch *&r_current_batch);
	void _render_batch(RD::DrawListID p_draw_list, PipelineVariants *p_pipeline_variants, RenderingDevice::FramebufferFormatID p_framebuffer_format, Light *p_lights, Batch const *p_batch, RenderingMethod::RenderInfo *r_render_info = nullptr);
	void _prepare_batch_texture(Batch *p_current_batch, RID p_texture) const;
	void _bind_canvas_texture(RD::DrawListID p_draw_list, RID p_uniform_set);
	[[nodiscard]] Batch *_new_batch(bool &r_batch_broken);
	void _add_to_batch(uint32_t &r_index, bool &r_batch_broken, Batch *&r_current_batch);
	void _allocate_instance_buffer();

	_FORCE_INLINE_ void _update_transform_2d_to_mat2x4(const Transform2D &p_transform, float *p_mat2x4);
	_FORCE_INLINE_ void _update_transform_2d_to_mat2x3(const Transform2D &p_transform, float *p_mat2x3);

	_FORCE_INLINE_ void _update_transform_2d_to_mat4(const Transform2D &p_transform, float *p_mat4);
	_FORCE_INLINE_ void _update_transform_to_mat4(const Transform3D &p_transform, float *p_mat4);

	void _update_shadow_atlas();

public:
	PolygonID request_polygon(const Vector<int> &p_indices, const Vector<Point2> &p_points, const Vector<Color> &p_colors, const Vector<Point2> &p_uvs = Vector<Point2>(), const Vector<int> &p_bones = Vector<int>(), const Vector<float> &p_weights = Vector<float>()) override;
	void free_polygon(PolygonID p_polygon) override;

	RID light_create() override;
	void light_set_texture(RID p_rid, RID p_texture) override;
	void light_set_use_shadow(RID p_rid, bool p_enable) override;
	void light_update_shadow(RID p_rid, int p_shadow_index, const Transform2D &p_light_xform, int p_light_mask, float p_near, float p_far, LightOccluderInstance *p_occluders) override;
	void light_update_directional_shadow(RID p_rid, int p_shadow_index, const Transform2D &p_light_xform, int p_light_mask, float p_cull_distance, const Rect2 &p_clip_rect, LightOccluderInstance *p_occluders) override;

	virtual void render_sdf(RID p_render_target, LightOccluderInstance *p_occluders) override;

	RID occluder_polygon_create() override;
	void occluder_polygon_set_shape(RID p_occluder, const Vector<Vector2> &p_points, bool p_closed) override;
	void occluder_polygon_set_cull_mode(RID p_occluder, RS::CanvasOccluderPolygonCullMode p_mode) override;

	void canvas_render_items(RID p_to_render_target, Item *p_item_list, const Color &p_modulate, Light *p_light_list, Light *p_directional_light_list, const Transform2D &p_canvas_transform, RS::CanvasItemTextureFilter p_default_filter, RS::CanvasItemTextureRepeat p_default_repeat, bool p_snap_2d_vertices_to_pixel, bool &r_sdf_used, RenderingMethod::RenderInfo *r_render_info = nullptr) override;

	virtual void set_shadow_texture_size(int p_size) override;

	void set_debug_redraw(bool p_enabled, double p_time, const Color &p_color) override;

	void set_time(double p_time);
	void update() override;
	bool free(RID p_rid) override;
	RendererCanvasRenderRD();
	~RendererCanvasRenderRD();
};

#endif // RENDERER_CANVAS_RENDER_RD_H
