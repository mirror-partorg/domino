#ifndef SEND_ERROR_H_INCLUDED
#define SEND_ERROR_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef USE_IM
#define send_error_message(a)
#define send_error_message_with_host_info(a)
#define send_error_message_printf(frmt, ...)
#define send_error_message_set_disable_send()
#define send_info_message_width_host_info_printf(frmt, ...)
//#define send_error_message_width_host_info_and_program_name_printf(program_name, frmt, ...)
#define send_error_message_set_enable_send()
#define send_error_message_width_host_info_printf(frmt, ...)

#else
int send_error_message(const char *message);
int send_error_message2(const char *message, const char *filename);
int send_error_set_sendto(const char *sendto);
int send_error_message_with_host_info(const char *msg);
int send_error_message_width_host_info_printf(const char *frmt, ...);
//int send_error_message_width_host_info_and_program_name_printf(const char *program_name, const char *frmt, ...);
int send_info_message_width_host_info_printf(const char *frmt, ...);
void send_error_message_set_disable_send();
//int send_error_message_width_host_info_and_program_name_printf(const char *program, const char *msgfmt, ...);
void send_error_message_set_enable_send();
int send_im_message(const char *name, const char *domain, const char *password, const char *recipients, const char *message);

#endif

#ifdef __cplusplus
}
#endif



#endif // SEND_ERROR_H_INCLUDED
