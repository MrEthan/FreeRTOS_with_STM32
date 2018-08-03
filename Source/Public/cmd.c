#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

#include "cmd.h"
#include "debug.h"
#include "my_portable.h"
#include "cursor_control.h"


#define ARG_NUM     8          //����������Ĳ�������
#define CMD_LEN     20         //������ռ�õ�����ַ�����
#define CMD_BUF_LEN 60         //��������󳤶�

typedef struct {
    char rec_buf[CMD_BUF_LEN];            //�����������
    int  rec_count;                       //���������ַ�����
    char processed_buf[CMD_BUF_LEN];      //�洢�ӹ��������(ȥ�������ַ�)
    int  cmd_arg[ARG_NUM];             //��������Ĳ���
}cmd_analyze_struct;


typedef struct{
    const char *cmd_name;                          //�����ַ���
    int max_args;                              //��������Ŀ
    void (*handle)(int argc, void *cmd_arg);   //����ص�����
    char *help;                                //������Ϣ
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
/* ����    ������Ŀ    ������      ������Ϣ                        */
{"hello",   0,      printf_hello,   "hello                      -��ӡHelloWorld!"},
{"arg",     8,      handle_arg,     "arg<arg1> <arg2> ...      -������, ��ӡ����Ĳ���"},
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

/*�ṩ�������жϷ�����򣬱��洮�ڽ��յ��ĵ����ַ�*/
void fill_rec_buf_isr(char data)
{
    //��������
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;//����Ϊfalse

    cmd_analyze.rec_buf[cmd_analyze.rec_count] = data;

    if (0x08 == cmd_analyze.rec_buf[cmd_analyze.rec_count]){
        //backspace
        if (cmd_analyze.rec_count > 0){
            cmd_analyze.rec_buf[cmd_analyze.rec_count] = 0;
            cmd_analyze.rec_buf[cmd_analyze.rec_count-1] = 0;
            cmd_analyze.rec_count--;
            /* ֪ͨ����ʾ����ˢ����ʾ */
            xTaskGenericNotifyFromISR(xCmdEchoHandle, (uint32_t)EN_NOTIFY_BACKSPACE, eSetBits, NULL, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

    }else if('\n' == cmd_analyze.rec_buf[cmd_analyze.rec_count] || '\r'== cmd_analyze.rec_buf[cmd_analyze.rec_count-1]){
        //'\r'--0x0D '\n' -- 0x0A
        cmd_analyze.rec_count = 0;
        /*�յ�һ֡���ݣ��������н�����������֪ͨ*/
        vTaskNotifyGiveFromISR(xCmdAnalyzeHandle, &xHigherPriorityTaskWoken);
        /*�Ƿ���Ҫǿ���������л�*/
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }else{
        cmd_analyze.rec_count++;
        /* ֪ͨ����ʾ���� */
        xTaskGenericNotifyFromISR(xCmdEchoHandle, (uint32_t)EN_NOTIFY_ECHO, eSetBits, NULL, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        /*�����Դ��룬��ֹ����Խ��*/
        if(cmd_analyze.rec_count >= CMD_BUF_LEN){
           cmd_analyze.rec_count = 0;
       }
    }

    return;
}

/**
* ʹ��SecureCRT�����շ�����,�ڷ��͵��ַ����п��ܴ��в���Ҫ���ַ��Լ������ַ�,
* �����˸��,�����ƶ����ȵ�,��ʹ�������й��߽����ַ���֮ǰ,��Ҫ����Щ�����ַ���
* �������ַ�ȥ����.
* ֧�ֵĿ����ַ���:
*   ����:1B 5B 41
*   ����:1B 5B 42
*   ����:1B 5B 43
*   ����:1B 5B 44
*   �س�����:0D 0A
*  Backspace:08
*  Delete:7F
*/
static uint32_t get_true_char_stream(char *dest, const char *src)
{
   uint32_t dest_count = 0;
   uint32_t src_count = 0;

    while(src[src_count] != 0x0D && src[src_count+1] != 0x0A){
       if(isprint(src[src_count])){
           /* ����ɼ��ַ� */
           dest[dest_count++] = src[src_count++];
       }
       else{
           switch(src[src_count])
           {
                case 0x08:                          //�˸����ֵ
                    if(dest_count > 0){
                        dest_count--;
                    }
                    src_count++;
                break;

                case 0x1B:
                    if(src[src_count + 1] == 0x5B){
                        if(src[src_count+2] == 0x41 || src[src_count+2] == 0x42){
                            src_count +=3;              //���ƺ����Ƽ���ֵ
                        }else if(src[src_count+2] == 0x43){
                            dest_count++;               //���Ƽ���ֵ
                            src_count+=3;
                        }else if(src[src_count+2] == 0x44){
                            if(dest_count >0) {          //���Ƽ���ֵ
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

/*�ַ���ת10/16������*/
static int32_t string_to_dec(uint8_t *buf, uint32_t len)
{
   uint32_t i = 0;
   uint32_t base = 10;       //����
   int32_t  neg = 1;         //��ʾ����,1=����
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
           //Ϊ�ո�
           break;
       }

       result *= base;
       if(isdigit(buf[i])){
           //�Ƿ�Ϊ0~9
           result += buf[i]-'0';
       }else if(isxdigit(buf[i])){
            //�Ƿ�Ϊa~f����A~F
            result += tolower(buf[i]) - 87;//a��asciiΪ97����ȥ87�պ�=10
       }else{
           result += buf[i]-'0';
       }
    }
   result *= neg;

    return result ;
}


/**
* ���������������,�Կո���Ϊһ����������,֧������ʮ��������(��:0x15),֧�����븺��(��-15)
* @param rec_buf   �������������
* @param len       ����������ܳ���
* @return -1:      ������������,����:��������
*/
static int32_t cmd_arg_analyze(char *rec_buf, unsigned int len)
{
   uint32_t i;
   uint32_t blank_space_flag=0;    //�ո��־
   uint32_t arg_num=0;             //������Ŀ
   uint32_t index[ARG_NUM];        //��Ч�����׸����ֵ���������

    /*����һ�����,�ҳ���������Ŀ,�Լ������ε��׸���������rec_buf�����е��±�*/
    for(i=0; i<len; i++){
       if(rec_buf[i] == 0x20){   //ȥ��ǰ���ո�
           blank_space_flag = 1;
           continue;
       }else if(rec_buf[i]==0x0D) {//����
           break;
       }else{
           if(blank_space_flag==1){
               blank_space_flag=0;
               if(arg_num < ARG_NUM){
                   index[arg_num] = i; //�洢�������ַ��±�i
                   arg_num++;
                }else{
                    return -1;      //��������̫��
                }
           }
       }
    }

    for(i = 0; i < arg_num; i++){
        cmd_analyze.cmd_arg[i] = string_to_dec((unsigned char *)(rec_buf + index[i]), len - index[i]);
    }
    return arg_num;
}

/* �����л��Ժ͹��������� */
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


/*�����з�������*/
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

       /*�ӽ�����������ȡ����*/
       for(i=0; i < CMD_LEN; i++){
           if((i > 0) &&
            (( cmd_analyze.processed_buf[i]==' ') ||
            (cmd_analyze.processed_buf[i] == '\r') ||
            (cmd_analyze.processed_buf[i] == '\n'))){
                cmd_buf[i]='\0';        //�ַ���������
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
       /* ������ͷ��ӡ# */
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

    /* ����cmd���� */
    ret = xTaskCreate(vTaskCmdAnalyze, "cmd_analyze_task", 1024, NULL, NORMAL_TASK_PRIORITY, &xCmdAnalyzeHandle);
    if (pdPASS != ret){
        DERROR("create cmd analyze task failed, ret:%d\r\n", ret);
    }

    return;
}
