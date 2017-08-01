/*
  oosdp-secure - open osdp secure channel routines

  (C)Copyright 2014-2017 Smithee,Spelvin,Agnew & Plinge, Inc.

  Support provided by the Security Industry Association
  http://www.securityindustry.org

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

#include <aes.h>

#include <open-osdp.h>
#include <osdp-tls.h>

extern OSDP_CONTEXT context;

void osdp_create_keys(OSDP_CONTEXT* ctx)

{ /* osdp_create_keys */

  unsigned char cleartext[OSDP_KEY_OCTETS];
  int i;
  unsigned char iv[OSDP_KEY_OCTETS];
  char tlogmsg[1024];
  char tlogmsg2[1024];

  fflush(ctx->log);
  memset(iv, 0, sizeof(iv));

  // S-ENC
  memset(ctx->s_enc, 0, sizeof(ctx->s_enc));
  memset(cleartext, 0, sizeof(cleartext));
  cleartext[0] = 1;
  cleartext[1] = 0x82;
  memcpy(cleartext + 2, ctx->rnd_a, 6);
  if (ctx->verbosity > 8) {
    tlogmsg[0] = 0;
    strcat(tlogmsg, "current_scbk calculating s_enc: ");
    for (i = 0; i < OSDP_KEY_OCTETS; i++) {
      sprintf(tlogmsg2, "%02x", ctx->current_scbk[i]);
      strcat(tlogmsg, tlogmsg2);
    };
    fprintf(stderr, "%s\n", tlogmsg);
    fprintf(ctx->log, "%s\n", tlogmsg);
    fflush(ctx->log);
    tlogmsg[0] = 0;
    strcat(tlogmsg, "cleartext calculating s_enc: ");
    for (i = 0; i < OSDP_KEY_OCTETS; i++) {
      sprintf(tlogmsg2, "%02x", cleartext[i]);
      strcat(tlogmsg, tlogmsg2);
    };
    fprintf(stderr, "%s\n", tlogmsg);
    fprintf(ctx->log, "%s\n", tlogmsg);
    fflush(ctx->log);
  };
  AES128_CBC_encrypt_buffer(ctx->s_enc, cleartext, OSDP_KEY_OCTETS,
                            ctx->current_scbk, iv);
  if (ctx->verbosity > 8) {
    int i;
    fprintf(stderr, "s_enc in osdp_create_keys: ");
    for (i = 0; i < OSDP_KEY_OCTETS; i++)
      fprintf(stderr, "%02x", ctx->s_enc[i]);
    fprintf(stderr, "\n");
  };

  // S-MAC-1
  memset(ctx->s_mac1, 0, sizeof(ctx->s_mac1));
  cleartext[0] = 1;
  cleartext[1] = 1;
  memcpy(cleartext + 2, ctx->rnd_a, 6);
  AES128_CBC_encrypt_buffer(ctx->s_mac1, cleartext, OSDP_KEY_OCTETS,
                            ctx->current_scbk, iv);

  // S-MAC-2
  memset(ctx->s_mac1, 0, sizeof(ctx->s_mac1));
  cleartext[0] = 1;
  cleartext[1] = 2;
  memcpy(cleartext + 2, ctx->rnd_a, 6);
  AES128_CBC_encrypt_buffer(ctx->s_mac2, cleartext, OSDP_KEY_OCTETS,
                            ctx->current_scbk, iv);

  return;

} /* osdp_create_keys */

int osdp_build_secure_message(unsigned char* buf, int* updated_length,
                              unsigned char command, int dest_addr,
                              int sequence, int data_length,
                              unsigned char* data, int sec_block_type,
                              int sec_block_length, unsigned char* sec_blk)

