
// Input vertex data
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec3 normalMapTangent;
in vec3 normalMapBitangent;

uniform mat4 mvp;	// perspective transform
uniform mat4 M;
uniform mat4 V;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;
// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 NORMAL;    // used to model day / night
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

void main(){
    gl_Position = mvp * vec4(position, 1.0);

    // Position of the vertex, in worldspace : M * position
    Position_worldspace = (M * vec4(position, 1.0)).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = ( V * M * vec4(position,1.0)).xyz;
    EyeDirection_cameraspace = vec3(0.0,0.0,0.0) - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1.0)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    UV = uv;
    NORMAL = normal;

    // model to camera = ModelView
    vec3 vertexTangent_cameraspace   = MV3x3 * normalMapTangent;
    vec3 vertexBitangent_cameraspace = MV3x3 * normalMapBitangent;
    vec3 vertexNormal_cameraspace    = MV3x3 * normal;

    mat3 TBN = transpose(mat3(
	    vertexTangent_cameraspace,
	    vertexBitangent_cameraspace,
	    vertexNormal_cameraspace
    )); // You can use dot products instead of building this matrix and transposing it. See References for details.

    LightDirection_tangentspace = TBN * LightDirection_cameraspace;
    EyeDirection_tangentspace =  TBN * EyeDirection_cameraspace;
}


