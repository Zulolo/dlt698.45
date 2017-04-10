﻿/*
 * dealProtocol.h
 *
 *  Created on: 2013-3-20
 *      Author: Administrator
 */

#ifndef DEALPROTOCOL_H_
#define DEALPROTOCOL_H_

#include "format3762.h"

#define		UP 		1	//上行
#define 	DOWN	0	//下行

//AFN=00
INT16S dealAFN00_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN00_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN00(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=01
INT16S dealAFN01_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN01_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN01_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN01(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=02
INT16S dealAFN02_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN02(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=03
INT16S dealAFN03_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F4(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F5(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F6(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F7(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F8(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F9(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F10(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03_F11(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN03(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=04
INT16S dealAFN04_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN04_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN04_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN04(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=05
INT16S dealAFN05_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN05_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN05_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN05_F4(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN05_F5(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN05(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=06
INT16S dealAFN06_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN06_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN06_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN06_F4(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN06_F5(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN06(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=10
INT16S dealAFN10_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10_F4(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10_F5(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10_F6(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN10(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=11
INT16S dealAFN11_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11_F4(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11_F5(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11_F6(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN11(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=12
INT16S dealAFN12_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN12_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN12_F3(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN12(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=13
INT16S dealAFN13_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN13(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=14
INT16S dealAFN14_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN14_F2(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN14(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);

//AFN=15
INT16S dealAFN15_F1(FORMAT3762* format3762, INT8U dir, INT8U* data);
INT16S dealAFN15(FORMAT3762* format3762, INT8U dir, INT8U fn, INT8U* data);


INT16S analyzeData(FORMAT3762* format3762, INT8U dir, INT8U afn, INT8U fn, INT8U* data);
INT16S composeData(FORMAT3762* format3762, INT8U dir, INT8U afn, INT8U fn, INT8U* data);
#endif /* DEALPROTOCOL_H_ */