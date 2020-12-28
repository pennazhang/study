//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include "hxLog.h"
#include "videoEP.h"
#include "irUtilities.h"

#define VOLANTE_PORT_IR   			50020

#define IRCODE_PATH                     "/var/www/htdocs/localPlay/ircodes/"

#if 0
// General settings
#define MAX_FILENAME_LENGTH         128
#define MAX_PATH_SIZE               256
#define MAX_DB_IMAGES               (MAX_HP_LP_FILES * NUM_LP_LISTS * 2 + 3) // + 3 is for default image and NSM and CPC images
#define MAX_FILE_NAME               256
#define MAX_STRNG                   128


// Factory Defaults
#define DEFAULT_SERIAL_NUMBER           "N2400A01000001"

// Configuration Files
#define CONFIG_FILE_PATH_ALL            "/svsi/config/config.txt"
#define CONFIG_FILE_PATH_MODE           "/svsi/config/configMode.txt"
#define CONFIG_FILE_PATH_STREAM         "/svsi/config/configStream.txt"
#define SHARED_PARAMS_FILEPATH          "/svsi/tmp/videoEPParams.xml"

// SPI Flash
#define SPIFLASH_MLO_FILENAME           "/svsi/config/N2400-MLO.byteswap"
#define SPIFLASH_UBOOT_FILENAME         "/svsi/config/N2400-u-boot_spi.img"
#define SPIFLASH_FILE_BUFFER_SIZE       (1<<19)

// Configuration serial port
#define INIT_DEV_PORT                   "/svsi/initDevPort.conf"

// Ethernet settings
#define IP_SETTINGS_PATH                "/svsi/config/ipset.txt"

// Parts
#define SII9396_0_I2C_ADDR              0x30
#define SII9396_1_I2C_ADDR              0x34

// Modes
#define MAX_MODE_NAME   128

// EDID Programmed initial status file
#define EDID_PROGRAM_INITIAL_DONE           "/svsi/config/edid"
#define EDID_PROGRAM_DIGITAL_STORED     "/svsi/config/edid_digital.bin"
#define EDID_PROGRAM_ANALOG_STORED     "/svsi/config/edid_analog.bin"

// Decoder decoded EDID
#define DECODER_EDID_DECODE                 "/var/www/htdocs/tmp/edidDecoder.txt"
#define ENCODER_EDID_DECODE_LOCAL           "/var/www/htdocs/tmp/edidEncoderLocal.txt"
#define ENCODER_EDID_DECODE_LOCAL_ANALOG    "/var/www/htdocs/tmp/edidEncoderLocalAnalog.txt"
#define ENCODER_EDID_DECODE_PT              "/var/www/htdocs/tmp/edidEncoderPT.txt"

#define SI598_SETTINGS                      "/svsi/config/si598.txt"

// -------------- Thread Priorities:
#define AUDIO_THREAD_PRIORITY       -10

// -------------- Local play settings

//! Maximum number of host play images
#define MAX_LP_IMAGES           64

#define NUM_LP_LISTS                8
#define MAX_HP_LP_FILES             MAX_LP_IMAGES

#define LOCALPLAY_FILENAME_CANNED_TEMP              "/svsi/tmp/LP_CannedFrame.bin"
#define LOCALPLAY_DEFAULT_PLAYLIST_NAME             "Default PlayList %d"
#define LOCALPLAY_DEFAULT_IMAGE_NAME                "Default"

#define LOCALPLAY_AUDIO_FILENAME_DEFAULT            "none" // Default is now NONE.  originally: "/svsi/config/audio.mp3"

#define LP_PARAMS_FILEPATH                          "/svsi/tmp/lpParams.xml"
#define IMAGE_DB_PARAMS_FILEPATH                    "/svsi/tmp/imageDBParams.xml"

#define LOCAL_PLAY_TMP_JPEG_DIR                     "/var/www/htdocs/tmp/"

