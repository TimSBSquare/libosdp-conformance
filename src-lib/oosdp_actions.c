/*
  oosdp-actions - open osdp action routines

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

#include <aes.h>

#include <open-osdp.h>
#include <osdp-tls.h>
#include <osdp_conformance.h>

extern OSDP_INTEROP_ASSESSMENT osdp_conformance;
extern OSDP_PARAMETERS p_card;
char tlogmsg[1024];

int action_osdp_CCRYPT(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_CCRYPT */

  OSDP_SC_CCRYPT
  *ccrypt_payload;
  unsigned char* client_cryptogram;
  int current_length;
  unsigned char iv[16];
  unsigned char message[16];
  unsigned char sec_blk[1];
  OSDP_SECURE_MESSAGE
  *secure_message;
  unsigned char server_cryptogram[16];
  int status;

  status = ST_OK;
  memset(iv, 0, sizeof(iv));

  // check for proper state

  if (ctx->secure_channel_use[OO_SCU_ENAB] EQUALS 128 + OSDP_SEC_SCS_11) {
    secure_message = (OSDP_SECURE_MESSAGE*)(msg->ptr);
    if (ctx->enable_secure_channel EQUALS 2)
      if (secure_message->sec_blk_data != OSDP_KEY_SCBK_D)
        status = ST_OSDP_UNKNOWN_KEY;

    if (ctx->enable_secure_channel EQUALS 1)
      if (secure_message->sec_blk_data != OSDP_KEY_SCBK)
        status = ST_OSDP_UNKNOWN_KEY;

    if (status EQUALS ST_OK) {
      ccrypt_payload = (OSDP_SC_CCRYPT*)(msg->data_payload);
      client_cryptogram = ccrypt_payload->cryptogram;
      // decrypt the client cryptogram (validate header, RND.A, collect RND.B)
      if (ctx->verbosity > 8) {
        int i;
        fprintf(stderr, "s_enc: ");
        for (i = 0; i < OSDP_KEY_OCTETS; i++)
          fprintf(stderr, "%02x", ctx->s_enc[i]);
        fprintf(stderr, "\n");
      };
      AES128_CBC_decrypt_buffer(message, client_cryptogram, sizeof(message),
                                ctx->s_enc, iv);

      if (0 != memcmp(message, ctx->rnd_a, sizeof(ctx->rnd_a)))
        status = ST_OSDP_CHLNG_DECRYPT;
    };
    if (status EQUALS ST_OK) {
      // client crytogram looks ok, save RND.B

      memcpy(ctx->rnd_b, message + sizeof(ctx->rnd_a), sizeof(ctx->rnd_b));

      memcpy(message, ctx->rnd_b, sizeof(ctx->rnd_b));
      memcpy(message + sizeof(ctx->rnd_b), ctx->rnd_a, sizeof(ctx->rnd_a));
      AES128_CBC_encrypt_buffer(server_cryptogram, message,
                                sizeof(server_cryptogram), ctx->s_enc, iv);

      if (ctx->enable_secure_channel EQUALS 1) sec_blk[0] = OSDP_KEY_SCBK;
      if (ctx->enable_secure_channel EQUALS 2) sec_blk[0] = OSDP_KEY_SCBK_D;

      status =
          send_secure_message(ctx, OSDP_SCRYPT, p_card.addr, &current_length,
                              sizeof(server_cryptogram), server_cryptogram,
                              OSDP_SEC_SCS_13, sizeof(sec_blk), sec_blk);
    };
  } else
    status = ST_OSDP_SC_WRONG_STATE;
  return (status);

} /* action_osdp_CCRYPT */

int action_osdp_MFG(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_MFG */

  int status;
#if 0
  unsigned char
    buffer [1024];
  int
    bufsize;
  extern unsigned char
    creds_buffer_a [];
  extern int
    creds_buffer_a_lth;
  extern int
    creds_buffer_a_next;
  extern int
    zzcreds_buffer_a_remaining;
  int
    current_length;
  OSDP_MULTI_HDR
    mmsg;
  int
    to_send;

  status = ST_OK;
fprintf (stderr, "osdp_MFG action stub\n");
  /*
    set up to send the header and what's in creds buffer a
  */
  memset (&mmsg, 0, sizeof (mmsg));
  mmsg.oui [0] = 0x08;
  mmsg.oui [1] = 0x00;
  mmsg.oui [2] = 0x1b;
  mmsg.total = creds_buffer_a_lth;
  mmsg.cmd = 0;
  bufsize = sizeof (mmsg);
  creds_buffer_a_next = 0;
  mmsg.offset = creds_buffer_a_next;
  if (creds_buffer_a_lth > 128)
  {
    to_send = 128;
    creds_buffer_a_remaining = creds_buffer_a_lth - 128;
  }
  else
  {
    to_send = creds_buffer_a_lth;
    creds_buffer_a_remaining = 0;
  };
  mmsg.length = to_send; 

  // filled in all of mmsg now copy it to buffer

  memcpy (buffer, &mmsg, sizeof (mmsg));

  // actual data goes after header in buffer.
  memcpy (buffer+bufsize, creds_buffer_a+creds_buffer_a_next, to_send);

  current_length = 0;
  status = send_message (ctx, OSDP_MFGREP, p_card.addr,
    &current_length,
    bufsize+to_send, buffer);
#endif
  status = -1;
  return (status);

} /* action_osdp_MFG */

