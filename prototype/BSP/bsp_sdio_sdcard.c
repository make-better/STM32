#include "bsp_sdio_sdcard.h"

SD_HandleTypeDef uSdHandle;

void BSP_SD_MspInit(void *params);


    
uint8_t BSP_SD_Init(void)
{
    uint8_t state = MSD_OK;
    
    uSdHandle.Instance = SDIO;
    uSdHandle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    uSdHandle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    uSdHandle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    uSdHandle.Init.BusWide = SDIO_BUS_WIDE_1B;
    uSdHandle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    uSdHandle.Init.ClockDiv = 16;
    
    BSP_SD_MspInit(NULL);
    
    if(HAL_SD_Init(&uSdHandle) != HAL_OK)
    {
        state = MSD_ERROR;
    }
    
    if(state == MSD_OK)
    {
        if(HAL_SD_ConfigWideBusOperation(&uSdHandle, SDIO_BUS_WIDE_4B) != HAL_OK)
        {
            state = MSD_ERROR;
        }
        else
        {
            state = MSD_OK;
        }
    }
    return state;
}

void BSP_SD_MspInit(void *params)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    __HAL_RCC_SDIO_CLK_ENABLE();
    
    __DMAx_TxRx_CLK_ENABLE();
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);
}

uint8_t SD_DMAConfigRx(SD_HandleTypeDef *hsd)
{
    static DMA_HandleTypeDef hdma_rx;
    HAL_StatusTypeDef   status = HAL_ERROR;
    
    if(hsd->hdmarx == NULL)
    {
        hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_rx.Instance = SD_DMAx_Rx_INSTANCE;
        
        __HAL_LINKDMA(hsd, hdmarx, hdma_rx);
        //stop any ongoing transfer and reset the state
        HAL_DMA_Abort(&hdma_rx);
        HAL_DMA_DeInit(&hdma_rx);
        
        status = HAL_DMA_Init(&hdma_rx);
        
        HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 0x0D, 0);
        HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);
    }
    else
    {
        status = HAL_OK;
    }
    return (status != HAL_OK?MSD_ERROR:MSD_OK);
}

uint8_t SD_DMAConfigTx(SD_HandleTypeDef *hsd)
{
    static DMA_HandleTypeDef hdma_tx;
    HAL_StatusTypeDef   status = HAL_ERROR;
    
    if(hsd->hdmatx == NULL)
    {
        hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_tx.Instance = SD_DMAx_Tx_INSTANCE;
        
        __HAL_LINKDMA(hsd, hdmatx, hdma_tx);
        //stop any ongoing transfer and reset the state
        HAL_DMA_Abort(&hdma_tx);
        HAL_DMA_DeInit(&hdma_tx);
        
        status = HAL_DMA_Init(&hdma_tx);
        
        HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 0x0D, 0);
        HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
    }
    else
    {
        status = HAL_OK;
    }
    return (status != HAL_OK?MSD_ERROR:MSD_OK);
}

uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
    if(HAL_SD_ReadBlocks(&uSdHandle, (uint8_t*)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        return MSD_ERROR;
    }
    else
    {
        return MSD_OK;
    }
}

uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
    if(HAL_SD_WriteBlocks(&uSdHandle, (uint8_t*)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        return MSD_ERROR;
    }
    else
    {
        return MSD_OK;
    }
}

uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
    uint8_t state = MSD_OK;
    uSdHandle.hdmatx = NULL;
    
    state = SD_DMAConfigRx(&uSdHandle);
    
    if(state == MSD_OK)
    {
        if(HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t*)pData, ReadAddr, NumOfBlocks) != HAL_OK)
        {
            return MSD_ERROR;
        }
        else
        {
            return MSD_OK;
        }
    }
    return state;
}

uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
    uint8_t state = MSD_OK;
    uSdHandle.hdmarx = NULL;
    
    state = SD_DMAConfigTx(&uSdHandle);
    
    if(state == MSD_OK)
    {
        if(HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t*)pData, WriteAddr, NumOfBlocks) != HAL_OK)
        {
            return MSD_ERROR;
        }
        else
        {
            return MSD_OK;
        }
    }
    return state;
}

uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
    if(HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != HAL_OK)
    {
        return MSD_ERROR;
    }
    else
    {
        return MSD_OK;
    }
}

uint8_t BSP_SD_GetCardState(void)
{
    return ((HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo)
{
    HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}

/**
  * @brief SD Abort callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_AbortCallback();
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  BSP_SD_ReadCpltCallback();
}

/**
  * @brief BSP SD Abort callbacks
  * @retval None
  */
__weak void BSP_SD_AbortCallback(void)
{

}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(void)
{

}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(void)
{

}