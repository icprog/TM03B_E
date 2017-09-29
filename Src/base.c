#include "base.h"
#include "tim.h"
#include "string.h"

//010,1010,1010,1001,0110,0110,1001<-0,1010101010,1001011001101001 separation bit+Preamble(10)+Pattern(16)
const uint32_t BaseHead =  0x02aa9669;
#define BaseHeadMsk 0x04000000
#define BaseDataMsk 0x8000000000000000

Base_Ch_t  Base_Ch0;//ͨ��0
Base_Ch_t  Base_Ch1;//ͨ��1
Base_Ch_t  Base_Ch2;//ͨ��2
Base_Ch_t  Base_Ch3;//ͨ��3
Base_Mode_Typedef Base_Mode;//����ģʽ
uint8_t finish;
//�ڲ�����������
#define PARA_RECORD_LEN									16
//�豸Ĭ�����в���
const uint8_t  para_default[PARA_RECORD_LEN] = 
{
0xAA,
0xAA,//��׼ͨ���ţ�����ͨ����0~3
0x00,//ÿͨ����������
0x81,//ͨ��0����
0x81,//ͨ��1����
0x81,//ͨ��2����
0x81//ͨ��3����
};
//�洢�ڲ����� 
uint8_t para_record[PARA_RECORD_LEN];

Time_t Time7;

/*
@Description:�õ����ֵ
@Input:
@Output:
@Return:
*/
uint8_t Get_Xor(uint8_t *src,uint16_t size)
{
	uint8_t temp = 0,i;
	for(i = 0;i<size;i++)
	{
		temp ^= src[i];
	}
	return temp;
}

/*������˹����Ԫת����bit 
  1λ����˹�����Ӧ��bit��1:10 0:01*/
/*
@Description:����˹����Ԫ
1λ����˹�����Ӧ��bit��1:10 0:01
@Input:
@Output:
@Return:
*/
uint64_t Manchester(uint32_t dat)
{
    uint8_t cnt;
	  uint64_t Mdata=0;
	  for(cnt=0;cnt<32;cnt++)
	  {
			Mdata<<=2;
			if(dat&0x80000000)
			{
					Mdata = Mdata|0X02;
			}
			else
			{
					Mdata = Mdata|0X01;
			}
			dat<<=1;
    }
		return Mdata;
}
/************************************************ 
@Description:��ʼ��
@Input:pBuff ���pwm����
       Channel ͨ����
@Output:
@Return:
*************************************************/ 
void Base_Data_Init(Base_Ch_t* Channel)
{
//	uint8_t i;
//	*pBuff++ = 350;//Carrier
//	*pBuff++ = gBitDuration;//ÿbit������
//	for(i=0;i<BaseHeadLen;i++)//Bit����
//	{
//		*pBuff++ = gBitDuration;
//	}
//	for(i=0;i<BaseDataLen;i++)
//	{
//		*pBuff++ = gBitDuration;
//	}
	Channel->BitPeriod[0] = 350;//Carrier
	Channel->BitPeriod[1] = gBitDuration;//ÿbit������
	Channel->SendBitCount = 0;
	Channel->PwmNum = 0;
	Channel->idx = 0;
	Channel->Data[BASE_XOR_IDX] = Get_Xor(&Channel->Data[BASE_DATA_IDX],3);
	Channel->tData0 = (Channel->Data[BASE_XOR_IDX]<<24)|(Channel->Data[BASE_DATA_IDX]<<16)
											|(Channel->Data[BASE_ID_IDX]<<8)|(Channel->Data[BASE_ID_IDX+1]);
	Channel->tData = Manchester(Channel->tData0);
//	Channel->tData =0x5555555555555555;
	Channel->tHead = BaseHead;
	Channel->sfinish = 0;
	Channel->SendState = S_BHead;
}
/************************************************* 
@Description:����ͨ��0
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch0_Start(void)
{
	Base_Data_Init(&Base_Ch0);
	HAL_GPIO_WritePin(BaseCh0_GPIO_Port, BaseCh0_Pin, GPIO_PIN_SET);
	HAL_GPIO_TogglePin(LedCh0_GPIO_Port,LedCh0_Pin);
//	HAL_TIM_DMABurst_MultiWriteStart(&htim1, TIM_DMABASE_RCR, TIM_DMA_CC2, 
//                                                   BitPeriod, TIM_DMABURSTLENGTH_1TRANSFER, 2);
	 /* Enable the TIM Update interrupt */