int action_osdp_OUT(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_OUT */

  unsigned char buffer[1024];
  int current_length;
  int done;
  OSDP_OUT_MSG
  *outmsg;
  int status;
  int to_send;

  status = ST_OK;
  osdp_conformance.cmd_out.test_status = OCONFORM_EXERCISED;
  fprintf(stderr, "data_length in OSDP_OUT: %d\n", msg->data_length);
#if 0
// if too many for me (my MAX) then error and NAK?
// set 'timer' to msb*256+lsb
#define OSDP_OUT_NOP (0)
#define OSDP_OUT_OFF_PERM_ABORT (1)
#define OSDP_OUT_OFF_PERM_TIMEOUT (3)
#define OSDP_OUT_ON_PERM_TIMEOUT (4)
#define OSDP_OUT_ON_TEMP_TIMEOUT (5)
#define OSDP_OUT_OFF_TEMP_TIMEOUT (6)
#endif
  done = 0;
  if (status != ST_OK) done = 1;
  while (!done) {
    outmsg = (OSDP_OUT_MSG*)(msg->data_payload);
    sprintf(tlogmsg, "  Out: Line %02x Ctl %02x LSB %02x MSB %02x",
            outmsg->output_number, outmsg->control_code, outmsg->timer_lsb,
            outmsg->timer_msb);
    fprintf(ctx->log, "%s\n", tlogmsg);
    if ((outmsg->output_number < 0) || (outmsg->output_number > OSDP_MAX_OUT))
      status = ST_OUT_TOO_MANY;
    if (status EQUALS ST_OK) {
      switch (outmsg->control_code) {
        case OSDP_OUT_ON_PERM_ABORT:
          ctx->out[outmsg->output_number].current = 1;
          ctx->out[outmsg->output_number].timer = 0;
          break;
        default:
          status = ST_OUT_UNKNOWN;
          break;
      };
    } else
      done = 1;

    done = 1;  // just first one for now.
  };

  // return osdp_OSTATR with now-current output state
  {
    int j;
    unsigned char out_status[OSDP_MAX_OUT];

    for (j = 0; j < OSDP_MAX_OUT; j++) {
      out_status[j] = ctx->out[j].current;
    };

    to_send = OSDP_MAX_OUT;
    memcpy(buffer, out_status, OSDP_MAX_OUT);
    current_length = 0;
    status = send_message(ctx, OSDP_OSTATR, p_card.addr, &current_length,
                          to_send, buffer);
  };
  status = ST_OK;
  return (status);

} /* action_osdp_OUT */

