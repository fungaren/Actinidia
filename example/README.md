# Lua API

These functions are available on lua scripts.

```c++
/**
 *  Create an image with black background,
 *  you should use DeleteImage() to release it.
 */
    Image CreateImage(width, height)

/**
 *  Create an image with specific background color,
 *  you should use DeleteImage() to release it.
 *  @param color color 0xAABBGGRR - A: opacity, R: red, G: green, B: blue
 */
    Image CreateImageEx(width, height, color)

/**
 *  Create a transparent image
 *  you should use DeleteImage() to release it.
 */
    Image CreateTransImage(width, height)

/**
 *  Release an image 
 *  Use it after CreateImage, CreateImageEx, CreateTransImage, LoadImage
 *  DO NOT release an image TWICE.
 */
    void DeleteImage(g)

/**
 *  Get width of an image
 *  If the image has been released, it fails.
 */
    int GetWidth(g)

/**
 *  Get height of an image
 *  If the image has been released, it fails.
 */
    int GetHeight(g)

/**
 *  Get text from package.
 *  @param pathname File path. eg. 'lua/002.lua'
 *  @return "" If fails, content if success.
 */
    string GetText(pathname)

/**
 *  Load an image from package.
 *  you should use DeleteImage() to release it.
 *  @param pathname File path. eg. 'img/1.jpg'
 *  @return nil if fails, image if success.
 */
    image GetImage(pathname)

/**
 *  Load music from package.
 *  If b_loop set true, you should StopSound() manually.
 *  @param pathname File path. eg. 'bgm/1.mp3'
 *  @return nil if fails, sound if success.
 */
    sound GetSound(pathname, b_loop)

/**
 *  Paste gSrc to gDest
 *  @param gDest destination image
 *  @param gSrc source image
 *  @param xDest destination x
 *  @param yDest destination y
 */
    void PasteToImage(gDest, gSrc, xDest, yDest)

/**
 *  Paste gSrc to gDest and stretch
 *  @param gDest destination image
 *  @param gSrc source image
 *  @param xDest destination x
 *  @param yDest destination y
 *  @param DestWidth    destination width
 *  @param DestHeight   destination height
 *  @param xSrc source x
 *  @param ySrc source y
 *  @param SrcWidth source width
 *  @param SrcHeight source height
 */
    void PasteToImageEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)

/**
 *  Alpha blend gSrc and gDest 
 *  @param gDest destination image
 *  @param gSrc source image
 *  @param opacity  [0,255]
 */
    void AlphaBlend(gDest, gSrc, xDest, yDest, opacity)

/**
 *  Alpha blend gSrc and gDest and stretch
 */
    void AlphaBlendEx(gDest, gSrc, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight, Opacity)

/**
 *  Display an image on window
 */
    void PasteToWnd(WndGraphic, g)

/**
 *  Display an image on window and stretch
 */
    void PasteToWndEx(WndGraphic, g, xDest, yDest, DestWidth, DestHeight, xSrc, ySrc, SrcWidth, SrcHeight)

/**
 *  Stop a background music.
 *  DO NOT stop a sound TWICE.
 */
    void StopSound(sound)

/**
 *  Set volume
 *  @param volume [0,1]
 */
    void SetVolume(sound,volume)

/**
 *  Play sound
 */
    void PlaySound(sound)

/**
 *  Save screenshot in 'screenshot'
 *  @return true for success.
 */
    bool Screenshot()

/**
 *  Get setting
 *  Actinidia will load settings from "data" on first start
 *  @param key string
 *  @return value string, nil if not exist
 */
    void GetSetting(key)


/**
 *  Save setting
 *  Actinidia will save settings to "data" on quit
 *  @param key string
 *  @param value string
 */
    void SaveSetting(key, value)
```
