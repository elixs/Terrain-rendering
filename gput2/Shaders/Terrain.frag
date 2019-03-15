#version 330 core

struct FMaterial {
    vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
    float Shininess;
}; 
uniform FMaterial UMaterial;
FMaterial Material;


struct FLight {

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct FDirectionalLight {
    vec3 Direction;

    FLight Light;
};
uniform FDirectionalLight UDirectionalLight;

struct FPointLight {    
    vec3 Position;

	float Constant;
	float Linear;
	float Quadratic;
  
    FLight Light;
};  
#define POINT_LIGHTS 1  
uniform FPointLight UPointLights[POINT_LIGHTS];

struct FSpotLight {
    vec3 Position;
    vec3 Direction;

	float Constant;
	float Linear;
	float Quadratic;

	float CutOff;
	float OuterCutOff;

    FLight Light;
};
uniform FSpotLight USpotLight;

uniform vec3 UViewPosition;

in vec3 FPosition;
in vec3 FNormal;

out vec4 OFragColor;

#define GREEN	vec3(  0.f,   0.5f,   0.f)
#define LIME	vec3(  0.f,    1.f,   0.f)
#define YELLOW	vec3(  1.f,    1.f,   0.f)
#define BROWN	vec3( 0.65f, 0.16f, 0.16f)
#define GRAY	vec3(  0.5f,  0.5f,  0.5f)
#define WHITE	vec3(   1.f,   1.f,   1.f)

uniform float UHeight;

vec3 CalculateDirectonalLight(FDirectionalLight DirectionalLight, vec3 Normal, vec3 ViewDirection);
vec3 CalculatePointLight(FPointLight PointLight, vec3 Normal, vec3 FPosition, vec3 ViewDirection);
vec3 CalculateSpotLight(FSpotLight Light, vec3 Normal, vec3 FPosition, vec3 ViewDirection);
void CalculateLight(FLight SpotLight, vec3 Normal, vec3 LightDirection, vec3 ViewDirection, out vec3 Ambient, out vec3 Diffuse, out vec3 Specular);

void main()
{
	vec3 Normal = normalize(FNormal);
	vec3 ViewDirection = normalize(UViewPosition - FPosition);

	Material.Diffuse = GREEN * (smoothstep( -UHeight/12.0, UHeight/12.0, FPosition.y) - smoothstep( UHeight/12.0, 3.0*UHeight/12.0, FPosition.y)) +
					LIME * (smoothstep( UHeight/12.0, 3.0*UHeight/12.0, FPosition.y) - smoothstep( 3.0*UHeight/12.0, 5.0*UHeight/12.0, FPosition.y)) +
					YELLOW * (smoothstep( 3.0*UHeight/12.0, 5.0*UHeight/12.0, FPosition.y) - smoothstep( 5.0*UHeight/12.0, 7.0*UHeight/12.0, FPosition.y)) +
					BROWN * (smoothstep( 5.0*UHeight/12.0, 7.0*UHeight/12.0, FPosition.y) - smoothstep( 7.0*UHeight/12.0, 9.0*UHeight/12.0, FPosition.y)) +
					GRAY * (smoothstep( 7.0*UHeight/12.0, 9.0*UHeight/12.0, FPosition.y) - smoothstep( 9.0*UHeight/12.0, 11.0*UHeight/12.0, FPosition.y)) +
					WHITE * (smoothstep( 9.0*UHeight/12.0, 11.0*UHeight/12.0, FPosition.y) - smoothstep( 11.0*UHeight/12.0, 13.0*UHeight/12.0, FPosition.y));

	vec3 Result = CalculateDirectonalLight(UDirectionalLight, Normal, ViewDirection);

	for(int i = 0; i < POINT_LIGHTS; ++i)
	{
		Result += CalculatePointLight(UPointLights[i], Normal, FPosition, ViewDirection);
	}

	Result += CalculateSpotLight(USpotLight, Normal, FPosition, ViewDirection);

	OFragColor = vec4(Result, 1.f);
}

vec3 CalculateDirectonalLight(FDirectionalLight DirectionalLight, vec3 Normal, vec3 ViewDirection)
{
    vec3 LightDirection = normalize(-DirectionalLight.Direction);

    vec3 Ambient, Diffuse, Specular;
	CalculateLight(DirectionalLight.Light, Normal, LightDirection, ViewDirection, Ambient, Diffuse, Specular);

    return  Ambient + Diffuse + Specular;
}

vec3 CalculatePointLight(FPointLight PointLight, vec3 Normal, vec3 FPosition, vec3 ViewDirection)
{
	vec3 LightDirection = normalize(PointLight.Position - FPosition);

    vec3 Ambient, Diffuse, Specular;
    CalculateLight(PointLight.Light, Normal, LightDirection, ViewDirection, Ambient, Diffuse, Specular);

	float Distance = length(PointLight.Position - FPosition);
	float Attenuation = 1.0 / (PointLight.Constant + PointLight.Linear * Distance + PointLight.Quadratic * (Distance * Distance));  

	Ambient *= Attenuation;
	Diffuse *= Attenuation;
	Specular *= Attenuation;

	return Ambient + Diffuse + Specular;
}

vec3 CalculateSpotLight(FSpotLight SpotLight, vec3 Normal, vec3 FPosition, vec3 ViewDirection)
{
	vec3 LightDirection = normalize(SpotLight.Position - FPosition);

    vec3 Ambient, Diffuse, Specular;
    CalculateLight(SpotLight.Light, Normal, LightDirection, ViewDirection, Ambient, Diffuse, Specular);

	float Distance = length(SpotLight.Position - FPosition);
	float Attenuation = 1.0 / (SpotLight.Constant + SpotLight.Linear * Distance + SpotLight.Quadratic * (Distance * Distance));

	float Theta = dot(LightDirection, normalize(-SpotLight.Direction));
	float Epsilon   = SpotLight.CutOff - SpotLight.OuterCutOff;
	float Intensity = clamp((Theta - SpotLight.OuterCutOff) / Epsilon, 0.0, 1.0);

	Ambient *= Attenuation * Intensity;
	Diffuse *= Attenuation * Intensity;
	Specular *= Attenuation * Intensity;

	return Ambient + Diffuse + Specular;
}

void CalculateLight(FLight Light, vec3 Normal, vec3 LightDirection, vec3 ViewDirection, out vec3 Ambient, out vec3 Diffuse, out vec3 Specular)
{
	float DiffuseRatio = max(dot(Normal, LightDirection), 0.f);
    vec3 ReflectionDirection = reflect(-LightDirection, Normal);
    float SpecularRatio = pow(max(dot(ViewDirection, ReflectionDirection), 0.f), UMaterial.Shininess);

    Ambient  = Light.Ambient  * Material.Diffuse / 2.f;
    Diffuse  = Light.Diffuse  * DiffuseRatio * Material.Diffuse;
    Specular = Light.Specular * SpecularRatio * UMaterial.Specular;
}

