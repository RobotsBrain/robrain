#ifndef __YUV2JPEG_H__
#define __YUV2JPEG_H__

namespace Image {

int Yuv422ToJpeg(char *filename, unsigned char *yuv, int width, int height, int quality);

int Yuv420ToJpeg(char *filename, unsigned char *yuv, int width, int height, int quality);

} // end namespace

#endif