#include <string.h>

#include "zbar.h"



namespace Image {


bool ParseQrcodeImage(void *raw, int width, int height)
{
	int res = false;
    zbar::ImageScanner scanner;

    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    zbar::Image image(width, height, "Y800", raw, width * height);

    int n = scanner.scan(image);
    if(n <= 0) {
        return res;
    }

    for(zbar::Image::SymbolIterator symbol = image.symbol_begin();
        symbol != image.symbol_end(); ++symbol) {

        printf("%s %s\n", symbol->get_type_name().c_str(),
            symbol->get_data().c_str());
        res = true;
    }

    image.set_data(NULL, 0);

    return res;
}

} // end namespace


