#include "dfu.h"
#include "dfu_file.h"
#include "dfu_util.h"
#include "dfu_load.h"
#include "dfuse.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "portable.h"

struct dfu_if* dfu_root = NULL;
char* match_path = NULL;
int match_vendor = 0x1d50;
int match_product = 0x6082;
int match_vendor_dfu = -1;
int match_product_dfu = -1;
int match_config_index = -1;
int match_iface_index = -1;
int match_iface_alt_index = -1;
const char* match_iface_alt_name = NULL;
const char* match_serial = NULL;
const char* match_serial_dfu = NULL;

int verbose = 0;

void firmware_dfu_init()
{
    dfu_root = NULL;
    match_path = NULL;
    match_vendor = 0x1d50;
    match_product = 0x6082;
    match_vendor_dfu = -1;
    match_product_dfu = -1;
    match_config_index = -1;
    match_iface_index = -1;
    match_iface_alt_index = -1;
    match_iface_alt_name = NULL;
    match_serial = NULL;
    match_serial_dfu = NULL;
}

int firmware_dfu_mode_probe()
{
	libusb_context* ctx;
	firmware_dfu_init();
	int ret = libusb_init(&ctx);
	if (ret) return 0;
	probe_devices(ctx);
	if (dfu_root == NULL) return 0;
	return 1;
}

int firmware_dfu_probe()
{
    libusb_context* ctx;
    firmware_dfu_init();
    int ret = libusb_init(&ctx);
    if(ret) return 0;
    probe_devices(ctx);
    if (dfu_root==NULL) return 0;
    return 1;
}

