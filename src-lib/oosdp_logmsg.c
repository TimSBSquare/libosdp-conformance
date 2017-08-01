/*
  oosdp-logmsg - open osdp log message routines

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
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <open-osdp.h>
#include <osdp-tls.h>

extern OSDP_CONTEXT context;
extern OSDP_PARAMETERS p_card;

char* osdp_pdcap_function(int func) {
  static char funcname[1024];
  switch (func) {
    default:
      sprintf(funcname, "Unknown(0x%2x)", func);
      break;
    case 1:
      strcpy(funcname, "Contact Status Monitoring");
      break;
    case 2:
      strcpy(funcname, "Output Control");
      break;
    case 3:
      strcpy(funcname, "Card Data Format");
      break;
    case 4:
      strcpy(funcname, "Reader LED Control");
      break;
    case 5:
      strcpy(funcname, "Reader Audible Output");
      break;
    case 6:
      strcpy(funcname, "Reader Text Output");
      break;
    case 7:
      strcpy(funcname, "Time Keeping");
      break;
    case 8:
      strcpy(funcname, "Check Character Support");
      break;
    case 9:
      strcpy(funcname, "Communication Security");
      break;
    case 10:
      strcpy(funcname, "Receive Buffer Size");
      break;
    case 11:
      strcpy(funcname, "Max Multi-Part Size");
      break;
    case 12:
      strcpy(funcname, "Smart Card Support");
      break;
    case 13:
      strcpy(funcname, "Readers");
      break;
    case 14:
      strcpy(funcname, "Biometrics");
      break;
  };
  return (funcname);
};

int oosdp_make_message(int msgtype, char* logmsg, void* aux)

{
  char filename[1024];
  FILE* identf;
  OSDP_MSG
  *msg;
  OSDP_HDR
  *oh;
  char tlogmsg[1024];
  int status;

  status = ST_OK;
  switch (msgtype) {
    case OOSDP_MSG_CCRYPT:
      msg = (OSDP_MSG*)aux;
      sprintf(tlogmsg, "CCRYPT stuff...");
      break;

    case OOSDP_MSG_KEYPAD: {
      int i;
      int keycount;
      char tmpstr[1024];

      msg = (OSDP_MSG*)aux;
      keycount = *(msg->data_payload + 1);
      memset(tmpstr, 0, sizeof(tmpstr));
      memcpy(tmpstr, msg->data_payload + 2, *(msg->data_payload + 1));
      for (i = 0; i < keycount; i++) {
        if (tmpstr[i] EQUALS 0x7F) tmpstr[i] = '#';
        if (tmpstr[i] EQUALS 0x0D) tmpstr[i] = '*';
      };
      sprintf(tlogmsg, "Keypad Input Rdr %d, %d digits: %s",
              *(msg->data_payload + 0), keycount, tmpstr);
    }; break;

    case OOSDP_MSG_OUT_STATUS: {
      int i;
      unsigned char* out_status;
      char tmpstr[1024];

      msg = (OSDP_MSG*)aux;
      tlogmsg[0] = 0;
      out_status = msg->data_payload;
      for (i = 0; i < OSDP_MAX_OUT; i++) {
        sprintf(tmpstr, " Out-%02d = %d\n", i, out_status[i]);
        strcat(tlogmsg, tmpstr);
      };
    }; break;

    case OOSDP_MSG_PD_CAPAS: {
      int count;
      int i;
      OSDP_HDR
      *oh;
      char tstr[1024];
      int value;

      msg = (OSDP_MSG*)aux;
      oh = (OSDP_HDR*)(msg->ptr);
      count = oh->len_lsb + (oh->len_msb << 8);
      count = count - 8;
      sprintf(tstr, "PD Capabilities (%d)\n", count / 3);
      strcpy(tlogmsg, tstr);

      for (i = 0; i < count; i = i + 3) {
        switch (*(i + 0 + msg->data_payload)) {
          case 4: {
            int compliance;
            char tstr2[1024];
            compliance = *(i + 1 + msg->data_payload);
            strcpy(tstr2, "Compliance=?");
            if (compliance == 1) strcpy(tstr2, "On/Off Only");
            if (compliance == 2) strcpy(tstr2, "Timed");
            if (compliance == 3) strcpy(tstr2, "Timed, Bi-color");
            if (compliance == 4) strcpy(tstr2, "Timed, Tri-color");
            sprintf(tstr, "  [%02d] %s %d LED's Compliance:%s;\n", 1 + i / 3,
                    osdp_pdcap_function(*(i + 0 + msg->data_payload)),
                    *(i + 2 + msg->data_payload), tstr2);
          }; break;
          case 10:
            value = *(i + 1 + msg->data_payload) +
                    256 * (*(i + 2 + msg->data_payload));
            sprintf(tstr, "  [%02d] %s %d;\n", 1 + i / 3,
                    osdp_pdcap_function(*(i + 0 + msg->data_payload)), value);
            break;
          case 11:
            value = *(i + 1 + msg->data_payload) +
                    256 * (*(i + 2 + msg->data_payload));
            context.max_message = value;  // SIDE EFFECT (naughty me) - sets
                                          // value when displaying it.
            sprintf(tstr, "  [%02d] %s %d;\n", 1 + i / 3,
                    osdp_pdcap_function(*(i + 0 + msg->data_payload)), value);
            break;
          default:
            sprintf(tstr, "  [%02d] %s %02x %02x;\n", 1 + i / 3,
                    osdp_pdcap_function(*(i + 0 + msg->data_payload)),
                    *(i + 1 + msg->data_payload), *(i + 2 + msg->data_payload));
            break;
        };
        strcat(tlogmsg, tstr);
      };
    }; break;

    case OOSDP_MSG_PD_IDENT:
      msg = (OSDP_MSG*)aux;
      oh = (OSDP_HDR*)(msg->ptr);
      sprintf(filename, "/opt/osdp-conformance/run/CP/ident_from_PD%02x.json",
              (0x7f & oh->addr));
      identf = fopen(filename, "w");
      if (identf != NULL) {
        fprintf(identf, "{\n");
        fprintf(identf, "  \"#\" : \"PD address %02x\",\n", (0x7f & oh->addr));
        fprintf(identf, "  \"OUI\" : \"%02x-%02x-%02x\",\n",
                *(msg->data_payload + 0), *(msg->data_payload + 1),
                *(msg->data_payload + 2));
        fprintf(identf, "  \"version\" : \"model-%d-ver-%d\",\n",
                *(msg->data_payload + 3), *(msg->data_payload + 4));
        fprintf(identf, "  \"serial\" : \"%02x%02x%02x%02x\",\n",
                *(msg->data_payload + 5), *(msg->data_payload + 6),
                *(msg->data_payload + 7), *(msg->data_payload + 8));
        fprintf(identf, "  \"firmware\" : \"%d.%d-build-%d\"\n",
                *(msg->data_payload + 9), *(msg->data_payload + 10),
                *(msg->data_payload + 11));
        fprintf(identf, "}\n");
        fclose(identf);
      };
      sprintf(tlogmsg,
              "  PD Identification\n    OUI %02x-%02x-%02x Model %d Ver %d SN "
              "%02x%02x%02x%02x FW %d.%d Build %d\n",
              *(msg->data_payload + 0), *(msg->data_payload + 1),
              *(msg->data_payload + 2), *(msg->data_payload + 3),
              *(msg->data_payload + 4), *(msg->data_payload + 5),
              *(msg->data_payload + 6), *(msg->data_payload + 7),
              *(msg->data_payload + 8), *(msg->data_payload + 9),
              *(msg->data_payload + 10), *(msg->data_payload + 11));
      break;

    case OOSDP_MSG_PKT_STATS:
      sprintf(tlogmsg,
              " CP Polls Sent %6d PD Acks %6d Sent NAKs %6d CkSumErr %6d\n",
              context.cp_polls, context.pd_acks, context.sent_naks,
              context.checksum_errs);
      break;
    default:
      sprintf(tlogmsg, "Unknown message type %d", msgtype);
      break;
  };
  strcpy(logmsg, tlogmsg);
  return (status);
}

int oosdp_log(OSDP_CONTEXT* context, int logtype, int level, char* message)

{ /* oosdp_log */

  time_t current_raw_time;
  struct tm* current_cooked_time;
  int llogtype;
  char* role_tag;
  int status;
  char timestamp[1024];

  status = ST_OK;
  llogtype = logtype;
  role_tag = "";
  strcpy(timestamp, "");
  if (logtype EQUALS OSDP_LOG_STRING_CP) {
    role_tag = "CP";
    llogtype = OSDP_LOG_STRING;
  };
  if (logtype EQUALS OSDP_LOG_STRING_PD) {
    role_tag = "PD";
    llogtype = OSDP_LOG_STRING;
  };
  if (llogtype == OSDP_LOG_STRING) {
    char address_suffix[1024];
    struct timespec current_time_fine;

    clock_gettime(CLOCK_REALTIME, &current_time_fine);
    (void)time(&current_raw_time);
    current_cooked_time = localtime(&current_raw_time);
    if (strcmp("CP", role_tag) == 0)
      strcpy(address_suffix, "");
    else
      sprintf(address_suffix, " A=%02x(hex)", p_card.addr);
    sprintf(timestamp,
            "OSDP %s Frame-in:%04d%s\nTimestamp:%04d%02d%02d-%02d%02d%02d "
            "(Sec/Nanosec: %ld %ld)\n",
            role_tag, context->packets_received, address_suffix,
            1900 + current_cooked_time->tm_year,
            1 + current_cooked_time->tm_mon, current_cooked_time->tm_mday,
            current_cooked_time->tm_hour, current_cooked_time->tm_min,
            current_cooked_time->tm_sec, current_time_fine.tv_sec,
            current_time_fine.tv_nsec);
  };
  if (context->role == OSDP_ROLE_MONITOR) {
    fprintf(context->log, "%s%s", timestamp, message);
    fflush(context->log);
  } else if (context->verbosity >= level) {
    fprintf(context->log, "%s%s", timestamp, message);
    fflush(context->log);
  };

  return (status);

} /* oosdp_log */