#define IMAGE_DATABASE_DIRECTORY                    "/var/www/htdocs/localPlay" // "/svsi/config/localPlay"
#define LOCALPLAY_FILENAME_INDEX                    "/svsi/config/lplayIndex%d.txt"
#define LOCALPLAY_AUDIO_FILENAME_INDEX              "/svsi/config/lplayAudioIndex%d.txt"
#define LOCALPLAY_FILENAME_MASK                     "/svsi/config/lplayIndex*.txt"
#define LOCALPLAY_AUDIO_FILENAME_MASK               "/svsi/config/lplayAudioIndex*.txt"
#define LOCALPLAY_FILENAME_DEFAULT_N1000            "/svsi/config/lplay1000.raw"
#define HOSTPLAY_FILENAME_DEFAULT_N1000             "/svsi/config/hplay1000.raw"
#define LOCALPLAY_FILENAME_DEFAULT_N2000            "/svsi/config/lplay2000.bin"
#define HOSTPLAY_FILENAME_DEFAULT_N2000             "/svsi/config/hplay2000.bin"
#define LOCALPLAY_FILENAME_DEFAULT_IDB_UNCOMPRESSED "LP_Default_Localplay_N1000_1280x720.raw"
#define HOSTPLAY_FILENAME_DEFAULT_IDB_UNCOMPRESSED  "LP_Default_Hostplay_N1000_1280x720.raw"
#define LOCALPLAY_FILENAME_DEFAULT_IDB_COMPRESSED   "LP_Default_Localplay_N2000_1280x720.bin"
#define HOSTPLAY_FILENAME_DEFAULT_IDB_COMPRESSED    "LP_Default_Hostplay_N2000_1280x720.bin"
#define LOCALPLAY_FILENAME_DEFAULT_Y                "/svsi/tmp/lplay_Y.bin"
#define LOCALPLAY_FILENAME_DEFAULT_CBCR             "/svsi/tmp/lplay_CBCR.bin"
#define LOCALPLAY_FILENAME_TEMPLATE_Y               "/svsi/tmp/lplay%d_Y.bin"
#define LOCALPLAY_FILENAME_TEMPLATE_CBCR            "/svsi/tmp/lplay%d_CBCR.bin"
#define LP_AUDIO_PATH                               "/var/www/htdocs/localPlay/audio"
#define IMAGE_THUMBNAIL_PATH                        "/var/www/htdocs/localPlay/thumbnails"

// LP fetch temp file path
#define LOCALPLAY_FETCH_LIST_FILENAME   "LP_Fetch_List.txt"
#define LOCALPLAY_FETCH_LIST_FILEPATH   "/var/www/htdocs/tmp/"LOCALPLAY_FETCH_LIST_FILENAME
#define LOCALPLAY_FETCH_IMAGES_FILENAME "/var/www/htdocs/tmp/LP_Fetch_Images.txt"

// LP Audio fetch temp file path
#define LOCALPLAY_AUDIO_FETCH_LIST_FILENAME   "LP_Fetch_Audio_List.txt"
#define LOCALPLAY_AUDIO_FETCH_LIST_FILEPATH   "/var/www/htdocs/tmp/"LOCALPLAY_AUDIO_FETCH_LIST_FILENAME

// fetch thread states
#define FETCHTHREAD_IDLE            0
#define FETCHTHREAD_DOWNLOADING     1
#define FETCHTHREAD_PROCESSING      2

// special LP list numbers
#define LP_NUM_SPECIAL_LISTS        4
#define LP_CPC_LIST_NUMBER          (NUM_LP_LISTS)
#define LP_NONSUPMODE_LIST_NUMBER   (NUM_LP_LISTS+1)
#define LP_ENCRYPTED_NUMBER         (NUM_LP_LISTS+2)
#define LP_INFOPAGE_NUMBER          (NUM_LP_LISTS+3) // a special local play list

#define NONSUP_MODE_FILENAME                        "/svsi/config/nsm.raw"
#define CONTENT_PROTECTED_FILENAME_RX               "/svsi/config/lplayCPC.raw"
#define CONTENT_PROTECTED_FILENAME_TX               "/svsi/config/hplayCPC.raw"
#define ENCRYPTED_MODE_FILENAME                     "/svsi/config/lplayEncrypted.raw"

// -------- EMCAST Features
#define NUM_EMCAST_LISTS            8
#define MAX_EMCAST_FILES            MAX_LP_IMAGES
#define MAX_EMCAST_DB_IMAGES        (MAX_EMCAST_FILES * NUM_EMCAST_LISTS * 2)

#define EMCAST_IMAGE_DATABASE_DIRECTORY "/var/www/htdocs/localPlay/emcast" // "/svsi/config/localPlay"
#define EMCAST_FILENAME_INDEX           "/svsi/config/emcastIndex%d.txt"
#define EMCAST_FILENAME_DEFAULT0        "/svsi/config/emcast0.bin"
#define EMCAST_FILENAME_DEFAULT1        "/svsi/config/emcast1.bin"
#define EMCAST_FILENAME_DEFAULT_Y       "/svsi/tmp/emcast_Y.bin"
#define EMCAST_FILENAME_DEFAULT_CBCR    "/svsi/tmp/emcast_CBCR.bin"
#define EMCAST_FILENAME_TEMPLATE_Y      "/svsi/tmp/emcast%d_Y.bin"
#define EMCAST_FILENAME_TEMPLATE_CBCR   "/svsi/tmp/emcast%d_CBCR.bin"

#define EMCAST_DEFAULT_PLAYLIST_NAME    "Default EMCAST List %d"
#define EMCAST_DEFAULT_IMAGE_NAME       "Default"

