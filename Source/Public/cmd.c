#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

#include "cmd.h"
#include "debug.h"
#include "my_portable.h"
#include "cursor_control.h"


#define ARG_NUM     8          //命令中允许的参数个数
#define CMD_LEN     20         //命令名占用的最大字符长度
#define CMD_BUF_LEN 60         //命令缓存的最大长度

typedef struct {
    char rec_buf[CMD_BUF_LEN];            //接收命令缓冲区
    int  rec_count;                       //缓冲区中字符个数
    char processed_buf[CMD_BUF_LEN];      //存储加工后的命令(去除控制字符)
    int  cmd_arg[ARG_NUM];             //保存命令的参数
}cmd_analyze_struct;


typedef struct{
    const char *cmd_name;                          //命令字符串
    int max_args;                              //最大参数数目
    void (*handle)(int argc, void *cmd_arg);   //命令回调函数
    char *help;                                //帮助信息
}cmd_list_struct;

typedef enum{
    EN_NOTIFY_ECHO = 1,
    EN_NOTIFY_BACKSPACE
}CMD_NOTIFY_ENUM;

static cmd_analyze_struct cmd_analyze;
static TaskHandle_t xCmdAnalyzeHandle;
static TaskHandle_t xCmdEchoHandle;

void printf_hello(int32_t argc, void * cmd_arg);
void handle_arg(int32_t argc, void * cmd_arg);

const cmd_list_struct cmd_list[]={
/* 命令    参数数目    处理函数      帮助信息                        */
{"hello",   0,      printf_hello,   "hello                      -打印HelloWorld!"},
{"arg",     8,      handle_arg,     "arg<arg1> <arg2> ...      -测试用, 打印输入的参数"},
};

void printf_hello(int32_t argc, void *cmd_arg)
{
    printf("hello world!!!\r\n");
    return;
}

void handle_arg(int32_t argc, void *cmd_arg)
{
   uint32_t i;
   int32_t *arg = (int32_t *)cmd_arg;

    if(argc == 0){
       DDEBUG("no para.\r\n");
    }else{
        for (i = 0; i < argc; i++){
            printf("para%d:%d\r\n", i + 1, arg[i]);
        }
    }
    return;
}

