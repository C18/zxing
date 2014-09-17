#include <stdlib.h>

typedef struct {
	char *text;
} ZXingDecodeResultNode;

typedef  struct {
	size_t size;
	ZXingDecodeResultNode *nodes;
} ZXingDecodeResult;

#ifdef __cplusplus
extern "C" {
#endif
	ZXingDecodeResult* zxing_decode_png(unsigned char *data, size_t size);
	ZXingDecodeResult* zxing_decode_jpeg(unsigned char *data, size_t size);
	void release_zxing_decode_result(ZXingDecodeResult* result);
	ZXingDecodeResultNode zxing_extract_decode_result(ZXingDecodeResult* result, size_t idx);
#ifdef __cplusplus
}
#endif
