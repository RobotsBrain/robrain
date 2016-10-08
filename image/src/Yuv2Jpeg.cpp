#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
	#include "jpeglib.h"
}
#endif



namespace Image {

int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;

    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));

    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;

    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;

    pixel[0] = r ;
    pixel[1] = g ;
    pixel[2] = b ;

    return pixel32;
}

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 |
                        yuv[in + 1] <<  8 | yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }

    return 0;
}

//平面YUV422转平面RGB24
static void YUV422p_to_RGB24(unsigned char *yuv422[3], unsigned char *rgb24, int width, int height)
{
	int R,G,B,Y,U,V;
 	int x,y;
 	int nWidth = width>>1; //色度信号宽度

 	for (y=0;y < height; y++) {
  		for (x=0; x < width; x++) {
   			Y = *(yuv422[0] + y*width + x);
   			U = *(yuv422[1] + y*nWidth + (x>>1));
   			V = *(yuv422[2] + y*nWidth + (x>>1));
   			R = Y + 1.402*(V-128);
   			G = Y - 0.34414*(U-128) - 0.71414*(V-128);
   			B = Y + 1.772*(U-128);
   
   			//防止越界
   			if (R>255)R=255;
   			if (R<0)R=0;
   			if (G>255)G=255;
   			if (G<0)G=0;
   			if (B>255)B=255;
   			if (B<0)B=0;
   
   			*(rgb24 + ((height-y-1)*width + x)*3) = B;
   			*(rgb24 + ((height-y-1)*width + x)*3 + 1) = G;
   			*(rgb24 + ((height-y-1)*width + x)*3 + 2) = R;  
  		}
 	}
}

//平面YUV420转平面YUV422
static void YUV420p_to_YUV422p(unsigned char *yuv420[3], unsigned char *yuv422, int width, int height)  
{
 	int x, y;
 	//亮度信号Y复制
 	int Ylen = width*height;
 	memcpy(yuv422, yuv420[0], Ylen);
 	//色度信号U复制
 	unsigned char *pU422 = yuv422 + Ylen; //指向U的位置
 	int Uwidth = width>>1; //422色度信号U宽度
 	int Uheight = height>>1; //422色度信号U高度 
 	for (y = 0; y < Uheight; y++) {         
  		memcpy(pU422 + y*width,          yuv420[1] + y*Uwidth, Uwidth);
  		memcpy(pU422 + y*width + Uwidth, yuv420[1] + y*Uwidth, Uwidth);
 	}
 	//色度信号V复制
 	unsigned char *pV422 = yuv422 + Ylen + (Ylen>>1); //指向V的位置
 	int Vwidth = Uwidth; //422色度信号V宽度
 	int Vheight = Uheight; //422色度信号U宽度
 	for (y = 0; y < Vheight; y++) {  
  		memcpy(pV422 + y*width,          yuv420[2] + y*Vwidth, Vwidth);
  		memcpy(pV422 + y*width + Vwidth, yuv420[2] + y*Vwidth, Vwidth);
 	} 
}

//平面YUV420转RGB24
static void YUV420p_to_RGB24(unsigned char *yuv420[3], unsigned char *rgb24, int width, int height) 
{
	//  int begin = GetTickCount();
 	int R,G,B,Y,U,V;
 	int x,y;
 	int nWidth = width>>1; //色度信号宽度

 	for (y=0;y < height; y++) {
  		for (x=0;x < width; x++) {
   			Y = *(yuv420[0] + y*width + x);
   			U = *(yuv420[1] + ((y>>1)*nWidth) + (x>>1));
   			V = *(yuv420[2] + ((y>>1)*nWidth) + (x>>1));
   			R = Y + 1.402*(V-128);
   			G = Y - 0.34414*(U-128) - 0.71414*(V-128);
   			B = Y + 1.772*(U-128);

   			//防止越界
   			if (R>255)R=255;
   			if (R<0)R=0;
   			if (G>255)G=255;
   			if (G<0)G=0;
   			if (B>255)B=255;
   			if (B<0)B=0;
   
   			*(rgb24 + ((height-y-1)*width + x)*3) = B;
   			*(rgb24 + ((height-y-1)*width + x)*3 + 1) = G;
   			*(rgb24 + ((height-y-1)*width + x)*3 + 2) = R;
//    *(rgb24 + (y*width + x)*3) = B;
//    *(rgb24 + (y*width + x)*3 + 1) = G;
//    *(rgb24 + (y*width + x)*3 + 2) = R;   
  		}
 	}
}

