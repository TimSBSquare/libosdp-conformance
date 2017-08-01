/*
  open-osdp.h - definitions for libosdp-conformance

  (C)Copyright 2014-2017 Smithee,Spelvin,Agnew & Plinge, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Support provided by the Security Industry Association
  http://www.securityindustry.org
*/

#include <termios.h>
#include <time.h>

#define OSDP_VERSION_MAJOR (0)
#define OSDP_VERSION_MINOR (1)
#define OSDP_VERSION_BUILD (4)

// default configuration

#define OOSDP_CFG_INPUTS (8)

#define OSDP_PROFILE_PERIPHERAL_TEST_PD (0x0000)
#define OSDP_PROFILE_PERIPHERAL_TEST_CP (0x1000)
#define OSDP_PROFILE_BASIC_TEST_PD (0x0100)
#define OSDP_PROFILE_BASIC_TEST_CP (0x1100)
#define OSDP_PROFILE_BASIC (1)
#define OSDP_PROFILE_BIO (2)
#define OSDP_PROFILE_PIV (3)
#define OSDP_PROFILE_MAX_PD (0x0FFF)
#define OSDP_PROFILE_MAX_CP (0x1FFF)

#define EQUALS ==

#define C_FALSE (0)
#define C_TRUE (1)
#define C_STRING_MX (1024)

// OSDP defined constants
#define C_SOM (0x53)
#define C_OSDP_MARK (0xff)  // used in OSDP TLS to poke CP

#define OSDP_CONFIGURATION_ADDRESS (0x7F)

#define OSDP_DEST_CP (0x00)

#define OSDP_KEY_SCBK_D (0)
#define OSDP_KEY_SCBK (1)
#define OSDP_SEC_SCS_11 (0x11)
#define OSDP_SEC_SCS_12 (0x12)
#define OSDP_SEC_SCS_13 (0x13)
#define OSDP_SEC_SCS_14 (0x14)

#define OSDP_KEY_OCTETS (16)  // AES-128 CBC

#define OSDP_SCBK_DEFAULT \
  "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"

// for Secure Channel
#define OSDP_KEY_SIZE (128)

// OSDP commands
// 0x6c - 0x 6f
// 0x70 - 0x79
// 0x80
// 0xA0 - 0xA1
// replies 0x40 41 45 46 48-4B 50-58 76 78 90 79 b1
#define OSDP_POLL (0x60)
#define OSDP_ID (0x61)
#define OSDP_CAP (0x62)
#define OSDP_DIAG (0x63)
#define OSDP_LSTAT (0x64)
#define OSDP_ISTAT (0x65)
#define OSDP_OSTAT (0x66)
#define OSDP_RSTAT (0x67)
#define OSDP_OUT (0x68)
#define OSDP_LED (0x69)
#define OSDP_BUZ (0x6A)
#define OSDP_TEXT (0x6B)
#define OSDP_TDSET (0x6D)
#define OSDP_COMSET (0x6E)
#define OSDP_DATA (0x6F)
#define OSDP_PROMPT (0x71)
#define OSDP_BIOREAD (0x73)
#define OSDP_KEYSET (0x75)
#define OSDP_CHLNG (0x76)
#define OSDP_MFG (0x80)

#define OSDP_ACK (0x40)
#define OSDP_NAK (0x41)
#define OSDP_PDID (0x45)
#define OSDP_PDCAP (0x46)
#define OSDP_LSTATR (0x48)
#define OSDP_ISTATR (0x49)
#define OSDP_OSTATR (0x4A)
#define OSDP_RSTATR (0x4B)
#define OSDP_RAW (0x50)
#define OSDP_KEYPAD (0x53)
#define OSDP_COM (0x54)
#define OSDP_CCRYPT (0x76)
#define OSDP_SCRYPT (0x77)
#define OSDP_RMAC_I (0x78)
#define OSDP_BUSY (0x79)  // yes it's a reply
#define OSDP_MFGREP (0x90)

