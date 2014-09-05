// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 *  Copyright 2010-2011 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <fstream>
#include <string>
#include "ImageReaderSource.h"
#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <exception>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/multi/qrcode/QRCodeMultiReader.h>
#include <zxing/multi/ByQuadrantReader.h>
#include <zxing/multi/MultipleBarcodeReader.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

#include <stdlib.h>

#include "zxingbridge.h"


ZXingDecodeResult* make_zxing_decode_result(size_t size);
ZXingDecodeResult* zxing_decode(unsigned char *data, size_t size, SOURCE_FORMAT format);
using namespace std;
using namespace zxing;
using namespace zxing::multi;
using namespace zxing::qrcode;


vector<Ref<Result> > decode_multi(Ref<BinaryBitmap> image, DecodeHints hints) {
	QRCodeMultiReader reader;
	return reader.decodeMultiple(image, hints);
}

vector<Ref<Result> > decode_image(Ref<LuminanceSource> source, bool hybrid, bool harder) {

	vector<Ref<Result> > results;
	string cell_result;

	try {
		Ref<Binarizer> binarizer;
		if (hybrid) {
			binarizer = new HybridBinarizer(source);
		} else {
			binarizer = new GlobalHistogramBinarizer(source);
		}
		DecodeHints hints(DecodeHints::DEFAULT_HINT);
//		hints.setTryHarder(harder);
		Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
		results = decode_multi(binary, hints);

	} catch (const ReaderException& e) {
		cell_result = "zxing::ReaderException: " + string(e.what());
	} catch (const zxing::IllegalArgumentException& e) {
		cell_result = "zxing::IllegalArgumentException: " + string(e.what());
	} catch (const zxing::Exception& e) {
		cell_result = "zxing::Exception: " + string(e.what());
	} catch (const std::exception& e) {
		cell_result = "std::exception: " + string(e.what());
	} 



	return results;
}


int main() {

	FILE *f = fopen("baidu.png", "rb");

	unsigned char *buf = (unsigned char *)malloc(1024 * 1024);

	size_t len = fread(buf, 1, 1024*1024, f);

	cout << len << endl;

	ZXingDecodeResult* result = zxing_decode_png(buf, len);
	release_zxing_decode_result(result);

	return 0;
}

ZXingDecodeResult* zxing_decode_jpeg(unsigned char *data, size_t size)
{
	return zxing_decode(data, size, FORMAT_JPEG);
}

ZXingDecodeResult* zxing_decode_png(unsigned char *data, size_t size)
{
	return zxing_decode(data, size, FORMAT_PNG);
}

ZXingDecodeResult* zxing_decode(unsigned char *data, size_t size, SOURCE_FORMAT format)
{
	Ref<LuminanceSource> source;
	try {
		source = ImageReaderSource::create(data, size, format);
	} catch (const zxing::IllegalArgumentException &e) {
		cerr << e.what() << " (ignoring)" << endl;
	} 
	vector<Ref<Result> > results = decode_image(source, false, false);

	ZXingDecodeResult *result = make_zxing_decode_result(results.size());	

	for(size_t i = 0; i < results.size(); i++)
	{ 
		string text = results[i]->getText()->getText();
		result->nodes[i].text = (char *)malloc(text.length() + 1);
		strncpy(result->nodes[i].text, text.c_str(), text.length() + 1);
	}

	return result;
}

ZXingDecodeResult* make_zxing_decode_result(size_t size)
{
	ZXingDecodeResult* result = (ZXingDecodeResult*)malloc(sizeof(ZXingDecodeResult));
	result->size = size;
	result->nodes = (ZXingDecodeResultNode*)malloc(sizeof(ZXingDecodeResultNode) * size);
	return result;
}

void release_zxing_decode_result(ZXingDecodeResult* result)
{
	for (size_t i = 0; i < result->size; i++)
	{
		free(result->nodes[i].text);
	}
	free(result->nodes);
	free(result);
}

ZXingDecodeResultNode zxing_extract_decode_result(ZXingDecodeResult* result, size_t idx)
{
	return result->nodes[idx];
}
