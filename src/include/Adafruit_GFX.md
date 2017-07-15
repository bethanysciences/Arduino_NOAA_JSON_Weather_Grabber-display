setCursor(int16_t x, int16_t y),
setTextColor(uint16_t c),
setTextColor(uint16_t c, uint16_t bg),
setTextSize(uint8_t s);
setRotation(uint8_t s);

drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
fillScreen(uint16_t color),
drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color),
fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color),
drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color), fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color),
drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color),
fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color),


setTextWrap(boolean w), cp437(boolean x=true), setFont(const GFXfont *f = NULL), getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),

int16_t height(void) const;
int16_t width(void) const;
uint8_t getRotation(void) const;
int16_t getCursorX(void) const;
int16_t getCursorY(void) const;
charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);

drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color),
drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg),
drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color), drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg),
drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h),
drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h),
drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h),
drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h),
drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size),

getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
