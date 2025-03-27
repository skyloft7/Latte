#pragma once
#include "Mesh.h"
#include "Scene.h"
#include <numbers>

using namespace glm;

class CookTorrance {
public:

    static vec3 fresnelSchlick(float cosTheta, vec3 F0) {
        return F0 + (vec3(1.0) - F0) * (float) pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    }

    static float distributionGGX(vec3 N, vec3 H, float roughness) {
        float a      = roughness*roughness;
        float a2     = a*a;
        float NdotH  = max(dot(N, H), 0.0f);
        float NdotH2 = NdotH*NdotH;

        float num   = a2;
        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        denom = std::numbers::pi * denom * denom;

        return num / denom;
    }

    static float GeometrySchlickGGX(float NdotV, float roughness) {
        float r = (roughness + 1.0);
        float k = (r*r) / 8.0;

        float num   = NdotV;
        float denom = NdotV * (1.0 - k) + k;

        return num / denom;
    }
    static float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
        float NdotV = max(dot(N, V), 0.0f);
        float NdotL = max(dot(N, L), 0.0f);
        float ggx2  = GeometrySchlickGGX(NdotV, roughness);
        float ggx1  = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }


    static vec4 color(std::shared_ptr<Mesh> mesh, vec4 hit, vec4 normal, std::shared_ptr<Scene> scene, Camera camera) {
        auto material = mesh->getMaterial();

        vec4 N = normalize(normal);
        vec4 V = normalize(camera.pos - hit);


        vec3 F0 = vec3(0.04);
        F0      = mix(F0, material.albedo, material.metallic);

        vec3 Lo = vec4(0.0);
        for (int lightIndex = 0; lightIndex < scene->getLights().size(); lightIndex++) {
            auto light = scene->getLights()[lightIndex];
            auto lightColor = light->getColor() * light->getIntensity();

            vec4 L = normalize(light->getPos() - hit);
            vec4 H = normalize(V + H);

            float distance = length(light->getPos() - hit);
            float attenuation = 1.0 / (distance * distance);
            vec3 radiance = lightColor * attenuation;

            float NDF = distributionGGX(N, H, material.roughness);
            float G   = geometrySmith(N, V, L, material.roughness);
            vec3 F    = fresnelSchlick(max(dot(H, V), 0.0f), F0);



            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f)  + 0.0001;
            vec3 specular     = numerator / denominator;

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;

            kD *= 1.0 - material.metallic;

            float NdotL = max(dot(N, L), 0.0f);
            Lo += (kD * material.albedo / (float) std::numbers::pi + specular) * radiance * NdotL;

        }

        vec3 ambient = vec3(0.03) * material.albedo * material.ao;
        vec3 color = ambient + Lo;

        return vec4(color, 1.0);
    }
};
