#include "texture_program.hpp"

#include "compile_program.hpp"
#include "gl_errors.hpp"

TextureProgram::TextureProgram() {
	program = compile_program(
		"#version 330\n"
		"uniform mat4 object_to_clip;\n"
		"uniform mat4x3 object_to_light;\n"
		"uniform mat3 normal_to_light;\n"
		"uniform mat4 light_to_spot;\n"
		"layout(location=0) in vec4 Position;\n" //note: layout keyword used to make sure that the location-0 attribute is always bound to something
		"in vec3 Normal;\n"
		"in vec4 Color;\n"
		"in vec2 TexCoord;\n"
		"out vec3 position;\n"
		"out vec3 normal;\n"
		"out vec4 color;\n"
		"out vec2 texCoord;\n"
		"out vec4 spotPosition;\n"
		"void main() {\n"
		"	gl_Position = object_to_clip * Position;\n"
		"	position = object_to_light * Position;\n"
		"	spotPosition = light_to_spot * vec4(position, 1.0);\n"
		"	normal = normal_to_light * Normal;\n"
		"	color = Color;\n"
		"	texCoord = TexCoord;\n"
		"}\n"
		,
		"#version 330\n"
		"uniform vec3 sun_direction;\n"
		"uniform vec3 sun_color;\n"
		"uniform vec3 sky_direction;\n"
		"uniform vec3 sky_color;\n"
		"uniform vec3 spot_position;\n"
		"uniform vec3 spot_direction;\n"
		"uniform vec3 spot_color;\n"
		"uniform vec2 spot_outer_inner;\n"
		"uniform sampler2D tex;\n"
		"uniform sampler2DShadow spot_depth_tex;\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec4 color;\n"
		"in vec2 texCoord;\n"
		"in vec4 spotPosition;\n"
		"vec4 fragColor;\n"

		"vec4 bloomColor;\n"
		"vec4 brightColor;\n"
		"float brightness;\n"
		"vec4 bloomBlurRes;\n"
		"out vec4 combine_result;\n"
		"vec2 tex_offset;\n"
		"vec3 result;\n"
		"bool horizontal;\n"
		"uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);\n"


		"void main() {\n"
		"	vec3 total_light = vec3(0.0, 0.0, 0.0);\n"
		"	vec3 n = normalize(normal);\n"
		"	{ //sky (hemisphere) light:\n"
		"		vec3 l = sky_direction;\n"
		"		float nl = 0.5 + 0.5 * dot(n,l);\n"
		"		total_light += nl * sky_color;\n"
		"	}\n"
		"	{ //sun (directional) light:\n"
		"		vec3 l = sun_direction;\n"
		"		float nl = max(0.0, dot(n,l));\n"
		"		total_light += nl * sun_color;\n"
		"	}\n"
		"	{ //spot (point with fov + shadow map) light:\n"
		"		vec3 l = normalize(spot_position - position);\n"
		"		float nl = max(0.0, dot(n,l));\n"
		"		//TODO: look up shadow map\n"
		"		float d = dot(l,-spot_direction);\n"
		"		float amt = smoothstep(spot_outer_inner.x, spot_outer_inner.y, d);\n"
		"		float shadow = textureProj(spot_depth_tex, spotPosition);\n"
		"		total_light += shadow * nl * amt * spot_color;\n"
		//"		fragColor = vec4(s,s,s, 1.0);\n" //DEBUG: just show shadow
		"	}\n"

		"	fragColor = texture(tex, texCoord) * vec4(color.rgb * total_light, color.a);\n"
		" bloomColor = texture(tex, texCoord);\n"
		" brightness = dot(bloomColor.rgb, vec3(0.2126, 0.7152, 0.0722));\n"
		" if(brightness > 0.7) brightColor = vec4(fragColor.rgb, 1.0);\n"
		" else brightColor = vec4(0.0, 0.0, 0.0, 1.0);\n"

		// "	tex_offset = 1.0 / textureSize(tex, 0); \n"
		// " result = brightColor * weight[0]; \n"
		// " for(int n = 0; n < 10; n++){\n"
		// "		for(int i = 1; i < 5; ++i)\n"
		// "		{\n"
		// "   	result += brightColor * weight[i];\n"
		// // "   	result += brightColor * weight[i];\n"
		// " 	}\n"
		// " 	for(int i = 1; i < 5; ++i)\n"
		// " 	{\n"
		// "   	result += brightColor * weight[i];\n"
		// // "   	result += brightColor * weight[i];\n"
		// " 	}\n"
		// " }\n"
		"tex_offset = 1.0 / textureSize(tex, 0);\n"
    "result = texture(tex, texCoord).rgb * weight[0]; \n"
		"for(int n = 0; n < 5; n++){\n"
    "    for(int i = 1; i < 5; ++i)\n"
    "    {\n"
    "        result += texture(tex, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
    "        result += texture(tex, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
    "    }\n"
    "    for(int i = 1; i < 5; ++i)\n"
    "    {\n"
    "        result += texture(tex, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
    "        result += texture(tex, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
    "    }\n"
		" }\n"
		"	bloomBlurRes = vec4(result, 1.0);\n"
 		" combine_result = bloomBlurRes + brightColor;\n"
		" combine_result = vec4(1.0) - exp(bloomBlurRes * 0.13);\n"
		" combine_result = pow(combine_result, vec4(1.0 / 2.2));\n"
		" combine_result[3] = 1.0;\n"
		// 2.2 is the value of gamma
		"}\n"
	);

	object_to_clip_mat4 = glGetUniformLocation(program, "object_to_clip");
	object_to_light_mat4x3 = glGetUniformLocation(program, "object_to_light");
	normal_to_light_mat3 = glGetUniformLocation(program, "normal_to_light");

	sun_direction_vec3 = glGetUniformLocation(program, "sun_direction");
	sun_color_vec3 = glGetUniformLocation(program, "sun_color");
	sky_direction_vec3 = glGetUniformLocation(program, "sky_direction");
	sky_color_vec3 = glGetUniformLocation(program, "sky_color");

	spot_position_vec3 = glGetUniformLocation(program, "spot_position");
	spot_direction_vec3 = glGetUniformLocation(program, "spot_direction");
	spot_color_vec3 = glGetUniformLocation(program, "spot_color");
	spot_outer_inner_vec2 = glGetUniformLocation(program, "spot_outer_inner");

	light_to_spot_mat4 = glGetUniformLocation(program, "light_to_spot");

	glUseProgram(program);

	GLuint tex_sampler2D = glGetUniformLocation(program, "tex");
	glUniform1i(tex_sampler2D, 0);

	GLuint spot_depth_tex_sampler2D = glGetUniformLocation(program, "spot_depth_tex");
	glUniform1i(spot_depth_tex_sampler2D, 1);

	glUseProgram(0);

	GL_ERRORS();
}

Load< TextureProgram > texture_program(LoadTagInit, [](){
	return new TextureProgram();
});
