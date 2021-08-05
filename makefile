#工程的名称及最后生成文件的名字
TARGET = rtthread

#设定临时性环境变量
export CC             = arm-none-eabi-gcc           
export AS             = arm-none-eabi-as
export LD             = arm-none-eabi-ld
export OBJCOPY        = arm-none-eabi-objcopy

#读取当前工作目录
TOP=$(shell %~dp0)

#设定包含文件目录
INC_FLAGS= -I $(TOP)/applications \
           -I $(TOP)/cubemx/Src \
           -I $(TOP)/drivers \
           -I $(TOP)/libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc \
           -I $(TOP)/libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates \
           -I $(TOP)/libraries/STM32F4xx_HAL_Driver/Src/Legacy \
           -I $(TOP)/libraries/STM32F4xx_HAL_Driver/Src \
		   -I $(TOP)/packages/rw007-v2.0.1/example \
		   -I $(TOP)/packages/rw007-v2.0.1/src \
		   -I $(TOP)/rt-thread/components/drivers/misc \
		   -I $(TOP)/rt-thread/components/drivers/serial \
		   -I $(TOP)/rt-thread/components/drivers/spi \
		   -I $(TOP)/rt-thread/components/drivers/src \
		   -I $(TOP)/rt-thread/components/drivers/wlan \
		   -I $(TOP)/rt-thread/components/finsh \
		   -I $(TOP)/rt-thread/components/libc/compilers/minilibc \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/api \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/apps/ping \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/arch \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/core \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/core/ipv4 \
		   -I $(TOP)/rt-thread/components/net/lwip-2.0.2/src/netif \
		   -I $(TOP)/rt-thread/libcpu/arm/common \
		   -I $(TOP)/rt-thread/libcpu/arm/cortex-m4 \
		   -I $(TOP)/rt-thread/src \


CFLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F4 -DUSE_HAL_DRIVER -DSTM32F407xx $(INC_FLAGS) -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" 
ASFLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -x assembler-with-cpp $(INC_FLAGS) -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

C_SRC=$(shell find ./ -name '*.c')  
C_OBJ=$(C_SRC:%.c=%.o)          

ASM_SRC=$(shell find ./ -name '*.s')
ASM_OBJ=$(ASM_SRC:%.s=%.o)

.PHONY: all clean update      

all: rtthread.elf secondary-outputs

rtthread.elf:$(TOP)/build/%.o
				$(CC) -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g3 -gdwarf-2 -T "$(TOP)\linkscripts\STM32F407VG\link.lds" -Xlinker --gc-sections -Wl,-Map,"rtthread.map" -Xlinker --cref -o "rtthread.elf" $(OBJS) $(USER_OBJS) $(LIBS)
  			  $(OBJCOPY) $(TARGET).elf  $(TARGET).bin -Obinary 
  			  $(OBJCOPY) $(TARGET).elf  $(TARGET).hex -Oihex


$(TOP)/build/%.o:%.c
    @$(CC) $(CFLAGS) -c -o $@ $<


$(TOP)/build/%.o:%.s
    $(CC) $(ASFLAGS) -c -o $@ $<

clean:
    rm -f $(shell find ./ -name '*.o')
    rm -f $(shell find ./ -name '*.d')
    rm -f $(shell find ./ -name '*.map')
    rm -f $(shell find ./ -name '*.elf')
    rm -f $(shell find ./ -name '*.bin')
    rm -f $(shell find ./ -name '*.hex')

