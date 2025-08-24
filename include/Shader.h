#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    // Constructeur : depuis fichiers
    Shader(const char* vertexPath, const char* fragmentPath);

    // Constructeur : depuis chaînes de caractères
    Shader(const std::string& vertexCode, const std::string& fragmentCode, bool fromString);

    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};
