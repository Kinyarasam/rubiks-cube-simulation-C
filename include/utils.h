#ifndef __UTILS_H__
#define __UTILS_H__

char * readFile(const char* filename);
GLuint compileShader(const char* path, GLenum shaderType);
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

#endif  /** __UTILS_H__ */
