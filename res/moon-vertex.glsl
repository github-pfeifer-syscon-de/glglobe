
in vec3 position;
in vec3 normal;
in vec2 uv;
uniform vec3 light;
uniform mat4 mvp;	// perspective-view-position transform
out float shade;
out vec2 UV;
void main() {
  shade = dot(normalize(light), normalize(normal));

  UV = uv;
  gl_Position = mvp * vec4(position, 1.0);
}