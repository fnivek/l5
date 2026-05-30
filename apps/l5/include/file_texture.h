#ifndef L5_FILE_TEXTURE_H_
#define L5_FILE_TEXTURE_H_

#include <iostream>

/** Load and manipulate a sdl texture from a file.
 */
class FileTexture {
public:
  FileTexture();
  ~FileTexture();

  bool LoadFromFile(std::string path);

  void Destroy();

  void Render(float x, float y);

  int width() const;
  int height() const;
  bool is_loaded() const;

private:
};

#endif // L5_FILE_TEXTURE_H_