int firmware_dfu_upload(const char* filename, int vendor, int product)
{
    firmware_dfu_init();

    if (vendor) match_vendor = vendor;
    if (product) match_product = product;

    int expected_size = 0;
    unsigned int transfer_size = 0;
    enum mode mode = MODE_DOWNLOAD;
    struct dfu_status status;
    libusb_context* ctx;
    struct dfu_file file;
    //	char *end;
    int final_reset = 0;
    int ret;
    int dfuse_device = 0;
    int fd;
    const char* dfuse_options = NULL;
    int detach_delay = 5;
    uint16_t runtime_vendor;
    uint16_t runtime_product;

    memset(&file, 0, sizeof(file));

    file.name = filename;

    if(mode == MODE_DOWNLOAD) {
        dfu_load_file(&file, MAYBE_SUFFIX, MAYBE_PREFIX);
        /* If the user didn't specify product and/or vendor IDs to match,
         * use any IDs from the file suffix for device matching */
        if(match_vendor < 0 && file.idVendor != 0xffff) {
            match_vendor = file.idVendor;
            printf("Match vendor ID from file: %04x\n", match_vendor);
        }
        if(match_product < 0 && file.idProduct != 0xffff) {
            match_product = file.idProduct;
            printf("Match product ID from file: %04x\n", match_product);
        }
    }

    ret = libusb_init(&ctx);
    if(ret)
        errxx(EX_IOERR, "unable to initialize libusb: %i", ret);

    if(verbose > 2) {
        libusb_set_debug(ctx, 255);
    }

    probe_devices(ctx);

    if(mode == MODE_LIST) {
        list_dfu_interfaces();
        return (0);
    }

    if(dfu_root == NULL) {
        errxx(EX_IOERR, "No DFU capable USB device available");
    } else if(dfu_root->next != NULL) {
        /* We cannot safely support more than one DFU capable device
         * with same vendor/product ID, since during DFU we need to do
         * a USB bus reset, after which the target device will get a
         * new address */
        errxx(EX_IOERR, "More than one DFU capable USB device found! "
                        "Try `--list' and specify the serial number "
                        "or disconnect all but one device\n");
    }

    /* We have exactly one device. Its libusb_device is now in dfu_root->dev */

    printf("Opening DFU capable USB device...\n");
    ret = libusb_open(dfu_root->dev, &dfu_root->dev_handle);
    if(ret || !dfu_root->dev_handle)
        errxx(EX_IOERR, "Cannot open device");

    printf("ID %04x:%04x\n", dfu_root->vendor, dfu_root->product);

    printf("Run-time device DFU version %04x\n", libusb_le16_to_cpu(dfu_root->func_dfu.bcdDFUVersion));

    /* Transition from run-Time mode to DFU mode */
    if(!(dfu_root->flags & DFU_IFF_DFU)) {
        int err;
        /* In the 'first round' during runtime mode, there can only be one
        * DFU Interface descriptor according to the DFU Spec. */

        /* FIXME: check if the selected device really has only one */

        runtime_vendor = dfu_root->vendor;
        runtime_product = dfu_root->product;

        printf("Claiming USB DFU Runtime Interface...\n");
        if(libusb_claim_interface(dfu_root->dev_handle, dfu_root->interface) < 0) {
            errxx(EX_IOERR, "Cannot claim interface %d", dfu_root->interface);
        }

        if(libusb_set_interface_alt_setting(dfu_root->dev_handle, dfu_root->interface, 0) < 0) {
            errxx(EX_IOERR, "Cannot set alt interface zero");
        }

        printf("Determining device status: ");

        err = dfu_get_status(dfu_root, &status);
        if(err == LIBUSB_ERROR_PIPE) {
            printf("Device does not implement get_status, assuming appIDLE\n");
            status.bStatus = DFU_STATUS_OK;
            status.bwPollTimeout = 0;
            status.bState = DFU_STATE_appIDLE;
            status.iString = 0;
        } else if(err < 0) {
            errxx(EX_IOERR, "error get_status");
        } else {
            printf("state = %s, status = %d\n", dfu_state_to_string(status.bState), status.bStatus);
        }
        milli_sleep(status.bwPollTimeout);

        switch(status.bState) {
        case DFU_STATE_appIDLE:
        case DFU_STATE_appDETACH:
            printf("Device really in Runtime Mode, send DFU "
                   "detach request...\n");
            if(dfu_detach(dfu_root->dev_handle, dfu_root->interface, 1000) < 0) {
                warnx("error detaching");
            }
            if(dfu_root->func_dfu.bmAttributes & USB_DFU_WILL_DETACH) {
                printf("Device will detach and reattach...\n");
            } else {
                printf("Resetting USB...\n");
                ret = libusb_reset_device(dfu_root->dev_handle);
                if(ret < 0 && ret != LIBUSB_ERROR_NOT_FOUND)
                    errxx(EX_IOERR, "error resetting "
                                    "after detach");
            }
            break;
        case DFU_STATE_dfuERROR:
            printf("dfuERROR, clearing status\n");
            if(dfu_clear_status(dfu_root->dev_handle, dfu_root->interface) < 0) {
                errxx(EX_IOERR, "error clear_status");
            }
        /* fall through */
        default:
            warnx("WARNING: Runtime device already in DFU state ?!?");
            libusb_release_interface(dfu_root->dev_handle, dfu_root->interface);
            goto dfustate;
        }
        libusb_release_interface(dfu_root->dev_handle, dfu_root->interface);
        libusb_close(dfu_root->dev_handle);
        dfu_root->dev_handle = NULL;

        if(mode == MODE_DETACH) {
            libusb_exit(ctx);
            return (0);
        }

        /* keeping handles open might prevent re-enumeration */
        disconnect_devices();

        milli_sleep(detach_delay * 1000);

        /* Change match vendor and product to impossible values to force
         * only DFU mode matches in the following probe */
        match_vendor = match_product = 0x10000;

        probe_devices(ctx);

        if(dfu_root == NULL) {
            errxx(EX_IOERR, "Lost device after RESET?");
        } else if(dfu_root->next != NULL) {
            errxx(EX_IOERR, "More than one DFU capable USB device found! "
                            "Try `--list' and specify the serial number "
                            "or disconnect all but one device");
        }

        /* Check for DFU mode device */
        if(!(dfu_root->flags | DFU_IFF_DFU))
            errxx(EX_SOFTWARE, "Device is not in DFU mode");

        printf("Opening DFU USB Device...\n");
        ret = libusb_open(dfu_root->dev, &dfu_root->dev_handle);
        if(ret || !dfu_root->dev_handle) {
            errxx(EX_IOERR, "Cannot open device");
        }
    } else {
        /* we're already in DFU mode, so we can skip the detach/reset
         * procedure */
        /* If a match vendor/product was specified, use that as the runtime
         * vendor/product, otherwise use the DFU mode vendor/product */
        runtime_vendor = match_vendor < 0 ? dfu_root->vendor : match_vendor;
        runtime_product = match_product < 0 ? dfu_root->product : match_product;
    }

dfustate:
#if 0
	printf("Setting Configuration %u...\n", dfu_root->configuration);
	if (libusb_set_configuration(dfu_root->dev_handle, dfu_root->configuration) < 0) {
		errx(EX_IOERR, "Cannot set configuration");
	}
#endif
    printf("Claiming USB DFU Interface...\n");
    if(libusb_claim_interface(dfu_root->dev_handle, dfu_root->interface) < 0) {
        errxx(EX_IOERR, "Cannot claim interface");
    }

    printf("Setting Alternate Setting #%d ...\n", dfu_root->altsetting);
    if(libusb_set_interface_alt_setting(dfu_root->dev_handle, dfu_root->interface, dfu_root->altsetting) < 0) {
        errxx(EX_IOERR, "Cannot set alternate interface");
    }

status_again:
    printf("Determining device status: ");
    if(dfu_get_status(dfu_root, &status) < 0) {
        errxx(EX_IOERR, "error get_status");
    }
    printf("state = %s, status = %d\n", dfu_state_to_string(status.bState), status.bStatus);

    milli_sleep(status.bwPollTimeout);

    switch(status.bState) {
    case DFU_STATE_appIDLE:
    case DFU_STATE_appDETACH:
        errxx(EX_IOERR, "Device still in Runtime Mode!");
        break;
    case DFU_STATE_dfuERROR:
        printf("dfuERROR, clearing status\n");
        if(dfu_clear_status(dfu_root->dev_handle, dfu_root->interface) < 0) {
            errxx(EX_IOERR, "error clear_status");
        }
        goto status_again;
        break;
    case DFU_STATE_dfuDNLOAD_IDLE:
    case DFU_STATE_dfuUPLOAD_IDLE:
        printf("aborting previous incomplete transfer\n");
        if(dfu_abort(dfu_root->dev_handle, dfu_root->interface) < 0) {
            errxx(EX_IOERR, "can't send DFU_ABORT");
        }
        goto status_again;
        break;
    case DFU_STATE_dfuIDLE:
        printf("dfuIDLE, continuing\n");
        break;
    default:
        break;
    }

    if(DFU_STATUS_OK != status.bStatus) {
        printf("WARNING: DFU Status: '%s'\n", dfu_status_to_string(status.bStatus));
        /* Clear our status & try again. */
        if(dfu_clear_status(dfu_root->dev_handle, dfu_root->interface) < 0)
            errxx(EX_IOERR, "USB communication error");
        if(dfu_get_status(dfu_root, &status) < 0)
            errxx(EX_IOERR, "USB communication error");
        if(DFU_STATUS_OK != status.bStatus)
            errxx(EX_SOFTWARE, "Status is not OK: %d", status.bStatus);

        milli_sleep(status.bwPollTimeout);
    }

    printf("DFU mode device DFU version %04x\n", libusb_le16_to_cpu(dfu_root->func_dfu.bcdDFUVersion));

    if(dfu_root->func_dfu.bcdDFUVersion == libusb_cpu_to_le16(0x11a))
        dfuse_device = 1;

    /* If not overridden by the user */
    if(!transfer_size) {
        transfer_size = libusb_le16_to_cpu(dfu_root->func_dfu.wTransferSize);
        if(transfer_size) {
            printf("Device returned transfer size %i\n", transfer_size);
        } else {
            errxx(EX_IOERR, "Transfer size must be specified");
        }
    }

#ifdef HAVE_GETPAGESIZE
/* autotools lie when cross-compiling for Windows using mingw32/64 */
#ifndef __MINGW32__
    /* limitation of Linux usbdevio */
    if((int)transfer_size > getpagesize()) {
        transfer_size = getpagesize();
        printf("Limited transfer size to %i\n", transfer_size);
    }
#endif /* __MINGW32__ */
#endif /* HAVE_GETPAGESIZE */

    if(transfer_size < dfu_root->bMaxPacketSize0) {
        transfer_size = dfu_root->bMaxPacketSize0;
        printf("Adjusted transfer size to %i\n", transfer_size);
    }

    switch(mode) {
    case MODE_UPLOAD:
        /* open for "exclusive" writing */
        fd = open(file.name, O_WRONLY | O_BINARY | O_CREAT | O_EXCL | O_TRUNC, 0666);
        if(fd < 0)
            errxx(EX_IOERR, "Cannot open file %s for writing", file.name);

        if(dfuse_device || dfuse_options) {
            if(dfuse_do_upload(dfu_root, transfer_size, fd, dfuse_options) < 0)
                return (1);
        } else {
            if(dfuload_do_upload(dfu_root, transfer_size, expected_size, fd) < 0) {
                return (1);
            }
        }
        close(fd);
        break;

    case MODE_DOWNLOAD:
        if(((file.idVendor != 0xffff && file.idVendor != runtime_vendor) ||
               (file.idProduct != 0xffff && file.idProduct != runtime_product)) &&
            ((file.idVendor != 0xffff && file.idVendor != dfu_root->vendor) ||
                (file.idProduct != 0xffff && file.idProduct != dfu_root->product))) {
            errxx(EX_IOERR, "Error: File ID %04x:%04x does "
                            "not match device (%04x:%04x or %04x:%04x)",
                file.idVendor, file.idProduct, runtime_vendor, runtime_product, dfu_root->vendor, dfu_root->product);
        }
        if(dfuse_device || dfuse_options || file.bcdDFU == 0x11a) {
            if(dfuse_do_dnload(dfu_root, transfer_size, &file, dfuse_options) < 0)
                return (1);
        } else {
            if(dfuload_do_dnload(dfu_root, transfer_size, &file) < 0)
                return (1);
        }
        break;
    case MODE_DETACH:
        if(dfu_detach(dfu_root->dev_handle, dfu_root->interface, 1000) < 0) {
            warnx("can't detach");
        }
        break;
    default:
        errxx(EX_IOERR, "Unsupported mode: %u", mode);
        break;
    }

    if(final_reset) {
        if(dfu_detach(dfu_root->dev_handle, dfu_root->interface, 1000) < 0) {
            /* Even if detach failed, just carry on to leave the
               device in a known state */
            warnx("can't detach");
        }
        printf("Resetting USB to switch back to runtime mode\n");
        ret = libusb_reset_device(dfu_root->dev_handle);
        if(ret < 0 && ret != LIBUSB_ERROR_NOT_FOUND) {
            errxx(EX_IOERR, "error resetting after download");
        }
    }

    libusb_close(dfu_root->dev_handle);
    dfu_root->dev_handle = NULL;
    libusb_exit(ctx);

    return (1);
}
