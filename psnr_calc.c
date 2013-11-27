/*
 * psnr_calc.c
 *
 *  Created on: Nov 26, 2013
 *      Author: solitaryx88
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned char u8_t;

u8_t *orig, *eval;
FILE *orig_file, *eval_file;
int frames;
int buf_size, Y_size, C_size;

void close_f() {

	if (!orig_file)
		fclose(orig_file);

	if (!eval_file)
		fclose(eval_file);

	if (!orig)
		free(orig);

	if (!eval)
		free(eval);

	exit(0);
}

FILE* init_f(char* name) {

	FILE *file = fopen(name, "rb");

	if (!file) {
		printf("Can't access file:  %s\n", name);
		close_f();
	}

	return file;
}

int init_res(char* size) {

	int im_size = atoi(size);

	if (im_size < 0) {
		fprintf(stderr, "Please insert a positive value! \n");
		close_f();
	}

	if (im_size > 4100) {
		fprintf(stderr, "Can't calculate YUV's over 4K! \n");
		close_f();
	}

	return im_size;

}

void calc_size(int height, int width, char* yuv_sub) {

	Y_size = height * width;

	if ((strcmp(yuv_sub, "420") == 0) || (strcmp(yuv_sub, "411") == 0)) {
		C_size = Y_size / 4;
		buf_size = (int) Y_size * 1.5;
		return;
	}
	
	if (strcmp(yuv_sub, "422") == 0) {
		C_size = height * (width/2);
		buf_size = (int) Y_size * 2;
		return;
	}

	if (strcmp(yuv_sub, "444") == 0) {
		C_size = Y_size;
		buf_size = (int) 3 * Y_size;
		return;
	}

	fprintf(stderr, "Not supported YUV sub-sampling! \n");

	close_f();
}

float psnr(u8_t* orig_buff, u8_t* eval_buff, int size) {

      double err = 0;   
      int i;  
      
      for (i = 0; i < size; i++) {  
        int diff = (int) orig_buff[i] - (int) eval_buff[i];  
        err += diff * diff;  
      }  
        
      return (20.0 * log(255.0 / sqrt(err / size)) / log(10.0));
}

int main(int argc, char* argv[]) {

	int height, width;
	float y, u, v;
	float y_avr = 0.0, u_avr = 0.0, v_avr = 0.0;

	if (argc != 7) {
		fprintf(stderr, "Wrong argument format!\n\n");
		fprintf(stderr,
				"Example: \n %s  original.yuv  eval.yuv  height width frames_num yuv_subsampling > stdout \n",
				argv[0]);
		close_f();
	}

	orig_file = init_f(argv[1]);
	eval_file = init_f(argv[2]);

	height = init_res(argv[3]);
	width = init_res(argv[4]);

	calc_size(height, width, argv[6]);

	orig = (u8_t *) malloc(buf_size);
	eval = (u8_t *) malloc(buf_size);

	fprintf(stdout, "Aver \t\t Y \t\t U \t\t V\n");

	for (frames = 0; frames < abs(atoi(argv[5])); frames++) {
		if (fread(orig, buf_size, 1, orig_file) <= 0) {
			break;
		}

		if (fread(eval, buf_size, 1, eval_file) <= 0) {
			break;
		}

		y = psnr(orig, eval, Y_size);
		u = psnr(orig + Y_size, eval + Y_size, C_size);
		v = psnr(orig + Y_size + C_size, eval + Y_size + C_size, C_size);

		fprintf(stdout, "%.3f \t\t %.3f \t %.3f \t %.3f\n", (y + u + v) / 3,
				y, u, v);

		y_avr += y;
		u_avr += u;
		v_avr += v;

	}

	fprintf(stdout, "\n\n \t\t===== END OF CALCULATIONS====\n\n ");

	fprintf(stdout, "Total N# of frames calculated: %d \n", frames);
	fprintf(stdout, "PSNR average values for all: %.4f Luma(Y): %.4f Blue-Luma(U): %.4f and Red-Luma(V): %.4f \n",
					(y_avr+u_avr+v_avr)/3/(frames),y_avr / frames, u_avr / frames, v_avr / frames);

	close_f();
	return 0;

}
