#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct _texture_atlas_frame_t {
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
} texture_atlas_frame_t;

#pragma pack()

typedef struct _texture_atlas_texture_t {
	const char* name;
	unsigned int num_frames;
	texture_atlas_frame_t* frames;
} texture_atlas_texture_t;

typedef struct _texture_atlas_t {
	unsigned int width;
	unsigned int height;
	unsigned int num_textures;
	texture_atlas_texture_t* textures;
} texture_atlas_t;

int texture_atlas_load(const char* path, texture_atlas_t** ta);

int texture_atlas_free(texture_atlas_t* ta);

texture_atlas_texture_t* texture_atlas_lookup(texture_atlas_t* ta, const char* name);

#ifdef __cplusplus
}
#endif

#endif /* TEXTURE_ATLAS_H */
