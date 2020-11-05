#!/usr/bin/env python3
import os
import sys
import argparse
import textwrap
from PIL import Image


def main(args):
    if args.width < 1:
        print("symbol in a row should be greater than 0")
        return 1
    if args.height < 1:
        print("symbol in a column should be greater than 0")
        return 1

    basename = os.path.splitext(args.font_image)[0]
    hfn = basename + ".hfont"
    basename = os.path.split(basename)[1]
    with Image.open(args.font_image).convert("1") as img:
        if img.width % args.width != 0:
            print("Image width {} not even to symbols in a row {}".format(img.width, args.width))
            return 1
        if img.height % args.height != 0:
            print("Image height {} not even to symbols in a column {}".format(img.height, args.height))
            return 1

        sym_width = img.width // args.width
        sym_height = img.height // args.height
        symbols = args.width * args.height

        if args.subparser_name == "convert":
            print("Converting image font \"{}\" to header file \"{}\", symbol size: {}x{}, chars: {}".format(args.font_image, hfn, sym_width, sym_height, symbols))

            with open(hfn, "w") as dest_hfont:
                def prepare_header_and_fbmp(val_size):
                    dest_hfont.write(textwrap.dedent("""\
                    /// Generated with {0}
                    
                    #include <stdint.h>
                    
                    #define FONT_{2}_WIDTH {3}
                    #define FONT_{2}_HEIGHT {4}
                    #define FONT_{2}_START_CODE {5}
                    #define FONT_{2}_CHARS {6}
                    
                    {7}const uint8_t g_font_bmp_{1}[FONT_{2}_CHARS * FONT_{2}_WIDTH * {8}] = {{
                    """.format(os.path.split(__file__)[1], basename, basename.upper(), sym_width, sym_height, args.startcode, symbols, "static " if args.static else "", val_size)))

                def finish_fbmp_start_fwidth():
                    dest_hfont.write(textwrap.dedent("""\
                    }};
                    {2}const uint8_t g_font_width_{0}[FONT_{1}_CHARS] = {{
                    """.format(basename, basename.upper(), "static " if args.static else "")))

                def finish_fwidth():
                    dest_hfont.write("};\n")

                wbytes = [0] * symbols
                symbol_idx = 0
                val_size = (sym_height + 7) >> 3
                prepare_header_and_fbmp(val_size)
                # foreach symbol
                for sr in range(args.height):
                    for sc in range(args.width):
                        fvalues = [0] * sym_width
                        symwidth = 0

                        # foreach column
                        for ci in range(sym_width):
                            # foreach dot in column
                            for ri in range(sym_height):
                                px = img.getpixel((sc * sym_width + ci, sr * sym_height + ri))
                                if px > 0:
                                    fvalues[ci] |= (1 << ri)

                            if fvalues[ci]:
                                symwidth = ci + 1

                        if symwidth == 0:
                            symwidth = args.spacewidth if args.spacewidth > 0 else (sym_width >> 1)

                        wbytes[symbol_idx] = symwidth

                        def make_bytes(val):
                            valbytes = []
                            for i in range(val_size):
                                valbytes.append(val % 256)
                                val = val // 256
                            return valbytes

                        ch = symbol_idx + args.startcode
                        bytes_part = ", ".join(map(lambda x: f"0x{x:02X}", (item for sublist in map(lambda x: make_bytes(x), fvalues) for item in sublist)))
                        end_part = f"{' ' if sc == (args.height - 1) and sr == (args.width - 1) else ','}   // char 0x{ch:02X}"
                        ch_part = f" - '{chr(ch)}'\n" if ch < 0x80 else "\n"
                        dest_hfont.write(" " * 4 + bytes_part + end_part + ch_part)
                        symbol_idx = symbol_idx + 1

                finish_fbmp_start_fwidth()
                for wi in range((len(wbytes) >> 4) + 1):
                    wstart = wi << 4
                    if wstart < len(wbytes):
                        wlen = len(wbytes) - wstart
                        if wlen > 16:
                            wlen = 16

                        chs = wstart + args.startcode
                        che = chs + wlen - 1
                        bytes_part = ", ".join(map(lambda x: f"0x{x:02X}", wbytes[wstart:wstart + wlen]))
                        end_part = f"{' ' if (wstart + wlen) >= len(wbytes) else ','}   // chars from 0x{chs:02X} to 0x{che:02X}"
                        ch_part = "\n"
                        if chs < 0x80:
                            ch_part = f" - '{''.join(map(lambda x: chr(x), range(chs, (che + 1) if che < 0x80 else 0x80)))}'\n"

                        dest_hfont.write(" " * 4 + bytes_part + end_part + ch_part)

                finish_fwidth()

        if args.subparser_name == "normalize":
            print("normalize not implemented")

    return 0


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="Font image processor")
    ap.add_argument("font_image", type=str, help="font image filename")
    ap.add_argument("width", type=int, help="width of the symbol")
    ap.add_argument("height", type=int, help="height of the symbol")
    sp = ap.add_subparsers(dest="subparser_name")

    # conversion command args
    p_convert = sp.add_parser("convert", description="Converts image to header file")
    p_convert.add_argument("--startcode", type=int, default=0x20, help="Starting code in chars table")
    p_convert.add_argument("--spacewidth", type=int, default=0, help="Space width (default is half of max width)")
    p_convert.add_argument("--static", action="store_true", help="Declare bmp as static")

    # normalization command args
    p_normalize = sp.add_parser("normalize", description="Normalize font image")

    sys.exit(main(ap.parse_args()))
