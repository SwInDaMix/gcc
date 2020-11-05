#!/usr/bin/env python3
import math
import os
import sys
import argparse
import textwrap
from PIL import Image

def main(args):
    basename = os.path.splitext(args.bmp_image)[0]
    hfn = basename + ".hbmp"
    basename = os.path.split(basename)[1]
    with Image.open(args.bmp_image).convert("1") as img:

        print("Converting image bitmap \"{}\" to header file \"{}\", thumbnail: {}, dithering {}".format(args.bmp_image, hfn, "none" if args.thumbnail is None else f"{args.thumbnail[0], args.thumbnail[1]}", args.dithering))

        if args.thumbnail is not None:
            img.thumbnail((int(args.thumbnail[0]), int(args.thumbnail[1])))

        img = img.convert("1", dither=(Image.FLOYDSTEINBERG if args.dithering else Image.NONE))

        with open(hfn, "w") as destf:
            def prepare_header_and_bmp():
                destf.write(textwrap.dedent("""\
                /// Generated with {0}
                
                #include <stdint.h>
                
                #define BITMAP_{2}_WIDTH {3}
                #define BITMAP_{2}_HEIGHT {4}
                
                {5}const uint8_t g_bitmap_{1}[BITMAP_{2}_WIDTH * ((BITMAP_{2}_HEIGHT + 7) >> 3)] = {{
                """.format(os.path.split(__file__)[1], basename, basename.upper(), img.width, img.height, "static " if args.static else "")))

            def finish_bmp():
                destf.write("};\n")

            prepare_header_and_bmp()

            column_bytes = (img.height + 7) >> 3;
            for ci in range(img.width):
                bytes = [0] * column_bytes
                for ri in range(img.height):
                    px = img.getpixel((ci, ri))
                    if px == 0:
                        bytes[ri >> 3] |= (1 << (ri & 0x7))

                bytes_part = ", ".join(map(lambda x: f"0x{x:02X}", bytes))
                destf.write(" " * 4 + bytes_part + ("\n" if ci == (img.width - 1) else ",\n"))

            finish_bmp()

    return 0


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="Font image processor")
    ap.add_argument("bmp_image", type=str, help="Bitmap image filename")
    ap.add_argument("--static", action="store_true", help="Declare bmp as static")
    ap.add_argument("--dithering", action="store_true", help="Perform FLOYDSTEINBERG dithering")
    ap.add_argument("--thumbnail", nargs=2, help="Downscale the image to preferred size")

    sys.exit(main(ap.parse_args()))
