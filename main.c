#include <string.h>

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "globals.h"

//
//------------------------------------------------------------------------------
//
#define MAX_SPI_DESCRIPTORS (8)

//
//  SPI clock divider. Actual frequency is SystemCoreClock (72MHz) / divider.
//  A divider of 8 gives a frequency of 9MHz.
//
//  /8 --   9.00 MHz
//  /7 --  10.28 MHz
//  /6 --  12.00 MHz (this and faster is not 100% reliable)
//  /5 --  14.40 MHz
//  /4 --  18.00 MHz
//  /3 --  24.00 MHz
//  /2 --  36.00 MHz
//  /1 --  72.00 MHz (does not work)
//
#define SPI0_CLOCK_DIVIDER (2)

//
//------------------------------------------------------------------------------
//
typedef struct dmaQueue_s
{
  volatile size_t count;
  volatile size_t head;
  volatile size_t tail;
  volatile size_t error;
  volatile size_t xfers;
  DMA_CHDESC_T dmaDesc [MAX_SPI_DESCRIPTORS] __attribute__ ((aligned(16)));
}
dmaQueue_t;

//
//------------------------------------------------------------------------------
//
static dmaQueue_t dmaQueue;

//
//------------------------------------------------------------------------------
//
//  Sets up system hardware
//
static void prvSetupHardware(void)
{
  SystemCoreClockUpdate ();
  Board_Init ();

  Board_LED_Set (0, false);
}

//
//------------------------------------------------------------------------------
//
//  Tasks to toggle LED at 4Hz, 8Hz, and UART task
//
static void vLEDTask1 (void *pvParameters __attribute__ ((unused)))
{
  bool LedState = false;

  while (1)
  {
    Board_LED_Set (0, LedState);
    LedState = (bool) !LedState;

    vTaskDelay (pdMS_TO_TICKS (250));
  }
}

static void vLEDTask2 (void *pvParameters __attribute__ ((unused)))
{
  bool LedState = false;

  while (1)
  {
    Board_LED_Set (1, LedState);
    LedState = (bool) !LedState;

    vTaskDelay (pdMS_TO_TICKS (125));
  }
}