//  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start_IT(&htim1,TIM_CHANNEL_2);
}
/************************************************* 
@Description:�ر�ͨ��0
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch0_Stop(void)
{
	HAL_TIM_PWM_Stop_IT(&htim1,TIM_CHANNEL_2);
}

/************************************************* 
@Description:����ͨ��һ
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch1_Start(void)
{
	Base_Data_Init(&Base_Ch1);//��ʼ��ͨ��1
	HAL_GPIO_TogglePin(LedCh1_GPIO_Port,LedCh1_Pin);
	HAL_GPIO_WritePin(BaseCh1_GPIO_Port, BaseCh1_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start_IT(&htim15,TIM_CHANNEL_2);
}
/************************************************* 
@Description:�ر�ͨ��1
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch1_Stop(void)
{
	HAL_TIM_PWM_Stop_IT(&htim15,TIM_CHANNEL_2);
}
/************************************************* 
@Description:����ͨ��2
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch2_Start(void)
{
	Base_Data_Init(&Base_Ch2);//��ʼ��ͨ��2
	HAL_GPIO_WritePin(BaseCh2_GPIO_Port, BaseCh2_Pin, GPIO_PIN_SET);
	HAL_GPIO_TogglePin(LedCh2_GPIO_Port,LedCh2_Pin);
	HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_4);
}
/************************************************* 
@Description:�ر�ͨ��2
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch2_Stop(void)
{
	HAL_TIM_PWM_Stop_IT(&htim2,TIM_CHANNEL_4);
}
/************************************************* 
@Description:����ͨ��3
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch3_Start(void)
{
	Base_Data_Init(&Base_Ch3);//��ʼ��ͨ��3
	HAL_GPIO_WritePin(BaseCh3_GPIO_Port, BaseCh3_Pin, GPIO_PIN_SET);
	HAL_GPIO_TogglePin(LedCh3_GPIO_Port,LedCh3_Pin);	
	HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_2);
}
/************************************************* 
@Description:�ر�ͨ��3
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Ch3_Stop(void)
{
	HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_2);
}
/************************************************* 
@Description:����ͨ��һ
@Input:
@Output:
@Return:
*************************************************/ 
void Base_Process(void)
{
	if(1 == Base_Ch0.sfinish)//ͨ��0����
	{
		Base_Mode.AllowSend = 1;
		Base_Ch0.sfinish = 0;
		Base_Mode.SendSeq = S_Channel1;
	}
	if(1 == Base_Ch1.sfinish)//ͨ��1����
	{
		Base_Mode.AllowSend = 1;
		Base_Ch1.sfinish = 0;
		Base_Mode.SendSeq = S_Channel2;		
	}
	if(1 == Base_Ch2.sfinish)//ͨ��2����
	{
		Base_Mode.AllowSend = 1;
		Base_Ch2.sfinish = 0;
		Base_Mode.SendSeq = S_Channel3;		
	}
	if(1 == Base_Ch3.sfinish)//ͨ��������
	{
		Base_Ch3.sfinish = 0;
		Base_Mode.SendSeq = S_Channel0;		
	}
	if(1 == Time7.flag)//�ȴ����ڶ�ʱ��
	{
		Time7.flag = 0;
		Base_Mode.AllowSend = 1;//��Ҫ����
		Base_Mode.SendSeq = S_Channel0;//ͨ��0->1>2->3->0
		Base_Mode.NewStartSend = 1;
	}
	if(1 == Base_Mode.AllowSend)
	{
		Base_Mode.AllowSend = 0;
		if(1 == Base_Mode.NewStartSend)
		{
			Base_Mode.NewStartSend = 0;
			if(Base_Ch0.sfinish||Base_Ch1.sfinish
			||Base_Ch2.sfinish||Base_Ch3.sfinish)
			{
				Base_Ch0_Stop();
				Base_Ch1_Stop();
				Base_Ch2_Stop();
				Base_Ch3_Stop();
			}
		}
		switch(Base_Mode.SendSeq)
		{
			case S_Channel0:
			{
				if(1== Base_Ch0.isOpen)
				{
					Base_Ch0_Start();
				}
				else
				{
					Base_Ch0.sfinish =1;
				}			
			}
			break;
			case S_Channel1:
			{
				if(1 == Base_Ch1.isOpen)
				{
					Base_Ch1_Start();
				}
				else
				{
					Base_Ch1.sfinish =1;
				}
	
			}
			break;
			case S_Channel2:
			{
				if(1 == Base_Ch2.isOpen)
				{
					Base_Ch2_Start();
				}
				else
				{
					Base_Ch2.sfinish =1;
				}

			}
			break;
			case S_Channel3:
			{
				if(1== Base_Ch3.isOpen)
				{
					Base_Ch3_Start();
				}
				else
				{
					Base_Ch3.sfinish =1;
				}

			}
			break;
		}
	}
}