/*提供给串口中断服务程序，保存串口接收到的单个字符*/
void fill_rec_buf_isr(char data)
{
    //接收数据
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;//必须为false

    cmd_analyze.rec_buf[cmd_analyze.rec_count] = data;

    if (0x08 == cmd_analyze.rec_buf[cmd_analyze.rec_count]){
        //backspace
        if (cmd_analyze.rec_count > 0){
            cmd_analyze.rec_buf[cmd_analyze.rec_count] = 0;
            cmd_analyze.rec_buf[cmd_analyze.rec_count-1] = 0;
            cmd_analyze.rec_count--;
            /* 通知回显示任务刷新显示 */
            xTaskGenericNotifyFromISR(xCmdEchoHandle, (uint32_t)EN_NOTIFY_BACKSPACE, eSetBits, NULL, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

    }else if('\n' == cmd_analyze.rec_buf[cmd_analyze.rec_count] || '\r'== cmd_analyze.rec_buf[cmd_analyze.rec_count-1]){
        //'\r'--0x0D '\n' -- 0x0A
        cmd_analyze.rec_count = 0;
        /*收到一帧数据，向命令行解释器任务发送通知*/
        vTaskNotifyGiveFromISR(xCmdAnalyzeHandle, &xHigherPriorityTaskWoken);
        /*是否需要强制上下文切换*/
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }else{
        cmd_analyze.rec_count++;
        /* 通知回显示任务 */
        xTaskGenericNotifyFromISR(xCmdEchoHandle, (uint32_t)EN_NOTIFY_ECHO, eSetBits, NULL, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        /*防御性代码，防止数组越界*/
        if(cmd_analyze.rec_count >= CMD_BUF_LEN){
           cmd_analyze.rec_count = 0;
       }
    }

    return;
}

/**
* 使用SecureCRT串口收发工具,在发送的字符流中可能带有不需要的字符以及控制字符,
* 比如退格键,左右移动键等等,在使用命令行工具解析字符流之前,需要将这些无用字符以
* 及控制字符去除掉.
* 支持的控制字符有:
*   上移:1B 5B 41
*   下移:1B 5B 42
*   右移:1B 5B 43
*   左移:1B 5B 44
*   回车换行:0D 0A
*  Backspace:08
*  Delete:7F
*/
static uint32_t get_true_char_stream(char *dest, const char *src)
{
   uint32_t dest_count = 0;
   uint32_t src_count = 0;

    while(src[src_count] != 0x0D && src[src_count+1] != 0x0A){
       if(isprint(src[src_count])){
           /* 保存可见字符 */
           dest[dest_count++] = src[src_count++];
       }
       else{
           switch(src[src_count])
           {
                case 0x08:                          //退格键键值
                    if(dest_count > 0){
                        dest_count--;
                    }
                    src_count++;
                break;

                case 0x1B:
                    if(src[src_count + 1] == 0x5B){
                        if(src[src_count+2] == 0x41 || src[src_count+2] == 0x42){
                            src_count +=3;              //上移和下移键键值
                        }else if(src[src_count+2] == 0x43){
                            dest_count++;               //右移键键值
                            src_count+=3;
                        }else if(src[src_count+2] == 0x44){
                            if(dest_count >0) {          //左移键键值
                                dest_count--;
                            }
                           src_count += 3;
                        }else{
                            src_count +=3;
                        }
                    }else{
                        src_count ++;
                    }
                break;
                default:
                    src_count++;
                    break;
           }
       }
    }
    dest[dest_count++] = src[src_count++];
    dest[dest_count++] = src[src_count++];
    return dest_count;
}

/*字符串转10/16进制数*/
static int32_t string_to_dec(uint8_t *buf, uint32_t len)
{
   uint32_t i = 0;
   uint32_t base = 10;       //基数
   int32_t  neg = 1;         //表示正负,1=正数
   int32_t  result = 0;

    if((buf[0] == '0') && (buf[1] == 'x')){
       base=16;
       neg=1;
       i=2;
    }else if(buf[0]=='-'){
       base=10;
       neg=-1;
       i=1;
    }

    for(;i < len; i++){
       if(buf[i]==0x20 || buf[i]==0x0D || buf[i] == 0x0A){
           //为空格
           break;
       }

       result *= base;
       if(isdigit(buf[i])){
           //是否为0~9
           result += buf[i]-'0';
       }else if(isxdigit(buf[i])){
            //是否为a~f或者A~F
            result += tolower(buf[i]) - 87;//a的ascii为97，减去87刚好=10
       }else{
           result += buf[i]-'0';
       }
    }
   result *= neg;

    return result ;
}


/**
* 命令参数分析函数,以空格作为一个参数结束,支持输入十六进制数(如:0x15),支持输入负数(如-15)
* @param rec_buf   命令参数缓存区
* @param len       命令的最大可能长度
* @return -1:      参数个数过多,其它:参数个数
*/
static int32_t cmd_arg_analyze(char *rec_buf, unsigned int len)
{
   uint32_t i;
   uint32_t blank_space_flag=0;    //空格标志
   uint32_t arg_num=0;             //参数数目
   uint32_t index[ARG_NUM];        //有效参数首个数字的数组索引

    /*先做一遍分析,找出参数的数目,以及参数段的首个数字所在rec_buf数组中的下标*/
    for(i=0; i<len; i++){
       if(rec_buf[i] == 0x20){   //去除前导空格
           blank_space_flag = 1;
           continue;
       }else if(rec_buf[i]==0x0D) {//换行
           break;
       }else{
           if(blank_space_flag==1){
               blank_space_flag=0;
               if(arg_num < ARG_NUM){
                   index[arg_num] = i; //存储参数首字符下标i
                   arg_num++;
                }else{
                    return -1;      //参数个数太多
                }
           }
       }
    }

    for(i = 0; i < arg_num; i++){
        cmd_analyze.cmd_arg[i] = string_to_dec((unsigned char *)(rec_buf + index[i]), len - index[i]);
    }
    return arg_num;
}

/* 命令行回显和光标控制任务 */
void vTaskCmdEcho(void *pvParameters)
{
    uint32_t notify_val;

    while(1)
    {
        //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        xTaskNotifyWait(UINT32_MAX, UINT32_MAX, &notify_val, portMAX_DELAY);
        if (EN_NOTIFY_ECHO == notify_val){
            //printf("%c", cmd_analyze.rec_buf[cmd_analyze.rec_count - 1]);
            fflush(stdout);
        }else if(EN_NOTIFY_BACKSPACE == notify_val){
            BACKSPACE();
            MOVELEFT(1);
            fflush(stdout);
        }
    }
}


/*命令行分析任务*/
void vTaskCmdAnalyze(void *pvParameters)
{
    uint32_t i;
    int32_t rec_arg_num;
    char cmd_buf[CMD_LEN];

    while(1)
    {
       uint32_t rec_num;

       ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
       vTaskDelay(100);
       rec_num = get_true_char_stream(cmd_analyze.processed_buf, cmd_analyze.rec_buf);

       /*从接收数据中提取命令*/
       for(i=0; i < CMD_LEN; i++){
           if((i > 0) &&
            (( cmd_analyze.processed_buf[i]==' ') ||
            (cmd_analyze.processed_buf[i] == '\r') ||
            (cmd_analyze.processed_buf[i] == '\n'))){
                cmd_buf[i]='\0';        //字符串结束符
                break;
           }else{
                cmd_buf[i] = cmd_analyze.processed_buf[i];
           }
       }

       rec_arg_num = cmd_arg_analyze(&cmd_analyze.processed_buf[i], rec_num);

       for(i=0; i < sizeof(cmd_list)/sizeof(cmd_list[0]); i++) {
           if(0 == strcmp(cmd_buf, cmd_list[i].cmd_name)) {
                if(rec_arg_num<0 || rec_arg_num > cmd_list[i].max_args){
                    printf("\r\ntoo much para.\r\n");
                }else{
                    printf("\r\n");
                    cmd_list[i].handle(rec_arg_num, (void *)cmd_analyze.cmd_arg);
                }
                break;
           }
       }

       if(i >= sizeof(cmd_list)/sizeof(cmd_list[0])){
           printf("\r\ncmd do not support.\r\n");
       }
       /* 命令行头打印# */
       printf("#");
       fflush(stdout);
    }
}

void cmd_create_task(void)
{
    BaseType_t ret = pdPASS;

    ret = xTaskCreate(vTaskCmdEcho, "cmd_echo_task", 1024, NULL, NORMAL_TASK_PRIORITY, &xCmdEchoHandle);
    if (pdPASS != ret){
        DERROR("create cmd echo task failed, ret:%d\r\n", ret);
    }

    /* 创建cmd任务 */
    ret = xTaskCreate(vTaskCmdAnalyze, "cmd_analyze_task", 1024, NULL, NORMAL_TASK_PRIORITY, &xCmdAnalyzeHandle);
    if (pdPASS != ret){
        DERROR("create cmd analyze task failed, ret:%d\r\n", ret);
    }

    return;
}
