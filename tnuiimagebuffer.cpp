#include "stdafx.h"
#include "tnuiimagebuffer.h"

#include <cmath>

#define BYTES_PER_PIXEL_RGB         4
#define BYTES_PER_PIXEL_INFRARED    2
#define BYTES_PER_PIXEL_BAYER       1
#define BYTES_PER_PIXEL_DEPTH       sizeof(NUI_DEPTH_IMAGE_PIXEL)

#define COLOR_INDEX_BLUE            0
#define COLOR_INDEX_GREEN           1
#define COLOR_INDEX_RED             2
#define COLOR_INDEX_ALPHA           3

#define MIN_DEPTH                   400
#define MAX_DEPTH                   16383
#define UNKNOWN_DEPTH               0
#define UNKNOWN_DEPTH_COLOR         0x003F3F07
#define TOO_NEAR_COLOR              0x001F7FFF
#define TOO_FAR_COLOR               0x007F0F3F
#define NEAREST_COLOR               0x00FFFFFF

// intensity shift table to generate different render colors for different tracked players
const BYTE TNuiImageBuffer::m_intensityShiftR[] = {0, 2, 0, 2, 0, 0, 2};
const BYTE TNuiImageBuffer::m_intensityShiftG[] = {0, 2, 2, 0, 2, 0, 0};
const BYTE TNuiImageBuffer::m_intensityShiftB[] = {0, 0, 2, 2, 0, 2, 0};

/// <summary>
/// Constructor
/// </summary>
TNuiImageBuffer::TNuiImageBuffer()
    : m_nearMode(false)
    , m_depthTreatment(CLAMP_UNRELIABLE_DEPTHS)
    , m_sizeInBytes(0)
    , m_width(0)
    , m_height(0)
    , m_srcWidth(0)
    , m_srcHeight(0)
    , m_buffer(nullptr)
{
    initDepthColorTable();
}

/// <summary>
/// Destructor
/// </summary>
TNuiImageBuffer::~TNuiImageBuffer()
{
    SafeDelete(m_buffer);
}

/// <summary>
/// Set image size according to image resolution
/// </summary>
/// <param name="resolution">Image resolution</param>
void TNuiImageBuffer::setImageSize(NUI_IMAGE_RESOLUTION resolution)
{
    imageSize(resolution, m_srcWidth, m_srcHeight);
}

/// <summary>
/// Calculate image width and height according to image resolution enumeration value.
/// If resolution enumeration is invalid, width and height will be set to zero.
/// </summary>
/// <param name="resolution">Enumeration value which indicates the image resolution format</param>
/// <param name="width">Calculated image width</param>
/// <param name="height">Calculated image height</param>
void TNuiImageBuffer::imageSize(NUI_IMAGE_RESOLUTION resolution, DWORD& width, DWORD& height)
{
    NuiImageResolutionToSize(resolution, width, height);
}

/// <summary>
/// Get width of image.
/// </sumamry>
/// <returns>Width of image.</returns>
DWORD TNuiImageBuffer::width() const
{
    return m_width;
}

/// <summary>
/// Get height of image.
/// </sumamry>
/// <returns>Width of height.</returns>
DWORD TNuiImageBuffer::height() const
{
    return m_height;
}

/// <suumary>
/// Get size of buffer.
/// <summary>
/// <returns>Size of buffer.</returns>
DWORD TNuiImageBuffer::bufferSize() const
{
    return m_sizeInBytes;
}

/// <summary>
/// Return allocated buffer.
/// </summary>
/// <returns>
/// The pointer to the allocated buffer
/// Return value could be nullptr if the buffer is not allocated
/// </returns>
BYTE* TNuiImageBuffer::buffer() const
{
    return m_buffer;
}

/// <summary>
/// Allocate a buffer of size and return it
/// </summary>
/// <param name="size">Size of buffer to allocate</param>
/// <returns>The pointer to the allocated buffer. If size hasn't changed, the previously allocated buffer is returned</returns>
BYTE* TNuiImageBuffer::resetBuffer(UINT size)
{
    if (!m_buffer || m_sizeInBytes != size)
    {
        SafeDeleteArray(m_buffer);

        if (0 != size)
        {
            m_buffer = new BYTE[size];
        }
        m_sizeInBytes = size;
    }

    return m_buffer;
}

/// <summary>
/// Clear buffer
/// </summary>
void TNuiImageBuffer::clear()
{
    m_width  = 0;
    m_height = 0;
    resetBuffer(0);
}