// NAK error codes
#define OO_NAK_CHECK_CRC (1)
#define OO_NAK_UNK_CMD (3)

#define OSDP_MENU_TOP (0x0000)
#define OSDP_MENU_CP_DIAG (0x0100)
#define OSDP_MENU_PD_DIAG (0x0200)
#define OSDP_MENU_SETUP (0x0800)

// commands used through breech-loading interface
// by convention starts above 1000
#define OSDP_CMDB_DUMP_STATUS (1001)
#define OSDP_CMDB_SEND_POLL (1002)
#define OSDP_CMDB_IDENT (1003)
#define OSDP_CMDB_CAPAS (1004)
#define OSDP_CMDB_RESET_POWER (1005)
#define OSDP_CMDB_PRESENT_CARD (1006)
#define OSDP_CMDB_OUT (1007)
#define OSDP_CMDB_LED (1008)
#define OSDP_CMDB_INIT_SECURE (1009)
#define OSDP_CMDB_TEXT (1010)
#define OSDP_CMDB_TAMPER (1011)
#define OSDP_CMDB_OSTAT (1012)
#define OSDP_CMDB_LSTAT (1013)
#define OSDP_CMDB_RSTAT (1014)
#define OSDP_CMDB_CONFORM_2_6_1 (1015)
#define OSDP_CMDB_ISTAT (1016)
#define OSDP_CMDB_COMSET (1017)
#define OSDP_CMDB_CONFORM_2_2_1 (1018)
#define OSDP_CMDB_CONFORM_2_2_2 (1019)
#define OSDP_CMDB_CONFORM_2_2_3 (1020)
#define OSDP_CMDB_CONFORM_2_2_4 (1021)
#define OSDP_CMDB_BUZZ (1022)
#define OSDP_CMDB_BUSY (1023)
#define OSDP_CMDB_KEYPAD (1024)

#define OSDP_CMD_NOOP (0)
#define OSDP_CMD_CP_DIAG (1)
#define OSDP_CMD_PD_DIAG (2)

#define OSDP_CMD_IDENT (2)
#define OSDP_CMD_CAP (3)
#define OSDP_CMD_COMSET (4)
#define OSDP_CMD_LCL_STAT (5)
#define OSDP_CMD_RDR_STAT (6)
#define OSDP_CMD_GET_CREDS_A (8)

// PD Diags
#define OSDP_CMD_PD_POWER (1)
#define OSDP_CMD_PD_CARD_PRESENT (2)

#define OSDP_CMD_DUMP_STATUS (7)
#define OSDP_CMD_SETUP (8)
#define OSDP_CMD_EXIT (9)

#define OSDP_CMD_CP_SEND_POLL (1)

#define OSDP_CMD_SET_CP (81)
#define OSDP_CMD_SET_PD (82)

#define OSDP_OPT_CP (101)
#define OSDP_OPT_PD (102)
#define OSDP_OPT_DEVICE (103)
#define OSDP_OPT_DEBUG (104)
#define OSDP_OPT_CHECKSUM (105)
#define OSDP_OPT_CRC (106)
#define OSDP_OPT_HELP (107)
#define OSDP_OPT_PDADDR (108)
#define OSDP_OPT_NOPOLL (109)
#define OSDP_OPT_INIT (110)
#define OSDP_OPT_MONITOR (111)
#define OSDP_OPT_SPECIAL (112)

// for secure channel
typedef struct osdp_secure_message {
  unsigned char som;
  unsigned char addr;
  unsigned char len_lsb;
  unsigned char len_msb;
  unsigned char ctrl;
  unsigned char sec_blk_len;
  unsigned char sec_blk_type;
  unsigned char sec_blk_data;
  unsigned char cmd_reply;
  unsigned char data_start;
} OSDP_SECURE_MESSAGE;

typedef struct osdp_sc_ccrypt {
  unsigned char client_id[8];
  unsigned char rnd_b[8];
  unsigned char cryptogram[16];
} OSDP_SC_CCRYPT;