/*
@Description:
@Input:
@Output:
@Return:
*/
void UpdateRunPara(void)
{
	
	Base_Mode.ID[0] = para_record[0];
	Base_Mode.ID[1] = para_record[1];//id
	Base_Ch0.isOpen = (para_record[BASEP_CH0OPEN_IDX] & BASEP_CHOPEN_Msk) >> BASEP_CHOPEN_Pos;//ͨ������
	Base_Ch1.isOpen = (para_record[BASEP_CH1OPEN_IDX] & BASEP_CHOPEN_Msk) >> BASEP_CHOPEN_Pos;//ͨ������
	Base_Ch2.isOpen = (para_record[BASEP_CH2OPEN_IDX] & BASEP_CHOPEN_Msk) >> BASEP_CHOPEN_Pos;//ͨ������
	Base_Ch3.isOpen = (para_record[BASEP_CH3OPEN_IDX] & BASEP_CHOPEN_Msk) >> BASEP_CHOPEN_Pos;//ͨ������	
	//��������
	Base_Mode.Send_Period = (para_record[BASEP_AUTOREPORT_IDX] & BASEP_AUTOREPORT_Msk) >> BASEP_AUTOREPORT_Pos;
	Base_Mode.Send_Period = 1<<Base_Mode.Send_Period;//1 2 4 8 16 32 64 128
	Base_Mode.SendSeq = S_Channel0;
	//��ʼ������
	Base_Ch0.Data[BASE_DATA_IDX] = (para_record[BASEP_CH0DATA_IDX] & BASEP_CHDATA_Msk);
	Base_Ch1.Data[BASE_DATA_IDX] = (para_record[BASEP_CH1DATA_IDX] & BASEP_CHDATA_Msk);
	Base_Ch2.Data[BASE_DATA_IDX] = (para_record[BASEP_CH2DATA_IDX] & BASEP_CHDATA_Msk);
	Base_Ch3.Data[BASE_DATA_IDX] = (para_record[BASEP_CH3DATA_IDX] & BASEP_CHDATA_Msk);	
	Base_Ch0.Data[BASE_ID_IDX] = Base_Mode.ID[0];
	Base_Ch0.Data[BASE_ID_IDX+1] = Base_Mode.ID[1]+B_Channel0;
	Base_Ch1.Data[BASE_ID_IDX] = Base_Mode.ID[0];
	Base_Ch1.Data[BASE_ID_IDX+1] = Base_Mode.ID[1]+B_Channel1;
	Base_Ch2.Data[BASE_ID_IDX] = Base_Mode.ID[0];
	Base_Ch2.Data[BASE_ID_IDX+1] = Base_Mode.ID[1]+B_Channel2;
	Base_Ch3.Data[BASE_ID_IDX] = Base_Mode.ID[0];
	Base_Ch3.Data[BASE_ID_IDX+1] = Base_Mode.ID[1]+B_Channel3;
}
/*
@Description:
@Input:
@Output:
@Return:
*/
void Base_Para_Init(void)
{
	memcpy(para_record,para_default,PARA_RECORD_LEN);
	UpdateRunPara();
}
/************************************************* 
@Description:�����¼��ص�����
@Input:
@Output:
@Return:
*************************************************/ 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if( htim->Instance == TIM1)
	{
		finish++;
		if(finish >= 1)
		{
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
		}
	}
	else if(htim->Instance == TIM7)
	{
		Time7.cnt++;
		if(Time7.cnt >= Base_Mode.Send_Period)
		{
			Time7.cnt = 0;
			Time7.flag=1;	
		}
	}
}
/************************************************* 
@Description:�Ƚ��¼��ص�����
@Input:
@Output:
@Return:
*************************************************/ 
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if( TIM1 ==htim->Instance )
	{
		Base_Ch0.PwmNum++;
		if(Base_Ch0.PwmNum >= Base_Ch0.BitPeriod[Base_Ch0.idx])
		{
			Base_Ch0.PwmNum = 0;
			Base_Ch0.idx=1;
			switch(Base_Ch0.SendState)
			{
				case S_BHead://ͷ
				{
					if (Base_Ch0.SendBitCount < BaseHeadLen)
					{
						Base_Ch0.IOCurrentState = (Base_Ch0.tHead&BaseHeadMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh0_GPIO_Port, BaseCh0_Pin, Base_Ch0.IOCurrentState);
						Base_Ch0.tHead<<=1;
						Base_Ch0.SendBitCount++;
					}	
					if(Base_Ch0.SendBitCount == BaseHeadLen)
					{
						Base_Ch0.SendState = S_BData;
						Base_Ch0.SendBitCount = 0;
					}
				}
				break;
				case S_BData://4�ֽ�����
				{
					if (Base_Ch0.SendBitCount < BaseDataLen)
					{
						Base_Ch0.IOCurrentState = (Base_Ch0.tData&BaseDataMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh0_GPIO_Port, BaseCh0_Pin, Base_Ch0.IOCurrentState);
						Base_Ch0.tData<<=1;
						Base_Ch0.SendBitCount++;
					}		
					else
					{
						Base_Ch0_Stop();
						Base_Ch0.sfinish = 1;
					}
				}
				break;
			}			
			
		}	
	}
	if(TIM15 == htim->Instance )
	{
		Base_Ch1.PwmNum++;
		if(Base_Ch1.PwmNum >= Base_Ch1.BitPeriod[Base_Ch1.idx])
		{
			Base_Ch1.PwmNum = 0;
			Base_Ch1.idx=1;
			switch(Base_Ch1.SendState)
			{
				case S_BHead://ͷ
				{
					if (Base_Ch1.SendBitCount < BaseHeadLen)
					{
						Base_Ch1.IOCurrentState = (Base_Ch1.tHead&BaseHeadMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh1_GPIO_Port, BaseCh1_Pin, Base_Ch1.IOCurrentState);
						Base_Ch1.tHead<<=1;
						Base_Ch1.SendBitCount++;
					}	
					if(Base_Ch1.SendBitCount == BaseHeadLen)
					{
						Base_Ch1.SendState = S_BData;
						Base_Ch1.SendBitCount = 0;
					}
				}
				break;
				case S_BData://4�ֽ�����
				{
					if (Base_Ch1.SendBitCount < BaseDataLen)
					{
						Base_Ch1.IOCurrentState = (Base_Ch1.tData&BaseDataMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh1_GPIO_Port, BaseCh1_Pin, Base_Ch1.IOCurrentState);
						Base_Ch1.tData<<=1;
						Base_Ch1.SendBitCount++;
					}		
					else
					{
						Base_Ch1_Stop();
						Base_Ch1.sfinish = 1;
					}
				}
				break;
			}			
		}	
	}	
	if( TIM2 == htim->Instance )
	{
		Base_Ch2.PwmNum++;
		if(Base_Ch2.PwmNum >= Base_Ch2.BitPeriod[Base_Ch2.idx])
		{
			Base_Ch2.PwmNum = 0;
			Base_Ch2.idx=1;
			switch(Base_Ch2.SendState)
			{
				case S_BHead://ͷ
				{
					if (Base_Ch2.SendBitCount < BaseHeadLen)
					{
						Base_Ch2.IOCurrentState = (Base_Ch2.tHead&BaseHeadMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh2_GPIO_Port, BaseCh2_Pin, Base_Ch2.IOCurrentState);
						Base_Ch2.tHead<<=1;
						Base_Ch2.SendBitCount++;
					}	
					if(Base_Ch2.SendBitCount == BaseHeadLen)
					{
						Base_Ch2.SendState = S_BData;
						Base_Ch2.SendBitCount = 0;
					}
				}
				break;
				case S_BData://4�ֽ�����
				{
					if (Base_Ch2.SendBitCount < BaseDataLen)
					{
						Base_Ch2.IOCurrentState = (Base_Ch2.tData&BaseDataMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh2_GPIO_Port, BaseCh2_Pin, Base_Ch2.IOCurrentState);
						Base_Ch2.tData<<=1;
						Base_Ch2.SendBitCount++;
					}		
					else
					{
						Base_Ch2_Stop();
						Base_Ch2.sfinish = 1;
					}
				}
				break;
			}			
		}				
	}
	if( TIM3 == htim->Instance )
	{
		Base_Ch3.PwmNum++;
		if(Base_Ch3.PwmNum >= Base_Ch3.BitPeriod[Base_Ch3.idx])
		{
			Base_Ch3.PwmNum = 0;
			Base_Ch3.idx=1;
			switch(Base_Ch3.SendState)
			{
				case S_BHead://ͷ
				{
					if (Base_Ch3.SendBitCount < BaseHeadLen)
					{
						Base_Ch3.IOCurrentState = (Base_Ch3.tHead&BaseHeadMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh3_GPIO_Port, BaseCh3_Pin, Base_Ch3.IOCurrentState);
						Base_Ch3.tHead<<=1;
						Base_Ch3.SendBitCount++;
					}	
					if(Base_Ch3.SendBitCount == BaseHeadLen)
					{
						Base_Ch3.SendState = S_BData;
						Base_Ch3.SendBitCount = 0;
					}
				}
				break;
				case S_BData://4�ֽ�����
				{
					if (Base_Ch3.SendBitCount < BaseDataLen)
					{
						Base_Ch3.IOCurrentState = (Base_Ch3.tData&BaseDataMsk)?GPIO_PIN_SET:GPIO_PIN_RESET;
						HAL_GPIO_WritePin(BaseCh3_GPIO_Port, BaseCh3_Pin, Base_Ch3.IOCurrentState);
						Base_Ch3.tData<<=1;
						Base_Ch3.SendBitCount++;
					}		
					else
					{
						Base_Ch3_Stop();
						Base_Ch3.sfinish = 1;
					}
				}
				break;
			}			
		}			
	}
}