// -------- Image Conversion:
#define IMAGE_CONVERSION_QUEUE_DEPTH    512     //!< Maximum number of images to queue up for conversion

// -------- Analog table
#define ANALOG_MODE_TABLE               "/svsi/config/analogModes.txt"         //!< Holds a list of all the analog modes
#define ANALOG_MODE_TABLE_HTTP          "/var/www/htdocs/tmp/analogModes.txt"  //!< Holds a list of all the analog modes

// -------- Additional Screens:
#define ADDITIONAL_SCREENS_FILEPATH             "/svsi/tmp/additionalScreens.xml"
#define ADDITIONAL_SCREENS_FILENAME_INDEX       "/svsi/config/additionalScreens.txt"

// -------- Command History:
#define COMMAND_HISTORY_FILEPATH        "/var/www/htdocs/tmp/cmdHistory.txt"

// -------- Generic Status Events
#define GENERIC_EVENTS_LIST_FILEPATH    "/var/www/htdocs/localPlay/genericEvents.txt"


// UNFILED
#define IRCODE_PATH                     "/var/www/htdocs/localPlay/ircodes/"
#define IRCODE_TEMPORARY_IRSENDRAW_FILE "/svsi/tmp/irsendraw.ir"

#define SERIAL_CODES_FILE_PATH          "/var/www/htdocs/localPlay/serialcodes.data"
#define CROPREF_FILE_PATH               "/var/www/htdocs/localPlay/croprefs.data"

#define DEBUG_ENABLE_FILENAME           "/svsi/tmp/debug"

#define SSL_KEYFILE                     "/etc/lighttpd/server.keys"
#define SSL_CERTIFICATE                 "/etc/lighttpd/server.pem"

#define VOLANTE_PORT                50001
#define VOLANTE_PORT_LOCAL_ONLY     (VOLANTE_PORT+2)
#define VOLANTE_PORT_NETLINX        50007


#define SAVEPARAMS_DELAY_SECS   10 // seconds

#define UPDATE_MAX_RETRY            5

#define AGC_TARGET_VALUE_COUNT      8
#define AGC_ATTACK_VALUE_COUNT      4
#define AGC_DECAY_VALUE_COUNT       4

#define MAX_QUALITY_SETTING            600
#define SKIP_MAX_VALUE                 100
#define MIN_INTER_PACKET_DELAY      0x0011

#define NACT_VERSION                101 // Version 1.01, last version added Generic events

#define CONFIG_FILE_HTTP_SETTINGS_DATA  "/var/www/htdocs/nseriessettings.data" //!< Holds account names and events
#define CONFIG_FILE_LDAP_SETTINGS_DATA  "/var/www/htdocs/ldapinfo" //!< Holds LDAP settings

#define CONFIG_FILE_SII9396                "/svsi/config/sii9396.txt"
#define CONFIG_FILE_SII9396_PROGRAMMED     "/svsi/config/sii9396.programmed"
#define SII9396_RX_FIRMWARE_FILE           "/svsi/config/FW_9396_HDMI_Rx.bin"
#define SII9396_TX_FIRMWARE_FILE           "/svsi/config/FW_9396_HDMI_Tx.bin"

#define DEFAULT_RESOLUTION              MODE_720P59_94
#define DEFAULT_RESOLUTION_MODE_NAME    "720p59.94.mode"

#define NETLINX_STATE                   "/svsi/tmp/netlinx.state"

// SPI Flash Programmed initial status file
#define SPIFLASH_PROGRAM_INITIAL_DONE   "/svsi/config/spiflash"

#define SPIFLASH_MLO_FILENAME       "/svsi/config/N2400-MLO.byteswap"
#define SPIFLASH_UBOOT_FILENAME     "/svsi/config/N2400-u-boot_spi.img"
#define SPIFLASH_FILE_BUFFER_SIZE   (1<<19)

#define CONFIG_OUT_OF_SYNC_LIMIT    (2000/50) //!< 2 seconds of 50 ms intervals until out of sync restarts
#endif // 0

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern HxLogger* logger;
extern const char *DAEMON_NAME;
extern clock_t gStartClock;

extern VE_Settings_t G_veSet;
extern irParamsFIFO_t G_irCommandFIFO;

#define SVSI_SYSLOG(format, ...) { \
	uint32_t clock_elapse = ((float)(clock() - gStartClock + 1) / 1000); \
	hxDebug(logger, "[%s]:[time elapse: %ds]:%s " format, DAEMON_NAME, clock_elapse, __PRETTY_FUNCTION__, ## __VA_ARGS__); \
}
#define SVSI_DEBUG_SYSLOG(f_, ...)      printf(f_, ## __VA_ARGS__)

#endif /* _CONFIG_H_ */