//
//------------------------------------------------------------------------------
//
static void initSPI0 (void)
{
  SPI_CFG_T spiCfg = {0};

  spiCfg.Mode = SPI_MODE_MASTER;
  spiCfg.ClkDiv = SPI0_CLOCK_DIVIDER;
  spiCfg.ClockMode = SPI_CLOCK_MODE3;
  spiCfg.DataOrder = SPI_DATA_MSB_FIRST;
  spiCfg.SSELPol = SPI_CFG_SPOL0_LO | SPI_CFG_SPOL1_LO | SPI_CFG_SPOL2_LO | SPI_CFG_SPOL3_LO;

  Chip_SPI_Init (LPC_SPI0);
  Chip_SPI_SetConfig (LPC_SPI0, &spiCfg);
  Chip_SPI_SetControlInfo (LPC_SPI0, 8, SPI_TXDATCTL_EOT);
  Chip_SPI_Enable (LPC_SPI0);

  Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_SWM);

  Chip_IOCON_PinMuxSet (LPC_IOCON, GPIO_SPI0_SCK_PORT,      GPIO_SPI0_SCK_PIN,      (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
  Chip_IOCON_PinMuxSet (LPC_IOCON, GPIO_SPI0_MISO_PORT,     GPIO_SPI0_MISO_PIN,     (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
  Chip_IOCON_PinMuxSet (LPC_IOCON, GPIO_SPI0_MOSI_PORT,     GPIO_SPI0_MOSI_PIN,     (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
  Chip_IOCON_PinMuxSet (LPC_IOCON, GPIO_LCD_RA8876_CS_PORT, GPIO_LCD_RA8876_CS_PIN, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));

  Chip_SWM_MovablePortPinAssign (SWM_SPI0_SCK_IO,      GPIO_SPI0_SCK_PORT,      GPIO_SPI0_SCK_PIN);
  Chip_SWM_MovablePortPinAssign (SWM_SPI0_MISO_IO,     GPIO_SPI0_MISO_PORT,     GPIO_SPI0_MISO_PIN);
  Chip_SWM_MovablePortPinAssign (SWM_SPI0_MOSI_IO,     GPIO_SPI0_MOSI_PORT,     GPIO_SPI0_MOSI_PIN);
  Chip_SWM_MovablePortPinAssign (SWM_SPI0_SSELSN_0_IO, GPIO_LCD_RA8876_CS_PORT, GPIO_LCD_RA8876_CS_PIN);

  Chip_Clock_DisablePeriphClock (SYSCTL_CLOCK_SWM);
}

//
//------------------------------------------------------------------------------
//
static void dmaTXSetup (void)
{
	Chip_DMA_EnableChannel (LPC_DMA, DMAREQ_SPI0_TX);
	Chip_DMA_EnableIntChannel (LPC_DMA, DMAREQ_SPI0_TX);
	Chip_DMA_SetupChannelConfig (LPC_DMA, DMAREQ_SPI0_TX, (DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_SNGL | DMA_CFG_CHPRIORITY (1)));

  memset (&dmaQueue, 0, sizeof (dmaQueue));
}

static void dmaClearChannel (DMA_CHID_T ch)
{
	Chip_DMA_DisableChannel (LPC_DMA, ch);

	while ((Chip_DMA_GetBusyChannels (LPC_DMA) & (1 << ch)) != 0)
    ;

	Chip_DMA_AbortChannel (LPC_DMA, ch);
	Chip_DMA_ClearErrorIntChannel (LPC_DMA, ch);
}

static void initDMA (void)
{
  Chip_DMA_Init (LPC_DMA);
  Chip_DMA_Enable (LPC_DMA);
  Chip_DMA_SetSRAMBase (LPC_DMA, DMA_ADDR (Chip_DMA_Table));

  dmaTXSetup ();

	NVIC_EnableIRQ (DMA_IRQn);
}

//
//   Only handles SPI0 TX conditions.
//
void DMA_IRQHandler (void);
void DMA_IRQHandler (void)
{
	uint32_t errors = Chip_DMA_GetErrorIntChannels (LPC_DMA);
	uint32_t pending = Chip_DMA_GetActiveIntAChannels (LPC_DMA);

	if ((errors | pending) & (1 << DMAREQ_SPI0_TX))
  {
		Chip_DMA_ClearActiveIntAChannel (LPC_DMA, DMAREQ_SPI0_TX);

		if (errors & (1 << DMAREQ_SPI0_TX))
    {
			dmaClearChannel (DMAREQ_SPI0_TX);
			dmaTXSetup ();
      dmaQueue.error++;
		}
		else
    {
      if (--dmaQueue.count)
      {
        Chip_DMA_SetupTranChannel (LPC_DMA, DMAREQ_SPI0_TX, &dmaQueue.dmaDesc [dmaQueue.tail]);
        Chip_DMA_SetupChannelTransfer (LPC_DMA, DMAREQ_SPI0_TX, dmaQueue.dmaDesc [dmaQueue.tail].xfercfg);
        Chip_DMA_SetValidChannel (LPC_DMA, DMAREQ_SPI0_TX);

        dmaQueue.tail = (dmaQueue.tail + 1) % MAX_SPI_DESCRIPTORS;
      }
    }
	}
}

//
//   1 -- dmaQueue is full, please call back later
//   0 -- Success
//  -1 -- Argument error
//
static int dmaSPI (const void *pvData, size_t dataLen, size_t wordSize)
{
  uint32_t width;

  if (dataLen > 1024)
    return -1;

  switch (wordSize)
  {
    case sizeof (uint8_t)  : width = DMA_XFERCFG_WIDTH_8;  break;
    case sizeof (uint16_t) : width = DMA_XFERCFG_WIDTH_16; break;
    case sizeof (uint32_t) : width = DMA_XFERCFG_WIDTH_32; break;
    default                : return -1;
  }

	NVIC_DisableIRQ (DMA_IRQn);

	if (dmaQueue.count >= MAX_SPI_DESCRIPTORS)
  {
		NVIC_EnableIRQ (DMA_IRQn);
    return 1;
  }

	dmaQueue.dmaDesc [dmaQueue.head].source  = DMA_ADDR ((uint8_t *) pvData + (dataLen * wordSize) - 1);
	dmaQueue.dmaDesc [dmaQueue.head].dest    = DMA_ADDR (&LPC_SPI0->TXDAT);
	dmaQueue.dmaDesc [dmaQueue.head].next    = DMA_ADDR (0);
	dmaQueue.dmaDesc [dmaQueue.head].xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_SETINTA | DMA_XFERCFG_SWTRIG | DMA_XFERCFG_SRCINC_1 | DMA_XFERCFG_DSTINC_0 | DMA_XFERCFG_XFERCOUNT (dataLen) | width;

	if (dmaQueue.count == 0)
  {
		Chip_DMA_SetupTranChannel (LPC_DMA, DMAREQ_SPI0_TX, &dmaQueue.dmaDesc [dmaQueue.head]);
		Chip_DMA_SetupChannelTransfer (LPC_DMA, DMAREQ_SPI0_TX, dmaQueue.dmaDesc [dmaQueue.head].xfercfg);
		Chip_DMA_SetValidChannel (LPC_DMA, DMAREQ_SPI0_TX);

    dmaQueue.tail = (dmaQueue.tail + 1) % MAX_SPI_DESCRIPTORS;
	}

  dmaQueue.count++;
  dmaQueue.head = (dmaQueue.head + 1) % MAX_SPI_DESCRIPTORS;
  dmaQueue.xfers++;

	NVIC_EnableIRQ (DMA_IRQn);

	return 0;
}

//
//  dataLen is the number of entries in dataPtr, e.g. if dataPtr is an array of
//  uint16_t [256], it would be 256, and wordSize would be sizeof (uint16_t).
//
static int dmaSPIEx (const void *dataPtr, size_t dataLen, size_t wordSize)
{
  if (dataLen <= 1024)
    return dmaSPI (dataPtr, dataLen, wordSize);
  else
  {
    int r;
    size_t blocks = dataLen / 1024;
    size_t remainder = dataLen % 1024;

    for (size_t block = 0; block < blocks; block++)
    {
      while ((r = dmaSPI (dataPtr, 1024, wordSize) > 0) && !dmaQueue.error)
        vTaskDelay (pdMS_TO_TICKS (1));

      if (r < 0)
        return r;
      if (dmaQueue.error)
        return -1;

      dataPtr = (uint8_t *) dataPtr + (1024 * wordSize);
    }

    while ((r = dmaSPI (dataPtr, remainder, wordSize) > 0) && !dmaQueue.error)
      vTaskDelay (pdMS_TO_TICKS (1));

    if (dmaQueue.error)
      return -1;

    return r;
  }
}

//
//------------------------------------------------------------------------------
//
typedef struct data_s
{
  const char *str;
  const size_t len;
}
data_t;

static void vDMATask (void *pvParameters __attribute__ ((unused)))
{
  static int err;
  static const data_t data [] = {
    { .len = 32, .str = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", }, //  1
    { .len = 32, .str = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", }, //  2
    { .len = 32, .str = "cccccccccccccccccccccccccccccccc", }, //  3
    { .len = 32, .str = "dddddddddddddddddddddddddddddddd", }, //  4
    { .len = 32, .str = "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", }, //  5
    { .len = 32, .str = "ffffffffffffffffffffffffffffffff", }, //  6
    { .len = 32, .str = "gggggggggggggggggggggggggggggggg", }, //  7
    { .len = 32, .str = "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh", }, //  8
    { .len = 32, .str = "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii", }, //  9
    { .len = 32, .str = "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj", }, // 10
    { .len = 32, .str = "kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk", }, // 11
    { .len = 32, .str = "llllllllllllllllllllllllllllllll", }, // 12
    { .len = 32, .str = "mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm", }, // 13
    { .len = 32, .str = "nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn", }, // 14
    { .len = 32, .str = "oooooooooooooooooooooooooooooooo", }, // 15
    { .len = 32, .str = "pppppppppppppppppppppppppppppppp", }, // 16
    { .len = 32, .str = "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq", }, // 17
    { .len = 32, .str = "rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr", }, // 18
    { .len = 32, .str = "ssssssssssssssssssssssssssssssss", }, // 19
    { .len = 32, .str = "tttttttttttttttttttttttttttttttt", }, // 20
    { .len = 32, .str = "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu", }, // 21
    { .len = 32, .str = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv", }, // 22
    { .len = 32, .str = "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww", }, // 23
    { .len = 32, .str = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", }, // 24
    { .len = 32, .str = "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy", }, // 25
    { .len = 32, .str = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", }, // 26
  };

  while (1)
  {
    for (size_t i = 0; i < arrsizeof (data); i++)
    {
      while (dmaSPIEx (data [i].str, data [i].len, sizeof (char)) && !dmaQueue.error)
        vTaskDelay (pdMS_TO_TICKS (1));

      if (dmaQueue.error)
      {
        err = 2;
        goto stopit;
      }
    }

    vTaskDelay (pdSEC_TO_TICKS (1));

    if (dmaQueue.count)
    {
      err = 3;
      goto stopit;
    }
  }

stopit:
  while (err)
    vTaskDelay (pdSEC_TO_TICKS (1));
}

//
//------------------------------------------------------------------------------
//
int main (void)
{
  prvSetupHardware ();

  initDMA ();
  initSPI0 ();

  xTaskCreate (vLEDTask1, "vTaskLed1", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
  xTaskCreate (vLEDTask2, "vTaskLed2", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
  xTaskCreate (vDMATask,  "vTaskDMA",  configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL), (TaskHandle_t *) NULL);

  vTaskStartScheduler ();

  return 1;
}
