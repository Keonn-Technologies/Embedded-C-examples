/**
 * Sample program that to demonstrate the usage of Gen2v2 ReadBuffer.
 * @file readbuffer.c
 */

#include <tm_reader.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>

#if WIN32
#define snprintf sprintf_s
#endif 

/* Enable this to use transportListener */
#ifndef USE_TRANSPORT_LISTENER
#define USE_TRANSPORT_LISTENER 1
#endif

#define usage() {errx(1, "Please provide reader URL, such as:\n"\
                         "tmr:///com4 or tmr:///com4 --ant 1,2\n"\
                         "tmr://my-reader.example.com or tmr://my-reader.example.com --ant 1,2\n");}

void errx(int exitval, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);

  exit(exitval);
}

void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg)
{
  if (TMR_SUCCESS != ret)
  {
    errx(exitval, "Error %s: %s\n", msg, TMR_strerr(rp, ret));
  }
}

void serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                   uint32_t timeout, void *cookie)
{
  FILE *out = cookie;
  uint32_t i;

  fprintf(out, "%s", tx ? "Sending: " : "Received:");
  for (i = 0; i < dataLen; i++)
  {
    if (i > 0 && (i & 15) == 0)
    {
      fprintf(out, "\n         ");
    }
    fprintf(out, " %02x", data[i]);
  }
  fprintf(out, "\n");
}

void stringPrinter(bool tx,uint32_t dataLen, const uint8_t data[],uint32_t timeout, void *cookie)
{
  FILE *out = cookie;

  fprintf(out, "%s", tx ? "Sending: " : "Received:");
  fprintf(out, "%s\n", data);
}

void parseAntennaList(uint8_t *antenna, uint8_t *antennaCount, char *args)
{
  char *token = NULL;
  char *str = ",";
  uint8_t i = 0x00;
  int scans;

  /* get the first token */
  if (NULL == args)
  {
    fprintf(stdout, "Missing argument\n");
    usage();
  }

  token = strtok(args, str);
  if (NULL == token)
  {
    fprintf(stdout, "Missing argument after %s\n", args);
    usage();
  }

  while(NULL != token)
  {
    scans = sscanf(token, "%"SCNu8, &antenna[i]);
    if (1 != scans)
    {
      fprintf(stdout, "Can't parse '%s' as an 8-bit unsigned integer value\n", token);
      usage();
    }
    i++;
    token = strtok(NULL, str);
  }
  *antennaCount = i;
}