{ /* osdp_build_secure_mesage */

  int check_size;
  unsigned char* cmd_ptr;
  int new_length;
  unsigned char* next_data;
  OSDP_HDR
  *p;
  int status;
  int whole_msg_lth;

  status = ST_OK;
  if (m_check EQUALS OSDP_CHECKSUM)
    check_size = 1;
  else
    check_size = 2;
  new_length = *updated_length;

  p = (OSDP_HDR*)buf;
  p->som = C_SOM;
  new_length++;

  // addr
  p->addr = dest_addr;
  new_length++;

  // length

  /*
  length goes in before CRC calc.
  length is 5 (fields to CTRL) + [if no sec] 1 for CMND + data
*/
  whole_msg_lth = 5;
  whole_msg_lth = whole_msg_lth + 1;  // CMND
  whole_msg_lth = whole_msg_lth + data_length;
  whole_msg_lth = whole_msg_lth + sec_block_length + 2;  // contents+hdr
  whole_msg_lth = whole_msg_lth + check_size;            // including CRC

  p->len_lsb = 0x00ff & whole_msg_lth;
  new_length++;
  p->len_msb = (0xff00 & whole_msg_lth) >> 8;
  new_length++;

  // control
  p->ctrl = 0;
  p->ctrl = p->ctrl | (0x3 & sequence);
  if (context.verbosity > 4)
    fprintf(stderr, "build msg: seq %d added ctl now %02x m_check %d\n",
            sequence, p->ctrl, m_check);

  // set CRC depending on current value of global parameter
  if (m_check EQUALS OSDP_CRC) p->ctrl = p->ctrl | 0x04;

  new_length++;

  // secure is bit 3 (mask 0x08)
  {
    p->ctrl = p->ctrl | 0x08;
    cmd_ptr =
        buf + 5;  // STUB pretend security block is 3 bytes len len 1 payload
  };

  // fill in secure data
  {
    unsigned char* sp;
    sp = buf + 5;
    *sp = sec_block_length + 2;
    sp++;
    *sp = sec_block_type;
    sp++;
    memcpy(sp, sec_blk, sec_block_length);
    sp = sp + sec_block_length;
    cmd_ptr = sp;
    printf("bef sec block to new length %d.\n", new_length);
    new_length = new_length + 2 + sec_block_length;  // account for lth/typ
  };

  *cmd_ptr = command;
  new_length++;
  next_data = 1 + cmd_ptr;

  if (data_length > 0) {
    int i;
    unsigned char* sptr;
    sptr = cmd_ptr + 1;
    if (context.verbosity > 3)
      fprintf(stderr, "orig (s) next_data %lx\n", (unsigned long)next_data);
    for (i = 0; i < data_length; i++) {
      *(sptr + i) = *(i + data);
      new_length++;
      next_data++;  // where crc goes (after data)
    };
  };

  // crc
  if (m_check EQUALS OSDP_CRC) {
    unsigned short int parsed_crc;
    unsigned char* crc_check;
    crc_check = next_data;
    parsed_crc = fCrcBlk(buf, new_length);

    // low order byte first
    *(crc_check + 1) = (0xff00 & parsed_crc) >> 8;
    *(crc_check) = (0x00ff & parsed_crc);
    new_length++;
    new_length++;
  } else {
    unsigned char cksum;
    unsigned char* pchecksum;

    pchecksum = next_data;
    cksum = checksum(buf, new_length);
    *pchecksum = cksum;
    new_length++;
  };

  *updated_length = new_length;
  return (status);

} /* osdp_build_message */

void osdp_create_client_cryptogram(OSDP_CONTEXT* ctx,
                                   OSDP_SC_CCRYPT* ccrypt_response)

{ /* osdp_create_client_cryptogram */

  unsigned char iv[16];
  unsigned char message[16];

  memset(iv, 0, sizeof(iv));
  memcpy(message, ctx->rnd_a, 8);
  memcpy(message + 8, ctx->rnd_b, 8);
  if (ctx->verbosity > 8) {
    int i;
    fprintf(stderr, "s_enc in osdp_create_client_cryptogram: ");
    for (i = 0; i < OSDP_KEY_OCTETS; i++)
      fprintf(stderr, "%02x", ctx->s_enc[i]);
    fprintf(stderr, "\n");
  };
  AES128_CBC_encrypt_buffer(ccrypt_response->cryptogram, message,
                            sizeof(message), ctx->s_enc, iv);

  return;

} /* osdp_create_client_cryptogram */

int osdp_get_key_slot(OSDP_CONTEXT* ctx, OSDP_MSG* msg, int* returned_key_slot)

{ /* osdp_get_key_slot */

  int key_slot;
  OSDP_SECURE_MESSAGE
  *s_msg;
  int status;

  status = ST_OK;
  key_slot = OSDP_KEY_SCBK;  // meaning our key
  s_msg = (OSDP_SECURE_MESSAGE*)(msg->ptr);
  if (s_msg->sec_blk_data EQUALS OSDP_KEY_SCBK_D) {
    key_slot = OSDP_KEY_SCBK_D;
  } else {
    if (s_msg->sec_blk_data EQUALS OSDP_KEY_SCBK) {
      key_slot = OSDP_KEY_SCBK;
      if (ctx->secure_channel_use[OO_SCU_KEYED] != OO_SECPOL_KEYLOADED)
        status = ST_OSDP_NO_KEY_LOADED;
    } else {
      status = ST_OSDP_BAD_KEY_SELECT;
    };
  };

  *returned_key_slot = key_slot;
  return (status);

} /* osdp_get_key_slot */

void osdp_reset_secure_channel(OSDP_CONTEXT* ctx)

{ /* osdp_reset_secure_channel */

  // secure channel processing is being reset.  set things
  // back to the beginning.

  // refresh rnd.a
  memcpy(ctx->rnd_a, "12345678", 8);

  // refresh rnd.b
  memcpy(ctx->rnd_b, "abcdefgh", 8);

  memset(ctx->current_received_mac, 0, sizeof(ctx->current_received_mac));
  ctx->secure_channel_use[OO_SCU_ENAB] = OO_SCS_USE_DISABLED;
  if (ctx->enable_secure_channel > 0)
    ctx->secure_channel_use[OO_SCU_ENAB] = OO_SCS_USE_ENABLED;
  fprintf(ctx->log, "Resetting Secure Channel\n");

} /* osdp_reset_secure_channel */

