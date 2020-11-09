/* See LICENSE file for license details. */
#include <stdarg.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

static xcb_connection_t * dpy;
static xcb_screen_t     * scre;
static xcb_window_t       root;

static size_t strlen_c(char * str) {
    size_t n = 0;
    char * p = str;
    while ((* (p++)) != 0) {
        ++n;
    }
    return n;
}

static int strcmp_c(char * str1, char * str2) {
    char * c1 = str1;
    char * c2 = str2;
    while ((* c1) && ((* c1) == (* c2))) {
        ++c1;
        ++c2;
    }
    int n = (* c1) - (* c2);
    return n;
}

static int die(char * errstr) {
    size_t n = strlen_c(errstr);
    write(STDERR_FILENO, errstr, n);
    return 1;
}

static uint32_t nameToPixel(char * name, uint32_t pixel) {
    int ret = -1;
    if (!name || !*name) {
        ret = pixel;
    }
    xcb_lookup_color_cookie_t  lcc;
    xcb_lookup_color_reply_t * lcr;
    xcb_coloritem_t            ecolor;
    if (ret == -1) {
        size_t n = strlen_c(name);
        lcc = xcb_lookup_color_unchecked(dpy, scre->default_colormap, n, name);
        lcr = xcb_lookup_color_reply(dpy, lcc, NULL);
    }
    if ((ret == -1) && lcr) {
        ecolor.red = lcr->exact_red;
        ecolor.green = lcr->exact_green;
        ecolor.blue = lcr->exact_blue;
    } else {
        die("unknown color\n");
        ret = 0;
    }
    if (ret == -1) {
        xcb_alloc_color_cookie_t acc = xcb_alloc_color_unchecked(dpy,
            scre->default_colormap, ecolor.red, ecolor.green, ecolor.blue);
        xcb_alloc_color_reply_t  * acr = xcb_alloc_color_reply(dpy, acc, NULL);
        if (acr != NULL) {
            ret = ecolor.pixel;
        } else {
            die("unable to allocate color on display\n");
            ret = 0;
        }
    }
    return ret;

}

int main(int argc, char *argv[]) {
    int      ret = 0;
    char   * solid = NULL;
    int      nbr;

    if ((argc == 2) && (strcmp_c("-v", argv[1]) == 0)) {
        ret = die("xbg-0.0.1, © 2020 Michael Czigler, see LICENSE for details\n");
    }
    if ((ret == 0) && (argc != 2)) {
        ret = die("usage: xbg [color] [-v]\n");
    }
    if (ret == 0) {
        solid = argv[1];
    }
    if ((ret == 0) && (solid == NULL)) {
        ret = die("no color provided\n");
    }
    if (ret == 0) {
        dpy = xcb_connect(NULL, &nbr);
        int errno = xcb_connection_has_error(dpy);
        if (errno > 0) {
            ret = die("unable to open display\n");
        }
    }
    if (ret == 0) {
        scre = xcb_aux_get_screen(dpy, nbr);
        root = scre->root;
        uint32_t params[1] = { nameToPixel(solid, scre->black_pixel) };
        xcb_change_window_attributes(dpy, root, XCB_CW_BACK_PIXEL, params);
        xcb_clear_area(dpy, 0, root, 0, 0, 0, 0);
        xcb_flush(dpy);
        xcb_disconnect(dpy);
    }
    return ret;
}
