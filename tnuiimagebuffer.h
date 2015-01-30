#ifndef TNUIIMAGEBUFFER_H
#define TNUIIMAGEBUFFER_H

#include <NuiApi.h>

#define MAX_PLAYER_INDEX    6

enum DEPTH_TREATMENT
{
    CLAMP_UNRELIABLE_DEPTHS,
    TINT_UNRELIABLE_DEPTHS,
    DISPLAY_ALL_DEPTHS,
};

class TNuiImageBuffer
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    TNuiImageBuffer();

    /// <summary>
    /// Destructor
    /// </summary>
   ~TNuiImageBuffer();

public:
    /// <summary>
    /// Set image size according to image resolution
    /// </summary>
    /// <param name="resolution">Image resolution</param>
    void setImageSize(NUI_IMAGE_RESOLUTION resolution);

    /// <summary>
    /// Clear buffer
    /// </summary>
    void clear();

    /// <summary>
    /// Get width of image.
    /// </sumamry>
    /// <returns>Width of image.</returns>
    DWORD width() const;

    /// <summary>
    /// Get height of image.
    /// </sumamry>
    /// <returns>Width of height.</returns>
    DWORD height() const;

    /// <suumary>
    /// Get size of buffer.
    /// <summary>
    /// <returns>Size of buffer.</returns>
    DWORD bufferSize() const;

    /// <summary>
    /// Return allocated buffer.
    /// </summary>
    /// <returns>
    /// The pointer to the allocated buffer
    /// Return value could be nullptr if the buffer is not allocated
    /// </returns>
    BYTE* buffer() const;

    /// <summary>
    /// Copy color frame image to image buffer
    /// </summary>
    /// <param name="pImage">The pointer to the frame image to copy</param>
    /// <param name="size">Size in bytes to copy</param>
    void copyRGB(const BYTE* source, UINT size);

    /// <summary>
    /// Copy raw bayer data and convert to RGB image
    /// </summary>
    /// <param name="pImage">The pointer to the frame image to copy</param>
    /// <param name="size">Size in bytes to copy</param>
    void copyBayer(const BYTE* source, UINT size);

    /// <summary>
    /// Copy and convert infrared frame image to image buffer
    /// </summary>
    /// <param name="pImage">The pointer to the frame image to copy</param>
    /// <param name="size">Size in bytes to copy</param>
    void copyInfrared(const BYTE* source, UINT size);

    /// <summary>
    /// Copy and convert depth frame image to image buffer
    /// </summary>
    /// <param name="pImage">The pointer to the frame image to copy</param>
    /// <param name="size">Size in bytes to copy</param>
    /// <param name="nearMode">Depth stream range mode</param>
    /// <param name="treatment">Depth treatment mode</param>
    void copyDepth(const BYTE* source, UINT size, BOOL nearMode, DEPTH_TREATMENT treatment);


private:
    /// <summary>
    /// Calculate image width and height according to image resolution enumeration value.
    /// If resolution enumeration is invalid, width and height will be set to zero.
    /// </summary>
    /// <param name="resolution">Enumeration value which indicates the image resolution format</param>
    /// <param name="width">Calculated image width</param>
    /// <param name="height">Calculated image height</param>
    void imageSize(NUI_IMAGE_RESOLUTION resolution, DWORD& width, DWORD& height);

    /// <summary>
    /// Initialize the depth-color mapping table.
    /// </summary>
    void initDepthColorTable();

    /// <summary>
    /// Set color value
    /// </summary>
    /// <param name="pColor">The pointer to the variable to be set with color</param>
    /// <param name="red">Red component of the color</param>
    /// <param name="green">Green component of the color</parma>
    /// <param name="blue">Blue component of the color</param>
    /// <param name="alpha">Alpha component of the color</param>
    inline void setColor(UINT* pColor, BYTE red, BYTE green, BYTE blue, BYTE alpha = 255);

    /// <summary>
    /// Calculate intensity of a certain depth
    /// </summary>
    /// <param name="depth">A certain depth</param>
    /// <returns>Intensity calculated from a certain depth</returns>
    BYTE getIntensity(int depth);

    /// <summary>
    /// Allocate a buffer of size and return it
    /// </summary>
    /// <param name="size">Size of buffer to allocate. Zeor to release buffer memory</param>
    /// <returns>The pointer to the allocated buffer. If size hasn't changed, the previously allocated buffer is returned</returns>
    BYTE* resetBuffer(UINT size);

private:
    static const BYTE    m_intensityShiftR[MAX_PLAYER_INDEX + 1];
    static const BYTE    m_intensityShiftG[MAX_PLAYER_INDEX + 1];
    static const BYTE    m_intensityShiftB[MAX_PLAYER_INDEX + 1];
    UINT                 m_depthColorTable[MAX_PLAYER_INDEX + 1][USHRT_MAX + 1];

    bool                m_nearMode;
    DWORD               m_width;
    DWORD               m_height;
    DWORD               m_srcWidth;
    DWORD               m_srcHeight;
    DWORD               m_sizeInBytes;
    BYTE*               m_buffer;
    DEPTH_TREATMENT     m_depthTreatment;
};

#endif // TNUIIMAGEBUFFER_H