#define OSDP_OUT_NOP (0)
#define OSDP_OUT_OFF_PERM_ABORT (1)
#define OSDP_OUT_ON_PERM_ABORT (2)
#define OSDP_OUT_OFF_PERM_TIMEOUT (3)
#define OSDP_OUT_ON_PERM_TIMEOUT (4)
#define OSDP_OUT_ON_TEMP_TIMEOUT (5)
#define OSDP_OUT_OFF_TEMP_TIMEOUT (6)

typedef struct osdp_out_cmd {
  int output_number;
  int control_code;
  unsigned int timer;
} OSDP_OUT_CMD;

typedef struct osdp_out_msg {
  unsigned char output_number;
  unsigned char control_code;
  unsigned char timer_lsb;
  unsigned char timer_msb;
} OSDP_OUT_MSG;

typedef struct osdp_out_state {
  unsigned int timer;
  int current;
  int permanent;
} OSDP_OUT_STATE;
#define OSDP_MAX_OUT (16)

typedef struct osdp_led_state {
  int state;
  unsigned int web_color;
} OSDP_LED_STATE;
#define OSDP_MAX_LED (256)
#define OSDP_LED_ACTIVATED (1)
#define OSDP_LED_DEACTIVATED (0)

typedef struct osdp_timer {
  time_t i_sec;
  long i_nsec;
  time_t current_seconds;
  long current_nanoseconds;
  int status;
} OSDP_TIMER;
// possible values for status
#define OSDP_TIMER_RUNNING (0)
#define OSDP_TIMER_RESTARTED (1)

typedef struct osdp_context {
  // configuration
  int disable_certificate_checking;
  int enable_secure_channel;  // 1=yes, 2=yes and use default
  char fqdn[1024];
  char log_path[1024];
  char serial_speed[1024];
  int verbosity;

  // IO context
  int current_pid;
  int fd;
  FILE *log;
  char network_address[1024];
  FILE *report;
  struct termios tio;

  // UI context
  int current_menu;

  // CP and PD context
  OSDP_LED_STATE
  led[OSDP_MAX_LED];
  int role;
  char text[1024];

  // OSDP protocol context
  char last_command_sent;
  char last_nak_error;
  char last_response_received;
  char next_response;
  int next_sequence;

  // secure channel
  unsigned char current_received_mac[OSDP_KEY_OCTETS];
  unsigned char current_scbk[OSDP_KEY_OCTETS];
  unsigned char rnd_a[8];
  unsigned char rnd_b[8];
  unsigned char s_enc[16];
  unsigned char s_mac1[16];
  unsigned char s_mac2[16];
  int secure_channel_use[4];  // see OO_SCU_... use

  char new_address;
  char test_in_progress[32];
  int profile;
  int timer_count;
  OSDP_TIMER
  timer[2];
  int last_errno;
  int tamper;
  int power_report;
  int card_data_valid;
  int creds_a_avail;
  int bytes_received;
  int bytes_sent;
  int packets_received;
  int cp_polls;
  int pd_acks;
  int sent_naks;
  int checksum_errs;
  char init_command[1024];
  int cparm;
  int cparm_v;
  unsigned char vendor_code[3];
  unsigned char model;
  unsigned char version;
  unsigned char serial_number[4];
  unsigned char fw_version[3];  // major minor build

  int max_message;  // max message from PD, if set

  // for multipart messages, in or out
  char *mmsgbuf;
  unsigned short int total_len;

  // for assembling multipart message.  assumes one context structure
  // per PD we talk to
  unsigned short int next_in;

  // for transmitting multi-part
  unsigned short int next_out;
  int authenticated;
  char command_path[1024];
  int cmd_hist_counter;
  char init_parameters_path[1024];

  OSDP_OUT_STATE
  out[16];

  int last_raw_read_bits;
  int slow_timer;
  char last_raw_read_data[1024];
  char last_keyboard_data[8];
} OSDP_CONTEXT;