int main(int argc, char *argv[])
{
  TMR_Reader r, *rp;
  TMR_Status ret;
  TMR_Region region;
  uint8_t *antennaList = NULL;
  uint8_t buffer[20];
  uint8_t i;
  uint8_t antennaCount = 0x0;
  TMR_String model;
  char str[64];
#if USE_TRANSPORT_LISTENER
  TMR_TransportListenerBlock tb;
#endif
 
  if (argc < 2)
  {
    usage();
  }
  
  for (i = 2; i < argc; i+=2)
  {
    if(0x00 == strcmp("--ant", argv[i]))
    {
      if (NULL != antennaList)
      {
        fprintf(stdout, "Duplicate argument: --ant specified more than once\n");
        usage();
      }
      parseAntennaList(buffer, &antennaCount, argv[i+1]);
      antennaList = buffer;
    }
    else
    {
      fprintf(stdout, "Argument %s is not recognized\n", argv[i]);
      usage();
    }
  }
  
  rp = &r;
  ret = TMR_create(rp, argv[1]);
  checkerr(rp, ret, 1, "creating reader");

#if USE_TRANSPORT_LISTENER

  if (TMR_READER_TYPE_SERIAL == rp->readerType)
  {
    tb.listener = serialPrinter;
  }
  else
  {
    tb.listener = stringPrinter;
  }
  tb.cookie = stdout;

  TMR_addTransportListener(rp, &tb);
#endif

  ret = TMR_connect(rp);
  checkerr(rp, ret, 1, "connecting reader");

  region = TMR_REGION_NONE;
  ret = TMR_paramGet(rp, TMR_PARAM_REGION_ID, &region);
  checkerr(rp, ret, 1, "getting region");

  if (TMR_REGION_NONE == region)
  {
    TMR_RegionList regions;
    TMR_Region _regionStore[32];
    regions.list = _regionStore;
    regions.max = sizeof(_regionStore)/sizeof(_regionStore[0]);
    regions.len = 0;

    ret = TMR_paramGet(rp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
    checkerr(rp, ret, __LINE__, "getting supported regions");

    if (regions.len < 1)
    {
      checkerr(rp, TMR_ERROR_INVALID_REGION, __LINE__, "Reader doesn't supportany regions");
    }
    region = regions.list[0];
    ret = TMR_paramSet(rp, TMR_PARAM_REGION_ID, &region);
    checkerr(rp, ret, 1, "setting region");  
  }

  model.value = str;
  model.max = 64;
  TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &model);
  if (((0 == strcmp("M6e Micro", model.value)) ||(0 == strcmp("M6e Nano", model.value)))
    && (NULL == antennaList))
  {
    fprintf(stdout, "Module doesn't has antenna detection support please provide antenna list\n");
    usage();
  }

  /* Read Plan */
  {
    TMR_ReadPlan plan;
    /**
     * for antenna configuration we need two parameters
     * 1. antennaCount : specifies the no of antennas should
     *    be included in the read plan, out of the provided antenna list.
     * 2. antennaList  : specifies  a list of antennas for the read plan.
     **/ 

    // initialize the read plan 
    ret = TMR_RP_init_simple(&plan, antennaCount, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
    checkerr(rp, ret, 1, "initializing the  read plan");

    /* Commit read plan */
    ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
    checkerr(rp, ret, 1, "setting read plan");

	{
	  
	  TMR_TagOp newtagop;
	  //TMR_TagFilter selectFilter;
	  TMR_uint8List key;
	  TMR_uint8List ichallenge;
	  bool SendRawData = false;
	  //uint8_t Offset = 0;
	  //uint8_t BlockCount = 1;
	  TMR_TagOp_GEN2_NXP_Tam1Authentication tam1;
	  //TMR_TagOp_GEN2_NXP_Tam2Authentication tam2;
	  TMR_TagOp_GEN2_NXP_Readbuffer readbuffer;
	  uint8_t data[128];
	  TMR_uint8List dataList;
	  char dataStr[255];
	  //uint8_t mask[128];
    uint8_t key0[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
	  //uint8_t key1[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
	  uint8_t challenge[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xAB, 0xCD };

	  key.list = key0;
	  key.max = key.len = sizeof(key0) / sizeof(key0[0]);

	  dataList.len = dataList.max = 256;
      dataList.list = data;
      
	  ichallenge.list = challenge;
	  ichallenge.max = ichallenge.len = sizeof(challenge) / sizeof(challenge[0]);
	  
	

	 /* mask[0] = 0xE2;
      mask[1] = 0xC0;
      mask[2] = 0x6F;
      mask[3] = 0x92;
      TMR_TF_init_gen2_select(&selectFilter, false, TMR_GEN2_BANK_EPC, 32, 16, mask);*/

	  ret = TMR_TagOp_init_GEN2_NXP_AES_Tam1authentication(&tam1, KEY0,&key,&ichallenge,SendRawData);
	  checkerr(rp, ret, 1, "initializing Tam1 authentication");
	  
	  readbuffer.authenticate.tam1Auth = tam1;
	  readbuffer.authenticate.type = TAM1_AUTHENTICATION;
	  
	  ret = TMR_TagOp_init_GEN2_NXP_AES_ReadBuffer(&newtagop,0,128,&readbuffer);
	  checkerr(rp, ret, 1, "initializing Untraceable");

	  ret = TMR_executeTagOp(rp, &newtagop, NULL, &dataList);
	  checkerr(rp, ret, 1, "executing Untraceable tagop");
	  //TODO:Currently C API don't have support for decrypting raw data.
	  // TO decrypt this data use online AES decryptor
	  //http://aes.online-domain-tools.com/
	  TMR_bytesToHex(dataList.list,dataList.len,dataStr);
	  printf("Returned buffer Data:%s\n",dataStr);

	  // Uncomment this to enable ReadBuffer with TAM1 with Key1
	  /*key.list = key1;
	  key.max = key.len = sizeof(key1) / sizeof(key1[0]);

	  ret = TMR_TagOp_init_GEN2_NXP_AES_Tam1authentication(&tam1, KEY1,&key,&ichallenge,SendRawData);
	  checkerr(rp, ret, 1, "initializing Tam1 authentication");
	  
	  readbuffer.authenticate.tam1Auth = tam1;
	  readbuffer.authenticate.type = TAM1_AUTHENTICATION;
	  
	  ret = TMR_TagOp_init_GEN2_NXP_AES_ReadBuffer(&newtagop,0,128,&readbuffer);
	  checkerr(rp, ret, 1, "initializing Untraceable");

	  ret = TMR_executeTagOp(rp, &newtagop, NULL, &dataList);
	  checkerr(rp, ret, 1, "executing Untraceable tagop");
	 
	  //TODO:Currently C API don't have support for decrypting raw data.
	  // TO decrypt this data use online AES decryptor
	  //http://aes.online-domain-tools.com/
	  TMR_bytesToHex(dataList.list,dataList.len,dataStr);
	  printf("Returned buffer Data:%s\n",dataStr);*/
	 

	  //Uncomment this to enable ReadBuffer with TAM2 with key1
	  /*key.list = key1;
	  key.max = key.len = sizeof(key1) / sizeof(key1[0]);

	  ret = TMR_TagOp_init_GEN2_NXP_AES_Tam2authentication(&tam2, KEY1,&key,&ichallenge,EPC,Offset,BlockCount,SendRawData);
	  checkerr(rp, ret, 1, "initializing Tam1 authentication");

	  readbuffer.authenticate.tam2Auth = tam2;
	  readbuffer.authenticate.type = TAM2_AUTHENTICATION;
	  
	  ret = TMR_TagOp_init_GEN2_NXP_AES_ReadBuffer(&newtagop,0,256,&readbuffer);
	  checkerr(rp, ret, 1, "initializing Untraceable");

	  ret = TMR_executeTagOp(rp, &newtagop, NULL, &dataList);
	  checkerr(rp, ret, 1, "executing Untraceable tagop");
	  //TODO:Currently C API don't have support for decrypting raw data.
	  // TO decrypt this data use online AES decryptor
	  //http://aes.online-domain-tools.com/
	  TMR_bytesToHex(dataList.list,dataList.len,dataStr);
	  printf("Returned buffer Data:%s\n",dataStr);*/
	} 
  }

  TMR_destroy(rp);
  return 0;
}
