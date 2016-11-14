

enum nal_unit_type_e {
	NAL_UNKNOWN     = 0,
	NAL_SLICE       = 1,
	NAL_SLICE_DPA   = 2,
	NAL_SLICE_DPB   = 3,
	NAL_SLICE_DPC   = 4,
	NAL_SLICE_IDR   = 5,    /* ref_idc != 0 */
	NAL_SEI         = 6,    /* ref_idc == 0 */
	NAL_SPS         = 7,
	NAL_PPS         = 8
	/* ref_idc == 0 for 6,9,10,11,12 */
};

typedef void (*STREAMCALLBACK)(unsigned char *data, int len);

void SetCallback(STREAMCALLBACK pcbk);

int WriteAAC2Ts(unsigned char *pData, int iDataSize);

int WriteH2642Ts(unsigned int framerate, unsigned char *pData, int iDataSize);