int action_osdp_POLL(OSDP_CONTEXT* ctx, OSDP_MSG* msg) { /* action_osdp_POLL */

  unsigned char buffer[1024];
  int bufsize;
  extern unsigned char creds_buffer_a[];
  extern int creds_buffer_a_lth;
  extern int creds_buffer_a_next;
  int current_length;
  int done;
  OSDP_MULTI_HDR
  mmsg;
  unsigned char osdp_lstat_response_data[2];
  unsigned char osdp_raw_data[4 + 1024];
  int raw_lth;
  int status;
  int to_send;

  status = ST_OK;
  done = 0;

  // i.e. we GOT a poll
  osdp_conformance.cmd_poll.test_status = OCONFORM_EXERCISED;

  /*
  poll response can be many things.  we do one and then return, which
  can cause some turn-the-crank artifacts.  may need multiple polls for
  expected behaviors to happen.

  if there was a power report return that.
*/
  if (!done) {
    if (ctx->next_response EQUALS OSDP_BUSY) {
      ctx->next_response = 0;
      done = 1;
      current_length = 0;
      status =
          send_message(ctx, OSDP_BUSY, p_card.addr, &current_length, 0, NULL);
      SET_PASS(ctx, "4-16-1");
      if (ctx->verbosity > 2) {
        sprintf(tlogmsg, "Responding with OSDP_BUSY");
        fprintf(ctx->log, "%s\n", tlogmsg);
      };
    };
  };
  if (ctx->tamper EQUALS 1) {
    done = 1;
    ctx->tamper = 1;
    osdp_lstat_response_data[0] = ctx->tamper;
    current_length = 0;
    status = send_message(ctx, OSDP_LSTATR, p_card.addr, &current_length,
                          sizeof(osdp_lstat_response_data),
                          osdp_lstat_response_data);
    osdp_conformance.resp_lstatr_tamper.test_status = OCONFORM_EXERCISED;
    if (ctx->verbosity > 2) {
      sprintf(tlogmsg, "Responding with OSDP_LSTATR (Tamper)");
      fprintf(ctx->log, "%s\n", tlogmsg);
    };
  };
  if (ctx->power_report EQUALS 1) {
    done = 1;
    // power change not yet reported
    ctx->power_report = 0;
    osdp_lstat_response_data[0] = ctx->tamper;
    osdp_lstat_response_data[1] = 1;  // report power failure
    current_length = 0;
    status = send_message(ctx, OSDP_LSTATR, p_card.addr, &current_length,
                          sizeof(osdp_lstat_response_data),
                          osdp_lstat_response_data);
    SET_PASS(ctx, "3-1-3");
    SET_PASS(ctx, "4-5-1");
    SET_PASS(ctx, "4-5-3");
    if (ctx->verbosity > 2) {
      sprintf(tlogmsg, "Responding with OSDP_LSTATR (Power)");
      fprintf(ctx->log, "%s\n", tlogmsg);
    };
  }
  if (!done) {
    /*
  the presence of card data to return is indicated because either the
  "raw" buffer or the "big" buffer is marked as non-empty when you get here.
*/
    /*
  if there's card data to return, do that.
  this is for the older "raw data" style.
*/
    if (ctx->card_data_valid > 0) {
      done = 1;
      // send data if it's there (value is number of bits)
      osdp_raw_data[0] = 0;  // one reader, reader 0
      osdp_raw_data[1] = 0;
      osdp_raw_data[2] = p_card.bits;
      osdp_raw_data[3] = 0;
      raw_lth = 4;
      memcpy(osdp_raw_data + 4, p_card.value, p_card.value_len);
      raw_lth = raw_lth + p_card.value_len;
      current_length = 0;
      status = send_message(ctx, OSDP_RAW, p_card.addr, &current_length,
                            raw_lth, osdp_raw_data);
      osdp_conformance.rep_raw.test_status = OCONFORM_EXERCISED;
      if (ctx->verbosity > 2) {
        sprintf(tlogmsg, "Responding with cardholder data (%d bits)",
                p_card.bits);
        fprintf(ctx->log, "%s\n", tlogmsg);
      };
      ctx->card_data_valid = 0;
    } else {
      /*
  this is the newer multi-part message for bigger credential responses,
  like a FICAM CHUID.
*/
      if (ctx->creds_a_avail > 0) {
        done = 1;

        // send another mfgrep message back and update things.

        memset(&mmsg, 0, sizeof(mmsg));
        if (creds_buffer_a_lth > 0xfffe) {
          status = ST_BAD_MULTIPART_BUF;
        } else {
          mmsg.VendorCode[0] = 0x08;
          mmsg.VendorCode[1] = 0x00;
          mmsg.VendorCode[2] = 0x1b;
          mmsg.Reply_ID = MFGREP_OOSDP_CAKCert;
          mmsg.MpdSizeTotal = creds_buffer_a_lth;
          mmsg.MpdOffset = creds_buffer_a_next;

          bufsize = sizeof(mmsg);  // used in send operation below
          if (ctx->creds_a_avail > 128) {
            to_send = 128;
            ctx->creds_a_avail = ctx->creds_a_avail - 128;
          } else {
            to_send = ctx->creds_a_avail;
            ctx->creds_a_avail = 0;
          };
          mmsg.MpdFragmentSize = to_send;

          // filled in all of mmsg now copy it to buffer

          memcpy(buffer, &mmsg, sizeof(mmsg));

          // actual data goes after header in buffer.
          memcpy(buffer + bufsize, creds_buffer_a + creds_buffer_a_next,
                 to_send);

          current_length = 0;
          status = send_message(ctx, OSDP_MFGREP, p_card.addr, &current_length,
                                bufsize + to_send, buffer);

          // and after all that move the pointer within the buffer for where
          // the next data is extracted from.

          creds_buffer_a_next = creds_buffer_a_next + to_send;
        };
      }
    };
  };
  if (!done) {
    /*
  if all else isn't interesting return a plain ack
*/
    current_length = 0;
    status = send_message(ctx, OSDP_ACK, p_card.addr, &current_length, 0, NULL);
    ctx->pd_acks++;
    osdp_conformance.cmd_poll.test_status = OCONFORM_EXERCISED;
    osdp_conformance.rep_ack.test_status = OCONFORM_EXERCISED;
    if (ctx->verbosity > 9) {
      sprintf(tlogmsg, "Responding with OSDP_ACK");
      fprintf(ctx->log, "%s\n", tlogmsg);
    };
  };

  // update status json
  if (status EQUALS ST_OK) status = write_status(ctx);

  return (status);

} /* action_osdp_MFG */

