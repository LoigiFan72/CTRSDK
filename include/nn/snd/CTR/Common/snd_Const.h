#pragma once

#define NN_SND_HW_BASE_CLOCK                (16756991)
#define NN_SND_HW_SYSTEM_CLOCK              (NN_SND_HW_BASE_CLOCK * 16)
#define NN_SND_HW_I2S_CLOCK_32KHZ           (NN_SND_HW_SYSTEM_CLOCK / (32 * 256))

#define NN_SND_BTDMP_OUTPUT_FRAME_TRIG_FREQ (20)
#define NN_SND_BTDMP_OUTPUT_FIFO_SIZE       (16)
#define NN_SND_OUTPUT_RINGBUF_LOADUNIT      (NN_SND_BTDMP_OUTPUT_FIFO_SIZE * NN_SND_BTDMP_OUTPUT_FRAME_TRIG_FREQ)

#define NN_SND_COM_DIRECT_ID_INIT                      (3)

#define NN_SND_SYNC_SEM_ID                   14
#define NN_SND_SYNC_SEM_MASK                 (1 << NN_SND_SYNC_SEM_ID)

#define NN_SND_SAMPLES_PER_FRAME            (NN_SND_OUTPUT_RINGBUF_LOADUNIT / 2)
#define NN_SND_USECS_PER_FRAME              ((NN_SND_SAMPLES_PER_FRAME * 1000000) / NN_SND_HW_I2S_CLOCK_32KHZ)

#define NN_SND_NEXT_BUFFER_NUM              (4)
#define NN_SND_VOICE_NUM                    (24)

#define NN_SND_HW_I2S_CLOCK_32KHZ_F32       (NN_SND_HW_SYSTEM_CLOCK / (32 * 256))