int Yuv422ToJpeg(char *filename, unsigned char *data, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];  /* pointer to JSAMPLE row[s] */
    int row_stride;    /* physical row width in image buffer */
    JSAMPIMAGE buffer;
    int band, i, buf_width[3], buf_height[3], mem_size, max_line, counter;
    unsigned char *yuv[3];
    uint8_t *pSrc, *pDst;
    FILE *fp;

    if((fp = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    yuv[0] = data;
    yuv[1] = yuv[0] + (width * height);
    yuv[2] = yuv[1] + (width * height)/2;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = width;  /* image width and height, in pixels */
    cinfo.image_height = height;
    cinfo.input_components = 3;    /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;  /* colorspace of input image */

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    cinfo.raw_data_in = TRUE;
    cinfo.jpeg_color_space = JCS_YCbCr;
    // cinfo.comp_info[0].h_samp_factor = 2;
    // cinfo.comp_info[0].v_samp_factor = 1;

    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;
    cinfo.comp_info[1].h_samp_factor = 1;
    cinfo.comp_info[1].v_samp_factor = 2;
    cinfo.comp_info[2].h_samp_factor = 1;
    cinfo.comp_info[2].v_samp_factor = 2;

    jpeg_start_compress(&cinfo, TRUE);

    buffer = (JSAMPIMAGE)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY));
    for(band = 0; band < 3; band++) {
      buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE;
      buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE;
      buffer[band] = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, buf_width[band], buf_height[band]);
    }

    max_line = cinfo.max_v_samp_factor*DCTSIZE;

    for(counter = 0; cinfo.next_scanline < cinfo.image_height; counter++) {
        //buffer image copy.
        for(band = 0; band < 3; band++) {
            mem_size = buf_width[band];
            pDst = (uint8_t *) buffer[band][0];
            pSrc = (uint8_t *) yuv[band] + counter * buf_height[band] * buf_width[band];

            for(i = 0; i < buf_height[band]; i++) {
                memcpy(pDst, pSrc, mem_size);
                pSrc += buf_width[band];
                pDst += buf_width[band];
            }
        }

        jpeg_write_raw_data(&cinfo, buffer, max_line);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(fp);

    return 0;
}

int Yuv420ToJpeg(char *filename, unsigned char *yuvData, int width, int height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;    // target file 
    JSAMPROW row_pointer[1];  // pointer to JSAMPLE row[s] 
    int row_stride;    // physical row width in image buffer
    JSAMPIMAGE  buffer;
    unsigned char *pSrc, *pDst;
    int band, i, buf_width[3], buf_height[3];

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    if((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;  // image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = 3;    // # of color components per pixel
    cinfo.in_color_space = JCS_RGB;  //colorspace of input image
 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    cinfo.raw_data_in = TRUE;
    cinfo.jpeg_color_space = JCS_YCbCr;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 2;

    jpeg_start_compress(&cinfo, TRUE);

    buffer = (JSAMPIMAGE)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY)); 

    for(band = 0; band < 3; band++) {
        buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE;
        buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE;
        buffer[band] = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, buf_width[band], buf_height[band]);
    } 

    unsigned char *rawData[3];

    rawData[0] = yuvData;
    rawData[1] = yuvData + width*height;
    rawData[2] = yuvData + width*height*5/4;

    int src_width[3], src_height[3];

    for(int i = 0; i < 3; i++) {
        src_width[i] = (i==0) ? width:width/2;
        src_height[i] = (i==0) ? height:height/2;
    }
 
    //max_line一般为16，外循环每次处理16行数据。
    int max_line = cinfo.max_v_samp_factor*DCTSIZE; 

    for(int counter = 0; cinfo.next_scanline < cinfo.image_height; counter++) {   
        //buffer image copy.
        for(band = 0; band < 3; band++) {  //每个分量分别处理
    			int mem_size = src_width[band];//buf_width[band];
    			pDst = (unsigned char *)buffer[band][0];
    			pSrc = (unsigned char *)rawData[band] + counter*buf_height[band] * src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址

    			for(i = 0; i < buf_height[band]; i++) { //处理每行数据
    				memcpy(pDst, pSrc, mem_size);
    				pSrc += src_width[band];//buf_width[band];
            pDst += buf_width[band];
          }
        }

        jpeg_write_raw_data(&cinfo, buffer, max_line);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(outfile);

    return 0;
}


} // end namespace


