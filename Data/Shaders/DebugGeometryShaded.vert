#version 330

uniform Globals
{
    mat4 ViewProjection;
};

uniform mat4 Transform;

in vec3 Position;
in vec3 Normal;

out vec3 f_Normal;

void main()
{
	vec4 transformed = ViewProjection * Transform * vec4(Position, 1.0f);

	gl_Position = transformed;
	f_Normal = (Transform * vec4(Normal, 0.0f)).xyz;
}

