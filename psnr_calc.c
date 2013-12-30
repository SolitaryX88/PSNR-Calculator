/*
 * psnr_aver.c
 *
 *  Created on: Dec 19, 2013
 *  Last edit : Dec 30, 2013
 *      Author: Charalampos Mysirlidis
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef unsigned char u8_t;

u8_t *orig, *eval;
FILE *orig_file, *eval_file;
int frame_num;
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
		C_size = height * (width / 2);
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

double mse(u8_t* orig_buff, u8_t* eval_buff, int size) {

	double err = 0.0;
	int i, diff;

	for (i = 0; i < size; i++) {
		diff = (int) orig_buff[i] - eval_buff[i];
		err += (diff * diff);
	}

#ifdef DEBUG
	printf("MSE: %f\n", err/size);
#endif

	return (err / size);
}

double psnr(double mse_aver) {

	return (10.0 * log10((255.0 * 255.0) / mse_aver));
}

int main(int argc, char* argv[]) {

	int height, width, mode = 0;

	double yuv_mse, mse_sum = 0.0;

#ifdef DESC
	double y_avr = 0.0, u_avr = 0.0, v_avr = 0.0;
	double y_mse, u_mse, v_mse;
#endif

	if (argc != 8) {
		fprintf(stderr, "Wrong argument format!\n\n");
		fprintf(stderr,
				"Example: \n %s original.yuv eval.yuv width height frames_num yuv_subsampling <per-frame || average> > stdout \n",
				argv[0]);
		close_f();
	}

	orig_file = init_f(argv[1]);
	eval_file = init_f(argv[2]);

	width = init_res(argv[3]);
	height = init_res(argv[4]);

	calc_size(height, width, argv[6]);

	if (strcmp(argv[7], "average") == 0 && argc > 7)
		mode = 1;

	orig = (u8_t *) malloc(buf_size);
	eval = (u8_t *) malloc(buf_size);

	if (!mode) {
#ifdef DESC
		fprintf(stdout, "Aver \t\t Y \t\t U \t\t V\n");
#endif
	}

	for (frame_num = 0; frame_num < abs(atoi(argv[5])); frame_num++) {
		if (fread(orig, buf_size, 1, orig_file) <= 0) {
			break;
		}

		if (fread(eval, buf_size, 1, eval_file) <= 0) {
			break;
		}

		yuv_mse = mse(orig, eval, buf_size);

		if (mode) // mode 0 -> per-frame || 1 -> average
			mse_sum += yuv_mse;
		else {

#ifdef DESC

			y_mse = mse(orig, eval, Y_size);
			u_mse = mse(orig + Y_size, eval + Y_size, C_size);
			v_mse = mse(orig + Y_size + C_size, eval + Y_size + C_size, C_size);

			mse_sum += yuv_mse;
			y_avr += y_mse;
			u_avr += u_mse;
			v_avr += v_mse;

			fprintf(stdout, "%.3f \t\t %.3f \t %.3f \t %.3f\n", psnr(yuv_mse), psnr(y_mse), psnr(u_mse), psnr(v_mse));
#else
			fprintf(stdout, "%.3f\n", psnr(yuv_mse));
#endif

		}
	}

	if (mode) {
#ifdef DESC
		fprintf(stdout, "Frame number: %d, MSE sum: %f\n", frame_num, mse_sum);

		fprintf(stdout, "Average PSNR value: %f\n", psnr(mse_sum/frame_num));
#else
		fprintf(stdout, "%f\n", psnr(mse_sum / frame_num));
#endif
	} else {
#ifdef DESC
		fprintf(stdout, "\n\n \t\t===== END OF CALCULATIONS====\n\n ");

		fprintf(stdout, "Total N# of frames calculated: %d \n", frame_num);
		fprintf(stdout, "PSNR average values for all: %.4f Luma(Y): %.4f Blue-Luma(U): %.4f and Red-Luma(V): %.4f \n",
				psnr(mse_sum/frame_num), psnr(y_avr / frame_num), psnr(u_avr / frame_num), psnr(v_avr / frame_num));
#endif

	}

	close_f();
	return 0;

}