int action_osdp_RAW(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_RAW */

  int bits;
  int processed;
  unsigned char* raw_data;
  long int sample_1[4];
  int status;

  status = ST_OK;
  osdp_conformance.rep_raw.test_status = OCONFORM_EXERCISED;
  osdp_conformance.cmd_poll_raw.test_status = OCONFORM_EXERCISED;
  processed = 0;
  raw_data = msg->data_payload + 4;
  /*
  this processes an osdp_RAW.  byte 0=rdr, b1=format, 2-3 are length (2=lsb)
*/
  bits = *(msg->data_payload + 2) + ((*(msg->data_payload + 3)) << 8);
  ctx->last_raw_read_bits = bits;

  {
    int octets;
    octets = (bits + 7) / 8;
    if (octets > sizeof(ctx->last_raw_read_data))
      octets = sizeof(ctx->last_raw_read_data);
    memcpy(ctx->last_raw_read_data, raw_data, octets);
  };
  status = write_status(ctx);
  if (bits EQUALS 26) {
    fprintf(ctx->log, "CARD DATA (%d bits):", bits);
    fprintf(ctx->log, " %02x-%02x-%02x-%02x\n", *(raw_data + 0),
            *(raw_data + 1), *(raw_data + 2), *(raw_data + 3));
    processed = 1;
  };
  if (bits EQUALS 75) {
    int i;
    unsigned char* p;
    long int tmp1_l;
    p = (unsigned char*)&(sample_1[0]);
    for (i = 0; i < 10; i++) *(p + i) = *(raw_data + i);
    tmp1_l = *(long int*)(raw_data);
    sample_1[0] = tmp1_l;
    status = fasc_n_75_to_string(tlogmsg, sample_1);
    fprintf(ctx->log, "CARD DATA (%d bits):\n%s\n", bits, tlogmsg);
    processed = 1;
  };
  if (!processed) {
    unsigned d;
    int i;
    char hstr[1024];
    int octet_count;
    char tstr[32];

    hstr[0] = 0;
    fprintf(stderr, "Raw Unknown:");
    octet_count = (bits + 7) / 8;
    for (i = 0; i < octet_count; i++) {
      d = *(unsigned char*)(msg->data_payload + 4 + i);
      fprintf(stderr, " %02x", d);
      sprintf(tstr, " %02x", d);
      strcat(hstr, tstr);
    };
    fprintf(stderr, "\n");
    fprintf(ctx->log, "Unknown RAW CARD DATA (%d. bits) first byte %02x\n %s\n",
            bits, *(msg->data_payload + 4), hstr);
    processed = 1;
  };

  return (status);

} /* action_osdp_RAW */

int action_osdp_RMAC_I(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_RMAC_I */

  unsigned char iv[16];
  int status;

  status = ST_OK;
  memset(iv, 0, sizeof(iv));

  // check for proper state

  if (ctx->secure_channel_use[OO_SCU_ENAB] EQUALS 128 + OSDP_SEC_SCS_13) {
    memcpy(ctx->current_received_mac, msg->data_payload, msg->data_length);
    ctx->secure_channel_use[OO_SCU_ENAB] = OO_SCS_OPERATIONAL;
    fprintf(ctx->log, "*** SECURE CHANNEL OPERATIONAL***\n");
  } else {
    status = ST_OSDP_SC_WRONG_STATE;
    osdp_reset_secure_channel(ctx);
  };
  return (status);

} /* action_osdp_RMAC_I */

