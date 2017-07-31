#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;


GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path)
{
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

  if (VertexShaderStream.is_open()) {
    std::string Line = "";
    while (getline(VertexShaderStream, Line)) {
      VertexShaderCode += ("\n" + Line);
    }
    VertexShaderStream.close();
  }

  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

  if (FragmentShaderStream.is_open()) {
    std::string Line = "";
    while (getline(FragmentShaderStream, Line)) {
      FragmentShaderCode += ("\n" + Line);
    }
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;

  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  printf("Linking program\n");

  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }


  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

int main( void )
{
  if (!glfwInit()) {
    fprintf(stderr, "GLFWの初期化に失敗しました\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window;
  window = glfwCreateWindow(1024, 768, "Tutotial 02", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "GLFWウィンドウのオープンに失敗しました。\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "GLEWの初期化に失敗しました。");
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  static const GLfloat g_vertex_buffer_data[] = {-1.0f, -1.0f, 0.0f,
                                                 1.0f, -1.0f, 0.0f,
                                                 0.0f, 1.0f, 0.0f};

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  GLuint programID = loadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

  do {
    glClear( GL_COLOR_BUFFER_BIT );
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(programID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
