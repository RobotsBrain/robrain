#include <string.h>

#include "zbar.h"



namespace Image {


bool ParseQrcodeImage(void *raw, int width, int height)
{
	int res = false;

	// create a reader
    zbar::ImageScanner scanner;

    // configure the reader
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

    // wrap image data
    zbar::Image image(width, height, "Y800", raw, width * height);

    // scan the image for barcodes
    int n = scanner.scan(image);

    // extract results
    for(zbar::Image::SymbolIterator symbol = image.symbol_begin();
        symbol != image.symbol_end(); ++symbol) {
        // do something useful with results
        printf("%s %s\n", symbol->get_type_name().c_str(), symbol->get_data().c_str());

        res = true;
    }

    // clean up
    image.set_data(NULL, 0);

    return res;
}

} // end namespace


