/*
 * psnr_aver.c
 *
 *  Created on: Dec 19, 2013
 *      Author: Charlampos Mysirlidis
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned char u8_t;

u8_t *orig, *eval;
FILE *orig_file, *eval_file;
int frame;
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

float mse(u8_t* orig_buff, u8_t* eval_buff, int size) {

      double err = 0;
      int i;

      for (i = 0; i < size; i++) {
        int diff = (int) orig_buff[i] - (int) eval_buff[i];
        err += diff * diff;
      }

      return (sqrt(err / size));
}

float aver_psnr(float mse_aver){

  return (20.0 * log(255.0 / mse_aver) / log(10.0));

}

int main(int argc, char* argv[]) {

	int height, width, frame_count = 0;
	float y, u, v;
        float mse_sum = 0 ;
	
	if (argc != 7) {
		fprintf(stderr, "Wrong argument format!\n\n");
		fprintf(stderr,	"Example: \n %s original.yuv eval.yuv width height frames_num yuv_subsampling > stdout \n",
				argv[0]);
		close_f();
	}

	orig_file = init_f(argv[1]);
	eval_file = init_f(argv[2]);

	width = init_res(argv[3]);
	height = init_res(argv[4]);

	calc_size(height, width, argv[6]);

	orig = (u8_t *) malloc(buf_size);
	eval = (u8_t *) malloc(buf_size);
	for (frame = 0; frame < abs(atoi(argv[5])); frame++) {
		if (fread(orig, buf_size, 1, orig_file) <= 0) {
			break;
		}

		if (fread(eval, buf_size, 1, eval_file) <= 0) {
			break;
		}

		y = mse(orig, eval, Y_size);
		u = mse(orig + Y_size, eval + Y_size, C_size);
		v = mse(orig + Y_size + C_size, eval + Y_size + C_size, C_size);
                mse_sum += (y+u+v)/3;

		frame_count++;
	}
#ifdef DESC
        fprintf(stdout, "Frame number: %d, MSE sum: %f\n", frame_count, mse_sum);

        fprintf(stdout, "Average PSNR value: %f\n", aver_psnr(mse_sum/frame_count));
#else
        fprintf(stdout, "%f\n", aver_psnr(mse_sum/frame_count));
#endif
	close_f();
	return 0;

}