int osdp_setup_scbk(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* osdp_setup_scbk */

  OSDP_SECURE_MESSAGE
  *secure_message;
  int status;

  status = ST_OK;
  if (msg != NULL) {
    secure_message = (OSDP_SECURE_MESSAGE*)(msg->ptr);
    if (secure_message->sec_blk_data EQUALS OSDP_KEY_SCBK_D) {
      memcpy(ctx->current_scbk, OSDP_SCBK_DEFAULT, sizeof(ctx->current_scbk));
      ctx->secure_channel_use[OO_SCU_KEYED] = OO_SECPOL_KEYLOADED;
    }
    if (secure_message->sec_blk_data EQUALS OSDP_KEY_SCBK)
      if (ctx->secure_channel_use[OO_SCU_KEYED] != OO_SECPOL_KEYLOADED)
        status = ST_OSDP_NO_SCBK;
  } else {
    if (ctx->enable_secure_channel EQUALS 2) {
      memcpy(ctx->current_scbk, OSDP_SCBK_DEFAULT, sizeof(ctx->current_scbk));
      ctx->secure_channel_use[OO_SCU_KEYED] = OO_SECPOL_KEYLOADED;
    }
    if (ctx->enable_secure_channel EQUALS 1)
      if (ctx->secure_channel_use[OO_SCU_KEYED] != OO_SECPOL_KEYLOADED)
        status = ST_OSDP_NO_SCBK;
  };
  return (status);

} /* osdp_setup_scbk */

/*
  send_secure_message - send an OSDP "security" message

  assumes command is a valid value.
*/

int send_secure_message(OSDP_CONTEXT* ctx, int command, int dest_addr,
                        int* current_length, int data_length,
                        unsigned char* data, int sec_block_type,
                        int sec_block_length, unsigned char* sec_blk)

{ /* send_secure_message */

  unsigned char buf[2];
  char tlogmsg[1024];
  int status;
  unsigned char test_blk[1024];
  int true_dest;

  status = ST_OK;
  fprintf(ctx->log, "Top of send_secure_message (%02x):\n", command);
  fflush(ctx->log);
  true_dest = dest_addr;
  *current_length = 0;

  // so we remember our state
  ctx->secure_channel_use[OO_SCU_ENAB] = 128 + sec_block_type;

  status = osdp_build_secure_message(
      test_blk,        // message itself
      current_length,  // returned message length in bytes
      command, true_dest, next_sequence(ctx),
      data_length,                                       // data length to use
      data, sec_block_type, sec_block_length, sec_blk);  // security values
  if (status EQUALS ST_OK) {
    if ((ctx->verbosity > 3) || (command != OSDP_ACK))
      if (m_dump) {
        int i;

        fprintf(ctx->log, "Sending(secure) lth %d.=", *current_length);
        for (i = 0; i < *current_length; i++)
          fprintf(ctx->log, " %02x", test_blk[i]);
        fprintf(ctx->log, "\n");
        fflush(ctx->log);
      };
    buf[0] = 0xff;
    // send start-of-message marker (0xff)
    send_osdp_data(ctx, &(buf[0]), 1);

    if (sec_block_type EQUALS OSDP_SEC_SCS_11)
      ctx->secure_channel_use[0] = 128 + OSDP_SEC_SCS_11;

    if (ctx->verbosity > 4)
      fprintf(ctx->log, "send_secure_message: sending(secure) %d\n",
              *current_length);

    send_osdp_data(ctx, test_blk, *current_length);
  };
  {
    OSDP_MSG
    m;
    int parse_role;
    OSDP_HDR
    returned_hdr;
    int status_monitor;

    memset(&m, 0, sizeof(m));

    m.ptr = test_blk;  // marshalled outbound message
    m.lth = *current_length;

    // parse the message, mostly for display.  role to parse_... is the OTHER
    // guy
    parse_role = OSDP_ROLE_CP;
    if (ctx->role EQUALS OSDP_ROLE_CP) parse_role = OSDP_ROLE_PD;
    status_monitor = osdp_parse_message(ctx, parse_role, &m, &returned_hdr);
    if (ctx->verbosity > 8)
      if (status_monitor != ST_OK) {
        sprintf(tlogmsg, "parse_message for monitoring returned %d.\n",
                status_monitor);
        status = oosdp_log(ctx, OSDP_LOG_STRING_CP, 1, tlogmsg);
      };
    (void)monitor_osdp_message(ctx, &m);
    fflush(ctx->log);
  };
  return (status);

} /* send_secure_message */
