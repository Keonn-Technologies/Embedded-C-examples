/**
 * Sample program to demonstrate BAP usage
 * @file bap.c
 */

#include <tm_reader.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>

/* Enable this to use transportListener */
#ifndef USE_TRANSPORT_LISTENER
#define USE_TRANSPORT_LISTENER 0
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
    TMR_GEN2_Bap bap;

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

    printf("case 1: read by using the default bap parameter values \n");
    //initialize the with -1
    ret = TMR_GEN2_init_BapParams(&bap, -1 , -1);
    checkerr(rp, ret, 1, "initialize  bap parameters");
    //set the parameters to the module
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_BAP,&bap);
    checkerr(rp, ret, 1, "setting bap parameters"); 
    ret = TMR_paramGet(rp, TMR_PARAM_GEN2_BAP, &bap);
    checkerr(rp, ret,1, "getting bap parameters");
    printf("powerupdelay:%d freqhopofftime:%d\n",bap.powerUpDelayUs,bap.freqHopOfftimeUs);
    ret = TMR_read(rp, 500, NULL); 
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret)
    {
      /* In case of TAG ID Buffer Full, extract the tags present
      * in buffer.
      */
      fprintf(stdout, "reading tags:%s\n", TMR_strerr(rp, ret));
    }
    else
    {
      checkerr(rp, ret, 1, "reading tags");
    }
    while (TMR_SUCCESS == TMR_hasMoreTags(rp))
    {
      TMR_TagReadData trd;
      char epcStr[128];

      ret = TMR_getNextTag(rp, &trd);
      checkerr(rp, ret, 1, "fetching tag");

      TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
      printf("EPC:%s ant:%d count:%d\n", epcStr, trd.antenna, trd.readCount);
    }

    printf("case 2:read by setting the bap parameters \n");
    //initialize the variable. 
    ret = TMR_GEN2_init_BapParams(&bap, 40000, 30000);
    checkerr(rp, ret, 1, "initialize  bap parameters");
    //set the parameters to the module
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_BAP,&bap);
    checkerr(rp, ret, 1, "setting bap parameters"); 
    ret = TMR_paramGet(rp, TMR_PARAM_GEN2_BAP, &bap);
    checkerr(rp, ret,1, "getting bap parameters");
    printf("powerupdelay:%d freqhopofftime:%d\n",bap.powerUpDelayUs,bap.freqHopOfftimeUs);
    ret = TMR_read(rp, 500, NULL); 
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret)
    {
      /* In case of TAG ID Buffer Full, extract the tags present
      * in buffer.
      */
      fprintf(stdout, "reading tags:%s\n", TMR_strerr(rp, ret));
    }
    else
    {
      checkerr(rp, ret, 1, "reading tags");
    }
    while (TMR_SUCCESS == TMR_hasMoreTags(rp))
    {
      TMR_TagReadData trd;
      char epcStr[128];

      ret = TMR_getNextTag(rp, &trd);
      checkerr(rp, ret, 1, "fetching tag");

      TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
      printf("EPC:%s ant:%d count:%d\n", epcStr, trd.antenna, trd.readCount);
    }

    printf("case 3: read by disabling the bap option \n");
    //initialize the with -1 
    //set the parameters to the module
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_BAP, NULL);
    checkerr(rp, ret, 1, "setting bap parameters"); 
    ret = TMR_paramGet(rp, TMR_PARAM_GEN2_BAP, &bap);
    checkerr(rp, ret,1, "getting bap parameters");
    printf("powerupdelay:%d freqhopofftime:%d\n",bap.powerUpDelayUs,bap.freqHopOfftimeUs);
    ret = TMR_read(rp, 500, NULL); 
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret)
    {
      /* In case of TAG ID Buffer Full, extract the tags present
      * in buffer.
      */
      fprintf(stdout, "reading tags:%s\n", TMR_strerr(rp, ret));
    }
    else
    {
      checkerr(rp, ret, 1, "reading tags");
    }
    while (TMR_SUCCESS == TMR_hasMoreTags(rp))
    {
      TMR_TagReadData trd;
      char epcStr[128];

      ret = TMR_getNextTag(rp, &trd);
      checkerr(rp, ret, 1, "fetching tag");

      TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);
      printf("EPC:%s ant:%d count:%d\n", epcStr, trd.antenna, trd.readCount);
    }
  }

  TMR_destroy(rp);
  return 0;
}
