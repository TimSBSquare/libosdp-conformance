/*
  diag 01 crc checker

  (C)2014-2015 Smithee Spelvin Agnew & Plinge, Inc.

to compile in libosdp/test/diag:

  gcc -c -Wall -Werror -g -I ../../include/  diag01.c
  gcc -o diag01 -g diag01.o ../../src-lib/libosdp.a

  Support provided by the Security Industry Association
  http://www.securityindustrya.org

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

#include <open-osdp.h>
#include <osdp_conformance.h>
OSDP_INTEROP_ASSESSMENT
osdp_conformance;
OSDP_CONTEXT
context;
OSDP_BUFFER
osdp_buf;
OSDP_PARAMETERS
p_card;
int creds_buffer_a_next;
int creds_buffer_a_lth;
int creds_buffer_a_remaining;
unsigned char creds_buffer_a[2];

unsigned char sample3[] = {0x53, 0x80, 0x14, 0x00, 0x04, 0x45,
                           0x08, 0x00, 0x1b, 0x02, 0x01, 0xca,
                           0xfe, 0xde, 0xad, 0x01, 0x00, 0x08};
int main(int argc, char* argv[])

{
  int msg_lth;
  unsigned char msg[1024];
  unsigned short int returned_crc;

  memset(msg, 0, sizeof(msg));
  msg[0] = 'A';
  msg_lth = 1;
  memcpy(msg, sample3, sizeof(sample3));
  msg_lth = sizeof(sample3);

  returned_crc = fCrcBlk(msg, msg_lth);
  // answer should be 0x9479 per web page
  fprintf(stderr, "msg lth %d CRC was %04x\n", msg_lth, returned_crc);

  memset(msg, 0, sizeof(msg));
  strcpy((char*)msg, "123456789");
  msg_lth = strlen((char*)msg);
  returned_crc = fCrcBlk(msg, msg_lth);
  // answer should be 0xE5CC per web page
  fprintf(stderr, "msg lth %d CRC was %04x\n", msg_lth, returned_crc);

  return (0);
}
