#include "vertex_color_program.hpp"

#include "compile_program.hpp"

VertexColorProgram::VertexColorProgram() {
	program = compile_program(
		"#version 330\n"
		"uniform mat4 object_to_clip;\n"
		"uniform mat4x3 object_to_light;\n"
		"uniform mat3 normal_to_light;\n"
		"layout(location=0) in vec4 Position;\n" //note: layout keyword used to make sure that the location-0 attribute is always bound to something
		"in vec3 Normal;\n"
		"in vec4 Color;\n"
		"out vec3 position;\n"
		"out vec3 normal;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"	gl_Position = object_to_clip * Position;\n"
		"	position = object_to_light * Position;\n"
		"	normal = normal_to_light * Normal;\n"
		"	color = Color;\n"
		"}\n"
		,
		"#version 330\n"
		"uniform vec3 sun_direction;\n"
		"uniform vec3 sun_color;\n"
		"uniform vec3 sky_direction;\n"
		"uniform vec3 sky_color;\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec4 color;\n"
		"vec4 fragColor;\n"

		"vec4 bloomColor;\n"
		"out vec4 brightColor;\n"
		"float brightness;\n"
		"vec4 bloomBlurRes;\n"
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
		"	fragColor = vec4(color.rgb * total_light, color.a);\n"


		" bloomColor = texture(tex, texCoord);\n"
		" brightness = dot(bloomColor.rgb, vec3(0.2126, 0.7152, 0.0722));\n"
		" if(brightness > 0.7) brightColor = vec4(fragColor.rgb, 1.0);\n"
		" else brightColor = vec4(0.0, 0.0, 0.0, 1.0);\n"

		// "	tex_offset = 1.0 / textureSize(tex, 0); \n"
		// " result = texture(tex, texCoord).rgb * weight[0]; \n"
		// "	for(int i = 1; i < 5; ++i)\n"
		// "	{\n"
		// "   result += texture(tex, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
		// "   result += texture(tex, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
		// " }\n"
		// " for(int i = 1; i < 5; ++i)\n"
		// " {\n"
		// "   result += texture(tex, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
		// "   result += texture(tex, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
		// " }\n"
		// "	bloomBlurRes = vec4(result, 1.0);\n"
		"}\n"
	);

	object_to_clip_mat4 = glGetUniformLocation(program, "object_to_clip");
	object_to_light_mat4x3 = glGetUniformLocation(program, "object_to_light");
	normal_to_light_mat3 = glGetUniformLocation(program, "normal_to_light");

	sun_direction_vec3 = glGetUniformLocation(program, "sun_direction");
	sun_color_vec3 = glGetUniformLocation(program, "sun_color");
	sky_direction_vec3 = glGetUniformLocation(program, "sky_direction");
	sky_color_vec3 = glGetUniformLocation(program, "sky_color");
}

Load< VertexColorProgram > vertex_color_program(LoadTagInit, [](){
	return new VertexColorProgram();
});
