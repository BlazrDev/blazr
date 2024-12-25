#pragma once

class Canvas {
  private:
	int mWidth;
	int mHeight;
	int tileSize = 16;
	bool update = false;

  public:
	Canvas(int width = 0, int height = 0) {
		mWidth = width * tileSize;
		mHeight = height * tileSize;
	}
	~Canvas() {}

	inline const int &GetWidth() const { return mWidth; }
	inline const int &GetHeight() const { return mHeight; }
	inline void SetWidth(int width) { mWidth = width; }
	inline void SetHeight(int height) { mHeight = height; }
	inline void SetTileSize(int size) { tileSize = size; }
	inline int GetTileSize() { return tileSize; }
	inline void SetUpdate(bool value) { update = value; }
	inline bool GetUpdate() { return update; }
};