int action_osdp_RSTAT(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_RSTAT */

  int current_length;
  unsigned char osdp_rstat_response_data[1];
  int status;

  status = ST_OK;
  osdp_conformance.cmd_rstat.test_status = OCONFORM_EXERCISED;
  osdp_rstat_response_data[0] = 1;  // hard code to "not connected"
  current_length = 0;
  status =
      send_message(ctx, OSDP_RSTATR, p_card.addr, &current_length,
                   sizeof(osdp_rstat_response_data), osdp_rstat_response_data);
  if (ctx->verbosity > 2) {
    sprintf(tlogmsg, "Responding with OSDP_RSTATR (Ext Tamper)");
    fprintf(ctx->log, "%s\n", tlogmsg);
    tlogmsg[0] = 0;
  };

  return (status);

} /* action_osdp_RSTAT */

int action_osdp_SCRYPT(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_SCRYPT */

  int current_key_slot;
  int current_length;
  unsigned char iv[16];
  unsigned char message1[16];
  unsigned char message2[16];
  unsigned char message3[16];
  unsigned char sec_blk[1];
  unsigned char server_cryptogram[16];  // size of RND.B plus RND.A
  int status;

  status = ST_OK;
  fprintf(stderr, "TODO: osdp_SCRYPT\n");
  memset(iv, 0, sizeof(iv));

  // check for proper state

  if (ctx->secure_channel_use[OO_SCU_ENAB] EQUALS 128 + OSDP_SEC_SCS_12) {
    status = osdp_get_key_slot(ctx, msg, &current_key_slot);
    if (status EQUALS ST_OK) {
      AES128_CBC_decrypt_buffer(server_cryptogram, msg->data_payload,
                                msg->data_length, ctx->s_enc, iv);
      if ((0 != memcmp(server_cryptogram, ctx->rnd_b, sizeof(ctx->rnd_b))) ||
          (0 != memcmp(server_cryptogram + sizeof(ctx->rnd_b), ctx->rnd_a,
                       sizeof(ctx->rnd_a))))
        status = ST_OSDP_SCRYPT_DECRYPT;
    };
    if (status EQUALS ST_OK) {
      sec_blk[0] = 1;  // means server cryptogram was good

      memcpy(message1, msg->data_payload, sizeof(server_cryptogram));
      AES128_CBC_encrypt_buffer(message2, message1, sizeof(message1),
                                ctx->s_mac1, iv);
      AES128_CBC_encrypt_buffer(message3, message2, sizeof(message2),
                                ctx->s_mac2, iv);
      ctx->secure_channel_use[OO_SCU_ENAB] = 128 + OSDP_SEC_SCS_14;
      current_length = 0;
      status = send_secure_message(ctx, OSDP_RMAC_I, p_card.addr,
                                   &current_length, sizeof(message3), message3,
                                   OSDP_SEC_SCS_14, sizeof(sec_blk), sec_blk);
    };
  } else {
    status = ST_OSDP_SC_WRONG_STATE;
    osdp_reset_secure_channel(ctx);
  };
  return (status);

} /* action_osdp_SCRYPT */

int action_osdp_TEXT(OSDP_CONTEXT* ctx, OSDP_MSG* msg)

{ /* action_osdp_TEXT */

  int current_length;
  int status;
  int text_length;
  char tlogmsg[1024];

  status = ST_OK;
  osdp_conformance.cmd_text.test_status = OCONFORM_EXERCISED;

  memset(ctx->text, 0, sizeof(ctx->text));
  text_length = (unsigned char)*(msg->data_payload + 5);
  strncpy(ctx->text, (char*)(msg->data_payload + 6), text_length);

  fprintf(ctx->log, "Text:");
  fprintf(ctx->log, " Rdr %x tc %x tsec %x Row %x Col %x Lth %x\n",
          *(msg->data_payload + 0), *(msg->data_payload + 1),
          *(msg->data_payload + 2), *(msg->data_payload + 3),
          *(msg->data_payload + 4), *(msg->data_payload + 5));

  memset(tlogmsg, 0, sizeof(tlogmsg));
  strncpy(tlogmsg, (char*)(msg->data_payload + 6), text_length);
  fprintf(ctx->log, "Text: %s\n", tlogmsg);

  // we always ack the TEXT command regardless of param errors

  current_length = 0;
  status = send_message(ctx, OSDP_ACK, p_card.addr, &current_length, 0, NULL);
  ctx->pd_acks++;
  if (ctx->verbosity > 2) fprintf(ctx->log, "Responding with OSDP_ACK\n");

  return (status);

} /* action_osdp_TEXT */