#define OO_SCU_ENAB (0)
#define OO_SCU_INST (1)
#define OO_SCU_POL (2)
#define OO_SCU_KEYED (3)

#define OO_SCS_USE_DISABLED (0)
#define OO_SCS_USE_ENABLED (1)
#define OO_SCS_OPERATIONAL (2)
// remember 128+SCSstate also goes here

#define OO_SECURE_NORMAL (0)
#define OO_SECURE_INSTALL (1)

#define OO_SECPOL_STRICT (0)
#define OO_SECPOL_RELAXED (1)

#define OO_SECPOL_ZEROIZED (0)
#define OO_SECPOL_KEYLOADED (1)

#define OSDP_ROLE_CP (0)
#define OSDP_ROLE_PD (1)
#define OSDP_ROLE_MONITOR (2)

#define OSDP_CHECKSUM (0)
#define OSDP_CRC (1)

typedef struct osdp_parameters {
  // card response

  int bits;
  unsigned char value[1024];
  int value_len;

  // PD device address
  int addr;

  //  Serial device filename
  char filename[1024];

  // poll delay
  int poll;
} OSDP_PARAMETERS;
#define PARAMETER_NONE (0)
#define PARAMETER_PARAMS (1)

#define PARMV_NONE (0)
#define PARMV_ADDR (21)
#define PARMV_CARD_BITS (11)
#define PARMV_CARD_VALUE (12)
#define PARMV_CP_POLL (31)
#define PARMV_FILENAME (22)
#define PARMV_ROLE (13)

// Log format: 0x80 addde if notimestamp; low 4 bits are a type field.
#define OSDP_LOG_NOTIMESTAMP (0x80)
#define OSDP_LOG_STRING (0x00)
#define OSDP_LOG_STRING_CP (3)
#define OSDP_LOG_STRING_PD (4)

#define OOSDP_MSG_PD_IDENT (1)
#define OOSDP_MSG_KEYPAD (2)
#define OOSDP_MSG_PKT_STATS (3)
#define OOSDP_MSG_PD_CAPAS (4)
#define OOSDP_MSG_OUT_STATUS (5)
#define OOSDP_MSG_CCRYPT (6)

#define OSDP_BUF_MAX (8192)
typedef struct osdp_buffer {
  unsigned char buf[OSDP_BUF_MAX];
  int next;
  int overflow;
} OSDP_BUFFER;

typedef struct osdp_command {
  int command;
  unsigned char details[128];
} OSDP_COMMAND;

typedef struct osdp_param { char device[1024]; } OSDP_PARAM;

typedef struct osdp_hdr {
  unsigned char som;
  unsigned char addr;
  unsigned char len_lsb;
  unsigned char len_msb;
  unsigned char ctrl;
  unsigned char command;
} OSDP_HDR;

typedef struct osdp_rdr_led_ctl {
  unsigned char reader;
  unsigned char led;
  unsigned char temp_control;
  unsigned char temp_on;
  unsigned char temp_off;
  unsigned char temp_on_color;
  unsigned char temp_off_color;
  unsigned char temp_timer_lsb;
  unsigned char temp_timer_msb;
  unsigned char perm_control;
  unsigned char perm_on_time;
  unsigned char perm_off_time;
  unsigned char perm_on_color;
  unsigned char perm_off_color;
} OSDP_RDR_LED_CTL;

#define OSDP_LED_TEMP_NOP (0)
#define OSDP_LED_TEMP_CANCEL (1)
#define OSDP_LED_TEMP_SET (2)

#define OSDP_LED_NOP (0)
#define OSDP_LED_SET (1)
#define OSDP_LEDCOLOR_BLACK (0)
#define OSDP_LEDCOLOR_RED (1)
#define OSDP_LEDCOLOR_GREEN (2)
#define OSDP_LEDCOLOR_AMBER (3)
#define OSDP_LEDCOLOR_BLUE (4)

typedef struct osdp_text_hdr {
  unsigned char reader;
  unsigned char tc;
  unsigned char tsec;
  unsigned char row;
  unsigned char col;
  unsigned char length;
  char text[1024];
} OSDP_TEXT_HEADER;

