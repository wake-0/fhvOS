extern int IpcManagerRegisterNamespace(char* namespace_name);
extern int IpcManagerSendMessage(char* sender_namespace, char* namespace_name, char* message, int len);
extern int IpcManagerHasMessage(char* namespace_name);
extern int IpcManagerGetNextMessage(char* namespace_name, char* message_buffer, int msg_buf_len, char* sender_namespace, int ns_buf_len);
extern int IpcManagerCloseNamespace(char* namespace_name);
extern int IpcManagerChannelCount();
extern int IpcManagerGetChannel(int index, char* buf, int len);