/// <summary>
/// Initialize the depth-color mapping table.
/// </summary>
void TNuiImageBuffer::initDepthColorTable()
{
    // Get the min and max reliable depth
    USHORT minReliableDepth = (m_nearMode ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
    USHORT maxReliableDepth = (m_nearMode ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;

    ZeroMemory(m_depthColorTable, sizeof(m_depthColorTable));

    // Set color for unknown depth
    m_depthColorTable[0][UNKNOWN_DEPTH] = UNKNOWN_DEPTH_COLOR;

    switch (m_depthTreatment)
    {
    case CLAMP_UNRELIABLE_DEPTHS:
        // Fill in the "near" portion of the table with solid color
        for (int depth = UNKNOWN_DEPTH + 1; depth < minReliableDepth; depth++)
        {
            m_depthColorTable[0][depth] = TOO_NEAR_COLOR;
        }

        // Fill in the "far" portion of the table with solid color
        for (int depth = maxReliableDepth + 1; depth <= USHRT_MAX; depth++)
        {
            m_depthColorTable[0][depth] = TOO_FAR_COLOR;
        }
        break;

    case TINT_UNRELIABLE_DEPTHS:
        {
            // Fill in the "near" portion of the table with a tinted gradient
            for (int depth = UNKNOWN_DEPTH + 1; depth < minReliableDepth; depth++)
            {
                BYTE intensity = getIntensity(depth);
                BYTE r = intensity >> 3;
                BYTE g = intensity >> 1;
                BYTE b = intensity;
                setColor(&m_depthColorTable[0][depth], r, g, b);
            }

            // Fill in the "far" portion of the table with a tinted gradient
            for (int depth = maxReliableDepth + 1; depth <= USHRT_MAX; depth++)
            {
                BYTE intensity = getIntensity(depth);
                BYTE r = intensity;
                BYTE g = intensity >> 3;
                BYTE b = intensity >> 1;
                setColor(&m_depthColorTable[0][depth], r, g, b);
            }
        }
        break;

    case DISPLAY_ALL_DEPTHS:
        minReliableDepth = MIN_DEPTH;
        maxReliableDepth = MAX_DEPTH;

        for (int depth = UNKNOWN_DEPTH + 1; depth < minReliableDepth; depth++)
        {
            m_depthColorTable[0][depth] = NEAREST_COLOR;
        }
        break;

    default:
        break;
    }

    for (USHORT depth = minReliableDepth; depth <= maxReliableDepth; depth++)
    {
        BYTE intensity = getIntensity(depth);

        for (int index = 0; index <= MAX_PLAYER_INDEX; index++)
        {
            BYTE r = intensity >> m_intensityShiftR[index];
            BYTE g = intensity >> m_intensityShiftG[index];
            BYTE b = intensity >> m_intensityShiftB[index];
            setColor(&m_depthColorTable[index][depth], r, g, b);
        }
    }
}

/// <summary>
/// Set color value
/// </summary>
/// <param name="pColor">The pointer to the variable to be set with color</param>
/// <param name="red">Red component of the color</param>
/// <param name="green">Green component of the color</parma>
/// <param name="blue">Blue component of the color</param>
/// <param name="alpha">Alpha component of the color</param>
void TNuiImageBuffer::setColor(UINT* pColor, BYTE red, BYTE green, BYTE blue, BYTE alpha)
{
    if (!pColor)
        return;

    BYTE* c = (BYTE*)pColor;
    c[COLOR_INDEX_RED]   = red;
    c[COLOR_INDEX_GREEN] = green;
    c[COLOR_INDEX_BLUE]  = blue;
    c[COLOR_INDEX_ALPHA] = alpha;
}

/// <summary>
/// Calculate intensity of a certain depth
/// </summary>
/// <param name="depth">A certain depth</param>
/// <returns>Intensity calculated from a certain depth</returns>
BYTE TNuiImageBuffer::getIntensity(int depth)
{
    // Validate arguments
    if (depth < MIN_DEPTH || depth > MAX_DEPTH)
    {
        return UCHAR_MAX;
    }

    // Use a logarithmic scale that shows more detail for nearer depths.
    // The constants in this formula were chosen such that values between
    // MIN_DEPTH and MAX_DEPTH will map to the full range of possible
    // byte values.
    const float depthRangeScale = 500.0f;
    const int intensityRangeScale = 74;
    return (BYTE)(~(BYTE)min(
        UCHAR_MAX,
        log((double)(depth - MIN_DEPTH) / depthRangeScale + 1) * intensityRangeScale));
}

/// <summary>
/// Copy color frame image to image buffer
/// </summary>
/// <param name="pImage">The pointer to the frame image to copy</param>
/// <param name="size">Size in bytes to copy</param>
void TNuiImageBuffer::copyRGB(const BYTE* pImage, UINT size)
{
    // Check source buffer size
    if (size != m_srcWidth * m_srcHeight * BYTES_PER_PIXEL_RGB)
    {
        return;
    }

    // Set image size to source image size
    m_width  = m_srcWidth;
    m_height = m_srcHeight;

    // Allocate buffer for image
    resetBuffer(m_width * m_height * BYTES_PER_PIXEL_RGB);

    // Copy source image to buffer
    memcpy_s(m_buffer, m_sizeInBytes, pImage, size);
}

/// <summary>
/// Copy raw bayer data and convert to RGB image
/// </summary>
/// <param name="pImage">The pointer to the frame image to copy</param>
/// <param name="size">Size in bytes to copy</param>
void TNuiImageBuffer::copyBayer(const BYTE* pImage, UINT size)
{
    // Check source buffer size
    if (size != m_srcWidth * m_srcHeight * BYTES_PER_PIXEL_BAYER)
    {
        return;
    }

    // Check if source image size can mod by 2
    if (0 != m_srcWidth % 2 || 0 != m_srcHeight % 2)
    {
        return;
    }

    // Converted image size is equal to source image size
    m_width  = m_srcWidth;
    m_height = m_srcHeight;

    // Allocate buffer for image
    UINT* pBuffer = (UINT*)resetBuffer(m_width * m_height * BYTES_PER_PIXEL_RGB);

    // Run through pixels
    for (DWORD y = 0; y < m_srcHeight; y += 2)
    {
        for (DWORD x = 0; x < m_srcWidth; x += 2)
        {
            int firstRowOffset  = (y * m_srcWidth) + x;
            int secondRowOffset = firstRowOffset + m_srcWidth;
                                                    //  _____
            // Get bayer colors from source image   // |  |  |
            BYTE r  = pImage[firstRowOffset + 1];   // |g1|r |
            BYTE g1 = pImage[firstRowOffset];       // |--|--|
            BYTE g2 = pImage[secondRowOffset + 1];  // |b |g2|
            BYTE b  = pImage[secondRowOffset];      // |__|__|

            // Set color to buffered pixel
            setColor(pBuffer + firstRowOffset,      r, g1, b);
            setColor(pBuffer + firstRowOffset  + 1, r, g1, b);
            setColor(pBuffer + secondRowOffset,     r, g2, b);
            setColor(pBuffer + secondRowOffset + 1, r, g2, b);
        }
    }
}

/// <summary>
/// Copy and convert infrared frame image to image buffer
/// </summary>
/// <param name="pImage">The pointer to the frame image to copy</param>
/// <param name="size">Size in bytes to copy</param>
void TNuiImageBuffer::copyInfrared(const BYTE* pImage, UINT size)
{
    // Check source buffer size
    if (size != m_srcWidth * m_srcHeight * BYTES_PER_PIXEL_INFRARED)
    {
        return;
    }

    // Converted image size is equal to source image size
    m_width  = m_srcWidth;
    m_height = m_srcHeight;

    // Allocate buffer for image
    UINT*   pBuffer   = (UINT*)resetBuffer(m_width * m_height * BYTES_PER_PIXEL_RGB);

    // Initialize pixel pointers
    USHORT* pPixelRun = (USHORT*)pImage;
    USHORT* pPixelEnd = pPixelRun + size / BYTES_PER_PIXEL_INFRARED;

    // Run through pixels
    while (pPixelRun < pPixelEnd)
    {
        // Convert pixel from 16-bit to 8-bit intensity
        BYTE intensity = (*pPixelRun) >> 8;

        // Set pixel color with R, G and B components all equal to intensity
        setColor(pBuffer, intensity, intensity, intensity);

        // Move to next pixel
        ++pPixelRun;
        ++pBuffer;
    }
}

/// <summary>
/// Copy and convert depth frame image to image buffer
/// </summary>
/// <param name="pImage">The pointer to the frame image to copy</param>
/// <param name="size">Size in bytes to copy</param>
/// <param name="nearMode">Depth stream range mode</param>
/// <param name="treatment">Depth treatment mode</param>
void TNuiImageBuffer::copyDepth(const BYTE* pImage, UINT size, BOOL nearMode, DEPTH_TREATMENT treatment)
{
    // Check source buffer size
    if (size != m_srcWidth * m_srcHeight * BYTES_PER_PIXEL_DEPTH)
    {
        return;
    }

    // Check if range mode and depth treatment have been changed. Re-initlialize depth-color table with changed parameters
    if (m_nearMode != (FALSE != nearMode) || m_depthTreatment != treatment)
    {
        m_nearMode       = (FALSE != nearMode);
        m_depthTreatment = treatment;

        initDepthColorTable();
    }

    // Converted image size is equal to source image size
    m_width  = m_srcWidth;
    m_height = m_srcHeight;

    // Allocate buffer for color image. If required buffer size hasn't changed, the previously allocated buffer is returned
    UINT* rgbrun = (UINT*)resetBuffer(m_width * m_height * BYTES_PER_PIXEL_RGB);

    // Initialize pixel pointers to start and end of image buffer
    NUI_DEPTH_IMAGE_PIXEL* pPixelRun = (NUI_DEPTH_IMAGE_PIXEL*)pImage;
    NUI_DEPTH_IMAGE_PIXEL* pPixelEnd = pPixelRun + m_srcWidth * m_srcHeight;

    // Run through pixels
    while (pPixelRun < pPixelEnd)
    {
        // Get pixel depth and player index
        USHORT depth = pPixelRun->depth;
        USHORT index = pPixelRun->playerIndex;

        // Get mapped color from depth-color table
        *rgbrun = m_depthColorTable[index][depth];

        // Move the pointers to next pixel
        ++rgbrun;
        ++pPixelRun;
    }
}