typedef struct osdp_msg {
  unsigned int lth;
  unsigned char *ptr;
  unsigned char msg_cmd;
  unsigned char *cmd_payload;
  unsigned char *data_payload;
  int data_length;
  unsigned char *crc_check;
  int check_size;
} OSDP_MSG;

typedef struct osdp_multi_getpiv {
  unsigned char oui[3];
  unsigned short int total;
  unsigned short int offset;
  unsigned short int length;
  unsigned short int cmd;
  unsigned char container_tag[8];
  unsigned char data_tag[8];
} ZZZOSDP_MULTI_GETPIV;

typedef struct osdp_multi_hdr {
  unsigned char VendorCode[3];
  unsigned short int Reply_ID;
  unsigned short int MpdSizeTotal;
  unsigned short int MpdOffset;
  unsigned short int MpdFragmentSize;
} OSDP_MULTI_HDR;

// open-osdp Reply_ID values...
#define MFGREP_OOSDP_CAKCert (0x01)

#define ST_OK (0)
#define ST_SELECT_ERROR (1)
#define ST_BAD_FCNTL (2)
#define ST_MSG_TOO_SHORT (3)
#define ST_MSG_BAD_SOM (4)
#define ST_PARSE_UNKNOWN_CMD (5)
#define ST_BAD_AES_1 (6)
#define ST_BAD_MENU (7)
#define ST_CMD_UNKNOWN (8)
#define ST_EXIT (9)
#define ST_TIMEOUT (10)
#define ST_SERIAL_OPEN_ERR (11)
#define ST_SERIAL_SET_ERR (12)
#define ST_SERIAL_READ_ERR (13)
#define ST_SERIAL_OVERFLOW (14)
#define ST_SERIAL_IN (15)
#define ST_BAD_CRC (16)
#define ST_MSG_UNKNOWN (17)
#define ST_LOG_OPEN_ERR (18)
#define ST_BAD_CHECKSUM (19)
#define ST_NOT_MY_ADDR (20)
#define ST_MONITOR_ONLY (21)
#define ST_ERR_INIT_CREDS (22)
#define ST_BAD_MULTIPART_BUF (23)
#define ST_MMSG_SEQ_ERR (24)
#define ST_MMSG_OUT_OF_ORDER (25)
#define ST_MMSG_LAST_FRAG_TOO_BIG (26)
#define ST_NET_INPUT_READY (27)
#define ST_CMD_ERROR (28)
#define ST_CMD_INVALID (29)
#define ST_CMD_OVERFLOW (30)

#define ST_OSDP_TLS_CLOSED (31)
#define ST_OSDP_TLS_ERROR (32)
#define ST_OSDP_TLS_HANDSHAKE (33)
#define ST_OSDP_TLS_NOCERT (34)
#define ST_OSDP_TLS_ERR (35)
#define ST_OSDP_TLS_BIND_ERR (36)
#define ST_OSDP_TLS_SOCKET_ERR (37)
#define ST_OSDP_TLS_LISTEN_ERR (38)
#define ST_OSDP_TLS_NONBLOCK (39)
#define ST_OSDP_TLS_CLIENT_HANDSHAKE (40)
#define ST_OSDP_TCP_NONBLOCK (41)

#define ST_CMD_UNDERFLOW (41)
#define ST_CMD_PATH (42)
#define ST_PARSE_ERROR (43)
#define ST_OUT_TOO_MANY (44)
#define ST_OUT_UNKNOWN (45)
#define ST_OSDP_NET_ERROR (46)
#define ST_OSDP_NET_CLOSED (47)
#define ST_OSDP_BAD_SEQUENCE (48)
#define ST_OSDP_BAD_SEQUENCE_BUSY (49)
#define ST_OSDP_BAD_COMMAND_REPLY (50)
#define ST_OSDP_CMDREP_FOUND (51)
#define ST_OSDP_CHLNG_DECRYPT (52)
#define ST_OSDP_SC_WRONG_STATE (53)
#define ST_OSDP_SCRYPT_DECRYPT (54)
#define ST_OSDP_NO_KEY_LOADED (55)
#define ST_OSDP_BAD_KEY_SELECT (56)
#define ST_OSDP_NO_SCBK (57)
#define ST_OSDP_UNKNOWN_KEY (58)

