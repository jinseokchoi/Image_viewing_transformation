#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <GL/glut.h>

#define PC 1
#define SGI 2

int system_type=PC;

unsigned char *read_bmp(char *name, int *width, int *height, int *components) {
  FILE* BMPfile;
  GLubyte garbage;
  long size;
  int start_point,x;
  GLubyte temp[3];
  GLubyte start[4],w[4],h[4];
  unsigned char *read_image;
  
  BMPfile = fopen(name , "rb" ) ;
 
  for(x=0;x<10;x++){
    fread(&garbage,1,1,BMPfile);
  }  
  
  fread(&start[0],1,4,BMPfile);
 
  for(x=0;x<4;x++){
    fread(&garbage,1,1,BMPfile);
  }  

  fread(&w[0],1,4,BMPfile);
  fread(&h[0],1,4,BMPfile);
  if (system_type == PC){
    (*width) = (w[0] + 256 * w[1] + 256*256*w[2] + 256*256*256*w[3]);
    (*height) = (h[0] + 256 * h[1] + 256*256*h[2] + 256*256*256*h[3]);
    size = (*width) * (*height);
    start_point = (start[0] + 256 * start[1] + 256*256*start[2] + 256*1256*256*start[3]);
  } 
  else if (system_type == SGI) {
    (*width) = (w[3] + 256 * w[2] + 256*256*w[1] + 256*256*256*w[0]);
    (*height) = (h[3] + 256 * h[2] + 256*256*h[1] + 256*256*256*h[0]);
    size = (*width) * (*height);
    start_point = (start[3] + 256 * start[2] + 256*256*start[1] + 256*256*256*start[0]);
  }

  read_image = (unsigned char *)malloc(size*3);

  for(x=0;x<(start_point-26);x++){
    fread(&garbage,1,1,BMPfile);
  }

  if (system_type == SGI){
    for(x=0;x<(size*3);x=x+3){
      fread(&read_image[x],1,3,BMPfile);
    }
  }
  else if (system_type == PC) {
    for(x=0;x<(size*3);x=x+3){
      fread(&temp[0],1,3,BMPfile);
      read_image[x]=temp[2];
      read_image[x+1]=temp[1];
      read_image[x+2]=temp[0];
    }  
  }  

/*  read_image = (unsigned char *)malloc(size*4);

  for(x=0;x<(start_point-26);x++){
    fread(&garbage,1,1,BMPfile);
  }

  if (system_type == SGI){
    for(x=0;x<(size*4);x=x+4){
      fread(&read_image[x],1,3,BMPfile);
      read_image[x+3]=(unsigned char)255;
    }
  }
  else if (system_type == PC) {
    for(x=0;x<(size*4);x=x+4){
      fread(&temp[0],1,3,BMPfile);
      read_image[x]=temp[2];
      read_image[x+1]=temp[1];
      read_image[x+2]=temp[0];
      if ((read_image[x]==0) && (read_image[x+1]==255) && (read_image[x+2]==0))
      {
        read_image[x]=255;
        read_image[x+1]=255;
        read_image[x+2]=255;
        read_image[x+3]=75;
      }
      else {
        read_image[x+3]=(unsigned char)255;
      }  
    }  
  }  
 */
  fclose(BMPfile);
  return (unsigned char *) read_image;
}


