PSNR Calculator
=============== 

This tool written in C can calculate the PSNR in each luma or chroma of a YUV video sequence in almost all formats.

Compile with(for average values only): 

	$ gcc psnr_calc.c -lm -o psnr_calc

Compile with(for full description): 

	$ gcc psnr_calc.c -lm -DDESC -o psnr_calc

Run with: 

	$ psnr_calc original.yuv eval.yuv width height frames_num yuv_subsampling [mode] > stdout

> **NOTE:** The *[modes]* are **average** and **per-frame**. The first one is for the average PSNR values and the second one only for the frame No#

### Road Map

* The StdOut is in text format but not necessery.
* The YUV formats are 444, 422, 420, 411 other formats are not supported.
* The height and width only in pixels and not the format (CIF, 4CIF, QCIF, ..).


------
### Version 0.2