int m_version_minor;
int m_build;
int m_check;
int m_dump;

int action_osdp_CCRYPT(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_MFG(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_OUT(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_POLL(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_RAW(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_RMAC_I(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_RSTAT(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_SCRYPT(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int action_osdp_TEXT(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int background(OSDP_CONTEXT *context);
unsigned char checksum(unsigned char *msg, int length);
int calc_parity(unsigned short value, int length, int sense);
int display_menu(int menu);
void display_sim_reader(OSDP_CONTEXT *ctx, char *str);
int fasc_n_75_to_string(char *s, long int *sample_1);
int next_sequence(OSDP_CONTEXT *ctx);
int initialize_osdp(OSDP_CONTEXT *ctx);
int init_serial(OSDP_CONTEXT *context, char *device);
int oosdp_log(OSDP_CONTEXT *context, int logtype, int level, char *message);
int oosdp_make_message(int msgtype, char *logmsg, void *aux);
int osdp_build_message(unsigned char *buf, int *updated_length,
                       unsigned char command, int dest_addr, int sequence,
                       int data_length, unsigned char *data, int security);
int osdp_build_secure_message(unsigned char *buf, int *updated_length,
                              unsigned char command, int dest_addr,
                              int sequence, int data_length,
                              unsigned char *data, int sec_blk_type,
                              int sec_blk_lth, unsigned char *sec_blk);
void osdp_create_client_cryptogram(OSDP_CONTEXT *context,
                                   OSDP_SC_CCRYPT *ccrypt_response);
void osdp_create_keys(OSDP_CONTEXT *ctx);
int osdp_get_key_slot(OSDP_CONTEXT *ctx, OSDP_MSG *msg, int *key_slot);
int osdp_parse_message(OSDP_CONTEXT *context, int role, OSDP_MSG *m,
                       OSDP_HDR *h);
void osdp_reset_background_timer(OSDP_CONTEXT *ctx);
void osdp_reset_secure_channel(OSDP_CONTEXT *ctx);
int osdp_setup_scbk(OSDP_CONTEXT *ctx, OSDP_MSG *msg);
int osdp_timeout(OSDP_CONTEXT *ctx, struct timespec *last_time_check_ex);
void preserve_current_command(void);
int process_command(int command, OSDP_CONTEXT *context, char *details);
int process_current_command(void);
int process_osdp_input(OSDP_BUFFER *osdpbuf);
int monitor_osdp_message(OSDP_CONTEXT *context, OSDP_MSG *msg);
int process_osdp_message(OSDP_CONTEXT *context, OSDP_MSG *msg);
int read_command(OSDP_CONTEXT *ctx, OSDP_COMMAND *cmd);
int read_config(OSDP_CONTEXT *context);
int send_bio_read_template(OSDP_CONTEXT *ctx);
int send_comset(OSDP_CONTEXT *ctx, unsigned char pd_address,
                unsigned char new_addr, char *speed_string);
int send_message(OSDP_CONTEXT *context, int command, int dest_addr,
                 int *current_length, int data_length, unsigned char *data);
int send_osdp_data(OSDP_CONTEXT *ctx, unsigned char *buf, int lth);
int send_secure_message(OSDP_CONTEXT *context, int command, int dest_addr,
                        int *current_length, int data_length,
                        unsigned char *data, int sec_blk_type, int sec_blk_lth,
                        unsigned char *sec_blk);
void signal_callback_handler(int signum);
unsigned short int fCrcBlk(unsigned char *pData, unsigned short int nLength);
int write_status(OSDP_CONTEXT *ctx);
